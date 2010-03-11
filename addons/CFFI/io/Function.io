/* CFFI - An Io interface to C
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/

CFFI Function do(
	library ::= Library
	name ::= ""
	returnType ::= ""
	argumentTypes ::= ""

	setTypeString("&")

	with := method(
		args := call message argsEvaluatedIn(call sender)
		f := self clone setReturnType(args at(0)) setArgumentTypes(args exSlice(1, args size))
		f
	)
)
