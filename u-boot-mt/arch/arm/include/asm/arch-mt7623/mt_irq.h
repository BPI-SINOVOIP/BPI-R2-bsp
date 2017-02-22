/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef __MT_IRQ_H__
#define __MT_IRQ_H__


#define GIC_DIST_PENDING_SET            0x200
#define GIC_DIST_ENABLE_SET             0x100
#define GIC_DIST_ENABLE_CLEAR           0x180
#define GIC_DIST_PENDING_CLEAR          0x280
#define GIC_DIST_CTR                    0x004
#define GIC_DIST_CTRL                   0x000
#define GIC_DIST_TARGET                 0x800
#define GIC_DIST_ACTIVE_BIT             0x300
#define GIC_DIST_PRI                    0x400
#define GIC_DIST_ICDISR                 0x80
#define GIC_DIST_CONFIG                 0xc00
#define GIC_DIST_SOFTINT                0xf00

#define GIC_CPU_HIGHPRI                 0x18
#define GIC_CPU_EOI                     0x10
#define GIC_CPU_RUNNINGPRI              0x14
#define GIC_CPU_BINPOINT                0x08
#define GIC_CPU_INTACK                  0x0c
#define GIC_CPU_CTRL                    0x00
#define GIC_CPU_PRIMASK                 0x04
enum {IRQ_MASK_HEADER = 0xF1F1F1F1, IRQ_MASK_FOOTER = 0xF2F2F2F2};

struct mtk_irq_mask
{
    unsigned int header;   /* for error checking */
    unsigned int mask0;
    unsigned int mask1;
    unsigned int mask2;
    unsigned int mask3;
    unsigned int mask4;
    unsigned int mask5;
    unsigned int mask6;
    unsigned int mask7;
    unsigned int footer;   /* for error checking */
};


/*
 * Define hadware registers.
 */

/*
 * Define IRQ code.
 */

#define GIC_PRIVATE_SIGNALS (32)

#define GIC_PPI_OFFSET          (27)
#define GIC_PPI_GLOBAL_TIMER    (GIC_PPI_OFFSET + 0)
#define GIC_PPI_LEGACY_FIQ      (GIC_PPI_OFFSET + 1)
#define GIC_PPI_PRIVATE_TIMER   (GIC_PPI_OFFSET + 2)
#define GIC_PPI_WATCHDOG_TIMER  (GIC_PPI_OFFSET + 3)
#define GIC_PPI_LEGACY_IRQ      (GIC_PPI_OFFSET + 4)

#define MT_USB0_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 32)
#define MT_MSDC0_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 39)
#define MT_MSDC1_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 40)
//#define MT_MSDC2_IRQ_ID               (GIC_PRIVATE_SIGNALS + 41) //6582 take off
//#define MT_MSDC3_IRQ_ID               (GIC_PRIVATE_SIGNALS + 42) //6582 take off
#define MT_AP_HIF_IRQ_ID                (GIC_PRIVATE_SIGNALS + 43)
#define MT_I2C0_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 44)
#define MT_I2C1_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 45)
#define MT_I2C2_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 46)
#define MT_I2C3_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 47)

#define MT_UART1_IRQ_ID                     (GIC_PRIVATE_SIGNALS + 51)
#define MT_UART2_IRQ_ID                     (GIC_PRIVATE_SIGNALS + 52)
#define MT_UART3_IRQ_ID                     (GIC_PRIVATE_SIGNALS + 53)
#define MT_UART4_IRQ_ID                     (GIC_PRIVATE_SIGNALS + 54)
#define MT_NFIECC_IRQ_ID                (GIC_PRIVATE_SIGNALS + 55)
#define MT_NFI_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 56)
#define MT_GDMA1_IRQ_ID                     (GIC_PRIVATE_SIGNALS + 57)
#define MT_GDMA2_IRQ_ID                     (GIC_PRIVATE_SIGNALS + 58)
#define MT_DMA_UART0_TX_IRQ_ID              (GIC_PRIVATE_SIGNALS + 63)
#define MT_DMA_UART0_RX_IRQ_ID              (GIC_PRIVATE_SIGNALS + 64)
#define MT_DMA_UART1_TX_IRQ_ID              (GIC_PRIVATE_SIGNALS + 65)
#define MT_DMA_UART1_RX_IRQ_ID              (GIC_PRIVATE_SIGNALS + 66)
#define MT_DMA_UART2_TX_IRQ_ID              (GIC_PRIVATE_SIGNALS + 67)
#define MT_DMA_UART2_RX_IRQ_ID              (GIC_PRIVATE_SIGNALS + 68)


