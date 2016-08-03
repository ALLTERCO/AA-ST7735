#!/bin/bash
for f in "$@" ; do 
	echo "$f: test starts"
	rm -f bin.out rle.out RLE_DEC_TEST_out.bin
	./png2bin_st7735rc_test "$f"  bin.out rle.out
	if [ "$?" != "0" ] ; then 
		echo "file $f: encoding failed!"
		break
	fi
	#md5sum bin.out RLE_DEC_TEST_out.bin
	cmp  bin.out RLE_DEC_TEST_out.bin
	if [ "$?" != "0" ] ; then 
		echo "$f: comparisson failed!"
		break
	fi
	echo "$f: TESTED OK!"
	rm -f bin.out rle.out RLE_DEC_TEST_out.bin
done
