TwitterAccountProfile := Object clone do(
	//metadoc TwitterAccountProfile category Networking
	account ::= nil
	
	name ::= nil
	url ::= nil
	location ::= nil
	description ::= nil
	
	backgroundColor ::= nil
	textColor ::= nil
	linkColor ::= nil
	sidebarFillColor ::= nil
	sidebarBorderColor ::= nil
	
	imageFile ::= nil
	backgroundImageFile ::= nil
	tilesBackgroundImage ::= nil
	
	update := method(
		account resultsFor(account request asUpdateAccountProfile\
			setName(name)\
			setUrl(url)\
			setLocation(location)\
			setDescription(description))
		self
	)
	
	updateColors := method(
		account resultsFor(account request asUpdateAccountProfileColors\
			setProfileBackgroundColor(backgroundColor)\
			setProfileTextColor(textColor)\
			setProfileLinkColor(linkColor)\
			setProfileSidebarFillColor(sidebarFillColor)\
			setProfileSidebarBorderColor(sidebarBorderColor))
		self
	)
	
	updateImage := method(
		account resultsFor(account request asUpdateAccountProfileImage setImage(imageFile))
		self
	)
	
	updateBackgroundImage := method(
		account resultsFor(account request asUpdateAccountProfileBackgroundImage\
			setImage(backgroundImageFile)\
			setTile(tilesBackgroundImage))
		self
	)
)