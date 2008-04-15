CGI

CGITest := UnitTest clone do(
	type := "CGITests"

	testDecodeEncode := method(
		enc_src := "Hello%2C+world%21...%0A%D0%97%D0%B4%D1%80%D0%B0%D0%B2%D1%81%D1%82%D0%B2%D1%83%D0%B9%2C+%D0%BC%D0%B8%D1%80%21%0A%E3%81%8A%E4%B8%96%E7%95%8C%E3%82%88%E3%80%82%E4%BB%8A%E6%97%A5%E3%81%AF%EF%BC%81"
		dec_src := "Hello, world!...\nЗдравствуй, мир!\nお世界よ。今日は！"

		// Along with testing encoder & decoder for coding conversion,
		// make sure they do not mutate their arguments
		enc := enc_src clone
		dec := dec_src clone
		assertEquals(enc, enc_src)
		assertEquals(dec, dec_src)

		// Test encode & decode for mutual compatibility
		assertTrue(CGI decodeUrlParam(CGI encodeUrlParam(dec)) == dec asUTF8)
		assertEquals(enc, enc_src)
		assertEquals(dec, dec_src)

		assertEquals(CGI encodeUrlParam(CGI decodeUrlParam(enc)), enc)
		assertEquals(enc, enc_src)
		assertEquals(dec, dec_src)

		// Test encode & decode for compatibility with sample outputs
		assertTrue(CGI decodeUrlParam(enc) == dec asUTF8)
		assertEquals(enc, enc_src)
		assertEquals(dec, dec_src)

		assertEquals(CGI encodeUrlParam(dec), enc)
		assertEquals(enc, enc_src)
		assertEquals(dec, dec_src)
	)
)