#define MT_SPI1_IRQ_ID                      (GIC_PRIVATE_SIGNALS + 78)
//#define MT_MSDC4_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 83) //6582 take off
#define MT_IRRX_IRQ_ID                       (GIC_PRIVATE_SIGNALS + 87)

#define MT_WDT_IRQ_ID                       (GIC_PRIVATE_SIGNALS + 88)//TBD:For build pass
#define MT_APARM_DOMAIN_IRQ_ID              (GIC_PRIVATE_SIGNALS + 94)
#define MT_APARM_DECERR_IRQ_ID              (GIC_PRIVATE_SIGNALS + 95)
#if 1 //cliff
#define MT_GPT_IRQ_ID                       (GIC_PRIVATE_SIGNALS + 112)//10.2 update
#define MT_EINT_IRQ_ID                      (GIC_PRIVATE_SIGNALS + 113)//10.2 update
#else
#define MT_GPT_IRQ_ID                       (GIC_PRIVATE_SIGNALS + 113)//10.2 update
#define MT_EINT_IRQ_ID                      (GIC_PRIVATE_SIGNALS + 116)//10.2 update
#endif
#define MT_PMIC_WRAP_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 115)//0x80
#define MT_KP_IRQ_ID                        (GIC_PRIVATE_SIGNALS + 116)
#define MT_SPM_IRQ_ID                       (GIC_PRIVATE_SIGNALS + 117)
#define MT_VENC_IRQ_ID                      (GIC_PRIVATE_SIGNALS + 139)
#define MT_VDEC_IRQ_ID                      (GIC_PRIVATE_SIGNALS + 140)
#define CAMERA_ISP_IRQ0_ID                  (GIC_PRIVATE_SIGNALS + 143) // cam_irq_b
#define CAMERA_ISP_IRQ1_ID                  (GIC_PRIVATE_SIGNALS + 144) // cam_irq1_b
#define CAMERA_ISP_IRQ2_ID                  (GIC_PRIVATE_SIGNALS + 145) // cam_irq2_b
//#define CAMERA_ISP_IRQ3_ID                (GIC_PRIVATE_SIGNALS + 144) // cam_irq3_b 6582 take off
#define MT_JPEG_ENC_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 141)


#define MT_NR_PPI   (5)
#define MT_NR_SPI   (224)
#define NR_IRQ_LINE  (GIC_PPI_OFFSET + MT_NR_PPI + MT_NR_SPI)    // 5 PPIs and 224 SPIs

#define MT65xx_EDGE_SENSITIVE 0
#define MT65xx_LEVEL_SENSITIVE 1

#define MT65xx_POLARITY_LOW   0
#define MT65xx_POLARITY_HIGH  1


int mt_irq_mask_all(struct mtk_irq_mask *mask); //(This is ONLY used for the sleep driver)
int mt_irq_mask_restore(struct mtk_irq_mask *mask); //(This is ONLY used for the sleep driver)
void mt_irq_set_sens(unsigned int irq, unsigned int sens);
void mt_irq_set_polarity(unsigned int irq, unsigned int polarity);
int mt_irq_mask_restore(struct mtk_irq_mask *mask);
void mt_irq_unmask(unsigned int irq);
void mt_irq_ack(unsigned int irq);

void platform_init_interrupts(void);

#endif  /* !__MT_IRQ_H__ */
