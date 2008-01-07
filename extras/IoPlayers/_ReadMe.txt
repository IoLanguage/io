
An IoPlayer is basically an executable that when launched:

- opens a window with a GL view

- looks in it's launch directory for a main.io file and executes it if found
  or opens an error dialog if not.

- glut input comes from the window and gl output goes to the window

- a debug window with a stacktrace opens upon errors.

The idea is to provide a binary release of Io that can be easily used to distribute software written in Io.


