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
		/*
		Tm := Structure clone do(
			appendProto(Types)
			
			appendElement("second", Int)
			appendElement("minute", Int)
			appendElement("hour", Int)
			appendElement("date", Int)
			appendElement("month", Int)
			appendElement("year", Int)
			appendElement("dayOfWeek", Int)
			appendElement("dayOfYear", Int)
			appendElement("isDST", Int)
			appendElement("gmtOffset", Long)
			appendElement("timezone", CString)
		)
		*/
	)
	
	_functions := method(
		_functions = Map clone do(
			appendProto(LibC Types)
	
			lib := Library clone setName("libSystem.dylib")
			
			add := method(name, retType, argTypeList,
				atPut(name, Function clone setLibrary(lib) setName(name) setReturnType(retType) setArgumentTypes(argTypeList)) 
			)
	
			add("strlen", Long, list(CString))
			add("strchr", CString, list(CString, Char))
			add("time", Time_t, list(Time_t ptr))
			//add("localtime", Tm ptr, list(Time_t ptr))
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

t1 := LibC malloc(Size_t clone setValue(15))
t1 setTypeString("^l")
t1 pointedToType = Long

t2 := LibC time(t1)

List clone do(
	append(t1 value == t2); LibC free(t1)
	append(LibC strlen("12345") == 5)
	append(LibC strchr("^^1^^2^^3", Char clone setValue("2")) == "2^^3")
) contains(false) ifTrue(
	"failure" println
) ifFalse(
	"success" println
)
