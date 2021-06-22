// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

typedef unsigned char UTF8;    /* typically 8 bits */
typedef unsigned char Boolean; /* 0 or 1 */

Boolean isLegalUTF8Sequence(const UTF8 *source, const UTF8 *sourceEnd);
