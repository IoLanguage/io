
SongJamNode := Object clone do(

    protocol := Object clone do(
	getkey  := "http://www.songjam.com/sjremote.php?isiteloginpassword=[password]&isiteloginusername=[username]&page=getsessionkey"
	getdir  := "http://www.songjam.com/sjremote.php?issh=[sessionkey]&page=sjlistdir&path=[path]"
	getfile := "http://www.songjam.com/sjremote.php?issh=[sessionkey]&page=sjmemberdownload&file=[path]"
    )

    username ::= "xxx"
    password ::= "yyy"
        
    urlNamed := method(name, 
	url := protocol getSlot(name) asMutable
	url replaceSeq("[username]", username urlEncoded)
	url replaceSeq("[password]", password urlEncoded)
	url replaceSeq("[path]", path)
	url replaceSeq(" ", "%20")
	url asSymbol
    )

    login := method(
	url := urlNamed("getkey")
	write("url = ", url, "\n")
	key := URL clone setURL(url) fetch 
	if (key == nil, Exception raise("URL", url .. " fetch failed"))
	key removeLastByte removeLastByte asString
	if (key contains("bad login") or key contains("invalid"), sessionKey = nil, sessionKey = key)
	isLoggedIn
    )
    
    isLoggedIn := method(sessionKey)
        
    sessionKey := nil
    completed  := nil
    
    path ::= "/"
    
    setPath := method(p,
	if (p == "", p = "/")
	path = p
	write("setPath('", p, "' ", p type, ")\n")
	self
    )
    
    subnodeNames := method(
	url := urlNamed("getdir") asMutable
	url replaceSeq("[sessionkey]", sessionKey)
	url replaceSeq("[path]", path)
	url replaceSeq(" ", "%20") asSymbol
	dir := URL clone setURL(url) fetch asString 
	//write("[", dir, "]\n")
	dir splitNoEmpties("\n", "\r") select(i, v, v beginsWithSeq(".") == nil)
    )
    
    subnodes := method(
      write("fetching nodes for ['", path, "' ", path type, "]\n")
      write("hasSubitems == [", hasSubitems type, "]\n")
      if (hasSubitems == nil, return List clone)
      write("subnodeNames size = [", subnodeNames size , "]\n")
      //subnodeNames translate(i, name, 
      subnodeNames mapInPlace(i, name, 
        SongJamNode clone setPath(Path with(path, name))
      )
    )
    
    subitems := method(
      subitems = subnodes
    )
    
    data := method(
	url := urlNamed("getfile")
	write("url := '", url, "\n")
	value := URL clone setURL(url) fetch
        return value
    )

    dataWithProgress := method( progressBlock,
	url := urlNamed("getfile")
	write("url := '", url, "\n")
	value := URL clone setURL(url) fetchWithProgress( getSlot( "progressBlock" ) )
        return value
    )
    
    // Browser protocol
    title := method(path lastPathComponent)
    hasSubitems := method(path contains(".wav") == nil)
)

SongJamNode sessionKey
//write("sessionKey = [", SongJamNode sessionKey, "]\n")

/*
SongJamNode clone subitems last subitems last subitems at(3) subitems foreach(n, write("'", n path, "' \n"))
item := SongJamNode clone subitems last subitems last subitems at(3) subitems last 
File clone setPath(item path lastPathComponent) open write(item data) close
*/

