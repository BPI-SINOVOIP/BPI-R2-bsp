/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/atomic.h>
#ifdef MT7622_PORTING
#include <mt-plat/aee.h>
#include "mtk_devinfo.h"
#endif

#include "mtk_spm_idle.h"

/* #include <mach/wd_api.h> */

#include "mtk_spm_internal.h"

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

void __weak aee_kernel_warning_api(const char *file, const int line, const int db_opt,
				   const char *module, const char *msg, ...)
{
}

void __iomem *spm_base;

/* device tree + 32 = IRQ number */
/* 88 + 32 = 120 */
u32 spm_irq_0 = 120;
u32 spm_irq_1 = 121;
u32 spm_irq_2 = 122;
u32 spm_irq_3 = 123;

/*
 * Config and Parameter
 */

/*
 * Define and Declare
 */
struct spm_irq_desc {
	unsigned int irq;
	irq_handler_t handler;
};

static twam_handler_t spm_twam_handler;

/*
 * Init and IRQ Function
 */
static irqreturn_t spm_irq0_handler(int irq, void *dev_id)
{
	u32 isr;
	unsigned long flags;
	struct twam_sig twamsig;

	spin_lock_irqsave(&__spm_lock, flags);
	/* get ISR status */
	isr = spm_read(SPM_SLEEP_ISR_STATUS);
	if (isr & ISRS_TWAM) {
		twamsig.sig0 = spm_read(SPM_SLEEP_TWAM_STATUS0);
		twamsig.sig1 = spm_read(SPM_SLEEP_TWAM_STATUS1);
		twamsig.sig2 = spm_read(SPM_SLEEP_TWAM_STATUS2);
		twamsig.sig3 = spm_read(SPM_SLEEP_TWAM_STATUS3);
	}

	/* clean ISR status */
	spm_write(SPM_SLEEP_ISR_MASK, spm_read(SPM_SLEEP_ISR_MASK) | ISRM_ALL_EXC_TWAM);
	spm_write(SPM_SLEEP_ISR_STATUS, isr);
	if (isr & ISRS_TWAM)
		udelay(100);	/* need 3T TWAM clock (32K/26M) */
	spm_write(SPM_PCM_SW_INT_CLEAR, PCM_SW_INT0);
	spin_unlock_irqrestore(&__spm_lock, flags);

	if ((isr & ISRS_TWAM) && spm_twam_handler)
		spm_twam_handler(&twamsig);

	if (isr & (ISRS_SW_INT0 | ISRS_PCM_RETURN))
		spm_err("IRQ0 HANDLER SHOULD NOT BE EXECUTED (0x%x)\n", isr);

	return IRQ_HANDLED;
}

static irqreturn_t spm_irq_aux_handler(u32 irq_id)
{
	u32 isr;
	unsigned long flags;

	spin_lock_irqsave(&__spm_lock, flags);
	isr = spm_read(SPM_SLEEP_ISR_STATUS);
	spm_write(SPM_PCM_SW_INT_CLEAR, (1U << irq_id));
	spin_unlock_irqrestore(&__spm_lock, flags);

	spm_err("IRQ%u HANDLER SHOULD NOT BE EXECUTED (0x%x)\n", irq_id, isr);

	return IRQ_HANDLED;
}

static irqreturn_t spm_irq1_handler(int irq, void *dev_id)
{
	return spm_irq_aux_handler(1);
}

static irqreturn_t spm_irq2_handler(int irq, void *dev_id)
{
	return spm_irq_aux_handler(2);
}

static irqreturn_t spm_irq3_handler(int irq, void *dev_id)
{
	return spm_irq_aux_handler(3);
}

