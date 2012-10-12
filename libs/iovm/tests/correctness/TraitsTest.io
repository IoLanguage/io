TraitsTest := UnitTest clone do(
  setUp := method(
    super(setUp)
    self A := Object clone do(foo := 42)
  )

  testNoConflicts := method(
    B := Object clone
    B addTrait(A)
    assertEquals(42, B foo)
  )

  testConflict := method(
    B := Object clone do(foo := 23)
    assertRaisesException(B addTrait(A))
  )

  testConflictRename := method(
    B := Object clone do(foo := 23)
    B addTrait(A, Map clone atPut("foo", "fooFromA"))
    assertEquals(42, B fooFromA)
    assertEquals(23, B foo)
  )

  testMissingArguments := method(
    assertRaisesException(A addTrait)
  )
)
