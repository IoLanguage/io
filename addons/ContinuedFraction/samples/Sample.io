#!/usr/bin/env io

/* Example usages of ContinuedFraction */

cf := ContinuedFraction with(2) + SQRT with(2) + SQRT with(3) + E

"2 + sqrt(2) + sqrt(3) + e = [" print
for(i, 0, 100, (cf at(i) .. ", ") print)
"...]" println
