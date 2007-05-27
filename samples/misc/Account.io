#!/usr/bin/env io

Account := Object clone
Account balance := 0.0
Account deposit := method(v, self balance := self balance + v)
Account show := method(write("Account balance: $", balance, "\n"))

"Inital: " print
Account show

"Depositing $10\n" print
Account deposit(10.0)

"Final: " print
Account show
