#!/usr/bin/perl

$JPL_SRC = "..";

use Config;

if ($^O eq 'MSWin32') {
    $CP = 'copy';
    require "$JPL_SRC/setvars.pl";
} else {
    $CP = 'cp';
    eval `$JPL_SRC/setvars -perl`;
}


if ($^O eq 'MSWin32') {
    $INSTDIR = "$Config{installarchlib}\\JPL";
    $MKDIR = "if not exist $INSTDIR mkdir $INSTDIR";
} else {
    $INSTDIR="$ENV{JPL_HOME}/perl/JPL";
    $MKDIR = "mkdir -p $INSTDIR";
}

open(MAKEFILE, ">Makefile");

print MAKEFILE <<"SUBS";
PERL = perl$]
ARCHNAME = $Config{archname}
JAVA_HOME = $ENV{JAVA_HOME}
JPL_HOME = $ENV{JPL_HOME}
PERLARCHDIR = $Config{archlib}

all:

debug:

test:

install:
	$MKDIR
	$CP *.pm $INSTDIR

clean:

SUBS

close MAKEFILE;