static int spm_irq_register(void)
{
	int i, err, r = 0;
	struct spm_irq_desc irqdesc[] = {
		{.irq = 0, .handler = spm_irq0_handler,},
		{.irq = 0, .handler = spm_irq1_handler,},
		{.irq = 0, .handler = spm_irq2_handler,},
		{.irq = 0, .handler = spm_irq3_handler,}
	};

	irqdesc[0].irq = SPM_IRQ0_ID;
	irqdesc[1].irq = SPM_IRQ1_ID;
	irqdesc[2].irq = SPM_IRQ2_ID;
	irqdesc[3].irq = SPM_IRQ3_ID;

	for (i = 0; i < ARRAY_SIZE(irqdesc); i++) {
		err = request_irq(irqdesc[i].irq, irqdesc[i].handler,
				  IRQF_TRIGGER_LOW | IRQF_NO_SUSPEND, "SPM", NULL);
		if (err) {
			spm_err("FAILED TO REQUEST IRQ%d (%d)\n", i, err);
			r = -EPERM;
		}
	}
#if 0
	mt_gic_set_priority(SPM_IRQ0_ID);
#endif
	return r;
}

static void spm_register_init(void)
{
	unsigned long flags;
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "mediatek,mt7622-scpsys");
	if (!node)
		spm_err("find mt7622-scpsys node failed\n");
	spm_base = of_iomap(node, 0);
	if (!spm_base)
		spm_err("base spm_base failed\n");

	spm_irq_0 = irq_of_parse_and_map(node, 0);
	if (!spm_irq_0)
		spm_err("get spm_irq_0 failed\n");
	spm_irq_1 = irq_of_parse_and_map(node, 1);
	if (!spm_irq_1)
		spm_err("get spm_irq_1 failed\n");
	spm_irq_2 = irq_of_parse_and_map(node, 2);
	if (!spm_irq_2)
		spm_err("get spm_irq_2 failed\n");
	spm_irq_3 = irq_of_parse_and_map(node, 3);
	if (!spm_irq_3)
		spm_err("get spm_irq_3 failed\n");

	spm_err("spm_base = %p\n", spm_base);
	spm_err("spm_irq_0 = %d, spm_irq_1 = %d, spm_irq_2 = %d, spm_irq_3 = %d\n", spm_irq_0,
		spm_irq_1, spm_irq_2, spm_irq_3);

	spin_lock_irqsave(&__spm_lock, flags);

	/* enable register control */
	spm_write(SPM_POWERON_CONFIG_SET, SPM_REGWR_CFG_KEY | SPM_REGWR_EN);

	/* init power control register */
	spm_write(SPM_POWER_ON_VAL0, 0);
	spm_write(SPM_POWER_ON_VAL1, POWER_ON_VAL1_DEF);
	spm_write(SPM_PCM_PWR_IO_EN, 0);

	/* reset PCM */
	spm_write(SPM_PCM_CON0, CON0_CFG_KEY | CON0_PCM_SW_RESET);
	spm_write(SPM_PCM_CON0, CON0_CFG_KEY);
	/* PCM reset failed */
	/* BUG_ON(spm_read(SPM_PCM_FSM_STA) != PCM_FSM_STA_DEF); */

	/* init PCM control register */
	spm_write(SPM_PCM_CON0, CON0_CFG_KEY | CON0_IM_SLEEP_DVS);
	spm_write(SPM_PCM_CON1, CON1_CFG_KEY | CON1_EVENT_LOCK_EN |
		  CON1_SPM_SRAM_ISO_B | CON1_SPM_SRAM_SLP_B | CON1_MIF_APBEN);
	spm_write(SPM_PCM_IM_PTR, 0);
	spm_write(SPM_PCM_IM_LEN, 0);

	spm_write(SPM_CLK_CON, spm_read(SPM_CLK_CON) | CC_SRCLKENA_MASK_0);
	/* CC_CLKSQ0_SEL is DONT-CARE in Suspend since PCM_PWR_IO_EN[0]=1 in Suspend */

	spm_write(SPM_PCM_SRC_REQ, 0);

	/* TODO: check if this means "Set SRCLKENI_MASK=1'b1" */
	spm_write(SPM_AP_STANBY_CON, spm_read(SPM_AP_STANBY_CON) | ASC_SRCCLKENI_MASK);

	/* unmask gce_busy_mask (set to 1b1); otherwise, gce (cmd-q) can not notify SPM to exit EMI self-refresh */
	spm_write(SPM_PCM_MMDDR_MASK, spm_read(SPM_PCM_MMDDR_MASK) | (1U << 4));

	/* clean ISR status */
	spm_write(SPM_SLEEP_ISR_MASK, ISRM_ALL);
	spm_write(SPM_SLEEP_ISR_STATUS, ISRC_ALL);
	spm_write(SPM_PCM_SW_INT_CLEAR, PCM_SW_INT_ALL);

	spin_unlock_irqrestore(&__spm_lock, flags);
}

