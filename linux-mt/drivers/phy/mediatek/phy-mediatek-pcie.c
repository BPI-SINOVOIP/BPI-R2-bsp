/*
 * Copyright (c) 2017 MediaTek Inc.
 * Author: Ryder Lee <ryder.lee@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

/* Offsets of sub-segment in each port registers */
#define PCIE_SIFSLV_PHYD_BANK2_BASE	0xa00
#define SSUSB_SIFSLV_PHYA_BASE		0xb00
#define SSUSB_SIFSLV_PHYA_DA_BASE	0xc00

/*
 * RX detection stable - 1 scale represent 8 reference cycles
 * cover reference clock from 1M~100MHz, 7us~40us
 */
#define B2_PHYD_RXDET1			(PCIE_SIFSLV_PHYD_BANK2_BASE + 0x28)
#define RG_SSUSB_RXDET_STB2		GENMASK(17, 9)
#define RG_SSUSB_RXDET_STB2_VAL(x)	((0x1ff & (x)) << 9)

#define B2_PHYD_RXDET2			(PCIE_SIFSLV_PHYD_BANK2_BASE + 0x2c)
#define RG_SSUSB_RXDET_STB2_P3		GENMASK(8, 0)
#define RG_SSUSB_RXDET_STB2_P3_VAL(x)	(0x1ff & (x))

#define U3_PHYA_REG0			(SSUSB_SIFSLV_PHYA_BASE + 0x00)
#define RG_PCIE_CLKDRV_OFFSET		GENMASK(3, 1)
#define RG_PCIE_CLKDRV_OFFSET_VAL(x)	((0x3 & (x)) << 2)

#define U3_PHYA_REG1			(SSUSB_SIFSLV_PHYA_BASE + 0x04)
#define RG_PCIE_CLKDRV_AMP		GENMASK(31, 29)
#define RG_PCIE_CLKDRV_AMP_VAL(x)	((0x7 & (x)) << 29)

#define DA_SSUSB_CDR_REFCK_SEL		(SSUSB_SIFSLV_PHYA_DA_BASE + 0x00)
#define RG_SSUSB_XTAL_EXT_PE1H		GENMASK(13, 12)
#define RG_SSUSB_XTAL_EXT_PE1H_VAL(x)	((0x3 & (x)) << 12)
#define RG_SSUSB_XTAL_EXT_PE2H		GENMASK(17, 16)
#define RG_SSUSB_XTAL_EXT_PE2H_VAL(x)		((0x3 & (x)) << 16)

#define DA_SSUSB_PLL_IC			(SSUSB_SIFSLV_PHYA_DA_BASE + 0x0c)
#define RG_SSUSB_PLL_IC_PE2H		GENMASK(15, 12)
#define RG_SSUSB_PLL_IC_PE2H_VAL(x)	((0xf & (x)) << 12)
#define RG_SSUSB_PLL_BR_PE2H		GENMASK(29, 28)
#define RG_SSUSB_PLL_BR_PE2H_VAL(x)	((0x3 & (x)) << 28)

#define DA_SSUSB_PLL_BC			(SSUSB_SIFSLV_PHYA_DA_BASE + 0x08)
#define RG_SSUSB_PLL_DIVEN_PE2H		GENMASK(21, 19)
#define RG_SSUSB_PLL_BC_PE2H		GENMASK(7, 6)
#define RG_SSUSB_PLL_BC_PE2H_VAL(x)	((0x3 & (x)) << 6)

#define DA_SSUSB_PLL_IR			(SSUSB_SIFSLV_PHYA_DA_BASE + 0x10)
#define RG_SSUSB_PLL_IR_PE2H		GENMASK(19, 16)
#define RG_SSUSB_PLL_IR_PE2H_VAL(x)	((0xf & (x)) << 16)

#define DA_SSUSB_PLL_BP			(SSUSB_SIFSLV_PHYA_DA_BASE + 0x14)
#define RG_SSUSB_PLL_BP_PE2H		GENMASK(19, 16)
#define RG_SSUSB_PLL_BP_PE2H_VAL(x)	((0xf & (x)) << 16)

#define DA_SSUSB_PLL_SSC_DELTA1_REG20	(SSUSB_SIFSLV_PHYA_DA_BASE + 0x3c)
#define RG_SSUSB_PLL_SSC_DELTA1_PE2H		GENMASK(31, 16)
#define RG_SSUSB_PLL_SSC_DELTA1_PE2H_VAL(x)	((0xffff & (x)) << 16)

