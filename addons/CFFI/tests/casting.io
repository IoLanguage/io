CFFI

appendProto(Types)

"------------" println
i:=Int with(0x30323436)
ip:=i ptr
cp:=ip castTo(Char ptr)
sp:=cp castTo(CString)

i value println
sp value println

vp:=ip castTo(Void ptr)
sp:=vp castTo(CString)
sp value println

"------------" println
S:=Structure with(list("x", Int))
T:=Structure with(list("a", Char),
			list("b", Char),
			list("c", Char),
			list("d", Char))
s:=S clone setValues(0x31323334)
vp:=s ptr castTo(Void ptr)
vp address println
svp:=vp castTo(S ptr)
svp value x value println

tp:=vp castTo(T ptr)
tp value a value println
tp value b value println
tp value c value println
tp value d value println

tp:=svp castTo(T ptr)
tp value a value println
tp value b value println
tp value c value println
tp value d value println

str:=tp castTo(CString)
str value println

charp:=tp castTo(Char ptr)
charp at(0) println
charp at(1) println
charp at(2) println
charp at(3) println
charp asBuffer(4) println

str:=charp castTo(CString)
str value println
