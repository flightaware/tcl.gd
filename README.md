This is tcl.gd, a new Tcl interface to GD that is significantly more feature-
complete with gd 2 than Gdtclft.

Version 1.2

This package is a freely available open source package under the "Berkeley"
license, same as Tcl.  You can do virtually anything you like with it, such as 
modifying it, redistributing it, and selling it either in whole or in part.  
See the file "license.terms" for complete information.

Gdtcl was written by Karl Lehenbauer.

## Using tcl.gd

```tcl
package require tclgd
```

## tclgd quick reference

Please see the tclgd manpage in the doc directory.  There's a pdf there too.

## Examples

A number of simple examples of the use of tcl.gd can be found in the playpen directory.

## Bugs

Probably.

## Integrating tclgd with existing C applications that use gd

You can attach an existing gd handle to a tcl.gd image manipulation command using tclgd_newGDObjectAttach.  This provides a way to grab a GD image created by some library that you've linked your application with and manipulate it from Tcl using tcl.gd.  Please see the tclgd_newGDObjectAttach manpage for details.

## mapserver mapscript patch

Included with tcl.gd is a patch to Mapserver's mapscript Tcl extension to use the aforementioned capability for integrating tcl.gd with C aplications that use gd to provide a tcl.gd capability for mapscript.

Consult the README file in the mapserver-patches subdirectory for details on how to install the mapserver patch to add this capability to mapserver.


## Contents

Makefile.in	Makefile template.  The configure script uses this file to produce the final Makefile.

README.md		This file

aclocal.m4	Generated file.  Do not edit.  Autoconf uses this as input when generating the final configure script.  See "tcl.m4" below.

configure	Generated file.  Do not edit.  This must be regenerated anytime configure.in or tclconfig/tcl.m4 changes.

configure.in	Configure script template.  Autoconf uses this file as input to produce the final configure script.

generic/tclgd.c		Interface between Tcl and GD
generic/tclgd.h		include file
generic/tclgdio.c	Channel interface to allow GD-oriented I/O to Tcl channels.
generic/tclgdtcl.c	Init routines.


tclconfig/	This directory contains various template files that build the configure script.  They should not need modification.

	install-sh	Program used for copying binaries and script files to their install locations.

	tcl.m4		Collection of Tcl autoconf macros.  Included by aclocal.m4 to define SC_* macros.

## Building on Unix / Linux

Building under most Unix systems is easy, just run the configure script and then run make. 

```sh
cd tcl.gd
./configure
make
make install
```

## Windows build

tcl.gd has not been built under Windows at this time.

The recommended method to build extensions under Windows is to use the Msys + Mingw build process. This provides a Unix-style build while generating native Windows binaries. Using the Msys + Mingw build tools means that you can use the same configure script as per the Unix build to create a Makefile.

If you have VC++, then you may wish to use the files in the win subdirectory and build the extension using just VC++. 

Instructions for using the VC++ makefile are written in the first part of the Makefile.vc file.

## Installation

The install looks like so:

         $exec_prefix
          /       \
        lib       bin
         |         |
   PACKAGEx.y   (dependent .dll files on Windows)
         |
  pkgIndex.tcl (.so|.dll files)

The main .so|.dll library file gets installed in the versioned PACKAGE directory, which is OK on all platforms because it will be directly referenced with by 'load' in the pkgIndex.tcl file.  Dependent DLL files on Windows must go in the bin directory (or other directory on the user's PATH) in order for them to be found.

