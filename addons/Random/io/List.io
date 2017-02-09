//metadoc Random category Math
# The following methods are added only if the Random addon is installed.
List do(
    //doc List shuffleInPlace Randomizes the order of the elements in the receiver. Returns self.
    shuffleInPlace := method(for(i, 0, size - 1, swapIndices(i, Random value(i, size) floor)))

    //doc List shuffle Randomizes the ordering of all the items of the receiver. Returns copy of receiver.
    shuffle := method(self itemCopy shuffleInPlace)

    //doc List anyOne Returns a random element of the receiver or nil if the receiver is empty.
    anyOne := method(at(Random value(0, size) floor))
)
