TwitterAccountProfile := Object clone do(
//metadoc TwitterAccountProfile category Networking
//metadoc TwitterAccountProfile description Object representing a twitter account profile. 

	account ::= nil
	//doc TwitterAccount account Returns account associated with this profile.
	//doc TwitterAccount setAccount(anAccount) Sets the account associated with this profile. Returns self.
	
	name ::= nil
	//doc TwitterAccount name Returns the name associated with the profile.
	//doc TwitterAccount setName(aSeq) Sets the name associated with the profile. Returns self.

	url ::= nil
	//doc TwitterAccount url Returns the url to the twitter profile.

	location ::= nil
	//doc TwitterAccount location Returns the geographical location set in the twitter profile.
	
	description ::= nil
	//doc TwitterAccount description Returns the description set in the twitter profile.
	
	backgroundColor ::= nil
	//doc TwitterAccount backgroundColor Returns the backgroundColor set in the twitter profile.
	
	textColor ::= nil
	//doc TwitterAccount textColor Returns the textColor set in the twitter profile.
	
	linkColor ::= nil
	//doc TwitterAccount linkColor Returns the linkColor set in the twitter profile.
	
	sidebarFillColor ::= nil
	//doc TwitterAccount sidebarFillColor Returns the sidebarFillColor set in the twitter profile.
	
	sidebarBorderColor ::= nil
	//doc TwitterAccount sidebarBorderColor Returns the sidebarBorderColor set in the twitter profile.
	
	imageFile ::= nil
	//doc TwitterAccount imageFile Returns the imageFile set in the twitter profile.

	backgroundImageFile ::= nil
	//doc TwitterAccount backgroundImageFile Returns the backgroundImageFile set in the twitter profile.

	tilesBackgroundImage ::= nil
	//doc TwitterAccount tilesBackgroundImage Returns the tilesBackgroundImage set in the twitter profile.
	
	update := method(
		//doc TwitterAccount update Fetch the latest basic profile settings from twitter.
		account resultsFor(account request asUpdateAccountProfile\
			setName(name)\
			setUrl(url)\
			setLocation(location)\
			setDescription(description))
		self
	)
	
	updateColors := method(
		//doc TwitterAccount updateColors Fetch the latest profile color settings from twitter.
		account resultsFor(account request asUpdateAccountProfileColors\
			setProfileBackgroundColor(backgroundColor)\
			setProfileTextColor(textColor)\
			setProfileLinkColor(linkColor)\
			setProfileSidebarFillColor(sidebarFillColor)\
			setProfileSidebarBorderColor(sidebarBorderColor))
		self
	)
	
	updateImage := method(
		//doc TwitterAccount updateImage Fetch the latest image settings from twitter.
		account resultsFor(account request asUpdateAccountProfileImage setImage(imageFile))
		self
	)
	
	updateBackgroundImage := method(
		//doc TwitterAccount updateBackgroundImage Fetch the latest background image settings from twitter.
		account resultsFor(account request asUpdateAccountProfileBackgroundImage\
			setImage(backgroundImageFile)\
			setTile(tilesBackgroundImage))
		self
	)
)