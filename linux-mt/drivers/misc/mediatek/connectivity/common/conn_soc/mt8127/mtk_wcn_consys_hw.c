/*! \file
    \brief  Declaration of library functions

    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG "[WMT-CONSYS-HW]"

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#if defined(CONFIG_MTK_CLKMGR)
#include <mt_clkmgr.h>
#else
#include <linux/clk.h>
#endif /* defined(CONFIG_MTK_CLKMGR) */
#include <linux/delay.h>
#include <asm/memblock.h>
#include "osal_typedef.h"
#include "mtk_wcn_consys_hw.h"

#if CONSYS_EMI_MPU_SETTING
#include <emi_mpu.h>
#endif

#if CONSYS_PMIC_CTRL_ENABLE
#include <upmu_common.h>
#include <linux/regulator/consumer.h>
#endif

#ifdef CONFIG_MTK_HIBERNATION
#include <mtk_hibernate_dpm.h>
#endif

#include <linux/of_reserved_mem.h>

#if CONSYS_CLOCK_BUF_CTRL
#include <mt_clkbuf_ctl.h>
#endif

#include <linux/pm_runtime.h>

#if CONFIG_RESET_CONTROL
#include <linux/reset.h>
#endif

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
static INT32 mtk_wmt_probe(struct platform_device *pdev);
static INT32 mtk_wmt_remove(struct platform_device *pdev);


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
UINT8 __iomem *pEmibaseaddr = NULL;
phys_addr_t gConEmiPhyBase;
struct CONSYS_BASE_ADDRESS conn_reg;

/* CCF part */
#if !defined(CONFIG_MTK_CLKMGR)
struct clk *clk_scp_conn_main;	/*ctrl conn_power_on/off */
#if CONSYS_AHB_CLK_MAGEMENT
struct clk *clk_infra_conn_main;	/*ctrl infra_connmcu_bus clk */
#endif
#endif /* !defined(CONFIG_MTK_CLKMGR) */

#ifdef CONFIG_OF
static const struct of_device_id apwmt_of_ids[] = {
	{.compatible = "mediatek,mt8127-consys",},
	{}
};
#endif

static struct platform_driver mtk_wmt_dev_drv = {
	.probe = mtk_wmt_probe,
	.remove = mtk_wmt_remove,
	.driver = {
		   .name = "mtk_wmt",
		   .owner = THIS_MODULE,
#ifdef CONFIG_OF
		   .of_match_table = apwmt_of_ids,
#endif
		   },
};

struct platform_device *my_pdev;
#if CONFIG_RESET_CONTROL
struct reset_control *rstc;
#endif

/* PMIC part */
#if CONSYS_PMIC_CTRL_ENABLE
#if !defined(CONFIG_MTK_PMIC_LEGACY)
struct regulator *reg_VCN18;
struct regulator *reg_VCN28;
struct regulator *reg_VCN33_BT;
struct regulator *reg_VCN33_WIFI;
#endif
#endif

/* GPIO part */
#if !defined(CONFIG_MTK_GPIO_LEGACY)
struct pinctrl *consys_pinctrl = NULL;
#endif

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#define DYNAMIC_DUMP_GROUP_NUM 5
#if CONSYS_ENALBE_SET_JTAG
UINT32 gJtagCtrl = 0;

#define JTAG_ADDR1_BASE 0x10002000

char *jtag_addr1 = (char *)JTAG_ADDR1_BASE;

#define JTAG1_REG_WRITE(addr, value)	\
writel(value, ((PUINT32)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))
#define JTAG1_REG_READ(addr)			\
readl(((PUINT32)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))

static INT32 mtk_wcn_consys_jtag_set_for_mcu(VOID)
{
#if 0
	int iRet = -1;

	WMT_PLAT_INFO_FUNC("WCN jtag_set_for_mcu start...\n");
	jtag_addr1 = ioremap(JTAG_ADDR1_BASE, 0x5000);
	if (jtag_addr1 == 0) {
		WMT_PLAT_ERR_FUNC("remap jtag_addr1 fail!\n");
		return iRet;
	}
	WMT_PLAT_INFO_FUNC("jtag_addr1 = 0x%p\n", jtag_addr1);

	JTAG1_REG_WRITE(0x100053c4, 0x11111100);
	JTAG1_REG_WRITE(0x100053d4, 0x00111111);

	/*Enable IES of all pins */
	JTAG1_REG_WRITE(0x10002014, 0x00000003);
	JTAG1_REG_WRITE(0x10005334, 0x55000000);
	JTAG1_REG_WRITE(0x10005344, 0x00555555);
	JTAG1_REG_WRITE(0x10005008, 0xc0000000);
	JTAG1_REG_WRITE(0x10005018, 0x0000000d);
	JTAG1_REG_WRITE(0x10005014, 0x00000032);
	JTAG1_REG_WRITE(0x100020a4, 0x000000ff);
	JTAG1_REG_WRITE(0x100020d4, 0x000000b4);
	JTAG1_REG_WRITE(0x100020d8, 0x0000004b);

	WMT_PLAT_INFO_FUNC("WCN jtag set for mcu start...\n");
	kal_int32 iRet = 0;
	kal_uint32 tmp = 0;
	kal_int32 addr = 0;
	kal_int32 remap_addr1 = 0;
	kal_int32 remap_addr2 = 0;

	remap_addr1 = ioremap(JTAG_ADDR1_BASE, 0x1000);
	if (remap_addr1 == 0) {
		WMT_PLAT_ERR_FUNC("remap jtag_addr1 fail!\n");
		return -1;
	}

	remap_addr2 = ioremap(JTAG_ADDR2_BASE, 0x100);
	if (remap_addr2 == 0) {
		WMT_PLAT_ERR_FUNC("remap jtag_addr2 fail!\n");
		return -1;
	}

	/*Pinmux setting for MT6625 I/F */
	addr = remap_addr1 + 0x03C0;
	tmp = DRV_Reg32(addr);
	tmp = tmp & 0xff;
	tmp = tmp | 0x11111100;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	addr = remap_addr1 + 0x03D0;
	tmp = DRV_Reg32(addr);
	tmp = tmp & 0xff000000;
	tmp = tmp | 0x00111111;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/*AP GPIO Setting 1 <default use> */
	/*Enable IES */
	/* addr = 0x10002014; */
	addr = remap_addr2 + 0x0014;
	tmp = 0x00000003;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	/*GPIO mode setting */
	/* addr = 0x10005334; */
	addr = remap_addr1 + 0x0334;
	tmp = 0x55000000;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/* addr = 0x10005344; */
	addr = remap_addr1 + 0x0344;
	tmp = 0x00555555;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	/*GPIO direction control */
	/* addr = 0x10005008; */
	addr = remap_addr1 + 0x0008;
	tmp = 0xc0000000;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/* addr = 0x10005018; */
	addr = remap_addr1 + 0x0018;
	tmp = 0x0000000d;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/* addr = 0x10005014; */
	addr = remap_addr1 + 0x0014;
	tmp = 0x00000032;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/*PULL Enable */
	/* addr = 0x100020a4; */
	addr = remap_addr2 + 0x00a4;
	tmp = 0x000000ff;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/*PULL select enable */
	/* addr = 0x100020d4; */
	addr = remap_addr2 + 0x00d4;
	tmp = 0x000000b4;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));

	/* addr = 0x100020d8; */
	addr = remap_addr2 + 0x00d8;
	tmp = 0x0000004b;
	DRV_WriteReg32(addr, tmp);
	WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
