# WeakLink

A WeakLink is a primitive that can hold a reference to
an object without preventing the garbage collector from
collecting it. The link reference is set with the setLink() method.
After the garbage collector collects an object, it informs any
(uncollected) WeakLink objects whose link value pointed to that
object by calling their "collectedLink" method.

## link

Returns the link pointer or Nil if none is set.

## setLink(aValue)

Sets the link pointer. Returns self.

