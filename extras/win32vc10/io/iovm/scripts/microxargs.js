var f = WScript.StdIn;

var shell = WScript.CreateObject("WScript.Shell");

var cmdString = ""
for(var i = 0; i < WScript.Arguments.count(); i++){ 
	if (WScript.Arguments(i).indexOf(" ") != -1)
	    cmdString = cmdString + "\"" + WScript.Arguments(i) + "\" ";
	else
	    cmdString = cmdString + "" + WScript.Arguments(i) + " ";
}

while (!f.AtEndOfStream)
{
	s = f.ReadLine();
/*
	if (s.indexOf(" ") != -1)
		cmd = (cmdString + " \"" + s + "\"");
	else
*/
	cmd = cmdString + " " + s;

	var exec = shell.Exec(cmd);
	WScript.StdOut.Write(exec.StdOut.ReadAll());
}

