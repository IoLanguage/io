#!/usr/bin/env io

/* The Computer Language Shootout
   http://shootout.alioth.debian.org
   contributed by Gavin Harrison */

Range // Load the Range addon

SNorm:=Object clone do(
   App:=method(n,
      n1:=n-1
      u:=List clone preallocateToSize(n)
      v:=List clone preallocateToSize(n)
      n repeat(u append(1);v append(0))
      0 to(9) foreach(n3,n2,MAtAv(n,u,v);MAtAv(n,v,u))
      vBv:=vv:=vAt:=0
      for(i,0,n1,vAt=v at(i);vBv=vBv+u at(i)*vAt;vv=vv+(vAt*vAt))
      (vBv/vv)sqrt)
   A:=method(i,j,ij:=i+j;1/(ij*(ij+1)/2+i+1))
   MAv:=method(n,v,Av,
      n1:=n-1
      for(i_i,0,n1,
         for(i_j,0,n1,Av atPut(i_i,Av at(i_i)+A(i_i,i_j)*v at(i_j)))))
   MAtv:=method(n,v,Atv,
      n1:=n-1
      for(i_i,0,n1,
         Atv atPut(i_i,0)
         for(i_j,0,n1,Atv atPut(i_i,Atv at(i_i)+A(i_j, i_i)*v at(i_j)))))
   MAtAv:=method(n,v,AtAv,
      u:=List clone preallocateToSize(n)
      n repeat(u append(0))
      MAv(n,v,u)
      MAtv(n,u,AtAv)))
n:= System args at(1) asNumber
SNorm App(n) asString(0,9) println