#endif

	return 0;
}

UINT32 mtk_wcn_consys_jtag_flag_ctrl(UINT32 en)
{
	WMT_PLAT_INFO_FUNC("%s jtag set for MCU\n", en ? "enable" : "disable");
	gJtagCtrl = en;
	return 0;
}

#endif

static INT32 mtk_wmt_probe(struct platform_device *pdev)
{
	int ret = 0;

	pm_runtime_enable(&pdev->dev);
	my_pdev = pdev;

#if !defined(CONFIG_MTK_CLKMGR)
#if CONSYS_AHB_CLK_MAGEMENT
	clk_infra_conn_main = devm_clk_get(&pdev->dev, "bus");
	if (IS_ERR(clk_infra_conn_main)) {
		WMT_PLAT_ERR_FUNC("[CCF]cannot get clk_infra_conn_main clock.\n");
		return PTR_ERR(clk_infra_conn_main);
	}
	WMT_PLAT_DBG_FUNC("[CCF]clk_infra_conn_main=%p\n", clk_infra_conn_main);
#endif
#endif /* !defined(CONFIG_MTK_CLKMGR) */

#if CONSYS_PMIC_CTRL_ENABLE
#if !defined(CONFIG_MTK_PMIC_LEGACY)
	reg_VCN18 = regulator_get(&pdev->dev, "vcn18");
	if (IS_ERR(reg_VCN18)) {
		ret = PTR_ERR(reg_VCN18);
		WMT_PLAT_ERR_FUNC("Regulator_get VCN_1V8 fail, ret=%d\n", ret);
	}
	reg_VCN28 = regulator_get(&pdev->dev, "vcn28");
	if (IS_ERR(reg_VCN28)) {
		ret = PTR_ERR(reg_VCN28);
		WMT_PLAT_ERR_FUNC("Regulator_get VCN_2V8 fail, ret=%d\n", ret);
	}
	reg_VCN33_BT = regulator_get(&pdev->dev, "vcn33_bt");
	if (IS_ERR(reg_VCN33_BT)) {
		ret = PTR_ERR(reg_VCN33_BT);
		WMT_PLAT_ERR_FUNC("Regulator_get VCN33_BT fail, ret=%d\n", ret);
	}
	reg_VCN33_WIFI = regulator_get(&pdev->dev, "vcn33_wifi");
	if (IS_ERR(reg_VCN33_WIFI)) {
		ret = PTR_ERR(reg_VCN33_WIFI);
		WMT_PLAT_ERR_FUNC("Regulator_get VCN33_WIFI fail, ret=%d\n", ret);
	}
#endif
#endif

#if CONFIG_RESET_CONTROL
	rstc = devm_reset_control_get(&pdev->dev, "connsys");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		WMT_PLAT_ERR_FUNC("CanNot get consys reset. ret=%d\n", ret);
		return PTR_ERR(rstc);
	}
#endif

#if !defined(CONFIG_MTK_GPIO_LEGACY)
	consys_pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(consys_pinctrl)) {
		ret = PTR_ERR(consys_pinctrl);
		WMT_PLAT_ERR_FUNC("CanNot find consys pinctrl. ret=%d\n", ret);
		return PTR_ERR(consys_pinctrl);
	}
#endif /* !defined(CONFIG_MTK_GPIO_LEGACY) */

	return 0;
}

static INT32 mtk_wmt_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);

	return 0;
}

VOID mtk_wcn_consys_power_on(VOID)
{
	INT32 iRet = -1;

#if defined(CONFIG_MTK_CLKMGR)
	iRet = conn_power_on(); /* consult clkmgr owner. */
	if (iRet)
		WMT_PLAT_ERR_FUNC("conn_power_on fail(%d)\n", iRet);
	WMT_PLAT_DBG_FUNC("conn_power_on ok\n");
#else
	iRet = pm_runtime_get_sync(&my_pdev->dev);
	if (iRet)
		WMT_PLAT_ERR_FUNC("pm_runtime_get_sync() fail(%d)\n", iRet);
	else
		WMT_PLAT_INFO_FUNC("pm_runtime_get_sync() CONSYS ok\n");

	iRet = device_init_wakeup(&my_pdev->dev, true);
	if (iRet)
		WMT_PLAT_ERR_FUNC("device_init_wakeup(true) fail.\n");
	else
		WMT_PLAT_INFO_FUNC("device_init_wakeup(true) CONSYS ok\n");
#endif /* defined(CONFIG_MTK_CLKMGR) */

}

VOID mtk_wcn_consys_power_off(VOID)
{
	INT32 iRet = -1;

#if defined(CONFIG_MTK_CLKMGR)
	/*power off connsys by API (MT6582, MT6572 are different) API: conn_power_off() */
	iRet = conn_power_off();	/* consult clkmgr owner */
	if (iRet)
		WMT_PLAT_ERR_FUNC("conn_power_off fail(%d)\n", iRet);
	WMT_PLAT_DBG_FUNC("conn_power_off ok\n");
#else
	iRet = pm_runtime_put_sync(&my_pdev->dev);
	if (iRet)
		WMT_PLAT_ERR_FUNC("pm_runtime_put_sync() fail.\n");
	else
		WMT_PLAT_INFO_FUNC("pm_runtime_put_sync() CONSYS ok\n");

	iRet = device_init_wakeup(&my_pdev->dev, false);
	if (iRet)
		WMT_PLAT_ERR_FUNC("device_init_wakeup(false) fail.\n");
	else
		WMT_PLAT_INFO_FUNC("device_init_wakeup(false) CONSYS ok\n");
#endif
}

