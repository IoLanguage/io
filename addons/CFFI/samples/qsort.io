
CFFI

appendProto(Types)

AI := Array with(Types Int, 5)
Size_t := Long

compare := block(a, b,
		("compare " .. a value .. " " .. b value) println
		return a value - b value
		)


CmpFunc := Function with(Int, Int ptr, Int ptr)
if(System platform asLowercase containsSeq("windows"),
	lib := Library clone setName("msvcrt.dll")
,
	lib := Library clone setName("libc.so.6")
)
qsort := Function with(Void, AI ptr, Size_t, Size_t, CmpFunc) setLibrary(lib) setName("qsort")

cf := CmpFunc clone setCallback(compare)
a := AI with(5, 1, 99, 7, 33)

qsort call(a ptr, Size_t with(a size / a arrayType size), Size_t with(Int size), cf)

for(i, 0, (a size / a arrayType size) - 1, a at(i) println)

