
#ifndef __NAND_DEVICE_LIST_H__
#define __NAND_DEVICE_LIST_H__

#define NAND_MAX_ID		5
#define CHIP_CNT		4
#define P_SIZE		16384
#define P_PER_BLK		256
#define C_SIZE		8192
#define RAMDOM_READ		(1<<0)
#define CACHE_READ		(1<<1)

#define RAND_TYPE_SAMSUNG 0
#define RAND_TYPE_TOSHIBA 1
#define RAND_TYPE_NONE 2

#define READ_RETRY_MAX 10
struct gFeature
{
	u32 address;
	u32 feature;
};

enum readRetryType
{
	RTYPE_MICRON,
	RTYPE_SANDISK,
	RTYPE_SANDISK_19NM,
	RTYPE_TOSHIBA,
	RTYPE_HYNIX,
	RTYPE_HYNIX_16NM
};

struct gFeatureSet
{
	u8 sfeatureCmd;
	u8 gfeatureCmd;
	u8 readRetryPreCmd;
	u8 readRetryCnt;
	u32 readRetryAddress;
	u32 readRetryDefault;
	u32 readRetryStart;
	enum readRetryType rtype;
	struct gFeature Interface;
	struct gFeature Async_timing;
};

struct gRandConfig
{
	u8 type;
	u32 seed[6];
};

enum pptbl
{
	MICRON_8K,
	HYNIX_8K,
	SANDISK_16K,
	PPTBL_NOT_SUPPORT,
};

struct MLC_feature_set
{
	enum pptbl ptbl_idx;
	struct gFeatureSet 	 FeatureSet;
	struct gRandConfig   randConfig;
};

enum flashdev_vendor
{
	VEND_SAMSUNG,
	VEND_MICRON,
	VEND_TOSHIBA,
	VEND_HYNIX,
	VEND_SANDISK,
	VEND_BIWIN,
	VEND_NONE,
};

enum flashdev_IOWidth
{
	IO_8BIT = 8,
	IO_16BIT = 16,
	IO_TOGGLEDDR = 9,
	IO_TOGGLESDR = 10,
	IO_ONFI = 12,
};

typedef struct
{
   u8 id[NAND_MAX_ID];
   u8 id_length;
   u8 addr_cycle;
   enum flashdev_IOWidth iowidth;
   u16 totalsize;
   u16 blocksize;
   u16 pagesize;
   u16 sparesize;
   u32 timmingsetting;
   u32 dqs_delay_ctrl;
   u32 s_acccon;
   u32 s_acccon1;
   u32 freq;
   enum flashdev_vendor vendor;
   u16 sectorsize;
   u8 devciename[30];
   u32 advancedmode;
   struct MLC_feature_set feature_set;
}flashdev_info,*pflashdev_info;

static const flashdev_info gen_FlashTable[]={
	{{0xC8,0xDA,0x90,0x95,0x44, 0x00}, 5,5,IO_8BIT,256,128,2048,64,0x30c77fff, 0x60000, 0xC03222,0x101,80,VEND_NONE,512, "F59L2G81A",0 ,
		{PPTBL_NOT_SUPPORT, {0xEF,0xEE,0xFF,16,0x11,0,1,RTYPE_MICRON,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_NONE,{0x2D2D,1,1,1,1,1}}}},
	{{0xC8,0xDC,0x90,0x95,0x54, 0x00}, 5,5,IO_8BIT,256,128,2048,64,0x30c77fff, 0x60000, 0xC03222,0x101,80,VEND_NONE,512, "F59L4G81A",0 ,
		{PPTBL_NOT_SUPPORT, {0xEF,0xEE,0xFF,16,0x11,0,1,RTYPE_MICRON,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_NONE,{0x2D2D,1,1,1,1,1}}}},
	{{0x1,0xD3,0xD1,0x95,0x58,0x00}, 5, 5, IO_8BIT,1024, 128, 2048,64, 0x30c77fff, 0x60000, 0xC03222,0x101,80,VEND_NONE,512, "S34ML08G101TFI",0 ,
	        {PPTBL_NOT_SUPPORT, {0xEF,0xEE,0xFF,16,0x11,0,1,RTYPE_MICRON,{0x80, 0x00},{0x80, 0x01}},
	        {RAND_TYPE_NONE,{0x2D2D,1,1,1,1,1}}}},	  
	{{0x02C,0xD3,0x90,0xA6,0x64,0x00}, 5, 5, IO_8BIT,1024, 256, 4096,224, 0x30c77fff, 0x60000, 0xC03222,0x101,80,VEND_MICRON,512, "MT29F8G08ABACA",0 ,
	        {PPTBL_NOT_SUPPORT, {0xEF,0xEE,0xFF,16,0x11,0,1,RTYPE_MICRON,{0x80, 0x00},{0x80, 0x01}},
	        {RAND_TYPE_NONE,{0x2D2D,1,1,1,1,1}}}},
	{{0x98,0xDC,0x91,0x26,0x76,0x00}, 5, 5, IO_8BIT,1024, 256, 4096,256, 0x30c77fff, 0x60000, 0xC03222,0x101,80,VEND_TOSHIBA,512, "TC58NVG2S0HTAI0",0 ,
	        {PPTBL_NOT_SUPPORT, {0xEF,0xEE,0xFF,16,0x11,0,1,RTYPE_MICRON,{0x80, 0x00},{0x80, 0x01}},
	        {RAND_TYPE_NONE,{0x2D2D,1,1,1,1,1}}}},
	{{0xAD, 0xDE, 0x14, 0xA7, 0x42} ,  5, 5, IO_TOGGLESDR, 8192, 4096, 16384, 1280, 0x10804222,  0x60000, 0x33418010, 0x01010100, 100, VEND_HYNIX, 1024,  "H27UCG8T2ETR", 0 ,
		{SANDISK_16K,  {0xEF, 0xEE, 0xFF, 8, 0xFF, 0, 0, RTYPE_HYNIX_16NM, {0x01,  0x20} , {0x01,  0x00} } ,
		{RAND_TYPE_SAMSUNG, {0x2D2D, 1, 1, 1, 1, 1}}}},
};

#endif