#define DA_SSUSB_PLL_SSC_DELTA_REG25	(SSUSB_SIFSLV_PHYA_DA_BASE + 0x48)
#define RG_SSUSB_PLL_SSC_DELTA_PE2H		GENMASK(15, 0)
#define RG_SSUSB_PLL_SSC_DELTA_PE2H_VAL(x)	(0xffff & (x))

#define HIF_SYSCFG1			0x14
#define HIF_SYSCFG1_PHY2_MASK		(0x3 << 20)

struct mtk_pcie_phy {
	struct device *dev;
	void __iomem *base;
	struct regmap *hif;
	struct clk *phya_ref;
	struct phy *phy;
};

static inline u32 phy_read(struct mtk_pcie_phy *phy, u32 reg)
{
	return readl(phy->base + reg);
}

static inline void phy_write(struct mtk_pcie_phy *phy, u32 val, u32 reg)
{
	writel(val, phy->base + reg);
}

static int mtk_pcie_phy_power_on(struct phy *phy)
{
	struct mtk_pcie_phy *mtk_phy = phy_get_drvdata(phy);
	int err;
	u32 val;

	/* enable PCIe mode if needed */
	if (mtk_phy->hif)
		regmap_update_bits(mtk_phy->hif, HIF_SYSCFG1,
				   HIF_SYSCFG1_PHY2_MASK, 0);

	err = clk_prepare_enable(mtk_phy->phya_ref);
	if (err) {
		dev_err(mtk_phy->dev, "failed to enable PCIe phy clock\n");
		return err;
	}

	val = phy_read(mtk_phy, DA_SSUSB_CDR_REFCK_SEL);
	val &= ~(RG_SSUSB_XTAL_EXT_PE1H | RG_SSUSB_XTAL_EXT_PE2H);
	val |= RG_SSUSB_XTAL_EXT_PE1H_VAL(0x2) |
	       RG_SSUSB_XTAL_EXT_PE2H_VAL(0x2);
	phy_write(mtk_phy, val, DA_SSUSB_CDR_REFCK_SEL);

	/* ref clk drive */
	val = phy_read(mtk_phy, U3_PHYA_REG1);
	val &= ~RG_PCIE_CLKDRV_AMP;
	val |= RG_PCIE_CLKDRV_AMP_VAL(0x4);
	phy_write(mtk_phy, val, U3_PHYA_REG1);

	val = phy_read(mtk_phy, U3_PHYA_REG0);
	val &= ~RG_PCIE_CLKDRV_OFFSET;
	val |= RG_PCIE_CLKDRV_OFFSET_VAL(0x1);
	phy_write(mtk_phy, val, U3_PHYA_REG0);

	/* SSC delta -5000ppm */
	val = phy_read(mtk_phy, DA_SSUSB_PLL_SSC_DELTA1_REG20);
	val &= ~RG_SSUSB_PLL_SSC_DELTA1_PE2H;
	val |= RG_SSUSB_PLL_SSC_DELTA1_PE2H_VAL(0x3c);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_SSC_DELTA1_REG20);

	val = phy_read(mtk_phy, DA_SSUSB_PLL_SSC_DELTA_REG25);
	val &= ~RG_SSUSB_PLL_SSC_DELTA_PE2H;
	val |= RG_SSUSB_PLL_SSC_DELTA_PE2H_VAL(0x36);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_SSC_DELTA_REG25);

	/* change pll BW 0.6M */
	val = phy_read(mtk_phy, DA_SSUSB_PLL_IC);
	val &= ~RG_SSUSB_PLL_BR_PE2H;
	val |= RG_SSUSB_PLL_BR_PE2H_VAL(0x1);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_IC);

	val = phy_read(mtk_phy, DA_SSUSB_PLL_BC);
	val &= ~(RG_SSUSB_PLL_DIVEN_PE2H | RG_SSUSB_PLL_BC_PE2H);
	val |= RG_SSUSB_PLL_BC_PE2H_VAL(0x3);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_BC);

	val = phy_read(mtk_phy, DA_SSUSB_PLL_IR);
	val &= ~RG_SSUSB_PLL_IR_PE2H;
	val |= RG_SSUSB_PLL_IR_PE2H_VAL(0x2);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_IR);

	val = phy_read(mtk_phy, DA_SSUSB_PLL_IC);
	val &= ~RG_SSUSB_PLL_IC_PE2H;
	val |= RG_SSUSB_PLL_IC_PE2H_VAL(0x1);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_IC);

	val = phy_read(mtk_phy, DA_SSUSB_PLL_BP);
	val &= ~RG_SSUSB_PLL_BP_PE2H;
	val |= RG_SSUSB_PLL_BP_PE2H_VAL(0xa);
	phy_write(mtk_phy, val, DA_SSUSB_PLL_BP);

	/* Tx Detect Rx Timing: 10us -> 5us */
	val = phy_read(mtk_phy, B2_PHYD_RXDET1);
	val &= ~RG_SSUSB_RXDET_STB2;
	val |= RG_SSUSB_RXDET_STB2_VAL(0x10);
	phy_write(mtk_phy, val, B2_PHYD_RXDET1);

	val = phy_read(mtk_phy, B2_PHYD_RXDET2);
	val &= ~RG_SSUSB_RXDET_STB2_P3;
	val |= RG_SSUSB_RXDET_STB2_P3_VAL(0x10);
	phy_write(mtk_phy, val, B2_PHYD_RXDET2);

	/* wait for PCIe subsys register to active */
	usleep_range(2500, 3000);

	return 0;
}

