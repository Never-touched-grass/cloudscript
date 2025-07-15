#!/usr/bin/perl
use strict;
use warnings;
use File::Basename;
my ($filepath) = @ARGV;
if(!$filepath) {
print "Enter the path to your Cloud script file: ";
chomp(my $filename = <STDIN>); 

my ($name, $path, $suffix) = fileparse($filename, qr/\.[^.]*/);

if (defined $suffix && lc($suffix) eq '.cl') {
    print "Valid .cl file: $filename\n\n";
    open(my $fh, '<', $filename) or die "Can't open file: $!";
    open(my $pipe, '|-', 'receiver.exe') or die "Failed to run receiver: $!";
    while (my $line = <$fh>) {
        print $pipe $line;
    }
    close($fh);
    close($pipe);
} else {
    print "ERROR: File must have a .cl extension.\n";
}
} else {
my ($name, $path, $suffix) = fileparse($filepath, qr/\.[^.]*/);

if (defined $suffix && lc($suffix) eq '.cl') {
    print "Valid .cl file: $filepath\n\n";
    open(my $fh, '<', $filepath) or die "Can't open file: $!";
    open(my $pipe, '|-', 'receiver.exe') or die "Failed to run receiver: $!";
    while (my $line = <$fh>) {
        print $pipe $line;
    }
    close($fh);
    close($pipe);
} else {
    print "ERROR: File must have a .cl extension.\n";
}
}