INT32 mtk_wcn_consys_hw_reg_ctrl(UINT32 on, UINT32 co_clock_type)
{
	UINT32 retry = 10;
	UINT32 consysHwChipId = 0;

	WMT_PLAT_DBG_FUNC("CONSYS-HW-REG-CTRL(0x%08x),start\n", on);

	if (on) {
		WMT_PLAT_DBG_FUNC("++\n");
/*step1.PMIC ctrl*/
#if CONSYS_PMIC_CTRL_ENABLE
		/*need PMIC driver provide new API protocol */
		/*1.AP power on VCN_1V8 LDO (with PMIC_WRAP API) VCN_1V8  */
#if CONSYS_PMIC_CTRL_UPMU
	upmu_set_vcn_1v8_lp_mode_set(0);
#else
	pmic_set_register_value(PMIC_RG_VCN18_ON_CTRL, 0);
#endif
		/* VOL_DEFAULT, VOL_1200, VOL_1300, VOL_1500, VOL_1800... */
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerOn(MT6323_POWER_LDO_VCN_1V8, VOL_1800, "wcn_drv");
#else
		if (reg_VCN18) {
			regulator_set_voltage(reg_VCN18, 1800000, 1800000);
			if (regulator_enable(reg_VCN18))
				WMT_PLAT_ERR_FUNC("enable VCN18 fail\n");
			else
				WMT_PLAT_DBG_FUNC("enable VCN18 ok\n");
		}
#endif
		udelay(150);

		if (co_clock_type) {
			/*step0,clk buf ctrl */
			WMT_PLAT_INFO_FUNC("co clock type(%d),turn on clk buf\n", co_clock_type);
#if CONSYS_CLOCK_BUF_CTRL
			clk_buf_ctrl(CLK_BUF_CONN, 1);
#endif
			/*if co-clock mode: */
			/*2.set VCN28 to SW control mode (with PMIC_WRAP API) */
			/*turn on VCN28 LDO only when FMSYS is activated"  */
#if CONSYS_PMIC_CTRL_UPMU
			upmu_set_vcn28_on_ctrl(0);
#else
			pmic_set_register_value(PMIC_RG_VCN28_ON_CTRL, 0);
#endif
		} else {
			/*if NOT co-clock: */
			/*2.1.switch VCN28 to HW control mode (with PMIC_WRAP API) */
#if CONSYS_PMIC_CTRL_UPMU
			upmu_set_vcn28_on_ctrl(1);
#else
			pmic_set_register_value(PMIC_RG_VCN28_ON_CTRL, 1);
#endif
			/*2.2.turn on VCN28 LDO (with PMIC_WRAP API)" */
			/*fix vcn28 not balance warning */
#if defined(CONFIG_MTK_PMIC_LEGACY)
			hwPowerOn(MT6323_POWER_LDO_VCN28, VOL_2800, "wcn_drv");
#else
			if (reg_VCN28) {
				regulator_set_voltage(reg_VCN28, 2800000, 2800000);
				if (regulator_enable(reg_VCN28))
					WMT_PLAT_ERR_FUNC("enable VCN_2V8 fail!\n");
				else
					WMT_PLAT_DBG_FUNC("enable VCN_2V8 ok\n");
			}
#endif
		}
#endif

/*step2.MTCMOS ctrl*/

#ifdef CONFIG_OF		/*use DT */
		/*3.assert CONNSYS CPU SW reset  0x10007018 "[12]=1'b1  [31:24]=8'h88 (key)" */
#if CONFIG_RESET_CONTROL
		/*reset_control_reset(rstc);*/
#else
		CONSYS_REG_WRITE((conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET),
				 CONSYS_REG_READ(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET) |
				 CONSYS_CPU_SW_RST_BIT | CONSYS_CPU_SW_RST_CTRL_KEY);
#endif

#if CONSYS_PWR_ON_OFF_API_AVAILABLE
		mtk_wcn_consys_power_on();
#else
		/*turn on SPM clock gating enable PWRON_CONFG_EN  0x10006000  32'h0b160001 */
		CONSYS_REG_WRITE((conn_reg.spm_base + CONSYS_PWRON_CONFG_EN_OFFSET), CONSYS_PWRON_CONFG_EN_VALUE);

		/*2.write conn_top1_pwr_on=1, power on conn_top1 0x10006280 [2]  1'b1 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) |
				 CONSYS_SPM_PWR_ON_BIT);
		/*3.read conn_top1_pwr_on_ack =1, power on ack ready 0x1000660C [1] */
		while (0 == (CONSYS_PWR_ON_ACK_BIT & CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_OFFSET)))
			NULL;
		/*5.write conn_top1_pwr_on_s=1, power on conn_top1 0x10006280 [3]  1'b1 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) |
				 CONSYS_SPM_PWR_ON_S_BIT);
		/*6.write conn_clk_dis=0, enable connsys clock 0x10006280 [4]  1'b0 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) &
				 ~CONSYS_CLK_CTRL_BIT);
		/*7.wait 1us    */
		udelay(1);
		/*8.read conn_top1_pwr_on_ack_s =1, power on ack ready 0x10006610 [1] */
		while (0 == (CONSYS_PWR_CONN_ACK_S_BIT &
			CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_S_OFFSET)))
			NULL;
		/*9.release connsys ISO, conn_top1_iso_en=0 0x10006280 [1]  1'b0 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) &
				 ~CONSYS_SPM_PWR_ISO_S_BIT);
		/*10.release SW reset of connsys, conn_ap_sw_rst_b=1  0x10006280[0]   1'b1 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) |
				 CONSYS_SPM_PWR_RST_BIT);
		/*disable AXI BUS protect */
		CONSYS_REG_WRITE(conn_reg.topckgen_base + CONSYS_TOPAXI_PROT_EN_OFFSET,
				 CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_TOPAXI_PROT_EN_OFFSET) &
				 ~CONSYS_PROT_MASK);
		while (CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_TOPAXI_PROT_STA1_OFFSET) & CONSYS_PROT_MASK)
			NULL;
#endif
		/*11.26M is ready now, delay 10us for mem_pd de-assert */
		udelay(10);
		/*enable AP bus clock : connmcu_bus_pd  API: enable_clock() ++?? */
#if CONSYS_AHB_CLK_MAGEMENT
#if defined(CONFIG_MTK_CLKMGR)
		enable_clock(MT_CG_INFRA_CONNMCU_BUS, "WCN_MOD");
		WMT_PLAT_DBG_FUNC("enable MT_CG_INFRA_CONNMCU_BUS CLK\n");
#else
		clk_prepare_enable(clk_infra_conn_main);
		WMT_PLAT_DBG_FUNC("[CCF]enable clk_infra_conn_main\n");
#endif /* defined(CONFIG_MTK_CLKMGR) */
#endif
		/*12.poll CONNSYS CHIP ID until chipid is returned  0x18070008 */
		while (retry-- > 0) {
			consysHwChipId = CONSYS_REG_READ(conn_reg.mcu_base + CONSYS_CHIP_ID_OFFSET);
			if ((consysHwChipId == 0x0321) || (consysHwChipId == 0x0335) || (consysHwChipId == 0x0337)) {
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry, consysHwChipId);
				break;
			}
			if ((consysHwChipId == 0x8163) || (consysHwChipId == 0x8127)) {
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry, consysHwChipId);
				break;
			}

			WMT_PLAT_ERR_FUNC("Read CONSYS chipId(0x%08x)", consysHwChipId);
			msleep(20);
		}

		if ((0 == retry) || (0 == consysHwChipId)) {
			WMT_PLAT_ERR_FUNC("Maybe has a consys power on issue,(0x%08x)\n", consysHwChipId);
#if CONFIG_RESET_CONTROL
#else
			WMT_PLAT_ERR_FUNC("reg dump:CONSYS_CPU_SW_RST_REG(0x%x)\n",
				  CONSYS_REG_READ(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET));
#endif
#if CONSYS_PWR_ON_OFF_API_AVAILABLE
#else
			WMT_PLAT_ERR_FUNC("reg dump:CONSYS_PWR_CONN_ACK_REG(0x%x)\n",
				   CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_OFFSET));
			WMT_PLAT_ERR_FUNC("reg dump:CONSYS_PWR_CONN_ACK_S_REG(0x%x)\n",
				   CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_S_OFFSET));
			WMT_PLAT_ERR_FUNC("reg dump:CONSYS_TOP1_PWR_CTRL_REG(0x%x)\n",
				   CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET));
