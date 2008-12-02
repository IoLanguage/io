#!/usr/bin/env io

/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org

   Contributed by Ian Osgood */

ALU := Sequence with(
       "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG",
       "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA",
       "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT",
       "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA",
       "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG",
       "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC",
       "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA")

# probabilities for symbols in Fasta codes

IUB := list(0.27, 0.12, 0.12, 0.27)
11 repeat( IUB append(0.02) )

HomoSap := list(0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008)

Fasta := Object clone do(
    last := 42
    
    gen_random := method(
        (last = ((last * 3877 + 29573) % 139968)) / 139968
    )
    
    repeatWithSeq := method(n,seq, i := li := 0
        line := Sequence clone setSize(60)
        n repeat(
            line atPut(li, seq at(i))
            li = li + 1
            if (li == line size, line println; li = 0)
            i = ((i+1) % seq size)
        )
        if (li != 0, line setSize(li) println)
    )
    
    codes := "acgtBDHKMNRSVWY"
    
    random := method(n, probs, sum := li := 0
        line := Sequence clone setSize(60)
        probs mapInPlace(prob, sum = sum + prob)
        n repeat(
            r := gen_random; i := 0
            while (r > probs at(i), i = i + 1)
            line atPut(li, codes at(i))
            li = li + 1
            if (li == line size, line println; li = 0)
        )
        if (li != 0, line setSize(li) println)
    )
)

n := System args at(1) asNumber

">ONE Homo sapiens alu" println
Fasta repeatWithSeq(2*n, ALU)

">TWO IUB ambiguity codes" println
Fasta random(3*n, IUB)

">THREE Homo sapiens frequency" println
Fasta random(5*n, HomoSap)
