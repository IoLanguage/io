AddonBuilder clone do(
  disable # Needs to be ported to the new Vector code

  dependsOnLib("ode")

  dependsOnBinding("OpenGL")

  debs    atPut("ode", "libode0-dev")
  ebuilds atPut("ode", "ode")
  pkgs    atPut("ode", "ode")
  rpms    atPut("ode", "ode-devel")
)