#endif
		}

#if 0
		/*13.{default no need}update ROMDEL/PATCH RAM DELSEL if needed 0x18070114 */

		/*
		 *14.write 1 to conn_mcu_confg ACR[1] if real speed MBIST
		 *(default write "1") ACR 0x18070110[18] 1'b1
		 *if this bit is 0, HW will do memory auto test under low CPU frequence (26M Hz)
		 *if this bit is 0, HW will do memory auto test under high CPU frequence(138M Hz)
		 *inclulding low CPU frequence
		 */
		CONSYS_REG_WRITE(conn_reg.mcu_base + CONSYS_MCU_CFG_ACR_OFFSET,
				 CONSYS_REG_READ(conn_reg.mcu_base + CONSYS_MCU_CFG_ACR_OFFSET) |
				 CONSYS_MCU_CFG_ACR_MBIST_BIT);


		/*15.default no need,update ANA_WBG(AFE) CR if needed, CONSYS_AFE_REG */
		CONSYS_REG_WRITE(CONSYS_AFE_REG_DIG_RCK_01, CONSYS_AFE_REG_DIG_RCK_01_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_PLL_02, CONSYS_AFE_REG_WBG_PLL_02_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_WB_TX_01, CONSYS_AFE_REG_WBG_WB_TX_01_VALUE);
#endif

#if CONFIG_RESET_CONTROL
#else
		/*16.deassert CONNSYS CPU SW reset 0x10007018 "[12]=1'b0 [31:24] =8'h88 (key)" */
		CONSYS_REG_WRITE(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET,
				 (CONSYS_REG_READ(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET) &
				 ~CONSYS_CPU_SW_RST_BIT) | CONSYS_CPU_SW_RST_CTRL_KEY);
#endif

		msleep(20);
#else /*use HADRCODE, maybe no use.. */
		/*3.assert CONNSYS CPU SW reset  0x10007018  "[12]=1'b1  [31:24]=8'h88 (key)" */
		CONSYS_REG_WRITE(CONSYS_CPU_SW_RST_REG,
				 (CONSYS_REG_READ(CONSYS_CPU_SW_RST_REG) | CONSYS_CPU_SW_RST_BIT |
				  CONSYS_CPU_SW_RST_CTRL_KEY));
		/*turn on SPM clock gating enable PWRON_CONFG_EN 0x10006000 32'h0b160001 */
		CONSYS_REG_WRITE(CONSYS_PWRON_CONFG_EN_REG, CONSYS_PWRON_CONFG_EN_VALUE);

#if CONSYS_PWR_ON_OFF_API_AVAILABLE
		mtk_wcn_consys_power_on();
#else
		/*2.write conn_top1_pwr_on=1, power on conn_top1 0x10006280 [2]  1'b1 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_ON_BIT);
		/*3.read conn_top1_pwr_on_ack =1, power on ack ready 0x1000660C [1] */
		while (0 == (CONSYS_PWR_ON_ACK_BIT & CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_REG)))
			NULL;
		/*5.write conn_top1_pwr_on_s=1, power on conn_top1 0x10006280 [3]  1'b1 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_ON_S_BIT);
		/*6.write conn_clk_dis=0, enable connsys clock 0x10006280 [4]  1'b0 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_CLK_CTRL_BIT);
		/*7.wait 1us    */
		udelay(1);
		/*8.read conn_top1_pwr_on_ack_s =1, power on ack ready 0x10006610 [1]  */
		while (0 == (CONSYS_PWR_CONN_ACK_S_BIT & CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_S_REG)))
			NULL;
		/*9.release connsys ISO, conn_top1_iso_en=0 0x10006280 [1]  1'b0 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_SPM_PWR_ISO_S_BIT);
		/*10.release SW reset of connsys, conn_ap_sw_rst_b=1 0x10006280[0] 1'b1 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_RST_BIT);
		/*disable AXI BUS protect */
		CONSYS_REG_WRITE(CONSYS_TOPAXI_PROT_EN, CONSYS_REG_READ(CONSYS_TOPAXI_PROT_EN) & ~CONSYS_PROT_MASK);
		while (CONSYS_REG_READ(CONSYS_TOPAXI_PROT_STA1) & CONSYS_PROT_MASK)
			NULL;
#endif
		/*11.26M is ready now, delay 10us for mem_pd de-assert */
		udelay(10);
		/*enable AP bus clock : connmcu_bus_pd  API: enable_clock() ++?? */
#if CONSYS_AHB_CLK_MAGEMENT
#if defined(CONFIG_MTK_CLKMGR)
		enable_clock(MT_CG_INFRA_CONNMCU_BUS, "WCN_MOD");
		WMT_PLAT_DBG_FUNC("enable MT_CG_INFRA_CONNMCU_BUS CLK\n");
#else
		clk_prepare_enable(clk_infra_conn_main);
		WMT_PLAT_DBG_FUNC("[CCF]enable clk_infra_conn_main\n");
#endif /* defined(CONFIG_MTK_CLKMGR) */
#endif
		/*12.poll CONNSYS CHIP ID until 6752 is returned 0x18070008 32'h6752 */
		while (retry-- > 0) {
			WMT_PLAT_DBG_FUNC("CONSYS_CHIP_ID_REG(0x%08x)", CONSYS_REG_READ(CONSYS_CHIP_ID_REG));
			consysHwChipId = CONSYS_REG_READ(CONSYS_CHIP_ID_REG);
			if ((consysHwChipId == 0x0321) || (consysHwChipId == 0x0335) || (consysHwChipId == 0x0337)) {
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry, consysHwChipId);
				break;
			}
			if ((consysHwChipId == 0x8163) || (consysHwChipId == 0x8127)) {
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry, consysHwChipId);
				break;
			}
			msleep(20);
		}

		if ((0 == retry) || (0 == consysHwChipId)) {
			WMT_PLAT_ERR_FUNC("Maybe has a consys power on issue,(0x%08x)\n", consysHwChipId);
			WMT_PLAT_INFO_FUNC("reg dump:CONSYS_CPU_SW_RST_REG(0x%x)\n",
					   CONSYS_REG_READ(CONSYS_CPU_SW_RST_REG));
			WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_REG(0x%x)\n",
					   CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_REG));
			WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_S_REG(0x%x)\n",
					   CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_S_REG));
			WMT_PLAT_INFO_FUNC("reg dump:CONSYS_TOP1_PWR_CTRL_REG(0x%x)\n",
					   CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG));
		}

		/*13.{default no need}update ROMDEL/PATCH RAM DELSEL if needed 0x18070114  */

