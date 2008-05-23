/*
foo := Object clone do(value := 0; increment := method(v, value = value + v))
foo acceptedMessageNames := list("increment")

MDOServer clone setHost("127.0.0.1") setPort(8000) setLocalObject(foo) start
*/
