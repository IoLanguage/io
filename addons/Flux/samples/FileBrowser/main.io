#!/usr/bin/env io

Flux

Application clone do(
    setTitle("FileBrowser")
    mainWindow resizeTo(800, 300)
    
    keyboard := method(key, x, y,
        resend
        if(key == GLUT_KEY_ESC, mainWindow toggleFullScreen)
    )
    
    appDidStart := method(
        resend
        cv := Browser clone
        
        cv backgroundColor := Color clone set(1,1,1,1)
        HScroller backgroundColor := Color clone set(1,1,1,1)
        TextCell backgroundColor := Color clone set(1,1,1,1)
        Button textColor set(1,1,1,1)
        
        cv resizeWithSuperview
        mainWindow contentView addSubview(cv)
        cv setRootItem(Directory clone setPath("/"))
        cv resizeTo(mainWindow width, mainWindow height)
    )
) run
