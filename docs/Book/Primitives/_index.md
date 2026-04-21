# Primitives

Built-in objects: numbers, strings, lists, maps, files, and more.

## Primitives

Primitives are objects built into Io whose methods are typically implemented in C and store some hidden data in their instances. For example, the Number primitive has a double precision floating point number as its hidden data and its methods that do arithmetic operations are C functions. All Io primitives inherit from the Object prototype and are mutable. That is, their methods can be changed. The reference docs contain more info on primitives.

This document is not meant as a reference manual, but an overview of the base primitives and bindings is provided here to give the user a jump start and a feel for what is available and where to look in the reference documentation for further details.


### Object

#### The ? Operator

Sometimes it's desirable to conditionally call a method only if it exists (to avoid raising an exception). Example:

```io
if(obj getSlot("foo"), obj foo)
```


Putting a "?" before a message has the same effect:

```io
obj ?foo
```


### List

A List is an array of references and supports all the standard array manipulation and enumeration methods. Examples:

Create an empty list:

```io
a := List clone
```


Create a list of arbitrary objects using the list() method:

```io
a := list(33, "a")
```


Append an item:

```io
a append("b")
==> list(33, "a", "b")
```


Get the list size:

```io
a size
==> 3
```


Get the item at a given index (List indexes begin at zero):

```io
a at(1)
==> "a"
```


Note: List indexes begin at zero and nil is returned if the accessed index doesn't exist.


Set the item at a given index:

```io
a atPut(2, "foo")
==> list(33, "a", "foo", "b")
```


```io
a atPut(6, "Fred")
==> Exception: index out of bounds
```


Remove an item at a given index:

```io
a remove("foo")
==> list(33, "a", "b")
```


Inserting an item at a given index:

```io
a atInsert(2, "foo")
==> list(33, "a", "foo", "56")
```


#### foreach

The foreach, map and select methods can be used in three forms:

```io
Io> a := list(65, 21, 122)
```


In the first form, the first argument is used as an index variable, the second as a value variable and the 3rd as the expression to evaluate for each value.

```io
Io> a foreach(i, v, write(i, ":", v, ", "))
==> 0:65, 1:21, 2:122,
```


The second form removes the index argument:

```io
Io> a foreach(v, v println)
==> 65
21
122
```


The third form removes the value argument and simply sends the expression as a message to each value:

```io
Io> a foreach(println)
==> 65
21
122
```


#### map and select

Io's map and select (known as filter in some other languages) methods allow arbitrary expressions as the map/select predicates.

```io
Io> numbers := list(1, 2, 3, 4, 5, 6)
```


```io
Io> numbers select(isOdd)
==> list(1, 3, 5)
```


```io
Io> numbers select(x, x isOdd)
==> list(1, 3, 5)
```


```io
Io> numbers select(i, x, x isOdd)
==> list(1, 3, 5)
```


```io
Io> numbers map(x, x*2)
==> list(2, 4, 6, 8, 10, 12)
```


```io
Io> numbers map(i, x, x+i)
==> list(1, 3, 5, 7, 9, 11)
```


```io
Io> numbers map(*3)
==> list(3, 6, 9, 12, 15, 18)
```


The map and select methods return new lists. To do the same operations in-place, you can use selectInPlace() and mapInPlace() methods.

### Sequence

In Io, an immutable Sequence is called a Symbol and a mutable Sequence is the equivalent of a Buffer or String. Literal strings(ones that appear in source code surrounded by quotes) are Symbols. Mutable operations cannot be performed on Symbols, but one can make mutable copy of a Symbol calling its asMutable method and then perform the mutation operations on the copy. Common string operations Getting the length of a string:

```io
"abc" size
==> 3
```


Checking if a string contains a substring:

```io
"apples" containsSeq("ppl")
==> true
```


Getting the character (byte) at position N:

```io
"Kavi" at(1)
==> 97
```



Slicing:

```io
"Kirikuro" slice(0, 2)
==> "Ki"
```


```io
"Kirikuro" slice(-2)  # NOT: slice(-2, 0)!
==> "ro"
```


