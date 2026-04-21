---
heroImage: ../images/Control Flow.png
heroLayout: wide
---

# Control Flow

Branching, loops, exceptions, and scope.

## Branching

### if, then, else

The if() method can be used in the form:

```io
if(<condition>, <do message>, <else do message>)
```


Example:

```io
if(a == 10, "a is 10" print)
```


The else argument is optional. The condition is considered false if the condition expression evaluates to false or nil, and true otherwise.

The result of the evaluated message is returned, so:

```io
if(y < 10, x := y, x := 0)
```


is the same as:

```io
x := if(y < 10, y, 0)
```



Conditions can also be used in this form:

```io
if(y < 10) then(x := y) else(x := 2)
```


elseif() is supported:

```io
if(y < 10) then(x := y) elseif(
y == 11) then(x := 0) else(x := 2)
```


### ifTrue, ifFalse

Also supported are Smalltalk style ifTrue, ifFalse, ifNil and ifNonNil methods:

```io
(y < 10) ifTrue(x := y) ifFalse(x := 2)
```


Notice that the condition expression must have parenthesis surrounding it.


#### loop

The loop method can be used for "infinite" loops:

```io
loop("foo" println)
```


### repeat

The Number repeat method can be used to repeat a loop a given number of times.

```io
3 repeat("foo" print)
==> foofoofoo
```


#### while

Arguments:

```io
while(<condition>, <do message>)
```


Example:

```io
a := 1
while(a < 10,
a print
a = a + 1
)
```


### for

Arguments:

```io
for(<counter>, <start>, <end>,
<optional step>, <do message>)
```


The start and end messages are only evaluated once, when the loop starts. Example:

```io
for(a, 0, 10,
a println
)
```


Example with a step:

```io
for(x, 0, 10, 3, x println)
```


Which would print:

```io
0
3
6
9
```


To reverse the order of the loop, add a negative step:

```io
for(a, 10, 0, -1, a println)
```


Note: the first value will be the first value of the loop variable and the last will be the last value on the final pass through the loop. So a loop of 1 to 10 will loop 10 times and a loop of 0 to 10 will loop 11 times.


### break, continue

loop, repeat, while and for support the break and continue methods. Example:

```io
for(i, 1, 10,
if(i == 3, continue)
if(i == 7, break)
i print
)
```


Outputs:

```io
12456
```


### return

Any part of a block can return immediately using the return method. Example:

```io
Io> test := method(123 print;
return "abc"; 456 print)
Io> test
123
==> abc
```


Internally, break, continue and return all work by setting a IoState internal variable called "stopStatus" which is monitored by the loop and message evaluation code.



## Comparison

### true, false and nil

There are singletons for true, false and nil. nil is typically used to indicate an unset or missing value.


### Comparison Methods

The comparison methods:

```io
==, !=, >=, <=, >, <
```


return either the true or false. The compare() method is used to implement the comparison methods and returns -1, 0 or 1 which mean less-than, equal-to or greater-than, respectively.




## Loops

### loop

The loop method can be used for "infinite" loops:

```io
loop("foo" println)
```


### repeat

The Number repeat method can be used to repeat a loop a given number of times.

```io
3 repeat("foo" print)
==> foofoofoo
```


### while

Arguments:

```io
while(<condition>, <do message>)
```


Example:

```io
a := 1
while(a < 10,
a print
a = a + 1
)
```


### for

Arguments:

```io
for(<counter>, <start>, <end>,
<optional step>, <do message>)
```


The start and end messages are only evaluated once, when the loop starts. Example:

```io
for(a, 0, 10,
a println
)
```


Example with a step:

```io
for(x, 0, 10, 3, x println)
```


Which would print:

```io
0
3
6
9
```


To reverse the order of the loop, add a negative step:

```io
for(a, 10, 0, -1, a println)
```


Note: the first value will be the first value of the loop variable and the last will be the last value on the final pass through the loop. So a loop of 1 to 10 will loop 10 times and a loop of 0 to 10 will loop 11 times.


### break, continue

loop, repeat, while and for support the break and continue methods. Example:

```io
for(i, 1, 10,
if(i == 3, continue)
if(i == 7, break)
i print
)
```


Output:

```io
12456
```


#### return

Any part of a block can return immediately using the return method. Example:

```io
Io> test := method(123 print; return "abc"; 456 print)
Io> test
123
==> abc
```


Internally, break, continue and return all work by setting a IoState internal variable called "stopStatus" which is monitored by the loop and message evaluation code.



## Exceptions

### Raise

An exception can be raised by calling raise() on an exception proto.

```io
Exception raise("generic foo exception")
```


### Try and Catch

To catch an exception, the try() method of the Object proto is used. try() will catch any exceptions that occur within it and return the caught exception or nil if no exception is caught.

```io
e := try(<doMessage>)
```


To catch a particular exception, the Exception catch() method can be used. Example:

```io
e := try(
// ...
)
```


```io
e catch(Exception,
writeln(e coroutine backtraceString)
)
```


The first argument to catch indicates which types of exceptions will be caught. catch() returns the exception if it doesn't match and nil if it does.


#### Pass

To re-raise an exception caught by try(), use the pass method. This is useful to pass the exception up to the next outer exception handler, usually after all catches failed to match the type of the current exception:

```io
e := try(
// ...
)
```


```io
e catch(Error,
// ...
) catch(Exception,
// ...
) pass
```


### Custom Exceptions

Custom exception types can be implemented by simply cloning an existing Exception type:

```io
MyErrorType := Error clone
```
