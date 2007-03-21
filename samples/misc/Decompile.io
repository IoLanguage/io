
// evaluating a string

"1 + 2 := " print
"1 + 2 " doString print
"\n" print


// test of decompiling a block

Dog := Object clone
Dog bark := method("woof!" print)

"decompiled block: " print
Dog get("bark") code print
"\n" print
