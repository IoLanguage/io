# MutableSequence

A Sequence whose contents can be modified in place. MutableSequence is the
receiver for the mutating methods of Sequence (append, insert, remove, etc.).

## IoSeq_insertSeqEvery(aSequence, aNumberOfItems)

Inserts aSequence every aNumberOfItems.  Returns self.

## IoSeq_leaveThenRemove(aNumberToLeave, aNumberToRemove)

Leaves aNumberToLeave items then removes aNumberToRemove items.  Returns
self.

