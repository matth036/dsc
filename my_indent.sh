#!/bin/bash
#
# Indent in the style of the Linux Kernel EXCEPT use -i2 where the kernel uses -i8.
# 
/usr/bin/indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4  \
                -cli0 -d0 -di1 -nfc1 -i2 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
                -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 \
                $@
#
#
# Clean up dos-ish newlines.
# Use cp rather than mv because my /tmp is a mount point for a partition.
#
TEMPFILE=$( mktemp  /tmp/fromdos_temp.XXXXXX ) || exit 1
# echo $TEMPFILE
for file in $@; do
    fromdos < $file > $TEMPFILE
    cp $TEMPFILE $file
done
#
#
rm -f $TEMPFILE
#
# eof
