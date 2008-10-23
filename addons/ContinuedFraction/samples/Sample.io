#!/usr/bin/env io

/* Example usages of ContinuedFraction */
ContinuedFraction

cf := ContinuedFraction with(2) + ContinuedFraction SQRT with(2) + ContinuedFraction SQRT with(3) + ContinuedFraction E

"2 + sqrt(2) + sqrt(3) + e = [" print
for(i, 0, 100, (cf at(i) .. ", ") print)
"...]" println
