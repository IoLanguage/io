#!/usr/bin/env io

SystemCall clone setCommand("ls") run split("\n") foreach(println)
