#!/usr/bin/perl

$JPL_SRC = "..";

use ExtUtils::MakeMaker;
use Config;
$ARCHNAME = $Config{archname};
require "JNIConfig";


if ($LIBLOC) {
    $LIBPATH="-R$LIBLOC -L$LIBLOC " .
             "-R$Config{archlib}/CORE -L$Config{archlib}/CORE";
    $LIBS = "-lperl -l$LIBJVM";
}

$INC = join(" -I", ("", @INCLUDE));

if ($EMBEDDEDPERL) {
    $CCFLAGS .= " -DEMBEDDEDPERL";
    $LIBPATH .= " -R$jpl/lib/$ARCHNAME -L$jpl/lib/$ARCHNAME";
    $LIBS    .= " -lperl -lPerlInterpreter";
}
if ($KAFFE) {
    $CCFLAGS .= " -DKAFFE";
}

my %Makefile = (
    NAME	=> 'JNI',
    VERSION_FROM => 'JNI.pm',
    DEFINE	=> '',
    LINKTYPE => 'dynamic',
    INC	=> $INC,
    CCFLAGS => "$Config{ccflags} $CCFLAGS", 
);

if ($LIBS && $LIBPATH) {
    $Makefile{LIBS} = ["$LIBPATH $LIBS"];
}
if ($MYEXTLIB) {
    $Makefile{MYEXTLIB} = $MYEXTLIB;
}

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
#
WriteMakefile(%Makefile);
