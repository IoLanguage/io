#!/usr/bin/env io

//debugCollector := 1

//write("launchPath = '", launchPath, "'\n")
//Importer debug := 1
Importer addSearchPath(Path with(launchPath, "../../Interface/Ion"))
Importer addSearchPath(Path with(launchPath, "Interface/Ion"))

Importer addSearchPath(Path with(launchPath, "../../Interface/Ion/Widgets"))
Importer addSearchPath(Path with(launchPath, "Interface/Ion/Widgets"))

ResourceManager

FontManager addPath(Path with(launchPath, "Library/Fonts"))
FontManager addPath(Path with(launchPath, "../../Library/Fonts"))

ImageManager addPath(Path with(launchPath, "Library/Themes/Neos"))
ImageManager addPath(Path with(launchPath, "../../Library/Themes/Neos"))

Button textColor = Color clone set(1,1,1,1)

Lobby doFile(Path with(launchPath, "IRC.io"))



