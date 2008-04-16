AlignTest := UnitTest clone do(
	testAlignNoTruncation := method(
		assertEquals("abc", "abc" alignLeft(0))
		assertEquals("abc", "abc" alignCenter(0))
		assertEquals("abc", "abc" alignRight(0))
	)

	testAlignNoTruncationOnNegative := method(
		assertEquals("abc", "abc" alignLeft(-10))
		assertEquals("abc", "abc" alignCenter(-10))
		assertEquals("abc", "abc" alignRight(-10))
	)

	testAlign := method(
		assertEquals("abc       ", "abc" alignLeft(10))
		assertEquals("   abc    ", "abc" alignCenter(10))
		assertEquals("       abc", "abc" alignRight(10))
	)

	testAlignPad := method(
		assertEquals("abc-------", "abc" alignLeft(10, "-"))
		assertEquals("---abc----", "abc" alignCenter(10, "-"))
		assertEquals("-------abc", "abc" alignRight(10, "-"))
	)

	testAlignPadSeq := method(
		assertEquals("[abc]12312", "[abc]" alignLeft(10, "123"))
		assertEquals("12[abc]123", "[abc]" alignCenter(10, "123"))
		assertEquals("12312[abc]", "[abc]" alignRight(10, "123"))
	)

	testEmptyPaddingString := method(
		assertEquals("  [abc]  ", "[abc]" alignCenter(9, ""))
	)
)
