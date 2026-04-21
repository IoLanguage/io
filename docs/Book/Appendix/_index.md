# Appendix

Grammar reference and citations.

## Grammar

#### messages

expression ::= { message | sctpad }

message ::= [wcpad] symbol [scpad] [arguments]

arguments ::= Open [argument [ { Comma argument } ]] Close

argument ::= [wcpad] expression [wcpad]



#### symbols

symbol ::= Identifier | number | Opereator | quote

Identifier ::= { letter | digit | "_" }

Operator ::= { ":" | "." | "'" | "~" | "!" | "@" | "$" |

"%" | "^" | "&" | "*" | "-" | "+" | "/" | "=" | "{" | "}" |

"[" | "]" | "|" | "\" | "<" | ">" | "?" }



#### quotes

quote ::= MonoQuote | TriQuote

MonoQuote ::= """ [ "\"" | not(""")] """

TriQuote ::= """"" [ not(""""")] """""



#### spans

Terminator ::= { [separator] ";" | "\n" | "\r" [separator] }

separator ::= { " " | "\f" | "\t" | "\v" }

whitespace ::= { " " | "\f" | "\r" | "\t" | "\v" | "\n" }

sctpad ::= { separator | Comment | Terminator }

scpad ::= { separator | Comment }

wcpad ::= { whitespace | Comment }


#### comments

Comment ::= slashStarComment | slashSlashComment | poundComment

slashStarComment ::= "/*" [not("*/")] "*/"

slashSlashComment ::= "//" [not("\n")] "\n"

poundComment ::= "#" [not("\n")] "\n"



#### numbers

number ::= HexNumber | Decimal

HexNumber ::= "0" anyCase("x") { [ digit | hexLetter ] }

hexLetter ::= "a" | "b" | "c" | "d" | "e" | "f"

Decimal ::= digits | "." digits |

digits "." digits ["e" [-] digits]



#### characters

Comma ::= ","

Open ::= "(" | "[" | "{"

Close ::= ")" | "]" | "}"

letter ::= "a" ... "z" | "A" ... "Z"

digit ::= "0" ... "9"

digits ::= { digit }



Uppercase words designate elements the lexer treats as tokens.




## References

1. Goldberg, A et al. *Smalltalk-80: The Language and Its Implementation.* Addison-Wesley, 1983.
2. Ungar, D and Smith, RB. *Self: The Power of Simplicity.* OOPSLA, 1987.
3. Smith, W. *Class-based NewtonScript Programming.* PIE Developers magazine, Jan 1994.
4. Lieberman, H. *Concurrent Object-Oriented Programming in Act 1.* MIT AI Lab, 1987.
5. McCarthy, J et al. *LISP I programmer's manual.* MIT Press, 1960.
6. Ierusalimschy, R, et al. *Lua: an extensible extension language.*
