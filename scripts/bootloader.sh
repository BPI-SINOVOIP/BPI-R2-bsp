#!/bin/sh

#gunzip -c BPI_M3_1080P.img.gz | dd of=/dev/mmcblk0 conv=sync,noerror bs=1k

die() {
        echo "$*" >&2
        exit 1
}

[ -s "./env.sh" ] || die "please run ./configure first."
. ./env.sh
O=$1
if [ ! -z $O ] ; then
	BOARD=$O
fi
U=$TOPDIR/out/${TARGET_PRODUCT}/100MB

mkdir -p $U
TMP_FILE=${U}/${BOARD}.tmp
IMG_FILE=${U}/${BOARD}-2k.img

(sudo dd if=/dev/zero of=${TMP_FILE} bs=1M count=1) >/dev/null 2>&1
LOOP_DEV=`sudo losetup -f --show ${TMP_FILE}`


PRELOADER=$TOPDIR/mt-pack/mtk/${TARGET_PRODUCT}/bin/preloader_iotg7623Np1_emmc.bin
UBOOT=$TOPDIR/u-boot-mt/u-boot.bin

(sudo dd if=$PRELOADER	of=${LOOP_DEV} bs=1k seek=2) >/dev/null 2>&1
(sudo dd if=$UBOOT	of=${LOOP_DEV} bs=1k seek=320) >/dev/null 2>&1

sudo sync

sudo losetup -d ${LOOP_DEV}

(dd if=${TMP_FILE} of=${IMG_FILE} bs=1k skip=2 count=1022 status=noxfer) >/dev/null 2>&1

rm -f ${IMG_FILE}.gz
echo "gzip ${IMG_FILE}"
gzip ${IMG_FILE}
rm -f ${TMP_FILE}
