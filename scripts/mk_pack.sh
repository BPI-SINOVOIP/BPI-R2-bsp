#!/bin/bash

die() {
        echo "$*" >&2
        exit 1
}

[ -s "./env.sh" ] || die "please run ./configure first."

set -e

. ./env.sh

#exit 0

PACK_ROOT="$TOPDIR/mt-pack"
PLATFORM="linux"

pack_bootloader()
{
  BOARD=$1
  echo "MACH=$MACH, PLATFORM=$PLATFORM, TARGET_PRODUCT=${TARGET_PRODUCT} BOARD=$BOARD"
  $TOPDIR/scripts/bootloader.sh $BOARD
}

BOARDS=`(cd mt-pack/mtk/${TARGET_PRODUCT}/configs ; ls -1d BPI*)`
for IN in $BOARDS ; do
  pack_bootloader $IN
done 
