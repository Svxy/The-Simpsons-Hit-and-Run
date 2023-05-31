#!/bin/perl

(my $file, my $ext) = split(/\./, @ARGV[0]);

my @stats = stat(@ARGV[0]);

my $size = @stats[7];

$file = uc( $file );

printf "static const unsigned int ${file}_SIZE = $size;\n\n";
printf "static const char* ${file}_NAME = \"@ARGV[1]\";\n\n";
