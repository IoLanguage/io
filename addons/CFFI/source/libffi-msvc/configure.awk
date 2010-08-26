#
# libffi - Copyright (c) 1996-2010  Anthony Green, Red Hat, Inc and others.
# See source files for details.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# ``Software''), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#
BEGIN {

    argc = 1;
    argp = ARGV[argc];
    options["version"]  = "3.1.10rc";
    options["target"]   = "X86_WIN32";
    options["prefix"]   = "";
    options["debug"]    = 0;
    options["static"]   = 1;
    options["shared"]   = 1;
    options["topdir"]   = ".";
    options["includes"] = "-I .";
    options["CC"]   = "cl.exe";
    options["AS"]   = "";
    options["RC"]   = "rc.exe";
    options["MT"]   = "mt.exe";
    options["AR"]   = "link.exe";
    options["LINK"] = "link.exe";
    options["CPP"]  = "cl.exe -EP";
    options["SO"]   = ".dll";
    options["LA"]   = ".lib";
    options["OBJ"]  = ".obj";
    options["EXE"]  = ".exe";

    options["LIBS"] = "kernel32.lib advapi32.lib shell32.lib user32.lib gdi32.lib"
    options["LDFLAGS"] = "/NOLOGO"
    options["SHFLAGS"] = "/DLL /INCREMENTAL:NO /DEBUG /SUBSYSTEM:WINDOWS"
    options["ARFLAGS"] = "/LIB"
    options["RCFLAGS"] = "/l 0x409"
    options["CFLAGS"] = "-nologo -Zi -D_MD -W3 -DWIN32 -DWINNT " \
                        "-D_WIN32 -D_WINDOWS -D_WINNT -D_WIN32_WINNT=0x0501 "  \
                        "-D_WIN32_IE=0x0600"

    while (argp > "") {
        if (gsub(/^\//, "", argp) <= 0)
            break;
        split(argp, av, "=");
        if (av[2] == "") {
            if (av[1] == "help") {
                usage();
                exit 0;
            }
            else if (av[1] == "disable-static") {
                av[2] = 1;
                options["static"] = 0;
            }
            else if (av[1] == "debug")
                av[2] = 1;
            else {
                print "Configure error: Unknown directive '/" av[1] "'";
                usage();
                exit 1;
            }
        }
        options[av[1]] = av[2];
        argc = argc + 1;
        argp = ARGV[argc];
    }
    split(options["version"], version, ".");
    vermajor = version[1];
    verminor = version[2];
    verpatch = version[3];

    if (options["name"] == "") {
        options["name"] = "ffi-" vermajor;
    }
    options["includes"] = options["includes"] " -I " options["topdir"] "\\include";
    options["includes"] = options["includes"] " -I " options["topdir"] "\\src\\x86";
    if (options["debug"] > 0) {
        options["MTFLAGS"] = " -DDEBUG -D_DEBUG -MDd -Od";
        options["RCFLAGS"] = options["RCFLAGS"] " -DDEBUG -D_DEBUG";
    }
    else {
        options["MTFLAGS"] = " -DNDEBUG -MD -O2 -Ob2";
        options["RCFLAGS"] = options["RCFLAGS"] " -DNDEBUG";
    }
    if (options["target"] == "X86_WIN32") {
        options["CFLAGS"]  = options["CFLAGS"] " -D_X86_=1";
        options["MTFLAGS"] = options["MTFLAGS"] " -Oy-"
        options["LDFLAGS"] = options["LDFLAGS"] " /MACHINE:X86";
        options["ASFLAGS"] = "-coff -W3 -Cx -Zm -Di386 -DQUIET -D?QUIET";
        if (options["AS"] == "")
            options["AS"] = "ml.exe";
        if (options["debug"] == 0) {
            options["SHFLAGS"] = options["SHFLAGS"] " /OPT:REF";
        }
    }
    else if (options["target"] == "X86_WIN64") {
        options["CFLAGS"]  = options["CFLAGS"] " -D_AMD64_=1 -GS-";
        options["LDFLAGS"] = options["LDFLAGS"] " /MACHINE:AMD64";
        options["ASFLAGS"] = "-W3 -Cx -Dx86_64 -DQUIET -D?QUIET";
        if (options["AS"] == "")
            options["AS"] = "ml64.exe";
    }
    else {
        print "Configure error: Target '" options["target"] "' not supported!";
        usage();
        exit 1;

    }
    targets = "$(SHAREDLIB)";
    if (options["static"] > 0)
        targets = targets " $(STATICLIB)";
    options["CFLAGS"] = options["CFLAGS"] options["MTFLAGS"];

    print "Configuring '" options["name"] "' for " options["target"] " ...";

    frewrite("include/ffi.h");
    frewrite("build/libffi.rc");
    frewrite("build/libffi.def");
    frewrite("Makefile.vc");

}

function frewrite(dstfile) {

    srcfile = dstfile ".in";

    while ((getline < srcfile) > 0) {

        gsub( /@VERSION_MAJOR@/, vermajor);
        gsub( /@VERSION_MINOR@/, verminor);
        gsub( /@VERSION_PATCH@/, verpatch);
        gsub( /@HAVE_LONG_DOUBLE@/, "0");
        gsub( /@name@/, options["name"]);
        gsub( /@prefix@/, options["prefix"]);
        gsub( /@VERSION@/, options["version"]);
        gsub( /@TARGET@/, options["target"]);
        gsub( /@topdir@/, options["topdir"]);

        gsub( /@as@/, options["AS"]);
        gsub( /@ar@/, options["AR"]);
        gsub( /@cc@/, options["CC"]);
        gsub( /@cpp@/, options["CPP"]);
        gsub( /@link@/, options["LINK"]);
        gsub( /@mt@/, options["MT"]);
        gsub( /@rc@/, options["RC"]);

        gsub( /@exe@/, options["EXE"]);
        gsub( /@obj@/, options["OBJ"]);
        gsub( /@so@/, options["SO"]);
        gsub( /@la@/, options["LA"]);

        gsub( /@libs@/, options["LIBS"]);
        gsub( /@cflags@/, options["CFLAGS"]);
        gsub( /@ldflags@/, options["LDFLAGS"]);
        gsub( /@shflags@/, options["SHFLAGS"]);
        gsub( /@arflags@/, options["ARFLAGS"]);
        gsub( /@asflags@/, options["ASFLAGS"]);
        gsub( /@rcflags@/, options["RCFLAGS"]);

        gsub( /@include@/, options["includes"]);
        gsub( /@targets@/, targets);

        print $0 > dstfile;
    }
    close(srcfile);

    if (close(dstfile) >= 0) {
        print "Rewrote " srcfile "\n to " dstfile;
    }
    else {
        print "Failed to rewrite " srcfile "\n to " dstfile;

    }

}

function usage()
{
    print "Usage: awk -f configure.awk [options]\n";
    print "Where options are:"
    print "    /target=X86_WIN32|X86_WIN64      Define build target";
    print "                                         [X86_WIN32]";
    print "    /debug                           Enable debug mode compilation";
    print "                                         [disabled]";
    print "    /disable-static                  Disable static library build";
    print "                                         [enabled]";
    print "    /version=major.minor.patch       Define library version";
    print "                                         [" options["version"] "]";
    print "    /name=<library name>             Define library name to produce";
    print "                                         [libffi-3]";
    print "    /topdir=<path>                   Define top build directory";
    print "                                         [.]";
    print "    /prefix=<path>                   Define destination prefix";
    print "                                         [none]";
    print "\nTool options:";
    print "    /CC=C                            Use C compiler instead default one";
    print "                                         [cl.exe]";
    print "    /CPP=CPP                         Use CPP preprocessor instead default one";
    print "                                         [cl.exe - nologo -EP]";
    print "    /RC=RC                           Use RC resource compiler instead default one";
    print "                                         [rc.exe]";
    print "    /LINK=LINK                       Use LINK linker instead default one";
    print "                                         [link.exe]";
    print "    /AR=AR                           Use AR librarian instead default one";
    print "                                         [lib.exe]";

}