static int __init spm_module_init(void)
{
	int r = 0;
	/* This following setting is moved to LK by WDT init, because of DTS init level issue */
#if 0
	struct wd_api *wd_api;
#endif

	spm_register_init();

	if (spm_irq_register() != 0)
		r = -EPERM;

#if defined(CONFIG_PM)
	if (spm_fs_init() != 0)
		r = -EPERM;
#endif

#if 0
	get_wd_api(&wd_api);
	if (wd_api->wd_spmwdt_mode_config) {
		wd_api->wd_spmwdt_mode_config(WD_REQ_EN, WD_REQ_RST_MODE);
	} else {
		spm_err("FAILED TO GET WD API\n");
		r = -ENODEV;
	}
#endif

#ifdef MT7622_PORTING
	spm_sodi_init();
#endif
	/* spm_mcdi_init(); */

#if 0
	if (spm_golden_setting_cmp(1) != 0) {
		/* r = -EPERM; */
		aee_kernel_warning("SPM Warring", "dram golden setting mismach");
	}
#endif

#ifdef SPM_VCORE_EN
	spm_go_to_vcore_dvfs(SPM_VCORE_DVFS_EN, 0);
#endif

	return r;
}
postcore_initcall(spm_module_init);

/*
 * PLL Request API
 */
void spm_mainpll_on_request(const char *drv_name)
{
	int req;

	req = atomic_inc_return(&__spm_mainpll_req);
	spm_debug("%s request MAINPLL on (%d)\n", drv_name, req);
}
EXPORT_SYMBOL(spm_mainpll_on_request);

void spm_mainpll_on_unrequest(const char *drv_name)
{
	int req;

	req = atomic_dec_return(&__spm_mainpll_req);
	spm_debug("%s unrequest MAINPLL on (%d)\n", drv_name, req);
}
EXPORT_SYMBOL(spm_mainpll_on_unrequest);


/*
 * TWAM Control API
 */
void spm_twam_register_handler(twam_handler_t handler)
{
	spm_twam_handler = handler;
}
EXPORT_SYMBOL(spm_twam_register_handler);

void spm_twam_enable_monitor(const struct twam_sig *twamsig, bool speed_mode,
			     unsigned int window_len)
{
	u32 sig0 = 0, sig1 = 0, sig2 = 0, sig3 = 0;
	unsigned long flags;

	if (twamsig) {
		sig0 = twamsig->sig0 & 0x1f;
		sig1 = twamsig->sig1 & 0x1f;
		sig2 = twamsig->sig2 & 0x1f;
		sig3 = twamsig->sig3 & 0x1f;
	}

	spin_lock_irqsave(&__spm_lock, flags);
	spm_write(SPM_SLEEP_ISR_MASK, spm_read(SPM_SLEEP_ISR_MASK) & ~ISRM_TWAM);
	spm_write(SPM_SLEEP_TWAM_CON, ((sig3 << 27) |
				       (sig2 << 22) |
				       (sig1 << 17) |
				       (sig0 << 12) |
				       (TWAM_MON_TYPE_HIGH << 4) |
				       (TWAM_MON_TYPE_HIGH << 6) |
				       (TWAM_MON_TYPE_HIGH << 8) |
				       (TWAM_MON_TYPE_HIGH << 10) |
				       (speed_mode ? TWAM_CON_SPEED_EN : 0) | TWAM_CON_EN));
	spm_write(SPM_SLEEP_TWAM_WINDOW_LEN, window_len);
	spin_unlock_irqrestore(&__spm_lock, flags);

	spm_crit("enable TWAM for signal %u, %u, %u, %u (%u)\n",
		 sig0, sig1, sig2, sig3, speed_mode);


}
EXPORT_SYMBOL(spm_twam_enable_monitor);

