#!/bin/bash
#--------------------------------------------------------------------------------------------------------------
# Copyright (c) 2017, Dolby Laboratories Inc.
# All rights reserved.

# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this list of conditions
#    and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
#    and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
#    promote products derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#--------------------------------------------------------------------------------------------------------------

#  Usage: test_spdif [binary directory]
#  NOTE: This must be run in the test directory
#  test_spdif.sh will look for the build binary in the directory specified
#  on the command line or, by default, it will look in ..\Release\<Arch>\bin\
#  where <Arch> is the architecture of the machine as supplied by uname


if [ "$1" = "/?" ]
then
	echo
	echo Usage: test_spdif [binary directory]
	echo NOTE: This must be run in the test directory
	echo test_spdif.sh will look for the build binary in the directory specified
	echo on the command line or, by default, it will look in ..\Release\<Arch>\bin\
	echo where <Arch> is the architecture of the machine as supplied by uname
	echo
	exit 1
fi

if [ "$1" = "" ]
then
	BINDIR="../Release"
else
	BINDIR="../$1"
fi

ARCH=`uname`
BIN="./$BINDIR/$ARCH/spdif"
REF="./ref/$ARCH/spdif"
LOG="./output/test_spdif.log"
IN="./input"
OUT="./output"
OUTREF="./output/ref"

echo $IN $BIN
if [ ! -d $IN ] || [ ! -f $BIN ]
then
	echo
	echo ===========================================================
	echo ERROR: test_spdif must be run in the root test directory.
	echo ===========================================================
	echo 
	exit 1
fi

mkdir -p ./output/ref