#if 0
		/*
		 *14.write 1 to conn_mcu_confg ACR[1] if real speed MBIST
		 *(default write "1") ACR 0x18070110[18] 1'b1
		 *if this bit is 0, HW will do memory auto test under low CPU frequence (26M Hz)
		 *if this bit is 0, HW will do memory auto test under high CPU frequence(138M Hz)
		 *inclulding low CPU frequence
		 */
		CONSYS_REG_WRITE(CONSYS_MCU_CFG_ACR_REG,
				 CONSYS_REG_READ(CONSYS_MCU_CFG_ACR_REG) | CONSYS_MCU_CFG_ACR_MBIST_BIT);

		/*update ANA_WBG(AFE) CR. AFE setting file:  AP Offset = 0x180B2000   */
#endif
#if 0
		/*15.default no need,update ANA_WBG(AFE) CR if needed, CONSYS_AFE_REG */
		CONSYS_REG_WRITE(CONSYS_AFE_REG_DIG_RCK_01, CONSYS_AFE_REG_DIG_RCK_01_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_PLL_02, CONSYS_AFE_REG_WBG_PLL_02_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_WB_TX_01, CONSYS_AFE_REG_WBG_WB_TX_01_VALUE);

		/*16.deassert CONNSYS CPU SW reset 0x10007018 "[12]=1'b0 [31:24] =8'h88(key)" */
		CONSYS_REG_WRITE(CONSYS_CPU_SW_RST_REG,
				 (CONSYS_REG_READ(CONSYS_CPU_SW_RST_REG) & ~CONSYS_CPU_SW_RST_BIT) |
				 CONSYS_CPU_SW_RST_CTRL_KEY);
#endif

#endif
		msleep(20);	/* msleep < 20ms can sleep for up to 20ms */

	} else {

#ifdef CONFIG_OF

#if CONSYS_AHB_CLK_MAGEMENT
#if defined(CONFIG_MTK_CLKMGR)
		disable_clock(MT_CG_INFRA_CONNMCU_BUS, "WMT_MOD");
#else
		clk_disable_unprepare(clk_infra_conn_main);
		WMT_PLAT_DBG_FUNC("[CCF] clk_disable_unprepare(clk_infra_conn_main) calling\n");
#endif /* defined(CONFIG_MTK_CLKMGR) */
#endif

#if CONSYS_PWR_ON_OFF_API_AVAILABLE
	mtk_wcn_consys_power_off();
#else
		{
			INT32 count = 0;

			CONSYS_REG_WRITE(conn_reg.topckgen_base + CONSYS_TOPAXI_PROT_EN_OFFSET,
					 CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_TOPAXI_PROT_EN_OFFSET) |
					 CONSYS_PROT_MASK);
			while ((CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_TOPAXI_PROT_STA1_OFFSET) &
				CONSYS_PROT_MASK) != CONSYS_PROT_MASK) {
				count++;
				if (count > 1000)
					break;
			}
		}
		/*release connsys ISO, conn_top1_iso_en=1  0x10006280 [1]  1'b1 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) |
				 CONSYS_SPM_PWR_ISO_S_BIT);
		/*assert SW reset of connsys, conn_ap_sw_rst_b=0  0x10006280[0] 1'b0 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) &
				 ~CONSYS_SPM_PWR_RST_BIT);
		/*write conn_clk_dis=1, disable connsys clock  0x10006280 [4]  1'b1 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) |
				 CONSYS_CLK_CTRL_BIT);
		/*wait 1us      */
		udelay(1);
		/*write conn_top1_pwr_on=0, power off conn_top1 0x10006280 [3:2] 2'b00 */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET,
				 CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) &
				 ~(CONSYS_SPM_PWR_ON_BIT | CONSYS_SPM_PWR_ON_S_BIT));

#endif

#else

#if CONSYS_PWR_ON_OFF_API_AVAILABLE

#if CONSYS_AHB_CLK_MAGEMENT
#if defined(CONFIG_MTK_CLKMGR)
		disable_clock(MT_CG_INFRA_CONNMCU_BUS, "WMT_MOD");
#else
		clk_disable_unprepare(clk_infra_conn_main);
#endif /* defined(CONFIG_MTK_CLKMGR) */
#endif

		mtk_wcn_consys_power_off();
#else
		{
			INT32 count = 0;

			CONSYS_REG_WRITE(CONSYS_TOPAXI_PROT_EN,
					 CONSYS_REG_READ(CONSYS_TOPAXI_PROT_EN) | CONSYS_PROT_MASK);
			while ((CONSYS_REG_READ(CONSYS_TOPAXI_PROT_STA1) & CONSYS_PROT_MASK) != CONSYS_PROT_MASK) {
				count++;
				if (count > 1000)
					break;
			}

		}
		/*release connsys ISO, conn_top1_iso_en=1 0x10006280 [1]  1'b1 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_ISO_S_BIT);
		/*assert SW reset of connsys, conn_ap_sw_rst_b=0 0x10006280[0] 1'b0  */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_SPM_PWR_RST_BIT);
		/*write conn_clk_dis=1, disable connsys clock 0x10006280 [4]  1'b1 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG,
				 CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_CLK_CTRL_BIT);
		/*wait 1us      */
		udelay(1);
		/*write conn_top1_pwr_on=0, power off conn_top1 0x10006280 [3:2] 2'b00 */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) &
		~(CONSYS_SPM_PWR_ON_BIT | CONSYS_SPM_PWR_ON_S_BIT));
#endif

#endif

#if CONSYS_PMIC_CTRL_ENABLE
		if (co_clock_type) {
			/*VCN28 has been turned off by GPS OR FM */
#if CONSYS_CLOCK_BUF_CTRL
			clk_buf_ctrl(CLK_BUF_CONN, 0);
#endif
		} else {
#if CONSYS_PMIC_CTRL_UPMU
			upmu_set_vcn28_on_ctrl(0);
#else
			pmic_set_register_value(PMIC_RG_VCN28_ON_CTRL, 0);
#endif
			/*turn off VCN28 LDO (with PMIC_WRAP API)" */
#if defined(CONFIG_MTK_PMIC_LEGACY)
			hwPowerDown(MT6323_POWER_LDO_VCN28, "wcn_drv");
#else
			if (reg_VCN28) {
				if (regulator_disable(reg_VCN28))
					WMT_PLAT_ERR_FUNC("disable VCN_2V8 fail!\n");
				else
					WMT_PLAT_DBG_FUNC("disable VCN_2V8 ok\n");
			}
#endif
		}

		/*AP power off MT6625L VCN_1V8 LDO */
#if CONSYS_PMIC_CTRL_UPMU
		upmu_set_vcn_1v8_lp_mode_set(0);
#else
		pmic_set_register_value(PMIC_RG_VCN18_ON_CTRL, 0);
