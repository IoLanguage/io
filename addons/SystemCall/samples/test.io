#!/usr/bin/env io



SystemCall clone setCommand("ls") run stdout readLines foreach(println)
