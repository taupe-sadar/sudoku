use strict;
use warnings;
use Data::Dumper;

$#ARGV == 0 or die "$0 input\n";

open(FILE, $ARGV[0]) or die "cannot open input";
my(@file)=<FILE>;
close(FILE);

my($flat)= uc(join("",@file));
my(@explode)=split(//,$flat);
my(%all_letters)=();
for(my($a)=0;$a<=$#explode;$a++)
{
    if( $explode[$a]=~m/(\w)/ && $explode[$a] ne "0" )
    {
	$all_letters{$1}=1;
    }
}
my(@mapping)=sort(keys(%all_letters));

for(my($a)=0;$a<=$#mapping;$a++)
{
    my($letter)= $mapping[$a];
    my($replace_with)=$a+1;
    $flat=~s/$letter/$replace_with/g;
}

print $flat;

