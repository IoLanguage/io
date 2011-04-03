var s = WScript.StdIn.ReadAll();
var re = new RegExp(WScript.Arguments(0), "gm");
var rep = WScript.Arguments(1);

WScript.StdOut.Write(s.replace(re, rep));