static int mtk_pcie_phy_power_off(struct phy *phy)
{
	struct mtk_pcie_phy *mtk_phy = phy_get_drvdata(phy);

	clk_disable_unprepare(mtk_phy->phya_ref);

	return 0;
}

static struct phy_ops mtk_pcie_phy_ops = {
	.power_on	= mtk_pcie_phy_power_on,
	.power_off	= mtk_pcie_phy_power_off,
	.owner		= THIS_MODULE,
};

static const struct of_device_id mtk_pcie_phy_of_match[];

static int mtk_pcie_phy_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	const struct of_device_id *match;
	struct phy_provider *phy_provider;
	struct mtk_pcie_phy *mtk_phy;
	struct resource *res;
	struct phy *phy;

	match = of_match_device(mtk_pcie_phy_of_match, &pdev->dev);
	if (!match)
		return -ENODEV;

	mtk_phy = devm_kzalloc(&pdev->dev, sizeof(*mtk_phy), GFP_KERNEL);
	if (!mtk_phy)
		return -ENOMEM;

	mtk_phy->dev = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mtk_phy->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(mtk_phy->base)) {
		dev_err(&pdev->dev, "failed to get phy base\n");
		return PTR_ERR(mtk_phy->base);
	}

	mtk_phy->phya_ref = devm_clk_get(&pdev->dev, "pciephya_ref");
	if (IS_ERR(mtk_phy->phya_ref)) {
		dev_err(&pdev->dev, "error to get pciephya_ref\n");
		return PTR_ERR(mtk_phy->phya_ref);
	}

	if (of_find_property(np, "mediatek,phy-switch", NULL)) {
		mtk_phy->hif = syscon_regmap_lookup_by_phandle(
						np, "mediatek,phy-switch");
		if (IS_ERR(mtk_phy->hif)) {
			dev_err(&pdev->dev, "missing \"mediatek,phy-switch\" phandle\n");
			return PTR_ERR(mtk_phy->hif);
		}
	}

	platform_set_drvdata(pdev, mtk_phy);
	phy = devm_phy_create(&pdev->dev, NULL, &mtk_pcie_phy_ops);
	if (IS_ERR(phy)) {
		dev_err(&pdev->dev, "failed to create phy device\n");
		return PTR_ERR(phy);
	}

	mtk_phy->phy = phy;
	phy_set_drvdata(phy, mtk_phy);

	phy_provider = devm_of_phy_provider_register(&pdev->dev,
						     of_phy_simple_xlate);
	if (IS_ERR(phy_provider)) {
		dev_err(&pdev->dev, "failed to register phy provider\n");
		return PTR_ERR(phy_provider);
	}

	return 0;
}

static const struct of_device_id mtk_pcie_phy_of_match[] = {
	{ .compatible = "mediatek,mt7623-pcie-phy"},
	{ .compatible = "mediatek,mt2701-pcie-phy"},
	{},
};
MODULE_DEVICE_TABLE(of, mtk_pcie_phy_of_match);

static struct platform_driver mtk_pcie_phy_driver = {
	.probe	= mtk_pcie_phy_probe,
	.driver = {
		.name	= "mtk-pcie-phy",
		.of_match_table	= mtk_pcie_phy_of_match,
	}
};
module_platform_driver(mtk_pcie_phy_driver);

MODULE_AUTHOR("Ryder Lee <ryder.lee@mediatek.com>");
MODULE_DESCRIPTION("Mediatek PCIe PHY driver");
MODULE_LICENSE("GPL v2");
