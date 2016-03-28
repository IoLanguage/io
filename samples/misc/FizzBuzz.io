#!/usr/bin/env io


// Iterate 1-100 using a
for (a, 1, 100,

    // Print the number
    if ((a % 3) != 0 and (a % 5) != 0,
        a print
    )

    // Number is divisible by 3
    if ((a % 3) == 0,
        "Fizz" print
    )
    
    // Number is divisible by 5
    if ((a % 5) == 0,
        "Buzz" print
    )

    // Newline
    "\n" print
)
