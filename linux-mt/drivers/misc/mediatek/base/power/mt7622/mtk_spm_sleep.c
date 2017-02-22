/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <mt-plat/aee.h>
#include <linux/i2c.h>
#include <linux/of_fdt.h>
#include <asm/setup.h>
#include <linux/lockdep.h>
#ifdef MT7622_PORTING
#include <mt-plat/mtk_cirq.h>
#endif
#include "mtk_spm_pcm.h"
#include "mtk_spm_sleep.h"
#ifdef MT7622_PORTING
#include "mtk_cpuidle.h"
#endif
/* #include <mach/wd_api.h> */

#include "mtk_spm_internal.h"

/* for MP0,1 AXI_CONFIG */
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

/**************************************
 * only for internal debug
 **************************************/
#ifdef CONFIG_MTK_LDVT
#define SPM_PWAKE_EN            0
#define SPM_PCMWDT_EN           0
#define SPM_BYPASS_SYSPWREQ     1
#else
#define SPM_PWAKE_EN            1
#define SPM_PCMWDT_EN           0
#define SPM_BYPASS_SYSPWREQ     0
#endif
#define SPM_PCMTIMER_DIS        0


#define I2C_CHANNEL 2

int spm_dormant_sta;	/* MT_CPU_DORMANT_RESET */
u32 spm_suspend_flag;

struct wake_status suspend_info[20];
u32 log_wakesta_cnt;
u32 log_wakesta_index;
u8 spm_snapshot_golden_setting;

struct wake_status spm_wakesta;	/* record last wakesta */

/**************************************
 * SW code for suspend
 **************************************/
#define SPM_SYSCLK_SETTLE       128      /* 3.9ms */

#define WAIT_UART_ACK_TIMES     10	/* 10 * 10us */

#define SPM_WAKE_PERIOD         600	/* sec */

#define WAKE_SRC_FOR_SUSPEND \
	(WAKE_SRC_KP | WAKE_SRC_EINT |  WAKE_SRC_CONN_WDT  | WAKE_SRC_CONN2AP | \
	WAKE_SRC_USB_CD | WAKE_SRC_USB_PDN | WAKE_SRC_ETH |\
	WAKE_SRC_SYSPWREQ)

#define spm_is_wakesrc_invalid(wakesrc)     (!!((u32)(wakesrc) & 0xFC7F3A9B))

#define reg_read(addr)         __raw_readl((void __force __iomem *)(addr))
#define reg_write(addr, val)   mt_reg_sync_writel((val), ((void *)addr))

#ifdef CONFIG_MTK_RAM_CONSOLE
#define SPM_AEE_RR_REC 1
#else
#define SPM_AEE_RR_REC 0
#endif

#if SPM_AEE_RR_REC
enum spm_suspend_step {
	SPM_SUSPEND_ENTER = 0,
	SPM_SUSPEND_ENTER_WFI,
	SPM_SUSPEND_LEAVE_WFI,
	SPM_SUSPEND_LEAVE
};
/* extern void aee_rr_rec_spm_suspend_val(u32 val); */
/* extern u32 aee_rr_curr_spm_suspend_val(void); */
void __attribute__ ((weak)) aee_rr_rec_spm_suspend_val(u32 val)
{
}

u32 __attribute__ ((weak)) aee_rr_curr_spm_suspend_val(void)
{
	return 0;
}
#endif

int __attribute__ ((weak)) get_dynamic_period(int first_use, int first_wakeup_time,
					      int battery_capacity_level)
{
	return 0;
}

void __attribute__ ((weak)) mt_cirq_clone_gic(void)
{
}

void __attribute__ ((weak)) mt_cirq_enable(void)
{
}

void __attribute__ ((weak)) mt_cirq_flush(void)
{
}

void __attribute__ ((weak)) mt_cirq_disable(void)
{
}

int __attribute__ ((weak)) request_uart_to_sleep(void)
{
	return 0;
}

int __attribute__ ((weak)) request_uart_to_wakeup(void)
{
	return 0;
}

void __attribute__ ((weak)) mtk_uart_restore(void)
{
}

void __attribute__ ((weak)) mt_eint_print_status(void)
{
}

static struct pwr_ctrl suspend_ctrl = {
	.wake_src = WAKE_SRC_FOR_SUSPEND,
	.r0_ctrl_en = 1,
	.r7_ctrl_en = 1,
	.infra_dcm_lock = 1,
	.wfi_op = WFI_OP_AND,

