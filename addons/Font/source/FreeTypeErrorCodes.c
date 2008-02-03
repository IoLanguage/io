//metadoc FreeTypeErrorCodes copyright Steve Dekorte 2002

const char *FreeTypeErrorStringForCode(int code)
{
	switch (code)
	{
		case 0x00: return "no error";

		case 0x01: return "cannot open resource";
		case 0x02: return "unknown file format";
		case 0x03: return "broken file";
		case 0x04: return "invalid FreeType version";
		case 0x05: return "module version is too low";
		case 0x06: return "invalid argument";
		case 0x07: return "unimplemented feature";
		case 0x08: return "broken table";
		case 0x09: return "broken offset within table";

		/* glyph/character errors */

		case 0x10: return "invalid glyph index";
		case 0x11: return "invalid character code";
		case 0x12: return "unsupported glyph image format";
		case 0x13: return "cannot render this glyph format";
		case 0x14: return "invalid outline";
		case 0x15: return "invalid composite glyph";
		case 0x16: return "too many hints";
		case 0x17: return "invalid pixel size";

		/* handle errors */

		case 0x20: return "invalid object handle";
		case 0x21: return "invalid library handle";
		case 0x22: return "invalid module handle";
		case 0x23: return "invalid face handle";
		case 0x24: return "invalid size handle";
		case 0x25: return "invalid glyph slot handle";
		case 0x26: return "invalid charmap handle";
		case 0x27: return "invalid cache manager handle";
		case 0x28: return "invalid stream handle";

		/* driver errors */

		case 0x30: return "too many modules";
		case 0x31: return "too many extensions";

		/* memory errors */

		case 0x40: return "out of memory";
		case 0x41: return "unlisted object";

		/* stream errors */

		case 0x51: return "cannot open stream";
		case 0x52: return "invalid stream seek";
		case 0x53: return "invalid stream skip";
		case 0x54: return "invalid stream read";
		case 0x55: return "invalid stream operation";
		case 0x56: return "invalid frame operation";
		case 0x57: return "nested frame access";
		case 0x58: return "invalid frame read";

		/* raster errors */

		case 0x60: return "raster uninitialized";
		case 0x61: return "raster corrupted";
		case 0x62: return "raster overflow";
		case 0x63: return "negative height while rastering";

		/* cache errors */

		case 0x70: return "too many registered caches";

		/* TrueType and SFNT errors */

		case 0x80: return "invalid opcode";
		case 0x81: return "too few arguments";
		case 0x82: return "stack overflow";
		case 0x83: return "code overflow";
		case 0x84: return "bad argument";
		case 0x85: return "division by zero";
		case 0x86: return "invalid reference";
		case 0x87: return "found debug opcode";
		case 0x88: return "found ENDF opcode in execution stream";
		case 0x89: return "nested DEFS";
		case 0x8A: return "invalid code range";
		case 0x8B: return "execution context too long";
		case 0x8C: return "too many function definitions";
		case 0x8D: return "too many instruction definitions";
		case 0x8E: return "SFNT font table missing";
		case 0x8F: return "horizontal header (hhea) table missing";
		case 0x90: return "locations (loca) table missing";
		case 0x91: return "name table missing";
		case 0x92: return "character map (cmap) table missing";
		case 0x93: return "horizontal metrics (hmtx) table missing";
		case 0x94: return "PostScript (post) table missing";
		case 0x95: return "invalid horizontal metrics";
		case 0x96: return "invalid character map (cmap) format";
		case 0x97: return "invalid ppem value";
		case 0x98: return "invalid vertical metrics";
		case 0x99: return "could not find context";
		case 0x9A: return "invalid PostScript (post) table format";
		case 0x9B: return "invalid PostScript (post) table";

		/* CFF, CID, and Type 1 errors */

		case 0xA0: return "opcode syntax error";
		case 0xA1: return "argument stack underflow";

		/* BDF errors */

		case 0xB0: return "`STARTFONT' field missing";
		case 0xB1: return "`FONT' field missing";
		case 0xB2: return "`SIZE' field missing";
		case 0xB3: return "`CHARS' field missing";
		case 0xB4: return "`STARTCHAR' field missing";
		case 0xB5: return "`ENCODING' field missing";
		case 0xB6: return "`BBX' field missing";
	}
	return "unknown error code";
}


