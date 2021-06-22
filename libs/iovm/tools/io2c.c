/*
Copyright (c) 2005, Steve Dekorte
All rights reserved. See _License.txt.
*/

#include <stdio.h>
#include <stdlib.h>

void showUsage(void) {
    printf("usage: io2c ObjectName ParseFunctionName ioFile1 ioFile2 ...\n");
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
    // fprintf(out, "\t%s(self, context, s, \"%s\");\n\n", parseFunctionName,
    // fileName); fprintf(out, "\tIoState_rawOn_doCString_withLabel_(self,
    // context, \"writeln(\\\"%s\\\")\", \"%s\");\n\n", fileName, fileName);
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
        int i;
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

        for (i = 3; i < argc; i++) {
            const char *fileName = argv[i];
            processFile(objectName, fileName, parseFunctionName);
        }

        fputs("}\n\n", out);
    }

    return 0;
}
