
// Define a Dog object

Dog := Object clone
Dog barkPhrase := "woof!"
Dog bark := method(barkPhrase print)

// Create Chiwawa subclass of Dog

Chiwawa := Dog clone
Chiwawa barkPhrase := "yip!"

"Dog bark: " print
Dog bark
"\n" print

"Chiwawa bark: " print
Chiwawa bark
"\n" print

// make an instance

myChiwawa := Chiwawa clone
myChiwawa barkPhrase := "Yo Quiero Taco Bell\n"

"myChiwawa bark: " print
myChiwawa bark