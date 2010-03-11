/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

CFFI Library do(
	newSlot("name")

    with := method(name,
        this := self clone setName(name)
    )
)
