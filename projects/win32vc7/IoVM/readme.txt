IoVM
====

IoVM.vcproj can become out of date when files are added or removed from the vm.

Use the included script to generate a new .vcproj with updated files, which can then be renamed to IoVM.vcproj to use.

_iovm_vcproj_replace_files.py : uses the existing IoVM.vcproj to create a new IoVM-replaced_files.vcproj with the <Files> section replaced.  This might even work with newer versions of .vcproj since the header is preserved.

There's also a cmd.exe version that has been tested on WinXP:

_iovm_vcproj_replace_files.cmd : does the same as the python version.
_iovm_vcproj_files.cmd : helper that generates the <Files> section.

Also:

runTests.bat : executes the unit tests using Debug\IoVM.exe

-- Kevin Edwards
2005-12-25 : created