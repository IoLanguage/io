
Directory statementCount := method(
  cFiles := items select(i, f, f name endsWithSeq(".c"))
  s := 0
  cFiles foreach(f, s = s + f asString occurancesOfSeq(";"))
  s
)

d := Directory clone
writeln("statements (via counting semicolons in c files):")
writeln("  vm     ", c1 := d statementCount)
writeln("  base   ", c2 := d setPath("base") statementCount)
writeln("  skipdb ", c3 := d setPath("SkipDB") statementCount)
writeln("  parser ", c4 := d setPath("SkipDB") statementCount)
writeln("  total  ", c1 + c2 + c3 + c4)