	.ca7top_idle_mask = 0,
	.ca15top_idle_mask = 1,
	.mcusys_idle_mask = 0,
	.disp0_req_mask = 0,
	.disp1_req_mask = 0,
	.mfg_req_mask = 0,
	.vdec_req_mask = 0,
	.mm_ddr_req_mask = 0,
	.conn_mask = 0,
#ifdef CONFIG_MTK_NFC
	.srclkenai_mask = 0,	/* unmask for NFC use */
#else
	.srclkenai_mask = 1,	/* mask for gpio/i2c use */
#endif
	/* .pcm_apsrc_req = 1, */

	/* .pcm_f26m_req = 1, */

	.ca7_wfi0_en = 1,
	.ca7_wfi1_en = 1,
	.ca7_wfi2_en = 1,
	.ca7_wfi3_en = 1,
	.ca15_wfi0_en = 0,
	.ca15_wfi1_en = 0,
	.ca15_wfi2_en = 0,
	.ca15_wfi3_en = 0,

#if SPM_BYPASS_SYSPWREQ
	.syspwreq_mask = 1,
#endif
};

struct spm_lp_scen __spm_suspend = {
	.pcmdesc = &suspend_pcm,
	.pwrctrl = &suspend_ctrl,
	.wakestatus = &suspend_info[0],
};

static void spm_set_sysclk_settle(void)
{
	u32 settle;

	/* SYSCLK settle = VTCXO settle time */
	spm_write(SPM_CLK_SETTLE, SPM_SYSCLK_SETTLE);
	settle = spm_read(SPM_CLK_SETTLE);

	spm_warn("settle = %u\n", settle);
}

static void spm_kick_pcm_to_run(struct pwr_ctrl *pwrctrl)
{
	/* enable PCM WDT (normal mode) to start count if needed */
#if SPM_PCMWDT_EN
	{
		u32 con1;

		con1 = spm_read(SPM_PCM_CON1) & ~(CON1_PCM_WDT_WAKE_MODE | CON1_PCM_WDT_EN);
		spm_write(SPM_PCM_CON1, CON1_CFG_KEY | con1);

		if (spm_read(SPM_PCM_TIMER_VAL) > PCM_TIMER_MAX)
			spm_write(SPM_PCM_TIMER_VAL, PCM_TIMER_MAX);
		spm_write(SPM_PCM_WDT_TIMER_VAL, spm_read(SPM_PCM_TIMER_VAL) + PCM_WDT_TIMEOUT);
		spm_write(SPM_PCM_CON1, con1 | CON1_CFG_KEY | CON1_PCM_WDT_EN);
	}
#endif

#if SPM_PCMTIMER_DIS
	{
		u32 con1;

		con1 = spm_read(SPM_PCM_CON1) & ~(CON1_PCM_TIMER_EN);
		spm_write(SPM_PCM_CON1, con1 | CON1_CFG_KEY);
	}
#endif

	/* init PCM_PASR_DPD_0 for DPD */
	spm_write(SPM_PCM_PASR_DPD_0, 0);

	__spm_kick_pcm_to_run(pwrctrl);
}

static void spm_trigger_wfi_for_sleep(struct pwr_ctrl *pwrctrl)
{
#ifdef MT7622_PORTING
	if (is_cpu_pdn(pwrctrl->pcm_flags)) {
		spm_dormant_sta = mt_cpu_dormant(CPU_SHUTDOWN_MODE /* | DORMANT_SKIP_WFI */);
		switch (spm_dormant_sta) {
		case MT_CPU_DORMANT_RESET:
			break;
		case MT_CPU_DORMANT_BYPASS:
			break;
		}
	} else {
		spm_dormant_sta = -1;
		wfi_with_sync();
	}

	if (is_infra_pdn(pwrctrl->pcm_flags))
		mtk_uart_restore();
#endif
}

static void spm_clean_after_wakeup(void)
{
	/* disable PCM WDT to stop count if needed */
#if SPM_PCMWDT_EN
	spm_write(SPM_PCM_CON1, CON1_CFG_KEY | (spm_read(SPM_PCM_CON1) & ~CON1_PCM_WDT_EN));
#endif

#if SPM_PCMTIMER_DIS
	spm_write(SPM_PCM_CON1, CON1_CFG_KEY | (spm_read(SPM_PCM_CON1) | CON1_PCM_TIMER_EN));
#endif

	__spm_clean_after_wakeup();
}

