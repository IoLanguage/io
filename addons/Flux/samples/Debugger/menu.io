#!/usr/bin/env io

writebr := method(
    thisMessage arguments foreach(arg, write(sender doMessage(arg)))
    write("<br>")
)

write("Content-type: text/html\n")
write("Cache-Control: no-store, no-cache, must-revalidate, max-age=0\n\n"); 

form := CGI parse

Page := Object clone do(
    type := "Page"
    folder ::= Directory clone

    init := method(
	resend
	setFolder(folder clone)
    )

    path := method(folder path)
    setPath := method(p, folder setPath(p removePrefix("/")); self)
    name := method(
	n := folder fileNamed("_visible") asString
	if (n != "", return n)
	folder name
    )
    
    parentPage := method(
	f := folder parentDirectory
	if (f == nil, return nil)
	try(
	    p := self clone setFolder(f)
	    p items
	    return p
	) 
	nil
    )

    isAccessible := method(
	//f := folder fileNamed("index.html") 
	//f exists and (f isUserExecutable)
	try(
	    return folder fileNames contains("_visible")
	)
	nil
    )
    
    parents := method(
	//folder parents foreach(i, v, write(i, " : ", v path, "<br>"))
	folder parents add(folder) map(i, f, Page clone setFolder(f)) select(i, v, v isAccessible)
    )

    children := method(
	folder folders map(i, f, Page clone setFolder(f)) select(i, v, v isAccessible)
    )

    headerTemplate := """<html>
<head>
    <title>@title@</title>
    <link rel="stylesheet" href="/site.css">
    <META HTTP-EQUIV="EXPIRES" CONTENT=0>
</head>
<body>
<ul>
<br>"""
    
    headerEnd := """<table cellpadding=0 cellspacing=0 width="100%" border=0>
<tr>
<td colspan=5><img src="/Images/grayDot.png" height=1 width=100%></td>
</tr>
</table>"""

    pathTitle := method(
	b := Buffer clone
	parents := parents
	parents foreach(p, 
		b append(p name)
		if (p != parents last, b append(" - "))
	)
	b
    )
    
    displayHeader := method(
	h := headerTemplate asBuffer
	h replace("@folderName@", folder name)
	h replace("@title@", pathTitle)
	write(h)
	write(allMenus)
	write(headerEnd)
	write(editMenu)
    )
    
    dividerTemplate := """<span class=divider>|</span>"""
    
    menu := method(selectedPages,
	selectedPaths := selectedPages clone map(i, v, v urlPath)
	b := Buffer clone
	children := children
	children foreach(child, 
	    t := if(selectedPaths contains(child urlPath), child selectedLink, child unselectedLink)
	    b append(t)
	    if(child != children last, b append("", dividerTemplate, "\n"))
	)
	b
    )

    urlPath := method(
	p := path removePrefix(parents first path)
	if (p at(0) != "/" at(0), p := "/" .. p)
	p
    )
    
    link := method(
	"<a href=" .. urlPath .. "><span class=@class@>" .. name .. "</span></a>\n"
    )
    
    selectedLink   := method(link replace("@class@", "selectedMenu"))
    unselectedLink := method(link replace("@class@", "unselectedMenu"))
    
    allMenus := method(
	pp := parents clone
	b := Buffer clone
	size := pp count
	
	if (pp last children count == 0) then(
	    pp foreach(i, p,
		    b append(p selectedLink)
		    if (i > size - 3, break)
		    if (i < size - 1, b append("<span class=menuDividerDark> - </span>"))
	    )
	    b append("\n<br><br>\n\n", p menu(pp))
	    
	) else (
	    pp foreach(i, p,
		    b append(p selectedLink)
		    if (i < size - 1, b append("<span class=menuDividerDark> / </span>"))
	    )
	    m := p menu(pp)
	    if (m != "", b append("\n<br><br>\n\n", m))
	)
	b append("</ul>")
	b
    )

    allMenusOld := method(
	pp := parents clone
	
	b := Buffer clone
	b append(pp first selectedLink)

	pp foreach(i, p, 
	    m := p menu(pp)
	    if (m != "", b append("\n<br><br>\n\n", m))
	)
	b append("</ul>")
	b
    )

    urlForAction := method(action, "/menu.cgi?action=" .. action .. "Action&path=" .. path)
    
    
    indexFile := method(folder fileNamed("index.html"))

    editMenuStyle1 := """
<table cellpadding=3 cellspacing=1 border=0>
<tr>
<td width=99%> </a>
<td bgcolor=#efefef><a href=@editUrl@><font color=white>edit</a></td>
<td bgcolor=#efefef><a href=@removeLink@><font color=white>remove</a></td>
<td bgcolor=#efefef><a href=@addLink@><font color=white>add</a></td>
</tr>
</table>
"""
    
    editMenuStyle2 := """
<table cellpadding=3 cellspacing=1 border=0 width=100%>
<tr>
<td align=right>
<a href=@editUrl@><span class=wikiMenuText>edit</span></a>
<span class=wikiMenuDivider>|</span>
<a href=@removeUrl@><span class=wikiMenuText>remove</span></a>
<span class=wikiMenuDivider>|</span>
<a href=@addUrl@><span class=wikiMenuText>add</span></a>  
&nbsp; 
</td>
</tr>
</table>
    """
    
    editMenu := method(
	s := editMenuStyle2 asBuffer
	s replace("@editUrl@", urlForAction("edit"))
	s replace("@removeUrl@", urlForAction("remove"))
	s replace("@addUrl@", urlForAction("add"))
    )

    editAction := method(
        displayHeader
	t := """
<ul>
<form action="menu.cgi?action=updateAction&path=@path@" method="post">
<textarea name="content" cols="80" rows="30" wrap="off">@content@</textarea>
<br>
Password: <input type=PASSWORD name=password size=10>
<input type="submit" value="Save">
<br>
</form>
</ul>

</body>
</html>
    """ asBuffer
	t replace("@path@", path)
	t replace("@content@", indexFile asString)
	t replace("@indexFile@", indexFile path)
	write(t)
    )
    
    redirectTo := method(path,
	r := """<META HTTP-EQUIV="Refresh" CONTENT="0; URL=@path@?num=@num@">""" asBuffer
	r replace("@num@", Date now clone asString("%Y%m%d%H%M%S"))
	r replace("@path@", path)
	write(r)    
    )
    
    displayError := method(msg,
	displayHeader
	write("<ul><h1>")
	write(msg)
	write("</ul></h1>")
    )
    
    updateAction := method(
	if (form at("password") != "moo", displayError("wrong password"); return)
	//writebr("currentWorkingDirectory = ", File currentWorkingDirectory)
	//p := File currentWorkingDirectory appendPath(indexFile path)
	indexFile open truncateToSize(0) write(form at("content")) close
	redirectTo(path)
    )

    addAction := method(
	//folder createSubdirectoryNamed("
        displayHeader
	t := """
<ul>
<form action="menu.cgi?action=addNameAction&path=@path@" method="post">
<table cellspacing=1 cellpadding=3 border=0>
<tr>
<td align=right>Page name:</td><td><input name="name" size=20> </input></td>
</tr><tr>
<td align=right>Password:</td><td><input type=PASSWORD name=password size=14>
<input type="submit" value="save"></td>
</tr>
</table>
</form>
</ul>

</body>
</html>
    """ asBuffer
	t replace("@path@", path)
	t replace("@content@", indexFile asString)
	t replace("@indexFile@", indexFile path)
	write(t)
    )
    
    addNameAction := method(
	if (form at("password") != "mmm", displayError("wrong password"); return)
	folder folderNamedCreateIfAbsent(form at("name"))
	redirectTo(path)
    )
)

//writebr("path = ", form at("path"))
page := Page clone setPath(form at("path"))
action := form at("action")

if (action and action endsWith("Action"), page perform(action), page displayHeader)


