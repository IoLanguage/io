#!/usr/bin/env io


/*
Connect to GroveSalad radio station example
http://64.236.34.161:80/stream/1023

telnet 64.236.34.161 80
GET /stream/1023 HTTP/1.0
User-Agent: Mozilla/4.0

*/
ip := "64.236.34.141"
port := 80
path := "/stream/1018"
fileName := "radio.mp3"

//DNSResolver addServerAddress("128.105.2.10") 

socket := Socket clone setHost(ip) setPort(port) connect 
if (socket error,write(socket error, "\n"); exit) 
socket write("GET " .. path .. " HTTP/1.0\r\nUser-Agent: Mozilla/4.0\r\n\r\n")

file := File clone openForUpdating(fileName)

while(socket read, 
  file write(socket readBuffer)
  write("got data\n")
  socket readBuffer empty
)

if (socket error, write(socket error, "\n") ) 

file close

