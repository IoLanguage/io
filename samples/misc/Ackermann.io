#!/usr/bin/env io

ack := method(m, n, 
  //writeln("ack(", m, ",", n, ")")
  if (m < 1, return n + 1) 
  if (n < 1, return ack(m - 1, 1)) 
  return ack(m - 1, ack(m, n - 1)) 
) 

ack(3, 4) print 
"\n" print
