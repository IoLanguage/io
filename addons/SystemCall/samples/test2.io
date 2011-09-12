#!/usr/bin/env io



SystemCall clone setCommand("io") setArguments(list("-e", "1+2")) run split("\n") foreach(println)
