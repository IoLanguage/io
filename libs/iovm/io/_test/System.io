/*
options := System getOptions(args) clone
options foreach(k, v,
	if(v type == List type,
		v foreach(i, j, writeln("Got unnamed argument with value: " .. j))
		continue
	)
	writeln("Got option: " .. k .. " with value: " .. v)
)
*/