```io
Io> "Kirikuro" slice(0, -2)
# "Kiriku"
```


Stripping whitespace:

```io
"  abc  " asMutable strip
==> "abc"
```


```io
"  abc  " asMutable lstrip
==> "abc  "
```


```io
"  abc  " asMutable rstrip
==> "  abc"
```


Converting to upper/lowercase:

```io
"Kavi" asUppercase
==> "KAVI"
"Kavi" asLowercase
==> "kavi"
```


Splitting a string:

```io
"the quick brown fox" split
==> list("the", "quick", "brown", "fox")
```


Splitting by others character is possible as well.

```io
"a few good men" split("e")
==> list("a f", "w good m", "n")
```


Converting to number:

```io
"13" asNumber
==> 13
"a13" asNumber
==> nil
```


String interpolation:

```io
name := "Fred"
==> Fred
"My name is #{name}" interpolate
==> My name is Fred
```


Interpolate will eval anything with #{} as Io code in the local context. The code may include loops or anything else but needs to return an object that responds to asString.


### Ranges

A range is a container containing a start and an end point, and instructions on how to get from the start, to the end. Using Ranges is often convenient when creating large lists of sequential data as they can be easily converted to lists, or as a replacement for the for() method.

#### The Range protocol

Each object that can be used in Ranges needs to implement a "nextInSequence" method which takes a single optional argument (the number of items to skip in the sequence of objects), and return the next item after that skip value. The default skip value is 1. The skip value of 0 is undefined. An example:

```io
Number nextInSequence := method(skipVal,
if(skipVal isNil, skipVal = 1)
self + skipVal
)
```


