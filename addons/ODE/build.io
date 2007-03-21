Binding clone do(
  dependsOnLib("ode")

  dependsOnBinding("Vector")
  dependsOnBinding("OpenGL")

  debs    atPut("ode", "libode0-dev")
  ebuilds atPut("ode", "ode")
  pkgs    atPut("ode", "ode")
  rpms    atPut("ode", "ode-devel")
)
