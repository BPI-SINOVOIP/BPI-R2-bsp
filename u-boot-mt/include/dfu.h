/*
 * dfu.h - DFU flashable area description
 *
 * Copyright (C) 2012 Samsung Electronics
 * authors: Andrzej Pietrasiewicz <andrzej.p@samsung.com>
 *	    Lukasz Majewski <l.majewski@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __DFU_ENTITY_H_
#define __DFU_ENTITY_H_

#include <common.h>
#include <linux/list.h>
#include <mmc.h>
#include <linux/usb/composite.h>

enum dfu_device_type {
	DFU_DEV_MMC = 1,
	DFU_DEV_ONENAND,
	DFU_DEV_NAND,
	DFU_DEV_RAM,
};

enum dfu_layout {
	DFU_RAW_ADDR = 1,
	DFU_FS_FAT,
	DFU_FS_EXT2,
	DFU_FS_EXT3,
	DFU_FS_EXT4,
	DFU_RAM_ADDR,
};

enum dfu_op {
	DFU_OP_READ = 1,
	DFU_OP_WRITE,
};

struct mmc_internal_data {
	/* RAW programming */
	unsigned int lba_start;
	unsigned int lba_size;
	unsigned int lba_blk_size;

	/* FAT/EXT */
	unsigned int dev;
	unsigned int part;
};

struct nand_internal_data {
	/* RAW programming */
	u64 start;
	u64 size;

	unsigned int dev;
	unsigned int part;
	/* for nand/ubi use */
	unsigned int ubi;
};

struct ram_internal_data {
	void		*start;
	unsigned int	size;
};

static inline unsigned int get_mmc_blk_size(int dev)
{
	return find_mmc_device(dev)->read_bl_len;
}

#define DFU_NAME_SIZE			32
#define DFU_CMD_BUF_SIZE		128
#ifndef CONFIG_SYS_DFU_DATA_BUF_SIZE
#define CONFIG_SYS_DFU_DATA_BUF_SIZE		(1024*1024*8)	/* 8 MiB */
#endif
#ifndef CONFIG_SYS_DFU_MAX_FILE_SIZE
#define CONFIG_SYS_DFU_MAX_FILE_SIZE CONFIG_SYS_DFU_DATA_BUF_SIZE
#endif
#ifndef DFU_DEFAULT_POLL_TIMEOUT
#define DFU_DEFAULT_POLL_TIMEOUT 0
#endif

struct dfu_entity {
	char			name[DFU_NAME_SIZE];
	int                     alt;
	void                    *dev_private;
	int                     dev_num;
	enum dfu_device_type    dev_type;
	enum dfu_layout         layout;

	union {
		struct mmc_internal_data mmc;
		struct nand_internal_data nand;
		struct ram_internal_data ram;
	} data;

	int (*read_medium)(struct dfu_entity *dfu,
			u64 offset, void *buf, long *len);

	int (*write_medium)(struct dfu_entity *dfu,
			u64 offset, void *buf, long *len);

	int (*flush_medium)(struct dfu_entity *dfu);

	struct list_head list;

	/* on the fly state */
	u32 crc;
	u64 offset;
	int i_blk_seq_num;
	u8 *i_buf;
	u8 *i_buf_start;
	u8 *i_buf_end;
	long r_left;
	long b_left;

	u32 bad_skip;	/* for nand use */

	unsigned int inited:1;
};

int dfu_config_entities(char *s, char *interface, int num);
void dfu_free_entities(void);
void dfu_show_entities(void);
int dfu_get_alt_number(void);
const char *dfu_get_dev_type(enum dfu_device_type t);
const char *dfu_get_layout(enum dfu_layout l);
struct dfu_entity *dfu_get_entity(int alt);
char *dfu_extract_token(char** e, int *n);
void dfu_trigger_reset(void);
int dfu_get_alt(char *name);
bool dfu_reset(void);
int dfu_init_env_entities(char *interface, int dev);
unsigned char *dfu_get_buf(void);
unsigned char *dfu_free_buf(void);
unsigned long dfu_get_buf_size(void);

int dfu_read(struct dfu_entity *de, void *buf, int size, int blk_seq_num);
int dfu_write(struct dfu_entity *de, void *buf, int size, int blk_seq_num);
/* Device specific */
#ifdef CONFIG_DFU_MMC
extern int dfu_fill_entity_mmc(struct dfu_entity *dfu, char *s);
#else
static inline int dfu_fill_entity_mmc(struct dfu_entity *dfu, char *s)
{
	puts("MMC support not available!\n");
	return -1;
}
#endif

#ifdef CONFIG_DFU_NAND
extern int dfu_fill_entity_nand(struct dfu_entity *dfu, char *s);
#else
static inline int dfu_fill_entity_nand(struct dfu_entity *dfu, char *s)
{
	puts("NAND support not available!\n");
	return -1;
}
#endif

#ifdef CONFIG_DFU_RAM
extern int dfu_fill_entity_ram(struct dfu_entity *dfu, char *s);
#else
static inline int dfu_fill_entity_ram(struct dfu_entity *dfu, char *s)
{
	puts("RAM support not available!\n");
	return -1;
}
#endif

#ifdef CONFIG_DFU_FUNCTION
int dfu_add(struct usb_configuration *c);
#else
int dfu_add(struct usb_configuration *c)
{
	return 0;
}
#endif
#endif /* __DFU_ENTITY_H_ */
