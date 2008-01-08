/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

// Create a Proto with the same name as the addon to play nice with AddonLoader.
Protos Addons CFFI CFFI := Protos Addons CFFI

CFFI do(
	docCopyright("Trevor Fancher", 2006)
	docCategory("Other")
	docLicense("New BSD license")

	// Because of the way building addons works, our Protos are prefixed
	// with CFFI.  Here we remove those prefixes.
	method(
		CFFI slotNames foreach(slot,
			slot beginsWithSeq("CFFI") and(slot size > 4) ifTrue(
				CFFI setSlot(slot slice(4), CFFI getSlot(slot))
				CFFI removeSlot(slot)
			)
		)
	) call

	removeSlot("type") // we are actually a namespace, not a type
)
