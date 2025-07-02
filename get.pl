#!/usr/bin/perl
use strict;
use warnings;
use File::Basename;

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
    print "Error: File must have a .cl extension.\n";
}
