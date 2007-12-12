
ODBObject := Object clone do(
  _odb := nil
  forward := method(k,
    write("ODBObject get ", k , "\n")
    return self _odb objectGetSlot(self, k)
  )
  setSlot := method(k, v, 
    write("ODBObject set ", k , "\n")
    super(setSlot(k, getSlot("v")))
    if (k != "_odb" and self _odb, self _odb objectSetSlot(self, k, getSlot("v")))
    return getSlot("v")
  )
)

ODB := Object clone do(
  init := method( 
    self db := SQLite clone debugOn
    self name := "ODB"
    self path := "Default.sqlite"
    self objectsToSave := Map clone
  )

  open := method(path,
    if(path, self path := path)
    db setPath(self path)
    db open
    self setup
    self rootObject := self fetchRootObject
    self
  )

  setup := method(
    if (db tableNames size > 0, return)
    self exec("CREATE TABLE Slot (object, slot, type, value)")
    self exec("CREATE UNIQUE INDEX SlotIndex ON Slot (object, slot)")
    self exec("CREATE TABLE Object (id, mark)")
    self exec("CREATE UNIQUE INDEX ObjectIndex ON Object (id)")  
  )

  close := method(self db close)

  exec := method(s, 
    r := db exec(s)
    if (db error, raiseException("ODB", db error))
    r
  )

  rowIdForObject := method(object, 
    row := self exec("SELECT ROWID FROM Object WHERE id=" .. object uniqueId) at(0) 
    return if(row, row at("ROWID"), nil)  
  )

  save := method(object,
    rowId := self rowIdForObject(object)
    rowId ifNil(
       db exec("INSERT INTO Object (id) VALUES (" .. object uniqueId .. ")")
       rowId := db lastInsertRowId
    )
    object foreach(slotName, slotValue, self objectRowIdSetSlot(rowId, slotName, getSlot("slotValue")))
    rowId
  )

  remove := method(object,
    objectRow := self exec("SELECT ROWID FROM Object WHERE id='" .. object uniqueId .. "'") at(0)
    if (objectRow,
      rowId := objectRow at("ROWID")
      self exec("DELETE FROM Object WHERE ROWID=" .. rowId )
      self exec("DELETE FROM Slot  WHERE object='" .. rowId .. "'")
    )
  )
  
  newODBObject := method(
    object := Object clone
    object _odb := self
    //write("self := ", self, "\n")
    //write("object _odb := ", object _odb, "\n")
    object proto := ODBObject
    return object
  )

  fetchRootObject := method(
    row := self exec("SELECT ROWID FROM Object WHERE ROWID=1") at(0)
    rowId := if(row, row at("ROWID"), nil)
    if(rowId) then(
        self rootObject := self objectWithRowId(rowId)
    ) else (
      self rootObject := Object clone
      self save(rootObject)
      self rootObject _odb := self
      self rootObject proto := ODBObject
    )
    return rootObject
  )
    
  objectWithRowId := method(rowId, noReplace,
    object := Object clone
    slotRows := self exec("SELECT * FROM Slot WHERE object=" .. rowId )
    slotRows ifNil(return object)
    slotRows foreach(row, 
      object setSlot(row at("slot"), self typeWithValue(row at("slotType"), row at("value")))
    )
    self exec("UPDATE Object SET id=" .. object uniqueId .. " WHERE ROWID=" .. rowId) 
    
    object _odb := self
    object proto := ODBObject
    return object
  )

  typeWithValue := method(slotType, value,
    if (slotType == "String", return value)
    if (slotType == "Number", return value asNumber)
    if (slotType == "Object", return self objectWithRowId(value asNumber))
    if (slotType == "nil", return nil)
  )
  
  objectGetSlot := method(object, slotName,
    rowId := self rowIdForObject(object)
    row := self exec("SELECT type, value FROM Slot WHERE object=" .. rowId .. " AND slot='" .. slotName .. "'") at(0)
    if (row, self typeWithValue(row at("slotType"), row at("value")), nil)
  )

  objectSetSlot := method(object, slotName, slotValue,
    self objectRowIdSetSlot(self rowIdForObject(object), slotName, slotValue)
  )
  
  objectRowIdSetSlot := method(rowId, slotName, slotValue,
    if (slotName == "proto" or slotName == "_odb", return)
    slotType := getSlot("slotValue") type
    if (slotType == "String" or slotType == "Number" or slotType == "Object",
      if (slotType == "String", v := db escapeString(slotValue))
      if (slotType == "Number", v := slotValue asString)
      if (slotType == "Object", 
        v := slotValue uniqueId asString
        objectsToSave atPut(v, slotValue)
      )
      if (slotType == "nil", v := "")
      
      cmd := Buffer clone
      cmd appendSeq("INSERT OR REPLACE INTO Slot (object, slot, type, value) VALUES (", 
        rowId, ", '",
        db escapeString(slotName), "', '",
        slotType, "', '",
        v, "')"
      )
      self exec(cmd)
    )
    while (objectsToSave first
  )
)


odb := ODB clone
r := odb open("Test.sqlite") rootObject
write("------------------------\n")
r a := 1
write("b := ", r b, "\n")
write("a := ", r a, "\n")


