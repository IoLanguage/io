TwitterAccountProfile := Object clone do(
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
		account request asUpdateAccountProfile\
			setName(name)\
			setUrl(url)\
			setLocation(location)\
			setDescription(description)\
			resultsOrError returnIfError
		
		self
	)
	
	updateColors := method(
		account request asUpdateAccountProfileColors\
			setProfileBackgroundColor(backgroundColor)\
			setProfileTextColor(textColor)\
			setProfileLinkColor(linkColor)\
			setProfileSidebarFillColor(sidebarFillColor)\
			setProfileSidebarBorderColor(sidebarBorderColor)\
			resultsOrError returnIfError
		
		self
	)
	
	updateImage := method(
		account request asUpdateAccountProfileImage\
			setImage(imageFile)\
			resultsOrError returnIfError
		self
	)
	
	updateBackgroundImage := method(
		account request asUpdateAccountProfileBackgroundImage\
			setImage(backgroundImageFile)\
			setTile(tilesBackgroundImage)\
			resultsOrError returnIfError
		self
	)
)