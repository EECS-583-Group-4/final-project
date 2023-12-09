#!/bin/sh

m=`uname -m`;
# if endianness argument is present use that. If no such argument is present,
# x86 or Itanium architectures default to little-endian all others
# default to big endian
if [ $# -gt 1 ]; then
  end=$2;
elif [ $m = "x86" -o $m = "x86_64" -o $m = "ia32" -o $m = "ia64" ]; then
  end="le";
else
  end="be";
fi

for file in `ls $1/*.$end.raw`
do
  x=`basename $file .$end.raw`;
  ln -s $file $x.raw;
done

