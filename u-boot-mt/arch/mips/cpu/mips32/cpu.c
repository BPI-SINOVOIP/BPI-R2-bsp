/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <netdev.h>
#include <asm/mipsregs.h>
#include <asm/cacheops.h>
#include <asm/reboot.h>

#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	push					\n"	\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	pop					\n"	\
	:								\
	: "i" (op), "R" (*(unsigned char *)(addr)))

void __attribute__((weak)) _machine_restart(void)
{
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	_machine_restart();

	fprintf(stderr, "*** reset failed ***\n");
	return 0;
}

#ifdef CONFIG_SYS_CACHELINE_SIZE

static inline unsigned long icache_line_size(void)
{
	return CONFIG_SYS_CACHELINE_SIZE;
}

static inline unsigned long dcache_line_size(void)
{
	return CONFIG_SYS_CACHELINE_SIZE;
}

#else /* !CONFIG_SYS_CACHELINE_SIZE */

static inline unsigned long icache_line_size(void)
{
	unsigned long conf1, il;
	conf1 = read_c0_config1();
	il = (conf1 & MIPS_CONF1_IL) >> MIPS_CONF1_IL_SHIFT;
	if (!il)
		return 0;
	return 2 << il;
}

static inline unsigned long dcache_line_size(void)
{
	unsigned long conf1, dl;
	conf1 = read_c0_config1();
	dl = (conf1 & MIPS_CONF1_DL) >> MIPS_CONF1_DL_SHIFT;
	if (!dl)
		return 0;
	return 2 << dl;
}

#endif /* !CONFIG_SYS_CACHELINE_SIZE */

void flush_cache(ulong start_addr, ulong size)
{
	unsigned long ilsize = icache_line_size();
	unsigned long dlsize = dcache_line_size();
	unsigned long addr, aend;

	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0)
		return;

	addr = start_addr & ~(dlsize - 1);
	aend = (start_addr + size - 1) & ~(dlsize - 1);

	if (ilsize == dlsize) {
		/* flush I-cache & D-cache simultaneously */
		while (1) {
			cache_op(HIT_WRITEBACK_INV_D, addr);
			cache_op(HIT_INVALIDATE_I, addr);
			if (addr == aend)
				break;
			addr += dlsize;
		}
		return;
	}

	/* flush D-cache */
	while (1) {
		cache_op(HIT_WRITEBACK_INV_D, addr);
		if (addr == aend)
			break;
		addr += dlsize;
	}

	/* flush I-cache */
	addr = start_addr & ~(ilsize - 1);
	aend = (start_addr + size - 1) & ~(ilsize - 1);
	while (1) {
		cache_op(HIT_INVALIDATE_I, addr);
		if (addr == aend)
			break;
		addr += ilsize;
	}
}

void flush_dcache_range(ulong start_addr, ulong stop)
{
	unsigned long lsize = dcache_line_size();
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (stop - 1) & ~(lsize - 1);

	while (1) {
		cache_op(HIT_WRITEBACK_INV_D, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void invalidate_dcache_range(ulong start_addr, ulong stop)
{
	unsigned long lsize = dcache_line_size();
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (stop - 1) & ~(lsize - 1);

	while (1) {
		cache_op(HIT_INVALIDATE_D, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void write_one_tlb(int index, u32 pagemask, u32 hi, u32 low0, u32 low1)
{
	write_c0_entrylo0(low0);
	write_c0_pagemask(pagemask);
	write_c0_entrylo1(low1);
	write_c0_entryhi(hi);
	write_c0_index(index);
	tlb_write_indexed();
}

int cpu_eth_init(bd_t *bis)
{
#ifdef CONFIG_SOC_AU1X00
	au1x00_enet_initialize(bis);
#endif
	return 0;
}
