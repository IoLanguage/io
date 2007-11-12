/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

CFFI do(
	Pointer do(
		init := method(nil)
		
		ptr := method(
			if(self isType,
				toType(self)
			,
				self proto ptr
			)
		)
		
		isType := method(
			self hasLocalSlot("pointedToType")
		)
		
		typeString := method(
			"^" .. self pointedToType typeString
		)
	)
)
