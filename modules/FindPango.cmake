# Copyright (c) 2009, Whispersoft s.r.l.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# * Neither the name of Whispersoft s.r.l. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Finds Pango library
#
#  Pango_1_0_INCLUDE_DIR - where to find pango.h, etc.
#  Pango_1_0_LIBRARIES   - List of libraries when using Pango.
#  Pango_1_0_FOUND       - True if Pango found.
#

if (Pango_1_0_INCLUDE_DIR)
  # Already in cache, be silent
  set(Pango_1_0_FIND_QUIETLY TRUE)
endif (Pango_1_0_INCLUDE_DIR)

find_path(Pango_1_0_INCLUDE_DIR pango/pango.h
  /opt/local/include
  /usr/local/include
  /usr/include
  /opt/local/include/pango-1.0
  /usr/local/include/pango-1.0
  /usr/include/pango-1.0
)

set(Pango_1_0_NAMES pango-1.0)
find_library(Pango_1_0_LIBRARY
  NAMES ${Pango_1_0_NAMES}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
)

if (Pango_1_0_INCLUDE_DIR AND Pango_1_0_LIBRARY)
   set(Pango_1_0_FOUND TRUE)
   set( Pango_1_0_LIBRARIES ${Pango_1_0_LIBRARY} )
else (Pango_1_0_INCLUDE_DIR AND Pango_1_0_LIBRARY)
   set(Pango_1_0_FOUND FALSE)
   set(Pango_1_0_LIBRARIES)
endif (Pango_1_0_INCLUDE_DIR AND Pango_1_0_LIBRARY)

if (Pango_1_0_FOUND)
   if (NOT Pango_1_0_FIND_QUIETLY)
      message(STATUS "Found Pango_1_0: ${Pango_1_0_LIBRARY}")
   endif (NOT Pango_1_0_FIND_QUIETLY)
else (Pango_1_0_FOUND)
   if (Pango_1_0_FIND_REQUIRED)
      message(STATUS "Looked for Pango_1_0 libraries named ${Pango_1_0_NAMES}.")
      message(STATUS "Include file detected: [${Pango_1_0_INCLUDE_DIR}].")
      message(STATUS "Lib file detected: [${Pango_1_0_LIBRARY}].")
      message(FATAL_ERROR "=========> Could NOT find Pango_1_0 library")
   endif (Pango_1_0_FIND_REQUIRED)
endif (Pango_1_0_FOUND)

mark_as_advanced(
  Pango_1_0_LIBRARY
  Pango_1_0_INCLUDE_DIR
  )