#endif
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerDown(MT6323_POWER_LDO_VCN_1V8, "wcn_drv");
#else
		if (reg_VCN18) {
			if (regulator_disable(reg_VCN18))
				WMT_PLAT_ERR_FUNC("disable VCN_1V8 fail!\n");
			else
				WMT_PLAT_DBG_FUNC("disable VCN_1V8 ok\n");
		}
#endif

#endif

	}
	WMT_PLAT_DBG_FUNC("CONSYS-HW-REG-CTRL(0x%08x),finish\n", on);
	return 0;
}

INT32 mtk_wcn_consys_hw_gpio_ctrl(UINT32 on)
{
	INT32 iRet = 0;

	WMT_PLAT_DBG_FUNC("CONSYS-HW-GPIO-CTRL(0x%08x), start\n", on);

	if (on) {

		/*if external modem used,GPS_SYNC still needed to control */
		iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC, PIN_STA_INIT);
		iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA, PIN_STA_INIT);

		iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP, PIN_STA_INIT);

		/* TODO: [FixMe][GeorgeKuo] double check if BGF_INT is implemented ok */
		/* iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT, PIN_STA_MUX); */
		iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_INIT);
		iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_EINT_DIS);
		WMT_PLAT_DBG_FUNC("CONSYS-HW, BGF IRQ registered and disabled\n");

	} else {

		/* set bgf eint/all eint to deinit state, namely input low state */
		iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_EINT_DIS);
		iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_DEINIT);
		WMT_PLAT_DBG_FUNC("CONSYS-HW, BGF IRQ unregistered and disabled\n");
		/* iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT, PIN_STA_DEINIT); */

		/*if external modem used,GPS_SYNC still needed to control */
		iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC, PIN_STA_DEINIT);
		iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP, PIN_STA_DEINIT);
		/* deinit gps_lna */
		iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA, PIN_STA_DEINIT);

	}
	WMT_PLAT_DBG_FUNC("CONSYS-HW-GPIO-CTRL(0x%08x), finish\n", on);
	return iRet;

}

INT32 mtk_wcn_consys_hw_pwr_on(UINT32 co_clock_type)
{
	INT32 iRet = 0;

	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-ON, start\n");

	iRet += mtk_wcn_consys_hw_reg_ctrl(1, co_clock_type);
	iRet += mtk_wcn_consys_hw_gpio_ctrl(1);
#if CONSYS_ENALBE_SET_JTAG
	if (gJtagCtrl)
		mtk_wcn_consys_jtag_set_for_mcu();
#endif
	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-ON, finish(%d)\n", iRet);
	return iRet;
}

INT32 mtk_wcn_consys_hw_pwr_off(VOID)
{
	INT32 iRet = 0;

	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-OFF, start\n");

	iRet += mtk_wcn_consys_hw_reg_ctrl(0, 0);
	iRet += mtk_wcn_consys_hw_gpio_ctrl(0);

	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-OFF, finish(%d)\n", iRet);
	return iRet;
}

INT32 mtk_wcn_consys_hw_rst(UINT32 co_clock_type)
{
	INT32 iRet = 0;

	WMT_PLAT_INFO_FUNC("CONSYS-HW, hw_rst start, eirq should be disabled before this step\n");

	/*1. do whole hw power off flow */
	iRet += mtk_wcn_consys_hw_reg_ctrl(0, co_clock_type);

	/*2. do whole hw power on flow */
	iRet += mtk_wcn_consys_hw_reg_ctrl(1, co_clock_type);

	WMT_PLAT_INFO_FUNC("CONSYS-HW, hw_rst finish, eirq should be enabled after this step\n");
	return iRet;
}

#if CONSYS_BT_WIFI_SHARE_V33
INT32 mtk_wcn_consys_hw_bt_paldo_ctrl(UINT32 enable)
{
	/* spin_lock_irqsave(&gBtWifiV33.lock,gBtWifiV33.flags); */
	if (enable) {
		if (1 == gBtWifiV33.counter) {
			gBtWifiV33.counter++;
			WMT_PLAT_DBG_FUNC("V33 has been enabled,counter(%d)\n", gBtWifiV33.counter);
		} else if (2 == gBtWifiV33.counter) {
			WMT_PLAT_DBG_FUNC("V33 has been enabled,counter(%d)\n", gBtWifiV33.counter);
		} else {
#if CONSYS_PMIC_CTRL_ENABLE
			/*do BT PMIC on,depenency PMIC API ready */
			/*switch BT PALDO control from SW mode to HW mode:0x416[5]-->0x1 */
			/* VOL_DEFAULT, VOL_3300, VOL_3400, VOL_3500, VOL_3600 */
			hwPowerOn(MT6323_POWER_LDO_VCN33, VOL_3300, "wcn_drv");
			upmu_set_vcn33_on_ctrl_bt(1);
#endif
			WMT_PLAT_INFO_FUNC("WMT do BT/WIFI v3.3 on\n");
			gBtWifiV33.counter++;
		}

	} else {
		if (1 == gBtWifiV33.counter) {
			/*do BT PMIC off */
			/*switch BT PALDO control from HW mode to SW mode:0x416[5]-->0x0 */
#if CONSYS_PMIC_CTRL_ENABLE
		    upmu_set_vcn33_on_ctrl_bt(0);
			hwPowerDown(MT6323_POWER_LDO_VCN33, "wcn_drv");
#endif
			WMT_PLAT_INFO_FUNC("WMT do BT/WIFI v3.3 off\n");
			gBtWifiV33.counter--;
		} else if (2 == gBtWifiV33.counter) {
			gBtWifiV33.counter--;
			WMT_PLAT_DBG_FUNC("V33 no need disabled,counter(%d)\n", gBtWifiV33.counter);
		} else {
			WMT_PLAT_DBG_FUNC("V33 has been disabled,counter(%d)\n", gBtWifiV33.counter);
		}

	}
	/* spin_unlock_irqrestore(&gBtWifiV33.lock,gBtWifiV33.flags); */
	return 0;
}

INT32 mtk_wcn_consys_hw_wifi_paldo_ctrl(UINT32 enable)
{
	mtk_wcn_consys_hw_bt_paldo_ctrl(enable);
	return 0;
}

