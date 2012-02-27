//doc Object addTrait Takes another object, whose slots will be copied into the receiver. Optionally takes a second argument, a Map object containing string -> string pairs, holding conflicting slot names and names to rename them to. I.e., if you have two objects A and B, both have a slot named foo, you issue A addTrait(B, Map clone atPut("foo", "newFoo")) the value of B foo will be placed in A newFoo.
Object addTrait := method(obj,
  if(call message arguments size == 0, 
    Exception raise("addTrait requires at least one argument"))
  resolutions := call evalArgAt(1)
  if(resolutions isNil, resolutions = Map clone)

  getSlot("obj") foreachSlot(name, value,
    if(getSlot("self") hasLocalSlot(name),
      if(name == "type", continue)
      if(resolutions at(name) isNil, Exception raise("""Slot '#{name}' already exists in #{getSlot("self") type}[#{getSlot("self") uniqueId}]. Give it a new name in the map argument.""" interpolate))
      getSlot("self") setSlot(resolutions at(name), getSlot("value"))
      continue
    )
    getSlot("self") setSlot(name, getSlot("value"))
  )

  getSlot("self")
)
