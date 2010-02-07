#!/bin/env python
#
"""
Replaces the <Files> section of an existing IoVM.vcproj with an
automatically generated section which assumes that all *.c and *.h
in "dir_iovm" and "subdirs" are involved in the build.

Instructions:

1) Place the script in the "projects\win32vc7\IoVM\" directory and execute.
  - an existing "IoVM.vcproj" is required and used as the base.
  - "IoVM-replaced_files.vcproj" is created.
   
2) rename "IoVM-replaced_files.vcproj" to "IoVM.vcproj" to use.

-- Kevin Edwards

2005-12-05 : created
"""

import os, sys, glob

# Load current .vcproj file to keep its other settings
file_in = open("IoVM.vcproj","r")
vcproj_in = file_in.read()
file_in.close()

# Settings
dir_io = os.path.join( "..", "..", ".." )
dir_iovm = os.path.join( dir_io, "vm" )
subdirs = ["base","SkipDB"]
#file_out = sys.stdout
file_out = open("IoVM-replaced_files.vcproj","w")

# create IoVMCode.c if it doesn't already exist
file_iovmcode = os.path.join( dir_iovm, "IoVMCode.c" )
if not os.path.isfile(file_iovmcode):
  os.close( os.open(file_iovmcode, os.O_CREAT, 0700) )
  # assign an old timestamp so make remakes it.
  os.utime( file_iovmcode, (0,0) )


def vcproj_subfilter(name, selection):
  """Generate a Filter Sub-Section."""
  
  lines = [ '\n  <Filter Name="%s" Filter="">' % (name) ]
  
  files = glob.glob(os.path.join(dir_iovm, name, selection))
  for f in files:
    f = f.replace("/","\\")
    lines.append( """<File RelativePath="%s"></File>""" % (f) )
  lines.append('  </Filter>\n')
  return lines


# Generate New <Files> Section
# --------------------------------------
lines = ["<Files>"]

# Source Files
# --------------------------------------
lines.append(
"""
<Filter
  Name="Source Files"
  Filter="cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
  UniqueIdentifier="{4FC737F1-C7A5-4376-A066-2A32D752A2FF}">
"""
)

files = glob.glob( os.path.join(dir_iovm, "*.c") )
for f in files:
  lines.append( '<File RelativePath="%s"></File>' % (f) )

# Subdirectories
for subdir in subdirs:
  lines += vcproj_subfilter(subdir,"*.c")

lines.append("</Filter>")


# Header Files
# --------------------------------------
lines.append(
"""
<Filter
  Name="Header Files"
  Filter="h;hpp;hxx;hm;inl;inc;xsd"
  UniqueIdentifier="{93995380-89BD-4b04-88EB-625FBE52EBFB}">
"""
)

files = glob.glob( os.path.join(dir_iovm, "*.h") )
for f in files:
  lines.append( '<File RelativePath="%s"></File>' % (f) )

# Subdirectories
for subdir in subdirs:
  lines += vcproj_subfilter(subdir,"*.h")

lines.append("</Filter>")

lines.append("</Files>")

vcproj_files_str = "\n".join(lines)


# Rewrite With New <Files> Section
# --------------------------------------

prefix_end = vcproj_in.find("<Files>")
postfix_start = vcproj_in.find("</Files>",prefix_end) + len("</Files>")

file_out.write( vcproj_in[:prefix_end] )
file_out.write( vcproj_files_str )
file_out.write( vcproj_in[postfix_start:] )

file_out.close()

