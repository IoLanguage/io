#!/usr/bin/env io

Flux
//debugCollector := 1

//write("System launchPath = '", System launchPath, "'\n")
//Importer debug := 1
Importer addSearchPath(Path with(System launchPath, "../../Interface/Ion"))
Importer addSearchPath(Path with(System launchPath, "Interface/Ion"))

Importer addSearchPath(Path with(System launchPath, "../../Interface/Ion/Widgets"))
Importer addSearchPath(Path with(System launchPath, "Interface/Ion/Widgets"))

ResourceManager

FontManager addPath(Path with(System launchPath, "Library/Fonts"))
FontManager addPath(Path with(System launchPath, "../../Library/Fonts"))

ImageManager addPath(Path with(System launchPath, "Library/Themes/Neos"))
ImageManager addPath(Path with(System launchPath, "../../Library/Themes/Neos"))

Button textColor = Color clone set(1,1,1,1)

Lobby doFile(Path with(System launchPath, "IRC.io"))