#else
INT32 mtk_wcn_consys_hw_bt_paldo_ctrl(UINT32 enable)
{

	if (enable) {
		/*do BT PMIC on,depenency PMIC API ready */
		/*switch BT PALDO control from SW mode to HW mode:0x416[5]-->0x1 */
#if CONSYS_PMIC_CTRL_ENABLE
		/* VOL_DEFAULT, VOL_3300, VOL_3400, VOL_3500, VOL_3600 */
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerOn(MT6323_POWER_LDO_VCN33_BT, VOL_3300, "wcn_drv");
#else
		if (reg_VCN33_BT) {
			regulator_set_voltage(reg_VCN33_BT, 3300000, 3300000);
			if (regulator_enable(reg_VCN33_BT))
				WMT_PLAT_ERR_FUNC("WMT do BT PMIC on fail!\n");
		}
#endif
#if CONSYS_PMIC_CTRL_UPMU
		upmu_set_vcn33_on_ctrl_bt(1);
#else
		pmic_set_register_value(PMIC_RG_VCN33_ON_CTRL_BT, 1);
#endif

#endif
		WMT_PLAT_INFO_FUNC("WMT do BT PMIC on\n");
	} else {
		/*do BT PMIC off */
		/*switch BT PALDO control from HW mode to SW mode:0x416[5]-->0x0 */
#if CONSYS_PMIC_CTRL_ENABLE
#if CONSYS_PMIC_CTRL_UPMU
		upmu_set_vcn33_on_ctrl_bt(0);
#else
		pmic_set_register_value(PMIC_RG_VCN33_ON_CTRL_BT, 0);
#endif
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerDown(MT6323_POWER_LDO_VCN33_BT, "wcn_drv");
#else
		if (reg_VCN33_BT)
			if (regulator_disable(reg_VCN33_BT))
				WMT_PLAT_ERR_FUNC("WMT do BT PMIC off fail!\n");
#endif
#endif
		WMT_PLAT_INFO_FUNC("WMT do BT PMIC off\n");
	}

	return 0;

}

INT32 mtk_wcn_consys_hw_wifi_paldo_ctrl(UINT32 enable)
{

	if (enable) {
		/*do WIFI PMIC on,depenency PMIC API ready */
		/*switch WIFI PALDO control from SW mode to HW mode:0x418[14]-->0x1 */
#if CONSYS_PMIC_CTRL_ENABLE
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerOn(MT6323_POWER_LDO_VCN33_WIFI, VOL_3300, "wcn_drv");
#else
		if (reg_VCN33_WIFI) {
			regulator_set_voltage(reg_VCN33_WIFI, 3300000, 3300000);
			if (regulator_enable(reg_VCN33_WIFI))
				WMT_PLAT_ERR_FUNC("WMT do WIFI PMIC on fail!\n");
			else
				WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC on !\n");
		}
#endif
#if CONSYS_PMIC_CTRL_UPMU
		upmu_set_vcn33_on_ctrl_wifi(1);
#else
		pmic_set_register_value(PMIC_RG_VCN33_ON_CTRL_WIFI, 1);
#endif
#endif
		WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC on\n");
	} else {
		/*do WIFI PMIC off */
		/*switch WIFI PALDO control from HW mode to SW mode:0x418[14]-->0x0 */
#if CONSYS_PMIC_CTRL_ENABLE
#if CONSYS_PMIC_CTRL_UPMU
		upmu_set_vcn33_on_ctrl_wifi(0);
#else
		pmic_set_register_value(PMIC_RG_VCN33_ON_CTRL_WIFI, 0);
#endif
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerDown(MT6323_POWER_LDO_VCN33_WIFI, "wcn_drv");
#else
		if (reg_VCN33_WIFI)
			if (regulator_disable(reg_VCN33_WIFI))
				WMT_PLAT_ERR_FUNC("WMT do WIFI PMIC off fail!\n");
#endif

#endif
		WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC off\n");
	}

	return 0;

}

#endif
INT32 mtk_wcn_consys_hw_vcn28_ctrl(UINT32 enable)
{
	if (enable) {
		/*in co-clock mode,need to turn on vcn28 when fm on */
#if CONSYS_PMIC_CTRL_ENABLE
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerOn(MT6323_POWER_LDO_VCN28, VOL_2800, "wcn_drv");
#else
		if (reg_VCN28) {
			regulator_set_voltage(reg_VCN28, 2800000, 2800000);
			if (regulator_enable(reg_VCN28))
				WMT_PLAT_ERR_FUNC("WMT do VCN28 PMIC on fail!\n");
		}
#endif
#endif
		WMT_PLAT_INFO_FUNC("turn on vcn28 for fm/gps usage in co-clock mode\n");
	} else {
		/*in co-clock mode,need to turn off vcn28 when fm off */
#if CONSYS_PMIC_CTRL_ENABLE
#if defined(CONFIG_MTK_PMIC_LEGACY)
		hwPowerDown(MT6323_POWER_LDO_VCN28, "wcn_drv");
#else
		if (reg_VCN28)
			if (regulator_disable(reg_VCN28))
				WMT_PLAT_ERR_FUNC("WMT do VCN28 PMIC off fail!\n");
#endif
#endif
		WMT_PLAT_INFO_FUNC("turn off vcn28 for fm/gps usage in co-clock mode\n");
	}
	return 0;
}

INT32 mtk_wcn_consys_hw_state_show(VOID)
{
	return 0;
}

INT32 mtk_wcn_consys_hw_restore(struct device *device)
{
	UINT32 addrPhy = 0;

	if (gConEmiPhyBase) {

#if CONSYS_EMI_MPU_SETTING
		/*set MPU for EMI share Memory */
		WMT_PLAT_INFO_FUNC("setting MPU for EMI share memory\n");

#if 0
	emi_mpu_set_region_protection(gConEmiPhyBase + SZ_1M/2,
		gConEmiPhyBase + SZ_1M,
		5,
		SET_ACCESS_PERMISSON(FORBIDDEN, NO_PROTECTION, FORBIDDEN, NO_PROTECTION));


#else
		WMT_PLAT_WARN_FUNC("not define platform config\n");
#endif

#endif
		/*consys to ap emi remapping register:10001310, cal remapping address */
		addrPhy = (gConEmiPhyBase & 0xFFF00000) >> 20;

		/*enable consys to ap emi remapping bit12 */
		addrPhy = addrPhy | 0x1000;

		CONSYS_REG_WRITE(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET,
				 CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET) | addrPhy);

		WMT_PLAT_INFO_FUNC("CONSYS_EMI_MAPPING dump in restore cb(0x%08x)\n",
				   CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET));

#if 1
		pEmibaseaddr = ioremap_nocache(gConEmiPhyBase + CONSYS_EMI_AP_PHY_OFFSET, CONSYS_EMI_MEM_SIZE);
#else
		pEmibaseaddr = ioremap_nocache(CONSYS_EMI_AP_PHY_BASE, CONSYS_EMI_MEM_SIZE);
#endif
		if (pEmibaseaddr) {
			WMT_PLAT_INFO_FUNC("EMI mapping OK(0x%p)\n", pEmibaseaddr);
			memset_io(pEmibaseaddr, 0, CONSYS_EMI_MEM_SIZE);
		} else {
			WMT_PLAT_ERR_FUNC("EMI mapping fail\n");
		}
	} else {
		WMT_PLAT_ERR_FUNC("consys emi memory address gConEmiPhyBase invalid\n");
	}

	return 0;
}

/*Reserved memory by device tree!*/
int reserve_memory_consys_fn(struct reserved_mem *rmem)
{
	WMT_PLAT_WARN_FUNC(" name: %s, base: 0x%llx, size: 0x%llx\n", rmem->name,
			   (unsigned long long)rmem->base, (unsigned long long)rmem->size);
	gConEmiPhyBase = rmem->base;
	return 0;
}

RESERVEDMEM_OF_DECLARE(reserve_memory_test, "mediatek,consys-reserve-memory", reserve_memory_consys_fn);


