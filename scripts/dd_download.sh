#!/bin/sh

# make partition table by fdisk command
# reserve part for fex binaries download 0~204799
# partition1 /dev/sdc1 vfat 204800~327679
# partition2 /dev/sdc2 ext4 327680~end

die() {
        echo "$*" >&2
        exit 1
}

[ $# -eq 1 ] || die "Usage: $0 /dev/sdc"

[ -s "./env.sh" ] || die "please run ./configure first."

. ./env.sh

O=$1
PRELOADER=$TOPDIR/mt-pack/mtk/${TARGET_PRODUCT}/bin/preloader_iotg7623Np1_emmc.bin
UBOOT=$TOPDIR/u-boot-mt/u-boot.bin

sudo dd if=$PRELOADER 	of=$O bs=1k seek=2
sudo dd if=$UBOOT 	of=$O bs=1k seek=320

