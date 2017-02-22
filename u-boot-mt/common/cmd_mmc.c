/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <mmc.h>

static int curr_device = -1;
#ifndef CONFIG_GENERIC_MMC
static int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mmc (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int dev;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (strcmp(argv[1], "init") == 0) {
		if (argc == 2) {
			if (curr_device < 0)
				dev = 1;
			else
				dev = curr_device;
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		if (mmc_legacy_init(dev) != 0) {
			puts("No MMC card found\n");
			return 1;
		}

		curr_device = dev;
		printf("mmc%d is available\n", curr_device);
	} else if (strcmp(argv[1], "device") == 0) {
		if (argc == 2) {
			if (curr_device < 0) {
				puts("No MMC device available\n");
				return 1;
			}
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);

#ifdef CONFIG_SYS_MMC_SET_DEV
			if (mmc_set_dev(dev) != 0)
				return 1;
#endif
			curr_device = dev;
		} else {
			return CMD_RET_USAGE;
		}

		printf("mmc%d is current device\n", curr_device);
	} else {
	    printf("mmc%d operation\n", curr_device);
		if (do_mmcops(cmdtp,flag,argc,argv) == 1) return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	mmc, 6, 1, do_mmc,
	"MMC sub-system",
	"init [dev] - init MMC sub system \n"
	"mmc device [dev] - show or set current device\n"
	"mmc read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
);

#if 1  
enum mmc_state {
	MMC_INVALID,
	MMC_READ,
	MMC_WRITE,
	MMC_ERASE,
};
static void print_mmcinfo(struct mmc *mmc)
{
	printf("Device: %s\n", mmc->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 8) & 0xf, mmc->version & 0xff);

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	print_size(mmc->capacity, "\n");

	printf("Bus Width: %d-bit\n", mmc->bus_width);
}

static int do_mmcinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;

#if 1
	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}
#endif

	mmc = find_mmc_device(curr_device);

	if (mmc) {
		mmc_init(mmc);

		print_mmcinfo(mmc);
		return 0;
	} else {
		printf("no mmc device at slot %x\n", curr_device);
		return 1;
	}
}

U_BOOT_CMD(
	mmcinfo, 1, 0, do_mmcinfo,
	"display MMC info",
	"- display info of the current MMC device"
);

#ifdef DUMP_ONE_BLOCK
void dump_one_block(char *addr) {
    unsigned long i = 0;
    char *p = addr;
    for ( i = 0 ; i < 512 ; i+=8) {
        printf("[0x%lx] %02x %02x %02x %02x %02x %02x %02x %02x\n", 
            (unsigned long)p + i,
            p[i],
            p[i+1],
            p[i+2],
            p[i+3],
            p[i+4],
            p[i+5],
            p[i+6],
            p[i+7]);
        }
}
#else
void dump_one_block(char *addr){}
#endif

