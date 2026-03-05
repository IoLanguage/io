
// metadoc Common copyright Steve Dekorte 2002
// metadoc Common license BSD revised
/*metadoc Common description
You may need to add an entry for your C compiler.
*/

/*
Trick to get inlining to work with various compilers
Kudos to Daniel A. Koepke
*/

#undef IO_DECLARE_INLINES
#undef IOINLINE
#undef IOINLINE_RECURSIVE

#define IO_DECLARE_INLINES
#define IOINLINE static inline
#define IOINLINE_RECURSIVE static inline