void spm_twam_disable_monitor(void)
{
	unsigned long flags;

	spin_lock_irqsave(&__spm_lock, flags);
	spm_write(SPM_SLEEP_TWAM_CON, spm_read(SPM_SLEEP_TWAM_CON) & ~TWAM_CON_EN);
	spm_write(SPM_SLEEP_ISR_MASK, spm_read(SPM_SLEEP_ISR_MASK) | ISRM_TWAM);
	spm_write(SPM_SLEEP_ISR_STATUS, ISRC_TWAM);
	spin_unlock_irqrestore(&__spm_lock, flags);

	spm_debug("disable TWAM\n");
}
EXPORT_SYMBOL(spm_twam_disable_monitor);

/* TODO: remove ddrphy golden compare first */
#if 0
/*
 * SPM Goldeng Seting API(MEMPLL Control, DRAMC)
 */
struct ddrphy_golden_cfg {
	u32 addr;
	u32 value;
};

static struct ddrphy_golden_cfg ddrphy_setting[] = {
	{0x5c0, 0x063c0000},
	{0x5c4, 0x00000000},
	{0x5c8, 0x0000fC10},	/* temp remove mempll2/3 control for golden setting refine */
	{0x5cc, 0x40101000},
};

int spm_golden_setting_cmp(bool en)
{
	int i, ddrphy_num, r = 0;

	if (!en)
		return r;

	/* Compare Dramc Goldeing Setting */
	ddrphy_num = ARRAY_SIZE(ddrphy_setting) / ARRAY_SIZE(ddrphy_setting[0]);
	for (i = 0; i < ddrphy_num; i++) {
		if (spm_read(spm_ddrphy_base + ddrphy_setting[i].addr) != ddrphy_setting[i].value) {
			spm_err("dramc setting mismatch addr: %p, val: 0x%x\n",
				spm_ddrphy_base + ddrphy_setting[i].addr,
				spm_read(spm_ddrphy_base + ddrphy_setting[i].addr));
			r = -EPERM;
		}
	}

	return r;
}
#endif

/*
 * SPM AP-BSI Protocol Generator
 */
/* TODO: Add BSI control */

#define SPMC_CPU_PWR_STA_MASK	0x10004
#define SPM_CPU_PWR_STA_MASK	0x3C00
#define SPM_CPU_PWR_STA_SHIFT	13
unsigned int spm_get_cpu_pwr_status(void)
{
#if CONFIG_SPMC_MODE
	u32 stat = 0;
	u32 val;
	u8 i;

	for (i = 0; i < num_possible_cpus(); i++) {
		val = spm_read(SPM_SPMC_MP0_CPU0_PWR_CON + 0x4*i) & SPMC_CPU_PWR_STA_MASK;
		if (val != 0x0)
			stat |= 0x1 << (SPM_CPU_PWR_STA_SHIFT - i);
	}

	return stat;
#else
	u32 val[2] = {0};
	u32 stat = 0;

	val[0] = spm_read(SPM_PWR_STATUS);
	val[1] = spm_read(SPM_PWR_STATUS_2ND);

	stat = val[0] & SPM_CPU_PWR_STA_MASK;
	stat &= val[1] & SPM_CPU_PWR_STA_MASK;

	return stat;
#endif
}
EXPORT_SYMBOL(spm_get_cpu_pwr_status);

MODULE_DESCRIPTION("SPM Driver v0.1");
