#!/usr/bin/env io

// Example:
// ./ioServer Sockets/_tests/WebDownload.io http://64.236.34.161:80/stream/1018 GrooveSalad.mp3

//doFile(launchPath .. "/URL.io")
url := System args at(0)
fileName := System args at(1)

//DNSResolver addServerAddress("128.105.2.10") 

url := URL clone setURL(url)
file := File clone openForUpdating(fileName)
url fetchToFile(file)
file close



