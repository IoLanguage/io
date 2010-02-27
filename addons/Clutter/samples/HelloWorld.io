#!/usr/bin/env io
Clutter

# The problem is that this will seagfault! (call to clutter_actor_show, to be exact)
# BUT, if you execute the same come from within REPL,
# it will work!
ClutterStage default do(
  setTitle("HelloWorld")
  setColor("green" asClutterColor)
  setSize(512, 512)
) show

Clutter main