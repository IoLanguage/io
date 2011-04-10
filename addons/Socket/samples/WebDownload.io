#!/usr/bin/env io

// Example use:
// ./WebDownload.io http://64.236.34.161:80/stream/1018 GrooveSalad.mp3

url := System args at(1)
fileName := System args at(2)

file := File with(fileName) openForUpdating
URL with(url) fetchToFile(file)
file close