With this method on Number (it's already there in the standard libraries), you can then use Numbers in Ranges, as demonstrated below:

```io
1 to(5) foreach(v, v println)
```


The above will print 1 through 5, each on its own line.


### File

The methods openForAppending, openForReading, or openForUpdating are used for opening files. To erase an existing file before opening a new open, the remove method can be used. Example:

```io
f := File with("foo.txt)
f remove
f openForUpdating
f write("hello world!")
f close
```


### Directory

Creating a directory object:

```io
dir := Directory with("/Users/steve/")
```


Get a list of file objects for all the files in a directory:

```io
files := dir files
==> list(File_0x820c40, File_0x820c40, ...)
```


Get a list of both the file and directory objects in a directory:

```io
items := Directory items
==> list(Directory_0x8446b0, File_0x820c40, ...)
```


```io
items at(4) name
==> DarkSide-0.0.1 # a directory name
```


Setting a Directory object to a certain directory and using it:

```io
root := Directory clone setPath("c:/")
==> Directory_0x8637b8
```


```io
root fileNames
==> list("AUTOEXEC.BAT", "boot.ini", "CONFIG.SYS", ...)
```


Testing for existence:

```io
Directory clone setPath("q:/") exists
==> false
```


Getthing the current working directory:

```io
Directory currentWorkingDirectory
==> "/cygdrive/c/lang/IoFull-Cygwin-2006-04-20"
```


### Date

Creating a new date instance:

```io
d := Date clone
```


Setting it to the current date/time:

```io
d now
```


Getting the date/time as a number, in seconds:

```io
Date now asNumber
==> 1147198509.417114
```


Getting individual parts of a Date object:

```io
d := Date now
==> 2006-05-09 21:53:03 EST
d
==> 2006-05-09 21:53:03 EST
d year
==> 2006
d month
==> 5
d day
==> 9
d hour
==> 21
d minute
==> 53
d second
==> 3.747125
```


Find how long it takes to execute some code:


```io
Date cpuSecondsToRun(100000 repeat(1+1))
==> 0.02
```


### Networking

All of Io's networking is done with asynchronous sockets underneath, but operations like reading and writing to a socket appear to be synchronous since the calling coroutine is unscheduled until the socket has completed the operation, or a timeout occurs. Note that you'll need to first reference the associated addon in order to cause it to load before using its objects. In these examples, you'll have to reference "Socket" to get the Socket addon to load first.


Creating a URL object:

```io
url := URL with("[http://example.com](http://example.com)/")
```


Fetching an URL:

```io
data := url fetch
```


Streaming a URL to a file:

```io
url streamTo(File with("out.txt"))
```


A simple whois client:

```io
whois := method(host,
socket := Socket clone \
setHostName("rs.internic.net") setPort(43)
socket connect streamWrite(host, "\n")
while(socket streamReadNextChunk, nil)
return socket readBuffer
)
```


A minimal web server:

```io
WebRequest := Object clone do(
handleSocket := method(aSocket,
aSocket streamReadNextChunk
request := aSocket readBuffer \
betweenSeq("GET ", " HTTP")
f := File with(request)
if(f exists,
f streamTo(aSocket)
,
aSocket streamWrite("not found")
)
aSocket close
)
)
```


```io
WebServer := Server clone do(
setPort(8000)
handleSocket := method(aSocket,
WebRequest clone asyncSend(handleSocket(aSocket))
)
)
```


```io
WebServer start
```


### XML

Using the XML parser to find the links in a web page:

```io
SGML // reference this to load the SGML addon
xml := URL with("[http://www.yahoo.com](http://www.yahoo.com)/") fetch asXML
links := xml elementsWithName("a") map(attributes at("href"))
```


### Vector

Io's Vectors are built on its Sequence primitive and are defined as:

```io
Vector := Sequence clone setItemType("float32")
```


The Sequence primitive supports SIMD acceleration on a number of float32 operations. Currently these include add, subtract, multiple and divide but in the future can be extended to support most math, logic and string manipulation related operations.

Here's a small example:

```io
iters := 1000
size := 1024
ops := iters * size
```


```io
v1 := Vector clone setSize(size) rangeFill
v2 := Vector clone setSize(size) rangeFill
```


```io
dt := Date secondsToRun(
iters repeat(v1 *= v2)
)
```


```io
writeln((ops/(dt*1000000000)) asString(1, 3), " GFLOPS")
```


Which when run on 2Ghz Mac Laptop, outputs:

```io
1.255 GFLOPS
```


A similar bit of C code (without SIMD acceleration) outputs:

```io
0.479 GFLOPS
```


So for this example, Io is about three times faster than plain C.




## Unicode

### Sequences

In Io, symbols, strings, and vectors are unified into a single Sequence prototype which is an array of any available hardware data type such as:

```io
uint8, uint16, uint32, uint64
int8, int16, int32, int64
float32, float64
```


### Encodings

Also, a Sequence has a encoding attribute, which can be:

```io
number, ascii, ucs2, ucs4, utf8
```


UCS-2 and UCS-4 are the fixed character width versions of UTF-16 and UTF-32, respectively. A String is just a Sequence with a text encoding, a Symbol is an immutable String and a Vector is a Sequence with a number encoding.

UTF encodings are assumed to be big endian.

Except for input and output, all strings should be kept in a fixed character width encoding. This design allows for a simpler implementation, code sharing between vector and string ops, fast index-based access, and SIMD acceleration of Sequence operations. All Sequence methods will do automatic type conversions as needed.


### Source Code

Io source files are assumed to be in UTF8 (of which ASCII is a subset). When a source file is read, its symbols and strings are stored in Sequences in their minimal fixed character width encoding. Examples:

```io
Io> "hello" encoding
==> ascii
```


```io
Io> "π" encoding
==> ucs2
```


```io
Io> "∞" encoding
==> ucs2
```


We can also inspect the internal representation:


```io
Io> "π" itemType
==> uint16
```


```io
Io> "π" itemSize
==> 2
```


### Conversion

The Sequence object has a number of conversion methods:

```io
asUTF8
asUCS2
asUCS4
```







## Importing

The Importer proto implements Io's built-in auto importer feature. If you put each of your proto's in their own file, and give the file the same name with and ".io" extension, the Importer will automatically import that file when the proto is first referenced. The Importer's default search path is the current working directory, but can add search paths using its addSearchPath() method.