# clean outputs
rm -f $OUT/*.sdf
rm -f $OUT/*.ac3
rm -f $OUT%/*.ddp
rm -f $LOG

fail=0

echo "Testing AC3 inputs..."
echo "Testing AC3 inputs..." > $LOG

for in_file in $IN/*.ac3
do
	echo $in_file
	stem=`basename $in_file .ac3`
	$BIN -i$in_file -o$OUT/$stem.sdf 2>&1 | tee -a $LOG
	$BIN -d -i$OUT/$stem.sdf -o$OUT/"$stem"_out.ac3 2>&1 | tee -a $LOG
	if [ ! -f $OUTREF/$stem.sdf ]
	then
		$REF -i$in_file -o$OUTREF/$stem.sdf 2>&1 | tee -a $LOG
	fi
	diff -s $OUTREF/$stem.ac3 $OUT/"$stem"_out.ac3 2>&1 | tee -a $LOG 
	if [ ${PIPESTATUS[0]} -ne 0 ]
	then
		fail=1
		echo $stem [ident]	[FAILED] 2>&1 | tee -a $LOG
	else
		echo $stem [ident]	[PASSED] 2>&1 | tee -a $LOG
	fi

	diff $OUTREF/$stem.sdf $OUT/$stem.sdf 2>&1 | tee -a $LOG 
	if [ ${PIPESTATUS[0]} -ne 0 ]
	then
		fail=1
		echo $stem [ref]	[FAILED] 2>&1 | tee -a $LOG
	else
		echo $stem [ref]	[PASSED] 2>&1 | tee -a $LOG
	fi
done

echo
echo Testing alternative format
echo Testing alternative format 2>&1 | tee -a $LOG

for c in $IN/classical*.ac3
do
	stem=`basename $c .ac3`
	$BIN -a -i$c -o$OUT/"$stem"_alt.sdf 2>&1 | tee -a $LOG 
	$BIN -a -d -i$OUT/"$stem"_alt.sdf -o$OUT/"$stem"_out_alt.ac3 2>&1 | tee -a $LOG 
	if [ ! -f $OUTREF/"$stem"_alt.sdf ]
	then
		$REF -i$c -a -o$OUTREF/"$stem"_alt.sdf 2>&1 | tee -a $LOG 
	fi
	diff $OUTREF/"$stem"_alt.sdf $OUT/"$stem"_alt.sdf 2>&1 | tee -a $LOG 
	if [ ${PIPESTATUS[0]} -ne 0 ]
	then
		fail=1
		echo $stem [ref]	[FAILED] 2>&1 | tee -a $LOG
	else
		echo $stem [ref]	[PASSED] 2>&1 | tee -a $LOG
	fi
done

echo
echo Testing DDP inputs...
echo Testing DDP inputs... 2>&1 | tee -a $LOG

for c in $IN/*.ec3
do
	stem=`basename $c .ec3`
	$BIN -i$c -o$OUT/$stem.sdf 2>&1 | tee -a $LOG 
	$BIN -d -i$OUT/$stem.sdf -o$OUT/$stem_out.ec3 2>&1 | tee -a $LOG
	if [ ! -f $OUTREF/$stem.sdf ]
	then
		$REF -i$c -o$OUTREF/$stem.sdf 2>&1 | tee -a $LOG 
	fi
	diff $OUTREF/$stem.ec3 $OUT/$stem_out.ec3 2>&1 | tee -a $LOG 
	if [ ${PIPESTATUS[0]} -ne 0 ]
	then
		fail=1
		echo $stem [ident]	[FAILED] 2>&1 | tee -a $LOG
	else
		echo $stem [ident]	[PASSED] 2>&1 | tee -a $LOG
	fi

	diff $OUTREF/$stem.sdf $OUT/$stem.sdf 2>&1 | tee -a $LOG 
	if [ ${PIPESTATUS[0]} -ne 0 ]
	then
		fail=1
		echo $stem [ref]	[FAILED] 2>&1 | tee -a $LOG
	else
		echo $stem [ref]	[PASSED] 2>&1 | tee -a $LOG
	fi
done

echo
echo Testing errorhandling...

$BIN -i$IN/error/classical_14_a7_invsync.ac3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo Invalid Sync Word  [FAILED] 2>&1 | tee -a $LOG
else
	echo Invalid Sync Word  [PASSED] 2>&1 | tee -a $LOG
fi

$BIN -i$IN/error/classical_766_a7.ec3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -ne 0 ]
then
	fail=1
	echo Boundary case for large frame size [1532]  [FAILED] 2>&1 | tee -a $LOG
else
	echo Boundary case for large frame size [1532]  [PASSED] 2>&1 | tee -a $LOG
fi

$BIN -i$IN/error/errtesterr.ec3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo file not found  [FAILED] 2>&1 | tee -a $LOG
else
	echo file not found  [PASSED] 2>&1 | tee -a $LOG
fi


$BIN -i$IN/error/empty.ec3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo empty file  [FAILED] 2>&1 | tee -a $LOG
else
	echo empty file  [PASSED] 2>&1 | tee -a $LOG
fi

$BIN -a -i$IN/female_256_a2.ec3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo invalid alternative  [FAILED] 2>&1 | tee -a $LOG
else
	echo invalid alternative  [PASSED] 2>&1 | tee -a $LOG
fi


$BIN -i$IN/error/classical_10_a2_invfrcode.ac3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo invalid frame code  [FAILED] 2>&1 | tee -a $LOG
else
	echo invalid frame code  [PASSED] 2>&1 | tee -a $LOG
fi


$BIN -i$IN/error/classical_10_a2_invsamprate.ac3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo invalid sample rate  [FAILED] 2>&1 | tee -a $LOG
else
	echo invalid sample rate  [PASSED] 2>&1 | tee -a $LOG
fi


$BIN -i$IN/error/classical_16_32_a7_invbsid.ac3 -o$OUT/errortest.sdf 2>&1 | tee -a $LOG 
if [ ${PIPESTATUS[0]} -eq 0 ]
then
	fail=1
	echo invalid bsid  [FAILED] 2>&1 | tee -a $LOG
else
	echo invalid bsid  [PASSED] 2>&1 | tee -a $LOG
fi

if [ $fail -eq 0 ]
then
	echo
	echo ========================== 
	echo Spdif binary PASSED TEST 
	echo ========================== 
	echo
	echo Binary location: $BIN
	echo Results written to $OUT/test_spdif.log 
	exit 0
else
	echo
	echo ========================== 
	echo Spdif binary FAILED TEST 
	echo ========================== 
	echo
	echo Binary location: $BIN 
	echo Results written to $OUT/test_spdif.log 
	exit 1
fi






