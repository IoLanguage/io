#!/usr/bin/env io

/* The Computer Language Shootout
   http://shootout.alioth.debian.org

   Contributed by Ian Osgood */

TreeNode := Object clone do(
    bottomUpTree := method(n,d,
        self item := n
        if (d>0,   n=2*n; d=d-1
            self left  := TreeNode clone bottomUpTree(n-1, d)
            self right := TreeNode clone bottomUpTree(n,   d)
        )
        self
    )
    itemCheck := method(
        if (self hasSlot("left"), item + left itemCheck - right itemCheck, item)
    )
)

minDepth := 4
maxDepth := System args at(1) asNumber max(minDepth+2)

check := TreeNode clone bottomUpTree(0, maxDepth+1) itemCheck
writeln("stretch tree of depth ", maxDepth+1, "\t check: ", check)

longLivedTree := TreeNode clone bottomUpTree(0, maxDepth)

for (depth, minDepth, maxDepth, 2,
    iterations := 1 clone shiftLeft(maxDepth - depth + minDepth)
    check = 0
    for (i, 1, iterations,
        check = check + TreeNode clone bottomUpTree( i, depth) itemCheck
        check = check + TreeNode clone bottomUpTree(-i, depth) itemCheck
    )
    writeln(iterations*2, "\t trees of depth ", depth, "\t check: ", check)
)

check = longLivedTree itemCheck
writeln("long lived tree of depth ", maxDepth, "\t check: ", check)
