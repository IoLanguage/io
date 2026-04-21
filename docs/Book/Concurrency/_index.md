---
heroImage: ../images/Concurrency.png
---

# Concurrency

Coroutines, actors, futures, and Io's concurrency model.

## Coroutines

Io uses coroutines (user level cooperative threads), instead of preemptive OS level threads to implement concurrency. This avoids the substantial costs (memory, system calls, locking, caching issues, etc) associated with native threads and allows Io to support a very high level of concurrency with thousands of active threads.

Under the stackless evaluator, a coroutine is simply a chain of heap-allocated evaluation frames. Switching coroutines is a matter of changing which frame chain the eval loop is walking — there is no platform-specific assembly, no `setjmp`/`longjmp`, and no `ucontext` or fibers. The same C code works on every host, including WebAssembly, which hides the native call stack entirely.


### Scheduler

The Scheduler object is responsible for resuming coroutines that are yielding. The current scheduling system uses a simple first-in-first-out policy with no priorities.





## Actors

An actor is an object with its own thread (in our case, its own coroutine) which it uses to process its queue of asynchronous messages. Any object in Io can be sent an asynchronous message by placing using the asyncSend() or futureSend() messages. Examples:

Synchronous:

```io
result := self foo
```


Asynchronous, immediately returns a Future:


```io
futureResult := self futureSend(foo)
```


Asynchronous, immediately returns nil:


```io
self asyncSend(foo)
```


When an object receives an asynchronous message it puts the message in its queue and, if it doesn't already have one, starts a coroutine to process the messages in its queue. Queued messages are processed sequentially in a first-in-first-out order. Control can be yielded to other coroutines by calling "yield".

Example:

```io
obj1 := Object clone
obj1 test := method(for(n, 1, 3, n print; yield))
obj2 := obj1 clone
obj1 asyncSend(test); obj2 asyncSend(test)
while(Scheduler yieldingCoros size > 1, yield)
```


This would print "112233".

Here's a more real world example:

```io
HttpServer handleRequest := method(aSocket,
HttpRequestHandler clone asyncSend(
handleRequest(aSocket)
)
)
```



## Futures

Io's futures are transparent. That is, when the result is ready, they become the result. If a message is sent to a future (besides the two methods it implements), it waits until it turns into the result before processing the message. Transparent futures are powerful because they allow programs to minimize blocking while also freeing the programmer from managing the fine details of synchronization.


### Auto Deadlock Detection

An advantage of using futures is that when a future requires a wait, it will check to see if pausing to wait for the result would cause a deadlock and if so, avoid the deadlock and raise an exception. It performs this check by traversing the list of connected futures.


### Futures and the Command Line Interface

The command line will attempt to print the result of expressions evaluated in it, so if the result is a Future, it will attempt to print it and this will wait on the result of Future. Example:

```io
Io> q := method(wait(1))
Io> futureSend(q)
[1-second delay]
==> nil
```


To avoid this, just make sure the Future isn't the result. Example:

```io
Io> futureSend(q); nil
[no delay]
==> nil
```


### Yield

An object will automatically yield between processing each of its asynchronous messages. The yield method only needs to be called if a yield is required during an asynchronous message execution.


### Pause and Resume

It's also possible to pause and resume an object. See the concurrency methods of the Object primitive for details and related methods.




## Continuations

Because evaluation state lives in heap-allocated frames rather than on the C stack, Io supports first-class continuations. `callcc` captures the current computation — its frame chain and local state — as a first-class object that can be stored, invoked later, or even serialized and resumed in another process.

The same property also makes it possible to model resumable exceptions: a handler can choose to resume the computation at the point of the raise, rather than unwinding past it, enabling Smalltalk- or Common-Lisp-style condition systems.

Because `callcc` is easy to misuse, it is not exposed in the top-level Lobby. It is available where needed through the VM's reflective interface.
