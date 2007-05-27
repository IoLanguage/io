#!/usr/bin/env io

/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org

   Contributed by Ian Osgood */

sum := 0
file := File standardInput
while (line := file readLine, sum = sum + line asNumber)
sum println