static wake_reason_t spm_output_wake_reason(struct wake_status *wakesta, struct pcm_desc *pcmdesc)
{
	wake_reason_t wr;

	wr = __spm_output_wake_reason(wakesta, pcmdesc, true);

#if 1
	memcpy(&suspend_info[log_wakesta_cnt], wakesta, sizeof(struct wake_status));
	suspend_info[log_wakesta_cnt].log_index = log_wakesta_index;

	if (log_wakesta_cnt >= 10) {
		log_wakesta_cnt = 0;
		spm_snapshot_golden_setting = 0;
	} else {
		log_wakesta_cnt++;
		log_wakesta_index++;
	}

	if (log_wakesta_index >= 0xFFFFFFF0)
		log_wakesta_index = 0;
#endif

	spm_warn("suspend dormant state = %d\n", spm_dormant_sta);
	spm_warn("log_wakesta_index = %d\n", log_wakesta_index);

	if (wakesta->r12 & WAKE_SRC_EINT)
		mt_eint_print_status();

	return wr;
}

#if SPM_PWAKE_EN
static u32 spm_get_wake_period(int pwake_time, wake_reason_t last_wr)
{
	int period = SPM_WAKE_PERIOD;
#if 1
	if (pwake_time < 0) {
		/* use FG to get the period of 1% battery decrease */
		period = get_dynamic_period(last_wr != WR_PCM_TIMER ? 1 : 0, SPM_WAKE_PERIOD, 1);
		if (period <= 0) {
			spm_warn("CANNOT GET PERIOD FROM FUEL GAUGE\n");
			period = SPM_WAKE_PERIOD;
		}
	} else {
		period = pwake_time;
		spm_crit2("pwake = %d\n", pwake_time);
	}

	if (period > 36 * 3600)	/* max period is 36.4 hours */
		period = 36 * 3600;
#endif
	return period;
}
#endif

/*
 * wakesrc: WAKE_SRC_XXX
 * enable : enable or disable @wakesrc
 * replace: if true, will replace the default setting
 */
int spm_set_sleep_wakesrc(u32 wakesrc, bool enable, bool replace)
{
	unsigned long flags;

	if (spm_is_wakesrc_invalid(wakesrc))
		return -EINVAL;

	spin_lock_irqsave(&__spm_lock, flags);
	if (enable) {
		if (replace)
			__spm_suspend.pwrctrl->wake_src = wakesrc;
		else
			__spm_suspend.pwrctrl->wake_src |= wakesrc;
	} else {
		if (replace)
			__spm_suspend.pwrctrl->wake_src = 0;
		else
			__spm_suspend.pwrctrl->wake_src &= ~wakesrc;
	}
	spin_unlock_irqrestore(&__spm_lock, flags);

	return 0;
}

/*
 * wakesrc: WAKE_SRC_XXX
 */
u32 spm_get_sleep_wakesrc(void)
{
	return __spm_suspend.pwrctrl->wake_src;
}

#if SPM_AEE_RR_REC
void spm_suspend_aee_init(void)
{
	aee_rr_rec_spm_suspend_val(0);
}
#endif

