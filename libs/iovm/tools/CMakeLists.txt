# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# io2c -- Take an Io script, and "compile" it into C source
# code. I use the term loosely, since all it does is make a
# big C String, and pipe that through IoState's evaluator.
# Nevertheless, this is it!

# Just build the bloody binary!
add_executable(io2c io2c.c)
