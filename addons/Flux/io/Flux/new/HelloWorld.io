
Importer addSearchPath(Path with(launchPath, "../../Interface/Ion"))
Importer addSearchPath(Path with(launchPath, "../../Interface/Ion/Widgets"))
ResourceManager
FontManager addPath(Path with(launchPath, "../../Library/Fonts"))

Screen setTitle("HelloWorld")

label := Label clone 
label setTitle("Hello World!")
Screen addSubview(label)
Screen open