wake_reason_t spm_go_to_sleep(u32 spm_flags, u32 spm_data)
{
	u32 sec = 2;
#ifdef MT7622_PORTING
	u32 capcode;
#endif
/*	int wd_ret; */
	unsigned long flags;
#ifdef MT7622_PORTING
	struct mtk_irq_mask mask;
#endif
	struct irq_desc *desc = irq_to_desc(spm_irq_0);
/*	struct wd_api *wd_api; */
	static wake_reason_t last_wr = WR_NONE;
	struct pcm_desc *pcmdesc;
	struct pwr_ctrl *pwrctrl;

#if SPM_AEE_RR_REC
	spm_suspend_aee_init();
	aee_rr_rec_spm_suspend_val(1 << SPM_SUSPEND_ENTER);
#endif

	pcmdesc = __spm_suspend.pcmdesc;
	pwrctrl = __spm_suspend.pwrctrl;

	set_pwrctrl_pcm_flags(pwrctrl, spm_flags);
	set_pwrctrl_pcm_data(pwrctrl, spm_data);

#ifdef MT7622_PORTING
	if (!mt_xo_has_ext_crystal())
		pwrctrl->pcm_flags |= SPM_32K_LESS;
#endif

	/* Read XO cap code */
#ifdef MT7622_PORTING
	capcode = mt_xo_get_current_capid();
	spm_write(SPM_PCM_RESERVE8, capcode);
#endif

#if SPM_PWAKE_EN
	sec = spm_get_wake_period(-1 /* FIXME */, last_wr);
#endif
	pwrctrl->timer_val = sec * 32768;
#if 0
	wd_ret = get_wd_api(&wd_api);
	if (!wd_ret)
		wd_api->wd_suspend_notify();
#endif
/*	mt_power_gs_dump_suspend(); */

	/* spm_suspend_pre_process(pwrctrl); */
	lockdep_off();
	spin_lock_irqsave(&__spm_lock, flags);
#ifdef MT7622_PORTING
	mt_irq_mask_all(&mask);
#endif
	if (desc)
		unmask_irq(desc);
	mt_cirq_clone_gic();
	mt_cirq_enable();

	spm_set_sysclk_settle();

	spm_warn("sec = %u, wakesrc = 0x%x (%u)(%u)\n",
		  sec, pwrctrl->wake_src, is_cpu_pdn(pwrctrl->pcm_flags),
		  is_infra_pdn(pwrctrl->pcm_flags));

	if (request_uart_to_sleep()) {
		last_wr = WR_UART_BUSY;
		goto RESTORE_IRQ;
	}

#if CONFIG_SUPPORT_PCM_ALLINONE
	if (!__spm_is_pcm_loaded())
		__spm_init_pcm_AllInOne(pcmdesc);

	__spm_set_power_control(pwrctrl);

	__spm_set_wakeup_event(pwrctrl);

	spm_kick_pcm_to_run(pwrctrl);

	__spm_set_pcm_cmd(PCM_CMD_SUSPEND_PCM);
#else
	__spm_reset_and_init_pcm(pcmdesc);

	__spm_kick_im_to_fetch(pcmdesc);

	__spm_init_pcm_register();

	__spm_init_event_vector(pcmdesc);

	__spm_set_power_control(pwrctrl);

	__spm_set_wakeup_event(pwrctrl);

	spm_kick_pcm_to_run(pwrctrl);
#endif

#if SPM_AEE_RR_REC
	aee_rr_rec_spm_suspend_val(aee_rr_curr_spm_suspend_val() | (1 << SPM_SUSPEND_ENTER_WFI));
#endif

	spm_trigger_wfi_for_sleep(pwrctrl);

#if SPM_AEE_RR_REC
	aee_rr_rec_spm_suspend_val(aee_rr_curr_spm_suspend_val() | (1 << SPM_SUSPEND_LEAVE_WFI));
#endif

	/* record last wakesta */
	__spm_get_wakeup_status(&spm_wakesta);

	spm_clean_after_wakeup();

	request_uart_to_wakeup();

	/* record last wakesta */
	/* last_wr = spm_output_wake_reason(&wakesta, pcmdesc); */
	last_wr = spm_output_wake_reason(&spm_wakesta, pcmdesc);

RESTORE_IRQ:
	mt_cirq_flush();
	mt_cirq_disable();

#ifdef MT7622_PORTING
	mt_irq_mask_restore(&mask);
#endif

	spin_unlock_irqrestore(&__spm_lock, flags);
	lockdep_on();
	/* spm_suspend_post_process(pwrctrl); */
#if 0
	if (!wd_ret)
		wd_api->wd_resume_notify();
#endif
#if SPM_AEE_RR_REC
	aee_rr_rec_spm_suspend_val(aee_rr_curr_spm_suspend_val() | (1 << SPM_SUSPEND_LEAVE));
#endif

	return last_wr;
}

bool spm_is_conn_sleep(void)
{
	return !(spm_read(SPM_PCM_REG13_DATA) & R13_CONN_SRCLKENA);
}

void spm_set_wakeup_src_check(void)
{
	/* clean wakeup event raw status */
	spm_write(SPM_SLEEP_WAKEUP_EVENT_MASK, 0xFFFFFFFF);

	/* set wakeup event */
	spm_write(SPM_SLEEP_WAKEUP_EVENT_MASK, ~WAKE_SRC_FOR_SUSPEND);
}

bool spm_check_wakeup_src(void)
{
	u32 wakeup_src;

	/* check wanek event raw status */
	wakeup_src = spm_read(SPM_SLEEP_ISR_RAW_STA);

	if (wakeup_src) {
		spm_crit2("WARNING: spm_check_wakeup_src = 0x%x", wakeup_src);
		return 1;
	} else
		return 0;
}

void spm_poweron_config_set(void)
{
	unsigned long flags;

	spin_lock_irqsave(&__spm_lock, flags);
	/* enable register control */
	spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));
	spin_unlock_irqrestore(&__spm_lock, flags);
}

void spm_output_sleep_option(void)
{
	spm_notice("PWAKE_EN:%d, PCMWDT_EN:%d, BYPASS_SYSPWREQ:%d, I2C_CHANNEL:%d\n",
		   SPM_PWAKE_EN, SPM_PCMWDT_EN, SPM_BYPASS_SYSPWREQ, I2C_CHANNEL);
}

/* record last wakesta */
u32 spm_get_last_wakeup_src(void)
{
	return spm_wakesta.r12;
}

u32 spm_get_last_wakeup_misc(void)
{
	return spm_wakesta.wake_misc;
}

MODULE_DESCRIPTION("SPM-Sleep Driver v0.1");
