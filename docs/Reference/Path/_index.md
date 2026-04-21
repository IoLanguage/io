# Path

Path manipulation: joining, splitting, absolute/relative conversion, and tilde expansion.

- **category**: FileSystem

## absolute

Returns an absolute version of the path.

## hasDriveLetters

returns true if the platform requires DOS C: style drive letters.

## isPathAbsolute

Returns true if path is absolute, false if it is relative.

## thisSourceFilePath

Uses call message label to return an absolute path to the file that contains the sender.

## with(aSequence)

Returns a new Path object for the given Sequence.