INT32 mtk_wcn_consys_hw_init(void)
{

	INT32 iRet = -1;
	UINT32 addrPhy = 0;
	INT32 i = 0;
	struct device_node *node = NULL;

	node = of_find_compatible_node(NULL, NULL, "mediatek,mt8127-consys");
	if (node) {
		/* registers base address */
		conn_reg.mcu_base = (SIZE_T) of_iomap(node, i);
		WMT_PLAT_DBG_FUNC("Get mcu register base(0x%zx)\n", conn_reg.mcu_base);
		i++;

#if CONFIG_RESET_CONTROL
#else
		conn_reg.ap_rgu_base = (SIZE_T) of_iomap(node, i);
		WMT_PLAT_DBG_FUNC("Get ap_rgu register base(0x%zx)\n", conn_reg.ap_rgu_base);
		i++;
#endif
		conn_reg.topckgen_base = (SIZE_T) of_iomap(node, i);
		WMT_PLAT_DBG_FUNC("Get topckgen register base(0x%zx)\n", conn_reg.topckgen_base);
		i++;

#if CONSYS_PWR_ON_OFF_API_AVAILABLE
#else
		conn_reg.spm_base = (SIZE_T) of_iomap(node, i);
		WMT_PLAT_DBG_FUNC("Get spm register base(0x%zx)\n", conn_reg.spm_base);
#endif
	} else {
		WMT_PLAT_ERR_FUNC("[%s] can't find CONSYS compatible node\n", __func__);
		return iRet;
	}

	if (gConEmiPhyBase) {
#if CONSYS_EMI_MPU_SETTING
		/*set MPU for EMI share Memory */
		WMT_PLAT_INFO_FUNC("setting MPU for EMI share memory\n");

#if 0
	emi_mpu_set_region_protection(gConEmiPhyBase + SZ_1M/2,
		gConEmiPhyBase + SZ_1M,
		5,
		SET_ACCESS_PERMISSON(FORBIDDEN, NO_PROTECTION, FORBIDDEN, NO_PROTECTION));
#else
		WMT_PLAT_WARN_FUNC("not define platform config\n");
#endif

#endif
		WMT_PLAT_DBG_FUNC("get consys start phy address(0x%zx)\n", (SIZE_T) gConEmiPhyBase);

		/*consys to ap emi remapping register:10001310, cal remapping address */
		addrPhy = (gConEmiPhyBase & 0xFFF00000) >> 20;

		/*enable consys to ap emi remapping bit12 */
		addrPhy = addrPhy | 0x1000;

		CONSYS_REG_WRITE(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET,
				 CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET) | addrPhy);

		WMT_PLAT_INFO_FUNC("CONSYS_EMI_MAPPING dump(0x%08x)\n",
				   CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET));

#if 1
		pEmibaseaddr = ioremap_nocache(gConEmiPhyBase + CONSYS_EMI_AP_PHY_OFFSET, CONSYS_EMI_MEM_SIZE);
#else
		pEmibaseaddr = ioremap_nocache(CONSYS_EMI_AP_PHY_BASE, CONSYS_EMI_MEM_SIZE);
#endif
		/* pEmibaseaddr = ioremap_nocache(0x80090400,270*KBYTE); */
		if (pEmibaseaddr) {
			WMT_PLAT_INFO_FUNC("EMI mapping OK(0x%p)\n", pEmibaseaddr);
			memset_io(pEmibaseaddr, 0, CONSYS_EMI_MEM_SIZE);
			iRet = 0;
		} else {
			WMT_PLAT_ERR_FUNC("EMI mapping fail\n");
		}
	} else {
		WMT_PLAT_ERR_FUNC("consys emi memory address gConEmiPhyBase invalid\n");
	}
#ifdef CONFIG_MTK_HIBERNATION
	WMT_PLAT_INFO_FUNC("register connsys restore cb for complying with IPOH function\n");
	register_swsusp_restore_noirq_func(ID_M_CONNSYS, mtk_wcn_consys_hw_restore, NULL);
#endif

	iRet = platform_driver_register(&mtk_wmt_dev_drv);
	if (iRet)
		WMT_PLAT_ERR_FUNC("WMT platform driver registered failed(%d)\n", iRet);

	return iRet;

}

INT32 mtk_wcn_consys_hw_deinit(void)
{
	if (pEmibaseaddr) {
		iounmap(pEmibaseaddr);
		pEmibaseaddr = NULL;
	}
#ifdef CONFIG_MTK_HIBERNATION
	unregister_swsusp_restore_noirq_func(ID_M_CONNSYS);
#endif

	platform_driver_unregister(&mtk_wmt_dev_drv);
	return 0;
}

UINT8 *mtk_wcn_consys_emi_virt_addr_get(UINT32 ctrl_state_offset)
{
	UINT8 *p_virtual_addr = NULL;

	if (!pEmibaseaddr) {
		WMT_PLAT_ERR_FUNC("EMI base address is NULL\n");
		return NULL;
	}
	WMT_PLAT_DBG_FUNC("ctrl_state_offset(%08x)\n", ctrl_state_offset);
	p_virtual_addr = pEmibaseaddr + ctrl_state_offset;

	return p_virtual_addr;
}

UINT32 mtk_wcn_consys_soc_chipid(void)
{
	return PLATFORM_SOC_CHIP;
}

#if !defined(CONFIG_MTK_GPIO_LEGACY)
struct pinctrl *mtk_wcn_consys_get_pinctrl()
{
	return consys_pinctrl;
}
#endif
INT32 mtk_wcn_consys_set_dbg_mode(UINT32 flag)
{
	INT32 ret = -1;
	PUINT8 vir_addr = NULL;

	vir_addr = mtk_wcn_consys_emi_virt_addr_get(EXP_APMEM_CTRL_CHIP_FW_DBGLOG_MODE);
	if (!vir_addr) {
		WMT_PLAT_ERR_FUNC("get vir address fail\n");
		return -2;
	}
	if (flag) {
		ret = 0;
		CONSYS_REG_WRITE(vir_addr, 0x1);
	} else {
		CONSYS_REG_WRITE(vir_addr, 0x0);
	}
	WMT_PLAT_ERR_FUNC("fw dbg mode register value(0x%08x)\n", CONSYS_REG_READ(vir_addr));
	return ret;
}
INT32 mtk_wcn_consys_set_dynamic_dump(PUINT32 str_buf)
{
	PUINT8 vir_addr = NULL;

	vir_addr = mtk_wcn_consys_emi_virt_addr_get(EXP_APMEM_CTRL_CHIP_DYNAMIC_DUMP);
	if (!vir_addr) {
		WMT_PLAT_ERR_FUNC("get vir address fail\n");
		return -2;
	}
	memcpy(vir_addr, str_buf, DYNAMIC_DUMP_GROUP_NUM*8);
	WMT_PLAT_INFO_FUNC("dynamic dump register value(0x%08x)\n", CONSYS_REG_READ(vir_addr));
	return 0;
}
