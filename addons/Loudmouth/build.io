AddonBuilder clone do(
  dependsOnLib("loudmouth-1.0")
  dependsOnHeader("loudmouth.h")

  headerSearchPaths foreach(path, appendHeaderSearchPath(path .. "/loudmouth-1.0"))
)
