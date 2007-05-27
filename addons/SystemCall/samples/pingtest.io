#!/usr/bin/env io


SystemCall with("ping www.yahoo.com") runWith(if(stdout readLine betweenSeq("time=", " ms") asNumber > 40, writeln("^V^G")))
