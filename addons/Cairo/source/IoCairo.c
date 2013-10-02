//metadoc Cairo copyright Daniel Rosengren, 2007
//metadoc Cairo license BSD revised
//metadoc Cairo category Graphics
/*metadoc Cairo description 
Cairo is a 2D graphics library. http://cairographics.org/
*/
#include "IoCairo.h"
#include "IoNumber.h"
#include <cairo-svg.h>

static const char *protoId = "Cairo";
static IoTag *IoCairo_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairo_rawClone);
	return tag;
}

IoCairo *IoCairo_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairo_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
			{"version", IoCairo_version},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	#define CONSTANT(name) IoObject_setSlot_to_(self, IOSYMBOL(#name), IONUMBER(CAIRO_ ## name));
	#define FLAG(name, value) IoObject_setSlot_to_(self, IOSYMBOL(#name), IOBOOL(self, value))

	CONSTANT(ANTIALIAS_DEFAULT);
	CONSTANT(ANTIALIAS_NONE);
	CONSTANT(ANTIALIAS_GRAY);
	CONSTANT(ANTIALIAS_SUBPIXEL);

	CONSTANT(CONTENT_COLOR);
	CONSTANT(CONTENT_ALPHA);
	CONSTANT(CONTENT_COLOR_ALPHA);

	CONSTANT(EXTEND_NONE);
	CONSTANT(EXTEND_REPEAT);
	CONSTANT(EXTEND_REFLECT);

	CONSTANT(FILL_RULE_WINDING);
	CONSTANT(FILL_RULE_EVEN_ODD);

	CONSTANT(FILTER_FAST);
	CONSTANT(FILTER_GOOD);
	CONSTANT(FILTER_BEST);
	CONSTANT(FILTER_NEAREST);
	CONSTANT(FILTER_BILINEAR);
	CONSTANT(FILTER_GAUSSIAN);

	CONSTANT(FONT_WEIGHT_NORMAL);
	CONSTANT(FONT_WEIGHT_BOLD);

	CONSTANT(FONT_SLANT_NORMAL);
	CONSTANT(FONT_SLANT_ITALIC);
	CONSTANT(FONT_SLANT_OBLIQUE);

	CONSTANT(FORMAT_ARGB32);
	CONSTANT(FORMAT_RGB24);
	CONSTANT(FORMAT_A8);
	CONSTANT(FORMAT_A1);
	CONSTANT(FORMAT_RGB16_565);

	CONSTANT(HINT_METRICS_DEFAULT);
	CONSTANT(HINT_METRICS_OFF);
	CONSTANT(HINT_METRICS_ON);

	CONSTANT(HINT_STYLE_DEFAULT);
	CONSTANT(HINT_STYLE_NONE);
	CONSTANT(HINT_STYLE_SLIGHT);
	CONSTANT(HINT_STYLE_MEDIUM);
	CONSTANT(HINT_STYLE_FULL);

	CONSTANT(LINE_CAP_BUTT);
	CONSTANT(LINE_CAP_ROUND);
	CONSTANT(LINE_CAP_SQUARE);

	CONSTANT(LINE_JOIN_MITER);
	CONSTANT(LINE_JOIN_ROUND);
	CONSTANT(LINE_JOIN_BEVEL);

	CONSTANT(OPERATOR_CLEAR);
	CONSTANT(OPERATOR_SOURCE);
	CONSTANT(OPERATOR_OVER);
	CONSTANT(OPERATOR_IN);
	CONSTANT(OPERATOR_OUT);
	CONSTANT(OPERATOR_ATOP);
	CONSTANT(OPERATOR_DEST);
	CONSTANT(OPERATOR_DEST_OVER);
	CONSTANT(OPERATOR_DEST_IN);
	CONSTANT(OPERATOR_DEST_OUT);
	CONSTANT(OPERATOR_DEST_ATOP);
	CONSTANT(OPERATOR_XOR);
	CONSTANT(OPERATOR_ADD);
	CONSTANT(OPERATOR_SATURATE);

	CONSTANT(PATH_MOVE_TO);
	CONSTANT(PATH_LINE_TO);
	CONSTANT(PATH_CURVE_TO);
	CONSTANT(PATH_CLOSE_PATH);

	CONSTANT(SUBPIXEL_ORDER_DEFAULT);
	CONSTANT(SUBPIXEL_ORDER_RGB);
	CONSTANT(SUBPIXEL_ORDER_BGR);
	CONSTANT(SUBPIXEL_ORDER_VRGB);
	CONSTANT(SUBPIXEL_ORDER_VBGR);

	#if CAIRO_HAS_PNG_FUNCTIONS
	FLAG(HAS_PNG_FUNCTIONS, 1);
	#else
	FLAG(HAS_PNG_FUNCTIONS, 0);
	#endif

	#if CAIRO_HAS_PDF_SURFACE
	FLAG(HAS_PDF_SURFACE, 1);
	#else
	FLAG(HAS_PDF_SURFACE, 0);
	#endif

	#if CAIRO_HAS_PS_SURFACE
	FLAG(HAS_PS_SURFACE, 1);
	#else
	FLAG(HAS_PS_SURFACE, 0);
	#endif

	#if CAIRO_HAS_SVG_SURFACE
	FLAG(HAS_SVG_SURFACE, 1);
	CONSTANT(SVG_VERSION_1_1);
	CONSTANT(SVG_VERSION_1_2);
	#else
	FLAG(HAS_SVG_SURFACE, 0);
	#endif

	#undef CONSTANT
	#undef FLAG

	return self;
}

IoCairo *IoCairo_rawClone(IoCairo *proto)
{
	/* Cairo is a singleton */
	return proto;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairo_version(IoCairo *self, IoObject *locals, IoMessage *m)
{
	return IOSYMBOL(cairo_version_string());
}
