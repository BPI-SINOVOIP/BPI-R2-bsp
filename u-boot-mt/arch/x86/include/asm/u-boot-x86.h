/*
 * (C) Copyright 2002
 * Daniel Engström, Omicron Ceti AB, daniel@omicron.se.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _U_BOOT_I386_H_
#define _U_BOOT_I386_H_	1

/* cpu/.../cpu.c */
int x86_cpu_init_r(void);
int cpu_init_r(void);
int x86_cpu_init_f(void);
int cpu_init_f(void);
void init_gd(gd_t *id, u64 *gdt_addr);
void setup_gdt(gd_t *id, u64 *gdt_addr);
int init_cache(void);
int cleanup_before_linux(void);
void panic_puts(const char *str);

/* cpu/.../timer.c */
void timer_isr(void *);
typedef void (timer_fnc_t) (void);
int register_timer_isr (timer_fnc_t *isr_func);
unsigned long get_tbclk_mhz(void);
void timer_set_base(uint64_t base);
int pcat_timer_init(void);

/* Architecture specific - can be in arch/x86/cpu/, arch/x86/lib/, or $(BOARD)/ */
int dram_init_f(void);

/* cpu/.../interrupts.c */
int cpu_init_interrupts(void);

/* board/.../... */
int dram_init(void);

void setup_pcat_compatibility(void);

void isa_unmap_rom(u32 addr);
u32 isa_map_rom(u32 bus_addr, int size);

/* arch/x86/lib/... */
int video_bios_init(void);

void	board_init_f_r_trampoline(ulong) __attribute__ ((noreturn));
void	board_init_f_r(void) __attribute__ ((noreturn));

/* Read the time stamp counter */
static inline __attribute__((no_instrument_function)) uint64_t rdtsc(void)
{
	uint32_t high, low;
	__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));
	return (((uint64_t)high) << 32) | low;
}

/* board/... */
void timer_set_tsc_base(uint64_t new_base);
uint64_t timer_get_tsc(void);

#endif	/* _U_BOOT_I386_H_ */
