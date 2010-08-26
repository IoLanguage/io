#!/usr/bin/env io

/* CFFI - A C function interface for Io
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/

CFFI

LibC := Object clone do(
	Types := Object clone do(
		appendProto(CFFI Types)
		
		Size_t := Long
		Time_t := Long
		
		Tm := Structure with(list("second", Types Int),
					list("minute", Types Int),
					list("hour", Types Int),
					list("day", Types Int),
					list("month", Types Int),
					list("year", Types Int),
					list("dayOfWeek", Types Int),
					list("dayOfYear", Types Int),
					list("isDST", Types Int),
					list("gmtOffset", Types Long),
					list("timezone", Types CString)
		)
		
	)
	
	_functions := method(
		_functions = Map clone do(
			appendProto(LibC Types)
			if(System platform asLowercase containsSeq("windows"),
				lib := Library clone setName("msvcrt.dll")
			,	
				lib := Library clone setName("libc.so.6")
			)
			
			add := method(name, retType, argTypeList,
				atPut(name, Function clone setLibrary(lib) setName(name) setReturnType(retType) setArgumentTypes(argTypeList)) 
			)
	
			add("strlen", Long, list(CString))
			add("strchr", CString, list(CString, Char))
			add("time", Time_t, list(Time_t ptr))
			add("localtime", Tm ptr, list(Time_t ptr))
			add("malloc", Void ptr, list(Size_t))
			add("free", Void, list(Void ptr))
		)
	)
	
	forward := method(
		m := Message clone setName("call") setArguments(call message arguments)
		m doInContext(_functions at(call message name))
	)
)

//need to be able to set address on Pointer so malloc will work
appendProto(LibC Types)

t1 := LibC malloc(Size_t clone setValue(15)) cast(Types Long ptr)

t2 := LibC time(t1)
tt := Types Time_t clone setValue(t2)

tm := LibC localtime(tt ptr)
t := tm value
date := Date now

List clone do(
	append(t1 value == t2); LibC free(t1)
	append(LibC strlen(CString clone setValue("12345")) == 5)
	append(LibC strchr(CString clone setValue("^^1^^2^^3"), Char clone setValue("2")) == "2^^3")
	append(date day == t day value and 
		date month == (t month value + 1) and 
		date year == (t year value + 1900) )
) contains(false) ifTrue(
	"failure" println
) ifFalse(
	"success" println
)
