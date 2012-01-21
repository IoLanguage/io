//metadoc TagLib copyright Steve Dekorte, 2004
//metadoc TagLib license BSD revised
//metadoc TagLib category Audio
/*metadoc TagLib description
Used to set meta data tags on the following audio formats:
<pre>
ape
flac
mp3
mpc
mpeg
ogg
</pre>
Supports reading and writing the following attributes:
<pre>title
artist
album
year
track
genre 
</pre>
And reading the attributes:
<pre>
bitRate
sampleRate
channels
length
</pre>

Example use (load and modify a track genre):

<pre>
t := TagLib clone setPath("foo.mp3") load
writeln("genre = ", t genre)
t setGenre("ambient")
t save
</pre>
*/

//doc TagLib setPath(aSeq) Sets the path to the file.
//doc TagLib path Returns the path to the file.

#include "IoTagLib.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <taglib/tag_c.h>

#define DATA(self) ((IoTagLibData *)IoObject_dataPointer(self))
static const char *protoId = "TagLib";

IoTag *IoTagLib_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TagLib");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTagLib_rawClone);
	return tag;
}

IoTagLib *IoTagLib_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoTagLib_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"load", IoTagLib_load},
		{"save", IoTagLib_save},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoTagLib *IoTagLib_rawClone(IoTagLib *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	return self;
}

IoTagLib *IoTagLib_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

// -----------------------------------------------------------


IoObject *IoTagLib_load(IoTagLib *self, IoObject *locals, IoMessage *m)
{
	/*doc TagLib load
	Loads tag data from the file specified in the path slot. Returns self.
	*/

	TagLib_File *file;
	TagLib_Tag *tag;
	const TagLib_AudioProperties *properties;
	IoSeq *path = IoObject_symbolGetSlot_(self, IOSYMBOL("path"));
	IOASSERT(path, "missing path slot");

	taglib_set_strings_unicode(0);

	file = taglib_file_new(CSTRING(path));

	IOASSERT(file, "unable to open file");

	tag = (TagLib_Tag *)taglib_file_tag(file);
	properties = taglib_file_audioproperties(file);

	IoObject_setSlot_to_(self, IOSYMBOL("title"), IOSYMBOL(taglib_tag_title(tag)));
	IoObject_setSlot_to_(self, IOSYMBOL("artist"), IOSYMBOL(taglib_tag_artist(tag)));
	IoObject_setSlot_to_(self, IOSYMBOL("album"), IOSYMBOL(taglib_tag_album(tag)));
	IoObject_setSlot_to_(self, IOSYMBOL("year"), IONUMBER(taglib_tag_year(tag)));
	IoObject_setSlot_to_(self, IOSYMBOL("comment"), IOSYMBOL(taglib_tag_comment(tag)));
	IoObject_setSlot_to_(self, IOSYMBOL("track"), IONUMBER(taglib_tag_track(tag)));
	IoObject_setSlot_to_(self, IOSYMBOL("genre"), IOSYMBOL(taglib_tag_genre(tag)));

	// audio data - can't write this - it's up to the mp3 encoder
	IoObject_setSlot_to_(self, IOSYMBOL("bitRate"), IONUMBER(taglib_audioproperties_bitrate(properties)));
	IoObject_setSlot_to_(self, IOSYMBOL("sampleRate"), IONUMBER(taglib_audioproperties_samplerate(properties)));
	IoObject_setSlot_to_(self, IOSYMBOL("channels"), IONUMBER(taglib_audioproperties_channels(properties)));
	IoObject_setSlot_to_(self, IOSYMBOL("length"), IONUMBER(taglib_audioproperties_length(properties)));

	taglib_tag_free_strings();
	taglib_file_free(file);

	return self;
}

IoObject *IoTagLib_save(IoTagLib *self, IoObject *locals, IoMessage *m)
{
	/*doc TagLib save
	Saves the tag settings and returns self.
	*/

	TagLib_File *file;
	TagLib_Tag *tag;
	//const TagLib_AudioProperties *properties;
	IoSeq *path = IoObject_symbolGetSlot_(self, IOSYMBOL("path"));
	IOASSERT(path, "missing path slot");

	taglib_set_strings_unicode(0);

	file = taglib_file_new(CSTRING(path));

	IOASSERT(file, "unable to open file");

	tag = (TagLib_Tag *)taglib_file_tag(file);

	taglib_tag_set_title(tag, CSTRING(IoObject_seqGetSlot_(self, IOSYMBOL("title"))));
	taglib_tag_set_album(tag, CSTRING(IoObject_seqGetSlot_(self, IOSYMBOL("album"))));
	taglib_tag_set_year(tag, (int)IoObject_doubleGetSlot_(self, IOSYMBOL("year")));
	taglib_tag_set_comment(tag, CSTRING(IoObject_seqGetSlot_(self, IOSYMBOL("comment"))));
	taglib_tag_set_track(tag, (int)IoObject_doubleGetSlot_(self, IOSYMBOL("track")));
	taglib_tag_set_genre(tag, CSTRING(IoObject_seqGetSlot_(self, IOSYMBOL("genre"))));

	taglib_file_save(file);

	taglib_tag_free_strings();
	taglib_file_free(file);

	return self;

}

