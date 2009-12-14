#!/usr/bin/env io

// unfinished

GoogleTranslate := Object clone do(
    translate := method(text, lang,
		u := URL with("http://translate.google.com/translate_t")
		m := Map clone atPut("text", text)  atPut("lang", lang)
		u post(m)
	)
)

gs := GoogleTranslate clone 
gs translate("yes")




