#!/usr/bin/env io

Terminal := GLApp clone 
Terminal do(
    setAppName("Terminal")
    leftMargin ::= 10
    bottomMargin ::= 10
    prompt ::= "Io> "

    font := Font clone open("../../Flux/resources/fonts/ProFont/Mono/Normal.ttf") setPixelSize(25)
    lines := List clone append(Sequence clone with(prompt))
    command := Sequence clone
    
    appendString := method(s, 
        if(s containsSeq("\n")) then(
            lines appendSeq(s split("\n"))
        ) else(
            lines last appendSeq(s)      
        )
    )
    
    newLine := method(lines append(Sequence clone))
        
    linesPerScreen := method(height / font pixelHeight)
        
    draw := method(
        glColor4d(.5, .5, .5, 1)
        
        glTranslated(leftMargin, bottomMargin, 0)
        
        lines reverseForeach(i, line,
            if(i < lines size - linesPerScreen, break)
            font drawString(line)
            glTranslated(0, font pixelHeight, 0)
        )
    )
    
    keyboard := method(k,
        resend
        if(k == GLUT_KEY_RETURN) then(
            lines last removeLast
            newLine
            e := try(v := Lobby doString(command))
            newLine
            if(e, appendString(e coroutine backTraceString), appendString("==> " .. v asString))
            newLine
            command empty
        ) elseif(k == GLUT_KEY_DELETE) then(
            command removeLast
        ) else(
            command append(k)
        )
        
        lines last empty appendSeq(prompt) appendSeq(command) appendSeq(127 asCharacter)
        glutPostRedisplay
    )    
) 

Number   print := method(Object write(self asString); nil)
Sequence print := method(Object write(self asString); nil)
Object   print := method(Object write(self asString); nil)

Object write := method(call evalArgs foreach(a, Terminal appendString(a asString)))

Terminal run