static int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	enum mmc_state state;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}
#if 0
	if (strcmp(argv[1], "rescan") == 0) {
		struct mmc *mmc;

		if (argc != 2)
			return CMD_RET_USAGE;

		mmc = find_mmc_device(curr_device);
		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		mmc->has_init = 0;

		if (mmc_init(mmc))
			return 1;
		else
			return 0;
	} else if (strcmp(argv[1], "part") == 0) {
		block_dev_desc_t *mmc_dev;
		struct mmc *mmc;

		if (argc != 2)
			return CMD_RET_USAGE;

		mmc = find_mmc_device(curr_device);
		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}
		mmc_init(mmc);
		mmc_dev = mmc_get_dev(curr_device);
		if (mmc_dev != NULL &&
				mmc_dev->type != DEV_TYPE_UNKNOWN) {
			print_part(mmc_dev);
			return 0;
		}

		puts("get mmc type error!\n");
		return 1;
	} else if (strcmp(argv[1], "list") == 0) {
		if (argc != 2)
			return CMD_RET_USAGE;
		print_mmc_devices('\n');
		return 0;
	} else if (strcmp(argv[1], "dev") == 0) {
		int dev, part = -1;
		struct mmc *mmc;

		if (argc == 2)
			dev = curr_device;
		else if (argc == 3)
			dev = simple_strtoul(argv[2], NULL, 10);
		else if (argc == 4) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
			part = (int)simple_strtoul(argv[3], NULL, 10);
			if (part > PART_ACCESS_MASK) {
				printf("#part_num shouldn't be larger"
					" than %d\n", PART_ACCESS_MASK);
				return 1;
			}
		} else
			return CMD_RET_USAGE;

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		mmc_init(mmc);
		if (part != -1) {
			int ret;
			if (mmc->part_config == MMCPART_NOAVAILABLE) {
				printf("Card doesn't support part_switch\n");
				return 1;
			}

			if (part != mmc->part_num) {
				ret = mmc_switch_part(dev, part);
				if (!ret)
					mmc->part_num = part;

				printf("switch to partitions #%d, %s\n",
						part, (!ret) ? "OK" : "ERROR");
			}
		}
		curr_device = dev;
		if (mmc->part_config == MMCPART_NOAVAILABLE)
			printf("mmc%d is current device\n", curr_device);
		else
			printf("mmc%d(part %d) is current device\n",
				curr_device, mmc->part_num);

		return 0;
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	} else if (strcmp(argv[1], "partconf") == 0) {
		int dev;
		struct mmc *mmc;
		u8 ack, part_num, access;

		if (argc == 6) {
			dev = simple_strtoul(argv[2], NULL, 10);
			ack = simple_strtoul(argv[3], NULL, 10);
			part_num = simple_strtoul(argv[4], NULL, 10);
			access = simple_strtoul(argv[5], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		if (IS_SD(mmc)) {
			puts("PARTITION_CONFIG only exists on eMMC\n");
			return 1;
		}

		/* acknowledge to be sent during boot operation */
		return mmc_set_part_conf(mmc, ack, part_num, access);
	} else if (strcmp(argv[1], "bootbus") == 0) {
		int dev;
		struct mmc *mmc;
		u8 width, reset, mode;

		if (argc == 6) {
			dev = simple_strtoul(argv[2], NULL, 10);
			width = simple_strtoul(argv[3], NULL, 10);
			reset = simple_strtoul(argv[4], NULL, 10);
			mode = simple_strtoul(argv[5], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		if (IS_SD(mmc)) {
			puts("BOOT_BUS_WIDTH only exists on eMMC\n");
			return 1;
		}

		/* acknowledge to be sent during boot operation */
		return mmc_set_boot_bus_width(mmc, width, reset, mode);
	} else if (strcmp(argv[1], "bootpart-resize") == 0) {
		int dev;
		struct mmc *mmc;
		u32 bootsize, rpmbsize;

		if (argc == 5) {
			dev = simple_strtoul(argv[2], NULL, 10);
			bootsize = simple_strtoul(argv[3], NULL, 10);
			rpmbsize = simple_strtoul(argv[4], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		if (IS_SD(mmc)) {
			printf("It is not a EMMC device\n");
			return 1;
		}

		if (0 == mmc_boot_partition_size_change(mmc,
							bootsize, rpmbsize)) {
			printf("EMMC boot partition Size %d MB\n", bootsize);
			printf("EMMC RPMB partition Size %d MB\n", rpmbsize);
			return 0;
		} else {
			printf("EMMC boot partition Size change Failed.\n");
			return 1;
		}
#endif /* CONFIG_SUPPORT_EMMC_BOOT */
	}

	else if (argc == 3 && strcmp(argv[1], "setdsr") == 0) {
		struct mmc *mmc = find_mmc_device(curr_device);
		u32 val = simple_strtoul(argv[2], NULL, 16);
		int ret;

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}
		ret = mmc_set_dsr(mmc, val);
		printf("set dsr %s\n", (!ret) ? "OK, force rescan" : "ERROR");
		if (!ret) {
			mmc->has_init = 0;
			if (mmc_init(mmc))
				return 1;
			else
				return 0;
		}
		return ret;
	}
#endif
	state = MMC_INVALID;
	if (argc == 5 && strcmp(argv[1], "read") == 0)
		state = MMC_READ;
	else if (argc == 5 && strcmp(argv[1], "write") == 0)
		state = MMC_WRITE;
	else if (argc == 4 && strcmp(argv[1], "erase") == 0)
#if 1
    state = MMC_ERASE;
#else
    {
        printf("we doesn't support mmc erase\n");
        return 1; 
    }
#endif

	if (state != MMC_INVALID) {
		struct mmc *mmc = find_mmc_device(curr_device);
		int idx = 2;
		u32 blk, cnt, n;
		void *addr;

		if (state != MMC_ERASE) {
			addr = (void *)simple_strtoul(argv[idx], NULL, 16);
			++idx;
		} else
			addr = NULL;
		blk = simple_strtoul(argv[idx], NULL, 16);
		cnt = simple_strtoul(argv[idx + 1], NULL, 16);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		printf("\nMMC %s: dev # %d, block # %d, count %d ... ",
				argv[1], curr_device, blk, cnt);

		mmc_init(mmc);

		if ((state == MMC_WRITE || state == MMC_ERASE)) {
			if (mmc_getwp(mmc) == 1) {
				printf("Error: card is write protected!\n");
				return 1;
			}
		}

		switch (state) {
		case MMC_READ:
			n = mmc->block_dev.block_read(curr_device, blk,
						      cnt, addr);            
			/* flush cache after read */
			flush_cache((ulong)addr, cnt * 512); /* FIXME */
            //printf("dump %lu in mmc read\n", (ulong)addr);
            dump_one_block(addr);
			break;
		case MMC_WRITE:
			n = mmc->block_dev.block_write(curr_device, blk,
						      cnt, addr);
            //printf("dump %lu in mmc write\n", (ulong)addr);
            dump_one_block(addr);
			break;
		case MMC_ERASE:
			n = mmc->block_dev.block_erase(curr_device, blk, cnt);
			break;
		default:
			BUG();
		}

		printf("%d blocks %s: %s\n",
				n, argv[1], (n == cnt) ? "OK" : "ERROR");
		return (n == cnt) ? 0 : 1;
	}

	return CMD_RET_USAGE;
}

#if 0
U_BOOT_CMD(
	mmc, 6, 1, do_mmcops,
	"MMC sub system",
	"read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
	"mmc erase blk# cnt\n"
	"mmc rescan\n"
	"mmc part - lists available partition on current mmc device\n"
	"mmc dev [dev] [part] - show or set current mmc device [partition]\n"
	"mmc list - lists available devices\n"
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	"mmc bootbus dev boot_bus_width reset_boot_bus_width boot_mode\n"
	" - Set the BOOT_BUS_WIDTH field of the specified device\n"
	"mmc bootpart-resize <dev> <boot part size MB> <RPMB part size MB>\n"
	" - Change sizes of boot and RPMB partitions of specified device\n"
	"mmc partconf dev boot_ack boot_partition partition_access\n"
	" - Change the bits of the PARTITION_CONFIG field of the specified device\n"
#endif
	"mmc setdsr - set DSR register value\n"
	);
#else
U_BOOT_CMD(
	mmc2, 6, 1, do_mmcops,
	"MMC sub system",
	"mmc read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
	"mmc erase blk# cnt\n"
#if 0	
    "mmc rescan\n"
	"mmc part - lists available partition on current mmc device\n"
	"mmc dev [dev] [part] - show or set current mmc device [partition]\n"
	"mmc list - lists available devices\n"
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	"mmc bootbus dev boot_bus_width reset_boot_bus_width boot_mode\n"
	" - Set the BOOT_BUS_WIDTH field of the specified device\n"
	"mmc bootpart-resize <dev> <boot part size MB> <RPMB part size MB>\n"
	" - Change sizes of boot and RPMB partitions of specified device\n"
	"mmc partconf dev boot_ack boot_partition partition_access\n"
	" - Change the bits of the PARTITION_CONFIG field of the specified device\n"
#endif
	"mmc setdsr - set DSR register value\n"
#endif
);
#endif

#endif 


#else /* !CONFIG_GENERIC_MMC */

enum mmc_state {
	MMC_INVALID,
	MMC_READ,
	MMC_WRITE,
	MMC_ERASE,
};
static void print_mmcinfo(struct mmc *mmc)
{
	printf("Device: %s\n", mmc->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 8) & 0xf, mmc->version & 0xff);

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	print_size(mmc->capacity, "\n");

	printf("Bus Width: %d-bit\n", mmc->bus_width);
}

static int do_mmcinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	mmc = find_mmc_device(curr_device);

	if (mmc) {
		mmc_init(mmc);

		print_mmcinfo(mmc);
		return 0;
	} else {
		printf("no mmc device at slot %x\n", curr_device);
		return 1;
	}
}

U_BOOT_CMD(
	mmcinfo, 1, 0, do_mmcinfo,
	"display MMC info",
	"- display info of the current MMC device"
);

static int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	enum mmc_state state;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	if (strcmp(argv[1], "rescan") == 0) {
		struct mmc *mmc;

		if (argc != 2)
			return CMD_RET_USAGE;

		mmc = find_mmc_device(curr_device);
		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		mmc->has_init = 0;

		if (mmc_init(mmc))
			return 1;
		else
			return 0;
	} else if (strcmp(argv[1], "part") == 0) {
		block_dev_desc_t *mmc_dev;
		struct mmc *mmc;

		if (argc != 2)
			return CMD_RET_USAGE;

		mmc = find_mmc_device(curr_device);
		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}
		mmc_init(mmc);
		mmc_dev = mmc_get_dev(curr_device);
		if (mmc_dev != NULL &&
				mmc_dev->type != DEV_TYPE_UNKNOWN) {
			print_part(mmc_dev);
			return 0;
		}

		puts("get mmc type error!\n");
		return 1;
	} else if (strcmp(argv[1], "list") == 0) {
		if (argc != 2)
			return CMD_RET_USAGE;
		print_mmc_devices('\n');
		return 0;
	} else if (strcmp(argv[1], "dev") == 0) {
		int dev, part = -1;
		struct mmc *mmc;

		if (argc == 2)
			dev = curr_device;
		else if (argc == 3)
			dev = simple_strtoul(argv[2], NULL, 10);
		else if (argc == 4) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
			part = (int)simple_strtoul(argv[3], NULL, 10);
			if (part > PART_ACCESS_MASK) {
				printf("#part_num shouldn't be larger"
					" than %d\n", PART_ACCESS_MASK);
				return 1;
			}
		} else
			return CMD_RET_USAGE;

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		mmc_init(mmc);
		if (part != -1) {
			int ret;
			if (mmc->part_config == MMCPART_NOAVAILABLE) {
				printf("Card doesn't support part_switch\n");
				return 1;
			}

			if (part != mmc->part_num) {
				ret = mmc_switch_part(dev, part);
				if (!ret)
					mmc->part_num = part;

				printf("switch to partitions #%d, %s\n",
						part, (!ret) ? "OK" : "ERROR");
			}
		}
		curr_device = dev;
		if (mmc->part_config == MMCPART_NOAVAILABLE)
			printf("mmc%d is current device\n", curr_device);
		else
			printf("mmc%d(part %d) is current device\n",
				curr_device, mmc->part_num);

		return 0;
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	} else if (strcmp(argv[1], "partconf") == 0) {
		int dev;
		struct mmc *mmc;
		u8 ack, part_num, access;

		if (argc == 6) {
			dev = simple_strtoul(argv[2], NULL, 10);
			ack = simple_strtoul(argv[3], NULL, 10);
			part_num = simple_strtoul(argv[4], NULL, 10);
			access = simple_strtoul(argv[5], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		if (IS_SD(mmc)) {
			puts("PARTITION_CONFIG only exists on eMMC\n");
			return 1;
		}

		/* acknowledge to be sent during boot operation */
		return mmc_set_part_conf(mmc, ack, part_num, access);
	} else if (strcmp(argv[1], "bootbus") == 0) {
		int dev;
		struct mmc *mmc;
		u8 width, reset, mode;

		if (argc == 6) {
			dev = simple_strtoul(argv[2], NULL, 10);
			width = simple_strtoul(argv[3], NULL, 10);
			reset = simple_strtoul(argv[4], NULL, 10);
			mode = simple_strtoul(argv[5], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		if (IS_SD(mmc)) {
			puts("BOOT_BUS_WIDTH only exists on eMMC\n");
			return 1;
		}

		/* acknowledge to be sent during boot operation */
		return mmc_set_boot_bus_width(mmc, width, reset, mode);
	} else if (strcmp(argv[1], "bootpart-resize") == 0) {
		int dev;
		struct mmc *mmc;
		u32 bootsize, rpmbsize;

		if (argc == 5) {
			dev = simple_strtoul(argv[2], NULL, 10);
			bootsize = simple_strtoul(argv[3], NULL, 10);
			rpmbsize = simple_strtoul(argv[4], NULL, 10);
		} else {
			return CMD_RET_USAGE;
		}

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		if (IS_SD(mmc)) {
			printf("It is not a EMMC device\n");
			return 1;
		}

		if (0 == mmc_boot_partition_size_change(mmc,
							bootsize, rpmbsize)) {
			printf("EMMC boot partition Size %d MB\n", bootsize);
			printf("EMMC RPMB partition Size %d MB\n", rpmbsize);
			return 0;
		} else {
			printf("EMMC boot partition Size change Failed.\n");
			return 1;
		}
#endif /* CONFIG_SUPPORT_EMMC_BOOT */
	}

	else if (argc == 3 && strcmp(argv[1], "setdsr") == 0) {
		struct mmc *mmc = find_mmc_device(curr_device);
		u32 val = simple_strtoul(argv[2], NULL, 16);
		int ret;

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}
		ret = mmc_set_dsr(mmc, val);
		printf("set dsr %s\n", (!ret) ? "OK, force rescan" : "ERROR");
		if (!ret) {
			mmc->has_init = 0;
			if (mmc_init(mmc))
				return 1;
			else
				return 0;
		}
		return ret;
	}

	state = MMC_INVALID;
	if (argc == 5 && strcmp(argv[1], "read") == 0)
		state = MMC_READ;
	else if (argc == 5 && strcmp(argv[1], "write") == 0)
		state = MMC_WRITE;
	else if (argc == 4 && strcmp(argv[1], "erase") == 0)
		state = MMC_ERASE;

	if (state != MMC_INVALID) {
		struct mmc *mmc = find_mmc_device(curr_device);
		int idx = 2;
		u32 blk, cnt, n;
		void *addr;

		if (state != MMC_ERASE) {
			addr = (void *)simple_strtoul(argv[idx], NULL, 16);
			++idx;
		} else
			addr = NULL;
		blk = simple_strtoul(argv[idx], NULL, 16);
		cnt = simple_strtoul(argv[idx + 1], NULL, 16);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		printf("\nMMC %s: dev # %d, block # %d, count %d ... ",
				argv[1], curr_device, blk, cnt);

		mmc_init(mmc);

		if ((state == MMC_WRITE || state == MMC_ERASE)) {
			if (mmc_getwp(mmc) == 1) {
				printf("Error: card is write protected!\n");
				return 1;
			}
		}

		switch (state) {
		case MMC_READ:
			n = mmc->block_dev.block_read(curr_device, blk,
						      cnt, addr);
			/* flush cache after read */
			flush_cache((ulong)addr, cnt * 512); /* FIXME */
			break;
		case MMC_WRITE:
			n = mmc->block_dev.block_write(curr_device, blk,
						      cnt, addr);
			break;
		case MMC_ERASE:
			n = mmc->block_dev.block_erase(curr_device, blk, cnt);
			break;
		default:
			BUG();
		}

		printf("%d blocks %s: %s\n",
				n, argv[1], (n == cnt) ? "OK" : "ERROR");
		return (n == cnt) ? 0 : 1;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	mmc, 6, 1, do_mmcops,
	"MMC sub system",
	"read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
	"mmc erase blk# cnt\n"
	"mmc rescan\n"
	"mmc part - lists available partition on current mmc device\n"
	"mmc dev [dev] [part] - show or set current mmc device [partition]\n"
	"mmc list - lists available devices\n"
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	"mmc bootbus dev boot_bus_width reset_boot_bus_width boot_mode\n"
	" - Set the BOOT_BUS_WIDTH field of the specified device\n"
	"mmc bootpart-resize <dev> <boot part size MB> <RPMB part size MB>\n"
	" - Change sizes of boot and RPMB partitions of specified device\n"
	"mmc partconf dev boot_ack boot_partition partition_access\n"
	" - Change the bits of the PARTITION_CONFIG field of the specified device\n"
#endif
	"mmc setdsr - set DSR register value\n"
	);
#endif /* !CONFIG_GENERIC_MMC */
