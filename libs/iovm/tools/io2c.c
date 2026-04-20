/*
Copyright (c) 2005, Steve Dekorte
All rights reserved. See _License.txt.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"

/* --- Dynamic file list --- */

typedef struct {
	char **paths;
	size_t count;
	size_t capacity;
} FileList;

static void FileList_init(FileList *fl) {
	fl->count = 0;
	fl->capacity = 64;
	fl->paths = malloc(fl->capacity * sizeof(char *));
}

static void FileList_add(FileList *fl, const char *path) {
	if (fl->count == fl->capacity) {
		fl->capacity *= 2;
		fl->paths = realloc(fl->paths, fl->capacity * sizeof(char *));
	}
	fl->paths[fl->count++] = strdup(path);
}

static void FileList_free(FileList *fl) {
	for (size_t i = 0; i < fl->count; i++)
		free(fl->paths[i]);
	free(fl->paths);
}

/* --- Path helpers --- */

/* Return directory portion of path (caller must free). Returns "" for bare filename. */
static char *pathDir(const char *path) {
	const char *lastSlash = strrchr(path, '/');
	if (!lastSlash)
		return strdup("");
	size_t len = lastSlash - path;
	char *dir = malloc(len + 1);
	memcpy(dir, path, len);
	dir[len] = '\0';
	return dir;
}

/* Join dir and file into a new path (caller must free). If dir is empty, returns copy of file. */
static char *pathJoin(const char *dir, const char *file) {
	if (dir[0] == '\0')
		return strdup(file);
	size_t dlen = strlen(dir);
	size_t flen = strlen(file);
	char *result = malloc(dlen + 1 + flen + 1);
	memcpy(result, dir, dlen);
	result[dlen] = '/';
	memcpy(result + dlen + 1, file, flen);
	result[dlen + 1 + flen] = '\0';
	return result;
}

/* --- JSON _imports.json walker --- */

static int endsWith(const char *s, const char *suffix) {
	size_t slen = strlen(s);
	size_t sufflen = strlen(suffix);
	if (sufflen > slen) return 0;
	return strcmp(s + slen - sufflen, suffix) == 0;
}

static void collectFiles(const char *importsPath, FileList *fl) {
	JSON_Value *root = json_parse_file(importsPath);
	if (!root) {
		fprintf(stderr, "io2c: error parsing %s\n", importsPath);
		exit(1);
	}

	JSON_Array *arr = json_value_get_array(root);
	if (!arr) {
		fprintf(stderr, "io2c: %s is not a JSON array\n", importsPath);
		json_value_free(root);
		exit(1);
	}

	char *dir = pathDir(importsPath);
	size_t count = json_array_get_count(arr);

	for (size_t i = 0; i < count; i++) {
		const char *entry = json_array_get_string(arr, i);
		if (!entry) {
			fprintf(stderr, "io2c: non-string entry at index %zu in %s\n", i, importsPath);
			json_value_free(root);
			free(dir);
			exit(1);
		}

		char *fullPath = pathJoin(dir, entry);

		if (endsWith(entry, ".json")) {
			collectFiles(fullPath, fl);
		} else {
			FileList_add(fl, fullPath);
		}

		free(fullPath);
	}

	free(dir);
	json_value_free(root);
}

/* --- Original io2c functionality --- */

void showUsage(void) {
	printf("usage: io2c ObjectName ParseFunctionName (ioFile1 ioFile2 ... | imports.json)\n");
	printf("output is sent to standard output\n");
	printf("ParseFunctionName is either IoState_on_doCString_withLabel_ or "
	       "IoState_on_doPackedCString_withLabel_\n\n");
}

void quoteStream(FILE *in, FILE *out) {
	fputc('"', out);

	for (;;) {
		char c = fgetc(in);

		if (feof(in))
			break;

		if (c == '\\' || c == '"') {
			fputc('\\', out);
		}

		if (c == '\n') {
			fputs("\\n\"\n  ", out);
			fputc('\t', out);
			fputc('"', out);
		} else if (c == '\r') {
			fputs("\\r\"\r  ", out);
			fputc('\t', out);
			fputc('"', out);
		} else {
			fputc(c, out);
		}
	}

	fputs("\";\n\n", out);
}

void processFile(const char *objectName, const char *fileName,
                 const char *parseFunctionName) {
	FILE *in = fopen(fileName, "r");
	FILE *out = stdout;

	if (!in) {
		printf("unable to open input file %s\n", fileName);
		exit(-1);
	}

	fputs("\ts = ", out);
	quoteStream(in, out);
	fprintf(
	    out,
	    "\tIoState_rawOn_doCString_withLabel_(self, context, s, \"%s\");\n\n",
	    fileName);

	fclose(in);
}

int main(int argc, const char *argv[]) {
	if (argc < 4) {
		showUsage();
	} else {
		const char *objectName = argv[1];
		FILE *out = stdout;
		const char *parseFunctionName = argv[2];

		fputs("#include \"IoState.h\"\n", out);
		fputs("#include \"IoObject.h\"\n\n", out);
		fputs("void Io", out);
		fputs(objectName, out);
		fputs("Init(IoObject *context)\n{\n", out);
		fputs("\tIoState *self = IoObject_state((IoObject *)context);\n", out);
		fputs("\tchar *s;\n\n", out);

		/* If argv[3] is a .json file, use _imports.json mode */
		if (argc == 4 && endsWith(argv[3], ".json")) {
			FileList fl;
			FileList_init(&fl);
			collectFiles(argv[3], &fl);

			for (size_t i = 0; i < fl.count; i++) {
				processFile(objectName, fl.paths[i], parseFunctionName);
			}

			FileList_free(&fl);
		} else {
			/* Legacy mode: bare file list on CLI */
			for (int i = 3; i < argc; i++) {
				const char *fileName = argv[i];
				processFile(objectName, fileName, parseFunctionName);
			}
		}

		fputs("}\n\n", out);
	}

	return 0;
}
