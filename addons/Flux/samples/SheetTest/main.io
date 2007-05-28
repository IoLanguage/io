#!/usr/bin/env io

Flux

app := Application clone do(
    appDidInit := method(
        mainWindow setTitle("Sheet Test")
        
        sv := SheetView clone
        mainWindow addSubview(sv)
        sv autoSize
        sv positionCenter
    )
)
app run


