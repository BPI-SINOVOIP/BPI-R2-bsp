#include <common.h>
#include <command.h>

#if defined (CONFIG_CMD_NET) && defined(CONFIG_RT2880_ETH)

#include <malloc.h>
#include <net.h>
#include <rt_mmap.h>
//#include <asm/addrspace.h>

#undef DEBUG
#define BIT(x)              ((1 << x))

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)   (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))

/* ====================================== */
//GDMA1 uni-cast frames destination port
#define GDM_UFRC_P_CPU     ((u32)(~(0x7 << 12)))
#define GDM_UFRC_P_GDMA1   (1 << 12)
#define GDM_UFRC_P_GDMA2   (2 << 12)
#define GDM_UFRC_P_DROP    (7 << 12)
//GDMA1 broad-cast MAC address frames
#define GDM_BFRC_P_CPU     ((u32)(~(0x7 << 8)))
#define GDM_BFRC_P_GDMA1   (1 << 8)
#define GDM_BFRC_P_GDMA2   (2 << 8)
#define GDM_BFRC_P_PPE     (6 << 8)
#define GDM_BFRC_P_DROP    (7 << 8)
//GDMA1 multi-cast MAC address frames
#define GDM_MFRC_P_CPU     ((u32)(~(0x7 << 4)))
#define GDM_MFRC_P_GDMA1   (1 << 4)
#define GDM_MFRC_P_GDMA2   (2 << 4)
#define GDM_MFRC_P_PPE     (6 << 4)
#define GDM_MFRC_P_DROP    (7 << 4)
//GDMA1 other MAC address frames destination port
#define GDM_OFRC_P_CPU     ((u32)(~(0x7)))
#define GDM_OFRC_P_GDMA1   1
#define GDM_OFRC_P_GDMA2   2
#define GDM_OFRC_P_PPE     6
#define GDM_OFRC_P_DROP    7

#define RST_DRX_IDX0      BIT(16)
#define RST_DTX_IDX0      BIT(0)

#define TX_WB_DDONE       BIT(6)
#define RX_DMA_BUSY       BIT(3)
#define TX_DMA_BUSY       BIT(1)
#define RX_DMA_EN         BIT(2)
#define TX_DMA_EN         BIT(0)

#define GP1_FRC_EN        BIT(15)
#define GP1_FC_TX         BIT(11)
#define GP1_FC_RX         BIT(10)
#define GP1_LNK_DWN       BIT(9)
#define GP1_AN_OK         BIT(8)

/*
 * FE_INT_STATUS
 */
#define CNT_PPE_AF       BIT(31)
#define CNT_GDM1_AF      BIT(29)
#define PSE_P1_FC        BIT(22)
#define PSE_P0_FC        BIT(21)
#define PSE_FQ_EMPTY     BIT(20)
#define GE1_STA_CHG      BIT(18)
#define TX_COHERENT      BIT(17)
#define RX_COHERENT      BIT(16)

#define TX_DONE_INT1     BIT(9)
#define TX_DONE_INT0     BIT(8)
#define RX_DONE_INT0     BIT(2)
#define TX_DLY_INT       BIT(1)
#define RX_DLY_INT       BIT(0)

/*
 * Ethernet chip registers.RT2880
 */
#if defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)

#define PDMA_RELATED		0x0800
/* 1. PDMA */
#define TX_BASE_PTR0            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x000)
#define TX_MAX_CNT0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x004)
#define TX_CTX_IDX0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x008)
#define TX_DTX_IDX0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x00C)

#define TX_BASE_PTR1            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x010)
#define TX_MAX_CNT1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x014)
#define TX_CTX_IDX1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x018)
#define TX_DTX_IDX1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x01C)

#define TX_BASE_PTR2            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x020)
#define TX_MAX_CNT2             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x024)
#define TX_CTX_IDX2             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x028)
#define TX_DTX_IDX2             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x02C)

#define TX_BASE_PTR3            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x030)
#define TX_MAX_CNT3             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x034)
#define TX_CTX_IDX3             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x038)
#define TX_DTX_IDX3             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x03C)

#define RX_BASE_PTR0            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x100)
#define RX_MAX_CNT0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x104)
#define RX_CALC_IDX0            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x108)
#define RX_DRX_IDX0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x10C)

#define RX_BASE_PTR1            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x110)
#define RX_MAX_CNT1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x114)
#define RX_CALC_IDX1            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x118)
#define RX_DRX_IDX1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x11C)

#define PDMA_INFO               (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x200)
#define PDMA_GLO_CFG            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x204)
#define PDMA_RST_IDX            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x208)
#define PDMA_RST_CFG            (RALINK_FRAME_ENGINE_BASE + PDMA_RST_IDX)
#define DLY_INT_CFG             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x20C)
#define FREEQ_THRES             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x210)
#define INT_STATUS              (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x220)
#define FE_INT_STATUS           (INT_STATUS)
#define INT_MASK                (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x228)
#define FE_INT_ENABLE           (INT_MASK)
#define PDMA_WRR                (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x280)
#define PDMA_SCH_CFG            (PDMA_WRR)

#define SDM_RELATED		0x0C00
#define SDM_CON                 (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x00)  //Switch DMA configuration
#define SDM_RRING               (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x04)  //Switch DMA Rx Ring
#define SDM_TRING               (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x08)  //Switch DMA Tx Ring
#define SDM_MAC_ADRL            (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x0C)  //Switch MAC address LSB
#define SDM_MAC_ADRH            (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x10)  //Switch MAC Address MSB
#define SDM_TPCNT               (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x100) //Switch DMA Tx packet count
#define SDM_TBCNT               (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x104) //Switch DMA Tx byte count
#define SDM_RPCNT               (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x108) //Switch DMA rx packet count
#define SDM_RBCNT               (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x10C) //Switch DMA rx byte count
#define SDM_CS_ERR              (RALINK_FRAME_ENGINE_BASE + SDM_RELATED+0x110) //Switch DMA rx checksum error count

#elif defined (RT6855_ASIC_BOARD) || defined (RT6855_FPGA_BOARD) || \
      defined (RT6855A_FPGA_BOARD) || defined (RT6855A_ASIC_BOARD) || \
      defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD) || \
      defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD) || \
      defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) || \
      defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)
/* Old FE with New PDMA */
#define PDMA_RELATED		0x0800
/* 1. PDMA */
#define TX_BASE_PTR0            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x000)
#define TX_MAX_CNT0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x004)
#define TX_CTX_IDX0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x008)
#define TX_DTX_IDX0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x00C)

#define TX_BASE_PTR1            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x010)
#define TX_MAX_CNT1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x014)
#define TX_CTX_IDX1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x018)
#define TX_DTX_IDX1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x01C)

#define TX_BASE_PTR2            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x020)
#define TX_MAX_CNT2             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x024)
#define TX_CTX_IDX2             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x028)
#define TX_DTX_IDX2             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x02C)

#define TX_BASE_PTR3            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x030)
#define TX_MAX_CNT3             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x034)
#define TX_CTX_IDX3             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x038)
#define TX_DTX_IDX3             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x03C)

#define RX_BASE_PTR0            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x100)
#define RX_MAX_CNT0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x104)
#define RX_CALC_IDX0            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x108)
#define RX_DRX_IDX0             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x10C)

#define RX_BASE_PTR1            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x110)
#define RX_MAX_CNT1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x114)
#define RX_CALC_IDX1            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x118)
#define RX_DRX_IDX1             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x11C)

#define PDMA_INFO               (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x200)
#define PDMA_GLO_CFG            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x204)
#define PDMA_RST_IDX            (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x208)
#define PDMA_RST_CFG            (RALINK_FRAME_ENGINE_BASE + PDMA_RST_IDX)
#define DLY_INT_CFG             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x20C)
#define FREEQ_THRES             (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x210)
#define INT_STATUS              (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x220) /* FIXME */
#define INT_MASK                (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x228) /* FIXME */
#define PDMA_WRR                (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED+0x280)
#define PDMA_SCH_CFG            (PDMA_WRR)

/* TODO: change FE_INT_STATUS->INT_STATUS 
 * FE_INT_ENABLE->INT_MASK */
#define MDIO_ACCESS         RALINK_FRAME_ENGINE_BASE + 0x00
#define MDIO_CFG            RALINK_FRAME_ENGINE_BASE + 0x04
#define FE_DMA_GLO_CFG      RALINK_FRAME_ENGINE_BASE + 0x08
#define FE_RST_GLO          RALINK_FRAME_ENGINE_BASE + 0x0C
#define FE_INT_STATUS       RALINK_FRAME_ENGINE_BASE + 0x10
#define FE_INT_ENABLE       RALINK_FRAME_ENGINE_BASE + 0x14
#define FC_DROP_STA         RALINK_FRAME_ENGINE_BASE + 0x18
#define FOE_TS_T            RALINK_FRAME_ENGINE_BASE + 0x1C

#if defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD)
#define GDMA1_RELATED       0x0600
#define GDMA1_FWD_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x00)
#define GDMA1_SHRP_CFG      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x04)
#define GDMA1_MAC_ADRL      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x08)
#define GDMA1_MAC_ADRH      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x0C)
#elif defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD) ||\
      defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) ||\
      defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)

#define PAD_RGMII2_MDIO_CFG            RALINK_SYSCTL_BASE + 0x58

#define GDMA1_RELATED       0x0500
#define GDMA1_FWD_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x00)
#define GDMA1_SHRP_CFG      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x04)
#define GDMA1_MAC_ADRL      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x08)
#define GDMA1_MAC_ADRH      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x0C)
#define GDMA2_RELATED       0x1500
#define GDMA2_FWD_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x00)
#define GDMA2_SHRP_CFG      (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x04)
#define GDMA2_MAC_ADRL      (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x08)
#define GDMA2_MAC_ADRH      (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x0C)

#else
#define GDMA1_RELATED       0x0020
#define GDMA1_FWD_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x00)
#define GDMA1_SCH_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x04)
#define GDMA1_SHRP_CFG      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x08)
#define GDMA1_MAC_ADRL      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x0C)
#define GDMA1_MAC_ADRH      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x10)
#endif

#define PSE_RELATED         0x0040
#define PSE_FQFC_CFG        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x00)
#define CDMA_FC_CFG         (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x04)
#define GDMA1_FC_CFG        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x08)
#define GDMA2_FC_CFG        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x0C)
#define CDMA_OQ_STA         (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x10)
#define GDMA1_OQ_STA        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x14)
#define GDMA2_OQ_STA        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x18)
#define PSE_IQ_STA          (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x1C)

#define CDMA_RELATED        0x0080
#define CDMA_CSG_CFG        (RALINK_FRAME_ENGINE_BASE + CDMA_RELATED + 0x00)
#define CDMA_SCH_CFG        (RALINK_FRAME_ENGINE_BASE + CDMA_RELATED + 0x04)

#else

#define MDIO_ACCESS         RALINK_FRAME_ENGINE_BASE + 0x00
#ifdef RT3883_USE_GE2
#define MDIO_CFG            RALINK_FRAME_ENGINE_BASE + 0x18
#else
#define MDIO_CFG            RALINK_FRAME_ENGINE_BASE + 0x04
#endif // RT3883_USE_GE2 //
#define FE_DMA_GLO_CFG      RALINK_FRAME_ENGINE_BASE + 0x08
#define FE_RST_GLO          RALINK_FRAME_ENGINE_BASE + 0x0C
#define FE_INT_STATUS       RALINK_FRAME_ENGINE_BASE + 0x10
#define FE_INT_ENABLE       RALINK_FRAME_ENGINE_BASE + 0x14
#define FC_DROP_STA         RALINK_FRAME_ENGINE_BASE + 0x18
#define FOE_TS_T            RALINK_FRAME_ENGINE_BASE + 0x1C

#define GDMA1_RELATED       0x0020
#define GDMA1_FWD_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x00)
#define GDMA1_SCH_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x04)
#define GDMA1_SHRP_CFG      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x08)
#define GDMA1_MAC_ADRL      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x0C)
#define GDMA1_MAC_ADRH      (RALINK_FRAME_ENGINE_BASE + GDMA1_RELATED + 0x10)

#define GDMA2_RELATED       0x0060
#define GDMA2_FWD_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x00)
#define GDMA2_SCH_CFG       (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x04)
#define GDMA2_SHRP_CFG      (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x08)
#define GDMA2_MAC_ADRL      (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x0C)
#define GDMA2_MAC_ADRH      (RALINK_FRAME_ENGINE_BASE + GDMA2_RELATED + 0x10)

#define PSE_RELATED         0x0040
#define PSE_FQFC_CFG        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x00)
#define CDMA_FC_CFG         (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x04)
#define GDMA1_FC_CFG        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x08)
#define GDMA2_FC_CFG        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x0C)
#define CDMA_OQ_STA         (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x10)
#define GDMA1_OQ_STA        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x14)
#define GDMA2_OQ_STA        (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x18)
#define PSE_IQ_STA          (RALINK_FRAME_ENGINE_BASE + PSE_RELATED + 0x1C)

#define CDMA_RELATED        0x0080
#define CDMA_CSG_CFG        (RALINK_FRAME_ENGINE_BASE + CDMA_RELATED + 0x00)
#define CDMA_SCH_CFG        (RALINK_FRAME_ENGINE_BASE + CDMA_RELATED + 0x04)

#define PDMA_RELATED        0x0100
#define PDMA_GLO_CFG        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x00)
#define PDMA_RST_IDX        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x04)
#define PDMA_SCH_CFG        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x08)
#define DELAY_INT_CFG       (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x0C)
#define TX_BASE_PTR0        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x10)
#define TX_MAX_CNT0         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x14)
#define TX_CTX_IDX0         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x18)
#define TX_DTX_IDX0         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x1C)
#define TX_BASE_PTR1        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x20)
#define TX_MAX_CNT1         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x24)
#define TX_CTX_IDX1         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x28)
#define TX_DTX_IDX1         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x2C)
#define RX_BASE_PTR0        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x30)
#define RX_MAX_CNT0         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x34)
#define RX_CALC_IDX0        (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x38)
#define RX_DRX_IDX0         (RALINK_FRAME_ENGINE_BASE + PDMA_RELATED + 0x3C)


#endif

#define INTERNAL_LOOPBACK_ENABLE 1
#define INTERNAL_LOOPBACK_DISABLE 0

//#define CONFIG_UNH_TEST

#define TOUT_LOOP   1000
#define ENABLE 1
#define DISABLE 0

VALID_BUFFER_STRUCT  rt2880_free_buf_list;
VALID_BUFFER_STRUCT  rt2880_busing_buf_list;
static BUFFER_ELEM   rt2880_free_buf[PKTBUFSRX];

/*=========================================
      PDMA RX Descriptor Format define
=========================================*/

//-------------------------------------------------
typedef struct _PDMA_RXD_INFO1_  PDMA_RXD_INFO1_T;

struct _PDMA_RXD_INFO1_
{
    unsigned int    PDP0;
};
//-------------------------------------------------
typedef struct _PDMA_RXD_INFO2_    PDMA_RXD_INFO2_T;

struct _PDMA_RXD_INFO2_
{
	unsigned int    PLEN1                   : 14;
	unsigned int    LS1                     : 1;
	unsigned int    UN_USED                 : 1;
	unsigned int    PLEN0                   : 14;
	unsigned int    LS0                     : 1;
	unsigned int    DDONE_bit               : 1;
};
//-------------------------------------------------
typedef struct _PDMA_RXD_INFO3_  PDMA_RXD_INFO3_T;

struct _PDMA_RXD_INFO3_
{
	unsigned int    PDP1;
};
//-------------------------------------------------
typedef struct _PDMA_RXD_INFO4_    PDMA_RXD_INFO4_T;

struct _PDMA_RXD_INFO4_
{
#if defined (PDMA_NEW)
	unsigned int    FOE_Entry           	: 14;
	unsigned int    CRSN                	: 5;
	unsigned int    SP               	: 3;
	unsigned int    L4F                 	: 1;
	unsigned int    L4VLD               	: 1;
	unsigned int    TACK                	: 1;
	unsigned int    IP4F                	: 1;
	unsigned int    IP4                 	: 1;
	unsigned int    IP6                 	: 1;
	unsigned int    UN_USE1             	: 4;
#else
	unsigned int    FOE_Entry               : 14;
	unsigned int    FVLD                    : 1;
	unsigned int    UN_USE1                 : 1;
	unsigned int    AI                      : 8;
	unsigned int    SP                      : 3;
	unsigned int    AIS                     : 1;
	unsigned int    L4F                     : 1;
	unsigned int    IPF                     : 1;
	unsigned int    L4FVLD_bit              : 1;
	unsigned int    IPFVLD_bit              : 1;
#endif
};

struct PDMA_rxdesc {
	PDMA_RXD_INFO1_T rxd_info1;
	PDMA_RXD_INFO2_T rxd_info2;
	PDMA_RXD_INFO3_T rxd_info3;
	PDMA_RXD_INFO4_T rxd_info4;
};
/*=========================================
      PDMA TX Descriptor Format define
=========================================*/
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO1_  PDMA_TXD_INFO1_T;

struct _PDMA_TXD_INFO1_
{
	unsigned int    SDP0;
};
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO2_    PDMA_TXD_INFO2_T;

struct _PDMA_TXD_INFO2_
{
	unsigned int    SDL1                  : 14;
	unsigned int    LS1_bit               : 1;
	unsigned int    BURST_bit             : 1;
	unsigned int    SDL0                  : 14;
	unsigned int    LS0_bit               : 1;
	unsigned int    DDONE_bit             : 1;
};
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO3_  PDMA_TXD_INFO3_T;

struct _PDMA_TXD_INFO3_
{
	unsigned int    SDP1;
};
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO4_    PDMA_TXD_INFO4_T;

struct _PDMA_TXD_INFO4_
{
#if defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD)
    unsigned int    VPRI_VIDX           : 8;
    unsigned int    SIDX                : 4;
    unsigned int    INSP                : 1;
    unsigned int    RESV                : 2;
    unsigned int    UDF                 : 5;
    unsigned int    FP_BMAP             : 8;
    unsigned int    TSO                 : 1;
    unsigned int    TUI_CO              : 3;
#elif defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD) ||\
      defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) ||\
      defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)
    unsigned int    VLAN_TAG            :16;
    unsigned int    INS                 : 1;
    unsigned int    RESV                : 2;
    unsigned int    UDF                 : 6;
    unsigned int    FPORT               : 3;
    unsigned int    TSO                 : 1;
    unsigned int    TUI_CO              : 3;
#else                            
    unsigned int    VIDX		: 4;
    unsigned int    VPRI                : 3;
    unsigned int    INSV                : 1;
    unsigned int    SIDX                : 4;
    unsigned int    INSP                : 1;
    unsigned int    UN_USE3             : 3;
    unsigned int    QN                  : 3;
    unsigned int    UN_USE2             : 5;
    unsigned int    PN                  : 3;
    unsigned int    UN_USE1             : 2;
    unsigned int    TUI_CO              : 3;
#endif
};

struct PDMA_txdesc {
	PDMA_TXD_INFO1_T txd_info1;
	PDMA_TXD_INFO2_T txd_info2;
	PDMA_TXD_INFO3_T txd_info3;
	PDMA_TXD_INFO4_T txd_info4;
};


static  struct PDMA_txdesc tx_ring0_cache[NUM_TX_DESC] __attribute__ ((aligned(32))); /* TX descriptor ring         */
static  struct PDMA_rxdesc rx_ring_cache[NUM_RX_DESC] __attribute__ ((aligned(32))); /* RX descriptor ring         */

static int rx_dma_owner_idx0;                             /* Point to the next RXD DMA wants to use in RXD Ring#0.  */
static int rx_wants_alloc_idx0;                           /* Point to the next RXD CPU wants to allocate to RXD Ring #0. */
static int tx_cpu_owner_idx0;                             /* Point to the next TXD in TXD_Ring0 CPU wants to use */
static volatile struct PDMA_rxdesc *rx_ring;
static volatile struct PDMA_txdesc *tx_ring0;

static char rxRingSize;
static char txRingSize;

static int   rt2880_eth_init(struct eth_device* dev, bd_t* bis);
static int   rt2880_eth_send(struct eth_device* dev, volatile void *packet, int length);
static int   rt2880_eth_recv(struct eth_device* dev);
void  rt2880_eth_halt(struct eth_device* dev);

int   mii_mgr_read(u32 phy_addr, u32 phy_register, u32 *read_data);
int   mii_mgr_write(u32 phy_addr, u32 phy_register, u32 write_data);

static int   rt2880_eth_setup(struct eth_device* dev);
static int   rt2880_eth_initd;


#if defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) ||\
    defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)
#define phys_to_bus(a) (a)
#else
#define phys_to_bus(a) (a & 0x1FFFFFFF)
#endif

#define PCI_WAIT_INPUT_CHAR(ch) while((ch = getc())== 0)

struct eth_device* 	rt2880_pdev;

volatile uchar	*PKT_HEADER_Buf;// = (uchar *)CFG_EMBEDED_SRAM_SDP0_BUF_START;
static volatile uchar	PKT_HEADER_Buf_Pool[(PKTBUFSRX * PKTSIZE_ALIGN) + PKTALIGN];
//extern volatile uchar	*NetTxPacket;	/* THE transmit packet			*/
extern uchar	*NetTxPacket;	/* THE transmit packet			*/
volatile uchar	RxPktBuf[PKTBUFSRX][1536];

#define PIODIR_R  (RALINK_PIO_BASE + 0X24)
#define PIODATA_R (RALINK_PIO_BASE + 0X20)
#define PIODIR3924_R  (RALINK_PIO_BASE + 0x4c)
#define PIODATA3924_R (RALINK_PIO_BASE + 0x48)


#define FREEBUF_OFFSET(CURR)  ((int)(((0x0FFFFFFF & (u32)CURR) - (u32) (0x0FFFFFFF & (u32) rt2880_free_buf[0].pbuf)) / 1536))

void START_ETH(struct eth_device *dev ) {
	s32 omr;
	omr=RALINK_REG(PDMA_GLO_CFG);
	udelay(100);
	omr |= TX_WB_DDONE | RX_DMA_EN | TX_DMA_EN ;
		
	RALINK_REG(PDMA_GLO_CFG)=omr;

	udelay(500);
}


void STOP_ETH(struct eth_device *dev)
{
	s32 omr;
	omr=RALINK_REG(PDMA_GLO_CFG);
	udelay(100);
	omr &= ~(TX_WB_DDONE | RX_DMA_EN | TX_DMA_EN) ;
	RALINK_REG(PDMA_GLO_CFG)=omr;
	udelay(500);
}


BUFFER_ELEM *rt2880_free_buf_entry_dequeue(VALID_BUFFER_STRUCT *hdr)
{
	int     zero = 0;           /* causes most compilers to place this */
	/* value in a register only once */
	BUFFER_ELEM  *node;

	/* Make sure we were not passed a null pointer. */
	if (!hdr) {
		return (NULL);
	}

	/* If there is a node in the list we want to remove it. */
	if (hdr->head) {
		/* Get the node to be removed */
		node = hdr->head;

		/* Make the hdr point the second node in the list */
		hdr->head = node->next;

		/* If this is the last node the headers tail pointer needs to be nulled
		   We do not need to clear the node's next since it is already null */
		if (!(hdr->head)) {
			hdr->tail = (BUFFER_ELEM *)zero;
		}

		node->next = (BUFFER_ELEM *)zero;




	}
	else {
		node = NULL;
		return (node);
	}

	/*  Restore the previous interrupt lockout level.  */

	/* Return a pointer to the removed node */

	//shnat_validation_flow_table_entry[node->index].state = SHNAT_FLOW_TABLE_NODE_USED;
	return (node);
}

static BUFFER_ELEM *rt2880_free_buf_entry_enqueue(VALID_BUFFER_STRUCT *hdr, BUFFER_ELEM *item)
{
	int zero =0;

	if (!hdr) {
		return (NULL);
	}

	if (item != NULL)
	{
		/* Temporarily lockout interrupts to protect global buffer variables. */
		// Sys_Interrupt_Disable_Save_Flags(&cpsr_flags);

		/* Set node's next to point at NULL */
		item->next = (BUFFER_ELEM *)zero;

		/*  If there is currently a node in the linked list, we want to add the
		    new node to the end. */
		if (hdr->head) {
			/* Make the last node's next point to the new node. */
			hdr->tail->next = item;

			/* Make the roots tail point to the new node */
			hdr->tail = item;
		}
		else {
			/* If the linked list was empty, we want both the root's head and
			   tial to point to the new node. */
			hdr->head = item;
			hdr->tail = item;
		}

		/*  Restore the previous interrupt lockout level.  */

	}
	else
	{
		printf("\n shnat_flow_table_free_entry_enqueue is called,item== NULL \n");
	}

	return(item);

} /* MEM_Buffer_Enqueue */


int rt2880_eth_initialize(bd_t *bis)
{
	struct	eth_device* 	dev;
	int	i;
	u32	regValue;

	if (!(dev = (struct eth_device *) malloc (sizeof *dev))) {
		printf("Failed to allocate memory\n");
		return 0;
	}

	memset(dev, 0, sizeof(*dev));

	sprintf(dev->name, "Eth0");

	dev->iobase = RALINK_FRAME_ENGINE_BASE;
	dev->init   = rt2880_eth_init;
	dev->halt   = rt2880_eth_halt;
	dev->send   = rt2880_eth_send;
	dev->recv   = rt2880_eth_recv;

	eth_register(dev);
	rt2880_pdev = dev;

	rt2880_eth_initd =0;
	PKT_HEADER_Buf = PKT_HEADER_Buf_Pool;
	NetTxPacket = NULL;
//	rx_ring = (struct PDMA_rxdesc *)KSEG1ADDR((ulong)&rx_ring_cache[0]);
//	tx_ring0 = (struct PDMA_txdesc *)KSEG1ADDR((ulong)&tx_ring0_cache[0]);
    rx_ring = (struct PDMA_rxdesc *)((ulong)&rx_ring_cache[0]);
    tx_ring0 = (struct PDMA_txdesc *)((ulong)&tx_ring0_cache[0]);

	rt2880_free_buf_list.head = NULL;
	rt2880_free_buf_list.tail = NULL;

	rt2880_busing_buf_list.head = NULL;
	rt2880_busing_buf_list.tail = NULL;

	//2880_free_buf

	/*
	 *	Setup packet buffers, aligned correctly.
	 */
	rt2880_free_buf[0].pbuf = (unsigned char *)&RxPktBuf[0];
	rt2880_free_buf[0].pbuf += PKTALIGN - 1;
	rt2880_free_buf[0].pbuf -= (ulong)rt2880_free_buf[0].pbuf % PKTALIGN;
	rt2880_free_buf[0].next = NULL;

	rt2880_free_buf_entry_enqueue(&rt2880_free_buf_list,&rt2880_free_buf[0]);

#ifdef DEBUG
	printf("\n rt2880_free_buf[0].pbuf = 0x%08X \n",rt2880_free_buf[0].pbuf);
#endif
	for (i = 1; i < PKTBUFSRX; i++) {
		rt2880_free_buf[i].pbuf = rt2880_free_buf[0].pbuf + (i)*PKTSIZE_ALIGN;
		rt2880_free_buf[i].next = NULL;
#ifdef DEBUG
		printf("\n rt2880_free_buf[%d].pbuf = 0x%08X\n",i,rt2880_free_buf[i].pbuf);
#endif
		rt2880_free_buf_entry_enqueue(&rt2880_free_buf_list,&rt2880_free_buf[i]);
	}

	for (i = 0; i < PKTBUFSRX; i++)
	{
		rt2880_free_buf[i].tx_idx = NUM_TX_DESC;
#ifdef DEBUG
		printf("\n rt2880_free_buf[%d] = 0x%08X,rt2880_free_buf[%d].next=0x%08X \n",i,&rt2880_free_buf[i],i,rt2880_free_buf[i].next);
#endif
	}
	
	return 1;
}

void mt7623_ethifsys_init(void)
{
#define TRGPLL_CON0                    (0x10209280)
#define TRGPLL_CON1                    (0x10209284)
#define TRGPLL_CON2                    (0x10209288)
#define TRGPLL_PWR_CON0                (0x1020928C)
#define ETHPLL_CON0                    (0x10209290)
#define ETHPLL_CON1                    (0x10209294)
#define ETHPLL_CON2                    (0x10209298)
#define ETHPLL_PWR_CON0                (0x1020929C)
#define ETH_PWR_CON                    (0x100062A0)
#define HIF_PWR_CON                    (0x100062A4)

	printf("Enter mt7623_ethifsys_init()\n");

	//=========================================================================
	// Enable ETHPLL & TRGPLL
	//=========================================================================
	// xPLL PWR ON
	u32 temp, pwr_ack_status;
	temp = RALINK_REG(ETHPLL_PWR_CON0);
	RALINK_REG(ETHPLL_PWR_CON0) = temp | 0x1;

	temp = RALINK_REG(TRGPLL_PWR_CON0);
	RALINK_REG(TRGPLL_PWR_CON0) = temp | 0x1;

	udelay(5); // wait for xPLL_PWR_ON ready (min delay is 1us)

	// xPLL ISO Disable
	temp = RALINK_REG(ETHPLL_PWR_CON0);
	RALINK_REG(ETHPLL_PWR_CON0) = temp & ~0x2;

	temp = RALINK_REG(TRGPLL_PWR_CON0);
	RALINK_REG(TRGPLL_PWR_CON0) = temp & ~0x2;

	// xPLL Frequency Set

	temp = RALINK_REG(ETHPLL_CON0);
	RALINK_REG(ETHPLL_CON0) = temp | 0x1;

#if defined (CONFIG_GE1_TRGMII_FORCE_2900)
	temp = RALINK_REG(TRGPLL_CON0);
	RALINK_REG(TRGPLL_CON0) = temp | 0x1;
#elif defined (CONFIG_GE1_TRGMII_FORCE_2600)	
	RALINK_REG(TRGPLL_CON1) = 0xB2000000;
	temp = RALINK_REG(TRGPLL_CON0);
	RALINK_REG(TRGPLL_CON0) = temp | 0x1;
#elif defined (CONFIG_GE1_TRGMII_FORCE_2000)
	RALINK_REG(TRGPLL_CON1) = 0xCCEC4EC5;
	RALINK_REG(TRGPLL_CON0) = 0x121;
#else
	temp = RALINK_REG(TRGPLL_CON0);
	RALINK_REG(TRGPLL_CON0) = temp | 0x1;
#endif

	udelay(40); // wait for PLL stable (min delay is 20us)

	//=========================================================================
	// Power on ETHDMASYS and HIFSYS
	//=========================================================================
	// Power on ETHDMASYS

	RALINK_REG(0x10006000) = 0x0b160001;
	
	pwr_ack_status = (RALINK_REG(ETH_PWR_CON) & 0x0000f000) >> 12;
	if(pwr_ack_status == 0x0) {
	        printf("ETH already turn on and power on flow will be skipped\n");
	} else {
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp | 0x4;          // PWR_ON
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp | 0x8;          // PWR_ON_S

	        udelay(5); // wait power settle time (min delay is 1us)

	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp & ~0x10;      // PWR_CLK_DIS
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp & ~0x2;        // PWR_ISO
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp & ~0x100;   // SRAM_PDN 0
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp & ~0x200;   // SRAM_PDN 1
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp & ~0x400;   // SRAM_PDN 2
	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp & ~0x800;   // SRAM_PDN 3

	        udelay(5); // wait SRAM settle time (min delay is 1Us)

	        temp = RALINK_REG(ETH_PWR_CON);
	        RALINK_REG(ETH_PWR_CON) = temp | 0x1;          // PWR_RST_B
	}

	// Power on HIFSYS
	pwr_ack_status = (RALINK_REG(HIF_PWR_CON) & 0x0000f000) >> 12;
	if(pwr_ack_status == 0x0) {
	        printf("HIF already turn on and power on flow will be skipped\n");

	} else {
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp | 0x4;	       // PWR_ON
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp | 0x8;	       // PWR_ON_S

		udelay(5); // wait power settle time (min delay is 1us)

		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp & ~0x10;      // PWR_CLK_DIS
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp & ~0x2;	      // PWR_ISO
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp & ~0x100;   // SRAM_PDN 0
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp & ~0x200;   // SRAM_PDN 1
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp & ~0x400;   // SRAM_PDN 2
		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp & ~0x800;   // SRAM_PDN 3

		udelay(5); // wait SRAM settle time (min delay is 1Us)

		temp = RALINK_REG(HIF_PWR_CON);
		RALINK_REG(HIF_PWR_CON) = temp | 0x1;	       // PWR_RST_B
	}

	/* Release mt7530 reset */
	temp = le32_to_cpu(*(volatile u_long *)(0x1b000034));
	temp &= ~(BIT(2));
	*(volatile u_long *)(0x1b000034) = temp;
}

void mt7623_pinmux_set(void)
{
	u32 regValue;
	
	printf("[mt7623_pinmux_set]start\n");
	/* Pin277: ESW_RST (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ad0));
	regValue &= ~(BITS(6,8));
	regValue |= BIT(6);
	*(volatile u_long *)(0x10005ad0) = regValue;

	/* Pin262: G2_TXEN (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005aa0));
	regValue &= ~(BITS(6,8));
	regValue |= BIT(6);
	*(volatile u_long *)(0x10005aa0) = regValue;
	/* Pin263: G2_TXD3 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005aa0));
	regValue &= ~(BITS(9,11));
	regValue |= BIT(9);
	*(volatile u_long *)(0x10005aa0) = regValue;
	/* Pin264: G2_TXD2 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005aa0));
	regValue &= ~(BITS(12,14));
	regValue |= BIT(12);
	*(volatile u_long *)(0x10005aa0) = regValue;
	/* Pin265: G2_TXD1 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ab0));
	regValue &= ~(BITS(0,2));
	regValue |= BIT(0);
	*(volatile u_long *)(0x10005ab0) = regValue;
	/* Pin266: G2_TXD0 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ab0));
	regValue &= ~(BITS(3,5));
	regValue |= BIT(3);
	*(volatile u_long *)(0x10005ab0) = regValue;
	/* Pin267: G2_TXC (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ab0));
	regValue &= ~(BITS(6,8));
	regValue |= BIT(6);
	*(volatile u_long *)(0x10005ab0) = regValue;
	/* Pin268: G2_RXC (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ab0));
	regValue &= ~(BITS(9,11));
	regValue |= BIT(9);
	*(volatile u_long *)(0x10005ab0) = regValue;
	/* Pin269: G2_RXD0 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ab0));
	regValue &= ~(BITS(12,14));
	regValue |= BIT(12);
	*(volatile u_long *)(0x10005ab0) = regValue;
	/* Pin270: G2_RXD1 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ac0));
	regValue &= ~(BITS(0,2));
	regValue |= BIT(0);
	*(volatile u_long *)(0x10005ac0) = regValue;
	/* Pin271: G2_RXD2 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ac0));
	regValue &= ~(BITS(3,5));
	regValue |= BIT(3);
	*(volatile u_long *)(0x10005ac0) = regValue;
	/* Pin272: G2_RXD3 (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ac0));
	regValue &= ~(BITS(6,8));
	regValue |= BIT(6);
	*(volatile u_long *)(0x10005ac0) = regValue;
	/* Pin274: G2_RXDV (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ac0));
	regValue &= ~(BITS(12,14));
	regValue |= BIT(12);
	*(volatile u_long *)(0x10005ac0) = regValue;

	/* Pin275: MDC (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ad0));
	regValue &= ~(BITS(0,2));
	regValue |= BIT(0);
	*(volatile u_long *)(0x10005ad0) = regValue;
	/* Pin276: MDIO (1) */
	regValue = le32_to_cpu(*(volatile u_long *)(0x10005ad0));
	regValue &= ~(BITS(3,5));
	regValue |= BIT(3);
	*(volatile u_long *)(0x10005ad0) = regValue;
	printf("[mt7623_pinmux_set]end\n");
}

void wait_loop(void) {
	int i,j;
	int read_data;
	j =0;
	while (j< 10) {
		for(i = 0; i<32; i = i+1){
			read_data = *(volatile u_long *)(0x1B110610);
		}
		j++;
	}
}

void trgmii_calibration_7623(void) {

	unsigned int  tap_a[5]; // minumum delay for all correct
	unsigned int  tap_b[5]; // maximum delay for all correct
	unsigned int  final_tap[5];
	unsigned int  bslip_en;
	unsigned int  rxc_step_size;
	unsigned int  rxd_step_size;
	unsigned int  read_data;
	unsigned int  tmp;
	unsigned int  rd_wd;
	int  i;
	unsigned int err_cnt[5];
	unsigned int init_toggle_data;
	unsigned int err_flag[5];
	unsigned int err_total_flag;
	unsigned int training_word;
	unsigned int rd_tap;

	u32  TRGMII_7623_base;
	u32  TRGMII_7623_RD_0;
	u32  TRGMII_RD_1;
	u32  TRGMII_RD_2;
	u32  TRGMII_RD_3;
	u32  TRGMII_RXCTL;
	u32  TRGMII_RCK_CTRL;
	u32 TRGMII_7530_base;
	TRGMII_7623_base = 0x1B110300;
	TRGMII_7623_RD_0 = TRGMII_7623_base + 0x10;
	TRGMII_RCK_CTRL = TRGMII_7623_base;
	rxd_step_size =0x1;
	rxc_step_size =0x4;
	init_toggle_data = 0x00000055;
	training_word    = 0x000000AC;

	//printk("Calibration begin ........");
	*(volatile u_long *)(TRGMII_7623_base +0x04) &= 0x3fffffff;   // RX clock gating in MT7623
	*(volatile u_long *)(TRGMII_7623_base +0x00) |= 0x80000000;   // Assert RX  reset in MT7623
	*(volatile u_long *)(TRGMII_7623_base +0x78) |= 0x00002000;   // Set TX OE edge in  MT7623
	*(volatile u_long *)(TRGMII_7623_base +0x04) |= 0xC0000000;   // Disable RX clock gating in MT7623
	*(volatile u_long *)(TRGMII_7623_base )      &= 0x7fffffff;   // Release RX reset in MT7623
	//printk("Check Point 1 .....\n");
	for (i = 0 ; i<5 ; i++) {
		*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) |= 0x80000000;   // Set bslip_en = 1
	}

	//printk("Enable Training Mode in MT7530\n");
	mii_mgr_read(0x1F,0x7A40,&read_data);
	read_data |= 0xc0000000;
	mii_mgr_write(0x1F,0x7A40,read_data);  //Enable Training Mode in MT7530
	err_total_flag = 0;
	//printk("Adjust RXC delay in MT7623\n");
	read_data =0x0;
	while (err_total_flag == 0 && read_data != 0x68) {
		//printk("2nd Enable EDGE CHK in MT7623\n");
		/* Enable EDGE CHK in MT7623*/
		for (i = 0 ; i<5 ; i++) {
			tmp = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			tmp |= 0x40000000;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = tmp & 0x4fffffff;
		}
		wait_loop();
		err_total_flag = 1;
		for  (i = 0 ; i<5 ; i++) {
			err_cnt[i] = ((*(volatile u_long *)(TRGMII_7623_RD_0 + i*8)) >> 8)  & 0x0000000f;
			rd_wd = ((*(volatile u_long *)(TRGMII_7623_RD_0 + i*8)) >> 16)  & 0x000000ff;
			//printk("ERR_CNT = %d, RD_WD =%x\n",err_cnt[i],rd_wd);
			if ( err_cnt[i] !=0 ) {
				err_flag[i] = 1;
			}
			else if (rd_wd != 0x55) {
				err_flag[i] = 1;
			}	
			else {
				err_flag[i] = 0;
			}
			err_total_flag = err_flag[i] &  err_total_flag;
		}

		//printk("2nd Disable EDGE CHK in MT7623\n");
		/* Disable EDGE CHK in MT7623*/
		for (i = 0 ; i<5 ; i++) {
			tmp = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			tmp |= 0x40000000;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = tmp & 0x4fffffff;
		}
		wait_loop();
		//printk("2nd Disable EDGE CHK in MT7623\n");
		/* Adjust RXC delay */
		*(volatile u_long *)(TRGMII_7623_base +0x00) |= 0x80000000;   // Assert RX  reset in MT7623
		*(volatile u_long *)(TRGMII_7623_base +0x04) &= 0x3fffffff;   // RX clock gating in MT7623
		read_data = *(volatile u_long *)(TRGMII_7623_base);
		if (err_total_flag == 0) {
		  tmp = (read_data & 0x0000007f) + rxc_step_size;
		  //printk(" RXC delay = %d\n", tmp);
		  read_data >>= 8;
		  read_data &= 0xffffff80;
		  read_data |= tmp;
		  read_data <<=8;
		  read_data &= 0xffffff80;
		  read_data |=tmp;
		  *(volatile u_long *)(TRGMII_7623_base)  =   read_data;
		}
		  read_data &=0x000000ff;
		  *(volatile u_long *)(TRGMII_7623_base )      &= 0x7fffffff;   // Release RX reset in MT7623
		  *(volatile u_long *)(TRGMII_7623_base +0x04) |= 0xC0000000;   // Disable RX clock gating in MT7623
		  for (i = 0 ; i<5 ; i++) {
		  	*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) =  (*(volatile u_long *)(TRGMII_7623_RD_0 + i*8)) | 0x80000000;  // Set bslip_en = ~bit_slip_en
		  }
	}
	//printk("Finish RXC Adjustment while loop\n");
	//printk("Read RD_WD MT7623\n");
	/* Read RD_WD MT7623*/
	for  (i = 0 ; i<5 ; i++) {
		rd_tap=0;
		while (err_flag[i] != 0) {
			/* Enable EDGE CHK in MT7623*/
			tmp = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			tmp |= 0x40000000;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = tmp & 0x4fffffff;
			wait_loop();
			read_data = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			err_cnt[i] = (read_data >> 8)  & 0x0000000f;     // Read MT7623 Errcnt
			rd_wd = (read_data >> 16)  & 0x000000ff;
			if (err_cnt[i] != 0 || rd_wd !=0x55){
		           err_flag [i] =  1;
			}   
			else {
			   err_flag[i] =0;
		        }	
			/* Disable EDGE CHK in MT7623*/
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) &= 0x4fffffff;
			tmp |= 0x40000000;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = tmp & 0x4fffffff;
			wait_loop();
			//err_cnt[i] = ((read_data) >> 8)  & 0x0000000f;     // Read MT7623 Errcnt
			if (err_flag[i] !=0) {
			    rd_tap    = (read_data & 0x0000007f) + rxd_step_size;                     // Add RXD delay in MT7623
			    read_data = (read_data & 0xffffff80) | rd_tap;
			    *(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = read_data;
			    tap_a[i] = rd_tap;
			} else {
                            rd_tap    = (read_data & 0x0000007f) + 4;
			    read_data = (read_data & 0xffffff80) | rd_tap;
			    *(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = read_data;
			}	
			//err_cnt[i] = (*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) >> 8)  & 0x0000000f;     // Read MT7623 Errcnt

		}
		//printk("%dth bit  Tap_a = %d\n", i, tap_a[i]);
	}
	//printk("Last While Loop\n");
	for  (i = 0 ; i<5 ; i++) {
		//printk(" Bit%d\n", i);
		rd_tap =0;
		while ((err_cnt[i] == 0) && (rd_tap !=128)) {
			read_data = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			rd_tap    = (read_data & 0x0000007f) + rxd_step_size;                     // Add RXD delay in MT7623
			read_data = (read_data & 0xffffff80) | rd_tap;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = read_data;
			/* Enable EDGE CHK in MT7623*/
			tmp = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			tmp |= 0x40000000;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = tmp & 0x4fffffff;
			wait_loop();
			err_cnt[i] = ((*(volatile u_long *)(TRGMII_7623_RD_0 + i*8)) >> 8)  & 0x0000000f;     // Read MT7623 Errcnt
			/* Disable EDGE CHK in MT7623*/
			tmp = *(volatile u_long *)(TRGMII_7623_RD_0 + i*8);
			tmp |= 0x40000000;
			*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = tmp & 0x4fffffff;
			wait_loop();
			//err_cnt[i] = ((*(volatile u_long *)(TRGMII_7623_RD_0 + i*8)) >> 8)  & 0x0000000f;     // Read MT7623 Errcnt

		}
		tap_b[i] =   rd_tap;// -rxd_step_size;                                        // Record the max delay TAP_B
		//printk("tap_b[%d] is %d \n", i,tap_b[i]);
		final_tap[i] = (tap_a[i]+tap_b[i])/2;                                              //  Calculate RXD delay = (TAP_A + TAP_B)/2
		//printk("%dth bit Final Tap = %d\n", i, final_tap[i]);
		read_data = (read_data & 0xffffff80) | final_tap[i];
		*(volatile u_long *)(TRGMII_7623_RD_0 + i*8) = read_data;
	}
//	/*word alignment*/
//	mii_mgr_read(0x1F,0x7A50,&read_data);
//	read_data &= ~(0xff);
//	read_data |= 0xac;
//	mii_mgr_write(0x1F,0x7A50,read_data);
//	while (i <10) {
//		wait_loop();
//		read_data = *(volatile u_long *)(TRGMII_7623_RD_0+i*8);
//		printk(" MT7623 training word = %x\n", read_data);
//	}


	mii_mgr_read(0x1F,0x7A40,&read_data);
	//printk(" MT7530 0x7A40 = %x\n", read_data);
	read_data &=0x3fffffff;
	mii_mgr_write(0x1F,0x7A40,read_data);
}


void trgmii_calibration_7530(void){ 

	unsigned int  tap_a[5];
	unsigned int  tap_b[5];
	unsigned int  final_tap[5];
	unsigned int  bslip_en;
	unsigned int  rxc_step_size;
	unsigned int  rxd_step_size;
	unsigned int  read_data;
	unsigned int  tmp;
	int  i,j;
	unsigned int err_cnt[5];
	unsigned int rd_wd;
	unsigned int init_toggle_data;
	unsigned int err_flag[5];
	unsigned int err_total_flag;
	unsigned int training_word;
	unsigned int rd_tap;
	unsigned int is_mt7623_e1 = 0;
#define DEVINFO_BASE                    0x17000000

	u32  TRGMII_7623_base;
	u32  TRGMII_7530_RD_0;
	u32  TRGMII_RD_1;
	u32  TRGMII_RD_2;
	u32  TRGMII_RD_3;
	u32  TRGMII_RXCTL;
	u32  TRGMII_RCK_CTRL;
	u32 TRGMII_7530_base;
	u32 TRGMII_7530_TX_base;
	TRGMII_7623_base = 0x1B110300;
	TRGMII_7530_base = 0x7A00;
	TRGMII_7530_RD_0 = TRGMII_7530_base + 0x10;
	TRGMII_RCK_CTRL = TRGMII_7623_base;
	rxd_step_size = 0x1;
	rxc_step_size = 0x8;
	init_toggle_data = 0x00000055;
	training_word = 0x000000AC;

	TRGMII_7530_TX_base = TRGMII_7530_base + 0x50;

	tmp = *(volatile u_long *)(DEVINFO_BASE+0x8);
	if(tmp == 0x0000CA00)
	{
		is_mt7623_e1 = 1;
		printf("===MT7623 E1 only===\n");
	}

	//printk("Calibration begin ........\n");
	*(volatile u_long *)(TRGMII_7623_base + 0x40) |= 0x80000000;
	mii_mgr_read(0x1F, 0x7a10, &read_data);
	//printk("TRGMII_7530_RD_0 is %x\n", read_data);

	mii_mgr_read(0x1F,TRGMII_7530_base+0x04,&read_data);
	read_data &= 0x3fffffff;
	mii_mgr_write(0x1F,TRGMII_7530_base+0x04,read_data);     // RX clock gating in MT7530

	mii_mgr_read(0x1F,TRGMII_7530_base+0x78,&read_data);
	read_data |= 0x00002000;
	mii_mgr_write(0x1F,TRGMII_7530_base+0x78,read_data);     // Set TX OE edge in  MT7530

	mii_mgr_read(0x1F,TRGMII_7530_base,&read_data);
	read_data |= 0x80000000;
	mii_mgr_write(0x1F,TRGMII_7530_base,read_data);          // Assert RX  reset in MT7530


	mii_mgr_read(0x1F,TRGMII_7530_base,&read_data);
	read_data &= 0x7fffffff;
	mii_mgr_write(0x1F,TRGMII_7530_base,read_data);          // Release RX reset in MT7530

	mii_mgr_read(0x1F,TRGMII_7530_base+0x04,&read_data);
	read_data |= 0xC0000000;
	mii_mgr_write(0x1F,TRGMII_7530_base+0x04,read_data);     // Disable RX clock gating in MT7530

	//printk("Enable Training Mode in MT7623\n");
	/*Enable Training Mode in MT7623*/
	*(volatile u_long *)(TRGMII_7623_base + 0x40) &= 0xbfffffff;
	if(is_mt7623_e1)
	*(volatile u_long *)(TRGMII_7623_base + 0x40) |= 0x80000000;
	else
#if defined (CONFIG_GE1_TRGMII_FORCE_2000)
		*(volatile u_long *)(TRGMII_7623_base + 0x40) |= 0xc0000000;
#else
        	*(volatile u_long *)(TRGMII_7623_base + 0x40) |= 0x80000000;
#endif
	*(volatile u_long *)(TRGMII_7623_base + 0x78) &= 0xfffff0ff;
	if(is_mt7623_e1)
	*(volatile u_long *)(TRGMII_7623_base + 0x78) |= 0x00000400;
	else{	
		*(volatile u_long *)(TRGMII_7623_base + 0x50) &= 0xfffff0ff;        
		*(volatile u_long *)(TRGMII_7623_base + 0x58) &= 0xfffff0ff;
		*(volatile u_long *)(TRGMII_7623_base + 0x60) &= 0xfffff0ff;
		*(volatile u_long *)(TRGMII_7623_base + 0x68) &= 0xfffff0ff;
		*(volatile u_long *)(TRGMII_7623_base + 0x70) &= 0xfffff0ff;
		*(volatile u_long *)(TRGMII_7623_base + 0x78) |= 0x00000800; 
	} 
        //==========================================

	err_total_flag =0;
	//printk("Adjust RXC delay in MT7530\n");
	read_data =0x0;
	while (err_total_flag == 0 && (read_data != 0x68)) {
		//printk("2nd Enable EDGE CHK in MT7530\n");
		/* Enable EDGE CHK in MT7530*/
		for (i = 0 ; i<5 ; i++) {
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
		        wait_loop();
		        //printk("2nd Disable EDGE CHK in MT7530\n");
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&err_cnt[i]);
		        //printk("***** MT7530 %dth bit ERR_CNT =%x\n",i, err_cnt[i]);
		        //printk("MT7530 %dth bit ERR_CNT =%x\n",i, err_cnt[i]);
			err_cnt[i] >>= 8;
			err_cnt[i] &= 0x0000ff0f;
			rd_wd  = err_cnt[i] >> 8;
		        rd_wd &= 0x000000ff;	
			err_cnt[i] &= 0x0000000f;
			//mii_mgr_read(0x1F,0x7a10,&read_data);
			if ( err_cnt[i] !=0 ) {
				err_flag[i] = 1;
			}
			else if (rd_wd != 0x55) {
                                err_flag[i] = 1;
			} else {	
				err_flag[i] = 0;
			}
			if (i==0) {
			   err_total_flag = err_flag[i];
			} else {
			   err_total_flag = err_flag[i] & err_total_flag;
			}    	
		/* Disable EDGE CHK in MT7530*/
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
		          wait_loop();
		}
		/*Adjust RXC delay*/
		if (err_total_flag ==0) {
	           mii_mgr_read(0x1F,TRGMII_7530_base,&read_data);
	           read_data |= 0x80000000;
	           mii_mgr_write(0x1F,TRGMII_7530_base,read_data);          // Assert RX  reset in MT7530

		   mii_mgr_read(0x1F,TRGMII_7530_base+0x04,&read_data);
		   read_data &= 0x3fffffff;
		   mii_mgr_write(0x1F,TRGMII_7530_base+0x04,read_data);       // RX clock gating in MT7530

		   mii_mgr_read(0x1F,TRGMII_7530_base,&read_data);
		   tmp = read_data;
		   tmp &= 0x0000007f;
		   tmp += rxc_step_size;
		   //printk("Current rxc delay = %d\n", tmp);
		   read_data &= 0xffffff80;
		   read_data |= tmp;
		   mii_mgr_write (0x1F,TRGMII_7530_base,read_data);
		   mii_mgr_read(0x1F,TRGMII_7530_base,&read_data);
		   //printk("Current RXC delay = %x\n", read_data); 

	           mii_mgr_read(0x1F,TRGMII_7530_base,&read_data);
	           read_data &= 0x7fffffff;
	           mii_mgr_write(0x1F,TRGMII_7530_base,read_data);          // Release RX reset in MT7530

		   mii_mgr_read(0x1F,TRGMII_7530_base+0x04,&read_data);
		   read_data |= 0xc0000000;
		   mii_mgr_write(0x1F,TRGMII_7530_base+0x04,read_data);       // Disable RX clock gating in MT7530
                }
		read_data = tmp;
	}
	//printk("RXC delay is %d\n", tmp);
	//printk("Finish RXC Adjustment while loop\n");

	//printk("Read RD_WD MT7530\n");
	/* Read RD_WD MT7530*/
	for  (i = 0 ; i<5 ; i++) {
		rd_tap = 0;
		while (err_flag[i] != 0 && rd_tap != 128) {
			/* Enable EDGE CHK in MT7530*/
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
		        wait_loop();
			err_cnt[i] = (read_data >> 8) & 0x0000000f; 
		        rd_wd = (read_data >> 16) & 0x000000ff;
		        //printk("##### %dth bit  ERR_CNT = %x RD_WD =%x ######\n", i, err_cnt[i],rd_wd);
			if (err_cnt[i] != 0 || rd_wd !=0x55){
		           err_flag [i] =  1;
			}   
			else {
			   err_flag[i] =0;
		        }	
			if (err_flag[i] !=0 ) { 
			   rd_tap = (read_data & 0x0000007f) + rxd_step_size;                        // Add RXD delay in MT7530
			   read_data = (read_data & 0xffffff80) | rd_tap;
			   mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
			   tap_a[i] = rd_tap;
			} else {
			   tap_a[i] = (read_data & 0x0000007f);			                    // Record the min delay TAP_A
	                   rd_tap   =  tap_a[i] + 0x4;  		   
			   read_data = (read_data & 0xffffff80) | rd_tap  ;
			   mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
			}	

			/* Disable EDGE CHK in MT7530*/
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
		        wait_loop();

		}
		//printk("%dth bit  Tap_a = %d\n", i, tap_a[i]);
	}
	//printk("Last While Loop\n");
	for  (i = 0 ; i<5 ; i++) {
	rd_tap =0;
		while (err_cnt[i] == 0 && (rd_tap!=128)) {
			/* Enable EDGE CHK in MT7530*/
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
			wait_loop();
		
			err_cnt[i] = (read_data >> 8) & 0x0000000f;
			rd_wd = (read_data >> 16) & 0x000000ff;
			if (err_cnt[i] != 0 || rd_wd !=0x55){
				err_flag [i] =  1;
			}
			else {
				err_flag[i] =0;
			}

			//rd_tap = (read_data & 0x0000007f) + 0x4;                                    // Add RXD delay in MT7530
			if (err_cnt[i] == 0 && (rd_tap!=128)) {
			    rd_tap = (read_data & 0x0000007f) + rxd_step_size;                        // Add RXD delay in MT7530
			    read_data = (read_data & 0xffffff80) | rd_tap;
			    mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
			}    
			/* Disable EDGE CHK in MT7530*/
			mii_mgr_read(0x1F,TRGMII_7530_RD_0+i*8,&read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
			wait_loop();
		}
		tap_b[i] = rd_tap;// - rxd_step_size;                                     // Record the max delay TAP_B
		//printk("%dth bit  Tap_b = %d, ERR_CNT=%d\n", i, tap_b[i],err_cnt[i]);
		final_tap[i] = (tap_a[i]+tap_b[i])/2;                                     //  Calculate RXD delay = (TAP_A + TAP_B)/2
		//printk("%dth bit Final Tap = %d\n", i, final_tap[i]);

		read_data = ( read_data & 0xffffff80) | final_tap[i];
		mii_mgr_write(0x1F,TRGMII_7530_RD_0+i*8,read_data);
	}
	if(is_mt7623_e1)
	        *(volatile u_long *)(TRGMII_7623_base + 0x40) &=0x3fffffff;
	else
#if defined (CONFIG_GE1_TRGMII_FORCE_2000)
	        *(volatile u_long *)(TRGMII_7623_base + 0x40) &=0x7fffffff;
#else
                *(volatile u_long *)(TRGMII_7623_base + 0x40) &=0x3fffffff;
#endif
	
}

static void IsSwitchVlanTableBusy(void)
{
	int j = 0;
	unsigned int value = 0;

	for (j = 0; j < 20; j++) {
	    mii_mgr_read(31, 0x90, &value);
	    if ((value & 0x80000000) == 0 ){ //table busy
		break;
	    }
	    udelay(70000);
	}
	if (j == 20)
	    printf("set vlan timeout value=0x%x.\n", value);
}

static void LANWANPartition(void)
{
#if defined (MAC_TO_MT7530_MODE)
	/* Set  MT7530 */
	printf("set LAN/WAN LLLLW\n");
	/* LLLLW, wan at P4 */
	/* LAN/WAN ports as security mode */
	mii_mgr_write(31, 0x2004, 0xff0003);	//port0
	mii_mgr_write(31, 0x2104, 0xff0003);	//port1
	mii_mgr_write(31, 0x2204, 0xff0003);	//port2
	mii_mgr_write(31, 0x2304, 0xff0003);	//port3
	mii_mgr_write(31, 0x2404, 0xff0003);	//port4

	/* set PVID */
	mii_mgr_write(31, 0x2014, 0x10001);		//port0
	mii_mgr_write(31, 0x2114, 0x10001);		//port1
	mii_mgr_write(31, 0x2214, 0x10001);		//port2
	mii_mgr_write(31, 0x2314, 0x10001);		//port3
	mii_mgr_write(31, 0x2414, 0x10002);		//port4

	/* VLAN member */
	IsSwitchVlanTableBusy();
	mii_mgr_write(31, 0x94, 0x404f0001);	//VAWD1
	mii_mgr_write(31, 0x90, 0x80001001);	//VTCR, VID=1
	IsSwitchVlanTableBusy();
	mii_mgr_write(31, 0x94, 0x40500001);	//VAWD1
	mii_mgr_write(31, 0x90, 0x80001002);	//VTCR, VID=2
	IsSwitchVlanTableBusy();
#endif
}

static int rt2880_eth_init(struct eth_device* dev, bd_t* bis)
{
	if(rt2880_eth_initd == 0)
	{
#if defined(MT7623_ASIC_BOARD)
		mt7623_ethifsys_init();
		mt7623_pinmux_set();
#endif
		rt2880_eth_setup(dev);
		/* init switch */	
#if defined (CONFIG_USE_GE1)
#if defined (CONFIG_GE1_TRGMII_FORCE_2000) || defined (CONFIG_GE1_TRGMII_FORCE_2600)
		*(volatile u_long *)(0x1b00002c) |=  (1<<11);
#else
		*(volatile u_long *)(0x1b00002c) &= ~(1<<11);
#endif	/* CONFIG_GE1_TRGMII_FORCE_2000 */
#endif	/* CONFIG_USE_GE1 */

		setup_internal_gsw();

#if defined(MT7623_ASIC_BOARD)
#if defined (CONFIG_USE_GE1)
		trgmii_calibration_7623();
		trgmii_calibration_7530();
		//*(volatile u_long *)(0xfb110300) |= (0x1f << 24);     //Just only for 312.5/325MHz
		*(volatile u_long *)(0x1b110340) = 0x00020000;
		*(volatile u_long *)(0x1b110304) &= 0x3fffffff;         // RX clock gating in MT7623
		*(volatile u_long *)(0x1b110300) |= 0x80000000;         // Assert RX  reset in MT7623
		*(volatile u_long *)(0x1b110300 )      &= 0x7fffffff;   // Release RX reset in MT7623
		*(volatile u_long *)(0x1b110300 +0x04) |= 0xC0000000;   // Disable RX clock gating in MT7623
#if defined (GE_RGMII_FORCE_1000)
	/*GE1@125MHz(RGMII mode) TX delay adjustment*/
	        *(volatile u_long *)(0x1b110350) = 0x55;
	        *(volatile u_long *)(0x1b110358) = 0x55;
	        *(volatile u_long *)(0x1b110360) = 0x55;
	        *(volatile u_long *)(0x1b110368) = 0x55;
	        *(volatile u_long *)(0x1b110370) = 0x55;
	        *(volatile u_long *)(0x1b110378) = 0x855;
#endif	/* CONFIG_GE1_RGMII_FORCE_1000 */
#endif	/* CONFIG_GE1_RGMII_FORCE_1000 */
#endif	/* defined(MT7623_ASIC_BOARD) */

		LANWANPartition();
	}
	else
	{
		START_ETH(dev);
	}

	rt2880_eth_initd = 1;
	return (1);
}

#if defined (P5_RGMII_TO_MAC_MODE) || defined (MAC_TO_VITESSE_MODE) || defined (MAC_TO_MT7530_MODE)
static void ResetSWusingGPIOx(void)
{
#ifdef GPIOx_RESET_MODE
	u32 value;

#if defined (RT2880_FPGA_BOARD) || defined (RT2880_ASIC_BOARD)

	printf("\n GPIO pin 10 reset to switch\n");

	//set spi/gpio share pin to gpio mode
	value = le32_to_cpu(*(volatile u_long *)RT2880_GPIOMODE_REG);
	value |= (1 << 1);
	*(volatile u_long *)(RT2880_GPIOMODE_REG) = cpu_to_le32(value);

	//Set Gpio pin 10 to output
	value = le32_to_cpu(*(volatile u_long *)PIODIR_R);
	value |= (1 << 10);
	*(volatile u_long *)(PIODIR_R) = cpu_to_le32(value);

	//Set Gpio pin 10 to low
	value = le32_to_cpu(*(volatile u_long *)PIODATA_R);
	value &= ~(1 << 10);
	*(volatile u_long *)(PIODATA_R) = cpu_to_le32(value);
	
	udelay(50000);
	//Set Gpio pin 10 to high
	value = le32_to_cpu(*(volatile u_long *)PIODATA_R);
	value |= (1 << 10);
	*(volatile u_long *)(PIODATA_R) = cpu_to_le32(value);

#elif defined (RT2883_FPGA_BOARD) || defined (RT2883_ASIC_BOARD)
	printf("\n GPIO pin 12 reset to switch\n");

	//Set UARTF_SHARED_MODE to 3'b111 bcs we need gpio 12, and SPI to normal mode
	value = le32_to_cpu(*(volatile u_long *)RT2880_GPIOMODE_REG);
	value |= (7 << 2);
	value &= ~(1 << 1);
	*(volatile u_long *)(RT2880_GPIOMODE_REG) = cpu_to_le32(value);

	//Set Gpio pin 12 to output, and pin 7(RTS) to input
	value = le32_to_cpu(*(volatile u_long *)PIODIR_R);
	value |= (1 << 12);
	value &= ~(1 << 7);
	*(volatile u_long *)(PIODIR_R) = cpu_to_le32(value);

	//Set Gpio pin 12 to low
	value = le32_to_cpu(*(volatile u_long *)PIODATA_R);
	value &= ~(1 << 12);
	*(volatile u_long *)(PIODATA_R) = cpu_to_le32(value);
	
	udelay(50000);
	//Set Gpio pin 12 to high
	value = le32_to_cpu(*(volatile u_long *)PIODATA_R);
	value |= (1 << 12);
	*(volatile u_long *)(PIODATA_R) = cpu_to_le32(value);

#elif defined (RT3052_ASIC_BOARD) || defined (RT3052_FPGA_BOARD) 
	printf("\n GPIO pin 36 reset to switch\n");

	//Set UARTF_SHARED_MODE to 3'b111 bcs we need gpio 36, and SPI to normal mode
	value = le32_to_cpu(*(volatile u_long *)RT2880_GPIOMODE_REG);
	value |= (7 << 2);
	value &= ~(1 << 1);
	*(volatile u_long *)(RT2880_GPIOMODE_REG) = cpu_to_le32(value);

	//Set Gpio pin 36 to output
	value = le32_to_cpu(*(volatile u_long *)0xb000064c);
	value |= (1 << 12);
	*(volatile u_long *)(0xb000064c) = cpu_to_le32(value);

	//Set Gpio pin 36 to low
	value = le32_to_cpu(*(volatile u_long *)0xb0000648);
	value &= ~(1 << 12);
	*(volatile u_long *)(0xb0000648) = cpu_to_le32(value);
	
	udelay(50000);
	//Set Gpio pin 36 to high
	value = le32_to_cpu(*(volatile u_long *)0xb0000648);
	value |= (1 << 12);
	*(volatile u_long *)(0xb0000648) = cpu_to_le32(value);

#elif defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD) 
	printf("\n Please FIXME... \n");

#elif defined (RT3883_ASIC_BOARD)
	printf("\n GPIO pin 24 reset to switch\n");
	/* MT7530 reset timing at least 2ms*/
	//Set Gpio pin 24 to output
	value = le32_to_cpu(*(volatile u_long *)PIODIR3924_R);
	value |= 1;
	*(volatile u_long *)(PIODIR3924_R) = cpu_to_le32(value);

	//Set Gpio pin 24 to low
	value = le32_to_cpu(*(volatile u_long *)PIODATA3924_R);
	value &= ~1;
	*(volatile u_long *)(PIODATA3924_R) = cpu_to_le32(value);

	udelay(50000);
	//Set Gpio pin 24 to high
	value = le32_to_cpu(*(volatile u_long *)PIODATA3924_R);
	value |= 1;
	*(volatile u_long *)(PIODATA3924_R) = cpu_to_le32(value);
#elif defined (RT6855A_ASIC_BOARD) || defined (RT6855A_FPGA_BOARD) 
	/* TODO */
#elif defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD) 
	printf("\n GPIO pin 10 reset to switch\n");
	/* MT7530 reset timing at least 2ms*/
	//set spi/gpio share pin to gpio mode
	value = le32_to_cpu(*(volatile u_long *)RT2880_GPIOMODE_REG);
	value |= (1 << 1);
	*(volatile u_long *)(RT2880_GPIOMODE_REG) = cpu_to_le32(value);

	//Set Gpio pin 10 to output
	value = le32_to_cpu(*(volatile u_long *)PIODIR_R);
	value |= (1 << 10);
	*(volatile u_long *)(PIODIR_R) = cpu_to_le32(value);

	//Set Gpio pin 10 to low
	value = le32_to_cpu(*(volatile u_long *)PIODATA_R);
	value &= ~(1 << 10);
	*(volatile u_long *)(PIODATA_R) = cpu_to_le32(value);
	
	udelay(10000);
	//Set Gpio pin 10 to high
	value = le32_to_cpu(*(volatile u_long *)PIODATA_R);
	value |= (1 << 10);
	*(volatile u_long *)(PIODATA_R) = cpu_to_le32(value);

#elif defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD) 
	/* TODO: reset MT7530 switch */
#else
#error "Unknown Chip"
#endif
#endif // GPIOx_RESET_MODE //
}
#endif

#if defined (MAC_TO_GIGAPHY_MODE) || defined (P5_MAC_TO_PHY_MODE) 
#define EV_ICPLUS_PHY_ID0 0x0243
#define EV_ICPLUS_PHY_ID1 0x0D90
static int isICPlusGigaPHY(int ge)
{
	u32 phy_id0,phy_id1;
        u32 phy_addr = 0;
	
	if(ge == 1)
	    phy_addr = MAC_TO_GIGAPHY_MODE_ADDR;
#if defined (P4_MAC_TO_PHY_MODE)	
	else 
	    phy_addr = MAC_TO_GIGAPHY_MODE_ADDR2;
#endif
	if( ! mii_mgr_read(phy_addr, 2, &phy_id0)){
		printf("\n Read PhyID 0 is Fail!!\n");
		phy_id0 =0;
	}

	if( ! mii_mgr_read(phy_addr, 3, &phy_id1)){
		printf("\n Read PhyID 1 is Fail!!\n");
		phy_id1 = 0;
	}

	if((phy_id0 == EV_ICPLUS_PHY_ID0) && ((phy_id1 & 0xfff0)== EV_ICPLUS_PHY_ID1))
		return 1;

	return 0;
}
#define EV_MARVELL_PHY_ID0 0x0141
#define EV_MARVELL_PHY_ID1 0x0CC2
static int isMarvellGigaPHY(int ge)
{
	u32 phy_id0,phy_id1;
        u32 phy_addr = 0;

	if(ge == 1)
	    phy_addr = MAC_TO_GIGAPHY_MODE_ADDR;
#if defined (P4_MAC_TO_PHY_MODE)	
	else 
	    phy_addr = MAC_TO_GIGAPHY_MODE_ADDR2;
#endif
	if( ! mii_mgr_read(phy_addr, 2, &phy_id0)){
		printf("\n Read PhyID 0 is Fail!!\n");
		phy_id0 =0;
	}

	if( ! mii_mgr_read(phy_addr, 3, &phy_id1)){
		printf("\n Read PhyID 1 is Fail!!\n");
		phy_id1 = 0;
	}

	if((phy_id0 == EV_MARVELL_PHY_ID0) && (phy_id1 == EV_MARVELL_PHY_ID1))
		return 1;

	return 0;
}

#define EV_VTSS_PHY_ID0 0x0007
#define EV_VTSS_PHY_ID1 0x0421
static int isVtssGigaPHY(int ge)
{
	u32 phy_id0,phy_id1;
        u32 phy_addr = 0;

	if(ge == 1)
	    phy_addr = MAC_TO_GIGAPHY_MODE_ADDR;
#if defined (P4_MAC_TO_PHY_MODE)	
	else 
	    phy_addr = MAC_TO_GIGAPHY_MODE_ADDR2;
#endif
	if( ! mii_mgr_read(phy_addr, 2, &phy_id0)){
		printf("\n Read PhyID 0 is Fail!!\n");
		phy_id0 =0;
	}

	if( ! mii_mgr_read(phy_addr, 3, &phy_id1)){
		printf("\n Read PhyID 1 is Fail!!\n");
		phy_id1 = 0;
	}

	if((phy_id0 == EV_VTSS_PHY_ID0) && (phy_id1 == EV_VTSS_PHY_ID1))
		return 1;

	return 0;
}

#endif // MAC_TO_GIGAPHY_MODE || P5_MAC_TO_PHY_MODE //

#if defined (MAC_TO_GIGAPHY_MODE) || defined (P5_MAC_TO_PHY_MODE) || defined (MAC_TO_100PHY_MODE)

#if defined (RT6855_ASIC_BOARD) || defined (RT6855_FPGA_BOARD) || \
    defined (RT6855A_ASIC_BOARD) || defined (RT6855A_FPGA_BOARD)

void enable_auto_negotiate(void)
{
	u32 regValue;
	u32 addr = MAC_TO_GIGAPHY_MODE_ADDR;	// define in config.mk

	regValue = le32_to_cpu(*(volatile u_long *)(RALINK_ETH_SW_BASE+0x7000));
	regValue |= (1<<31);
	regValue &= ~(0x1f);
	regValue &= ~(0x1f<<8);
	regValue |= (addr << 0);// setup PHY address for auto polling (start Addr).
	regValue |= (addr << 8);// setup PHY address for auto polling (End Addr).
	
	*(volatile u_long *)(RALINK_ETH_SW_BASE+0x7000) = cpu_to_le32(regValue);
}

#elif defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD) || \
      defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD) || \
      defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) || \
      defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)

void enable_auto_negotiate(void)
{
	u32 regValue;

#if defined (MT7621_FPGA_BOARD) || defined (MT7621_ASIC_BOARD)
	//enable MDIO mode all the time
	regValue = le32_to_cpu(*(volatile u_long *)(RALINK_SYSCTL_BASE + 0x60));
	regValue &= ~(0x3 << 12);
        *(volatile u_long *)(RALINK_SYSCTL_BASE + 0x60) = regValue;
#endif

	/* FIXME: we don't know how to deal with PHY end addr */
	//regValue = sysRegRead(ESW_PHY_POLLING);
	regValue = le32_to_cpu(*(volatile u_long *)(RALINK_ETH_SW_BASE+0x0000));
	regValue |= (1<<31);        /* phy auto-polling enable */
	regValue &= ~(0x1f);        /* reset phy start addr */
	regValue &= ~(0x1f<<8);     /* reset phy end addr */
#if defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD)
	regValue |= ((addr-1) << 0);// setup PHY address for auto polling (start Addr).
	regValue |= (addr << 8);// setup PHY address for auto polling (End Addr).
#elif defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD) ||\ 
      defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) ||\
      defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)
#ifdef	CONFIG_USE_GE1
	regValue |= (MAC_TO_GIGAPHY_MODE_ADDR << 0);//setup PHY address for auto polling (Start Addr).
	regValue |= (MAC_TO_GIGAPHY_MODE_ADDR << 8);// setup PHY address for auto polling (End Addr).
#else
	regValue |= ((MAC_TO_GIGAPHY_MODE_ADDR-1)&0x1f << 0);//setup PHY address for auto polling (Start Addr).
	regValue |= (MAC_TO_GIGAPHY_MODE_ADDR << 8);// setup PHY address for auto polling (End Addr).
#endif
#else
	regValue |= (addr << 0);// setup PHY address for auto polling (start Addr).
	regValue |= (addr << 8);// setup PHY address for auto polling (End Addr).
#endif

	/*kurtis: AN is strange*/
	//sysRegWrite(ESW_PHY_POLLING, regValue);
	*(volatile u_long *)(RALINK_ETH_SW_BASE + 0x7000) = regValue;

#if defined (P4_MAC_TO_PHY_MODE)      
	*(unsigned long *)(RALINK_ETH_SW_BASE+0x3400) = 0x56330;
#endif
#if defined (P5_MAC_TO_PHY_MODE) 
	*(unsigned long *)(RALINK_ETH_SW_BASE+0x3500) = 0x56330;
#endif

}


#elif defined (RT2880_ASIC_BOARD) || defined (RT2880_FPGA_BOARD) || \
      defined (RT3883_ASIC_BOARD) || defined (RT3883_FPGA_BOARD) || \
      defined (RT3052_ASIC_BOARD) || defined (RT3052_FPGA_BOARD) || \
      defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD)
void enable_auto_negotiate(void)
{
	u32 regValue;
	u32 addr = MAC_TO_GIGAPHY_MODE_ADDR;	// define in config.mk

#if defined (RT3052_ASIC_BOARD) || defined (RT3052_FPGA_BOARD) || \
    defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD) 
	regValue = le32_to_cpu(*(volatile u_long *)(RALINK_ETH_SW_BASE+0x00C8));
#else
	regValue = RALINK_REG(MDIO_CFG);
#endif

	regValue &= 0xe0ff7fff;				// clear auto polling related field:
							// (MD_PHY1ADDR & GP1_FRC_EN).
	regValue |= 0x20000000;				// force to enable MDC/MDIO auto polling.
	regValue |= (addr << 24);			// setup PHY address for auto polling.

#if defined (RT3052_ASIC_BOARD) || defined (RT3052_FPGA_BOARD) || \
    defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD)
	*(volatile u_long *)(RALINK_ETH_SW_BASE+0x00C8) = cpu_to_le32(regValue);
#else
	RALINK_REG(MDIO_CFG) = cpu_to_le32(regValue);
#endif

}
#else
#error "unknown platform"
#endif
#endif // defined (MAC_TO_GIGAPHY_MODE) || defined (P5_MAC_TO_PHY_MODE) || defined (MAC_TO_100PHY_MODE) //

int isDMABusy(struct eth_device* dev)
{
	u32 reg;

	reg = RALINK_REG(PDMA_GLO_CFG);

	if((reg & RX_DMA_BUSY)){
		return 1;
	}

	if((reg & TX_DMA_BUSY)){
		printf("\n  TX_DMA_BUSY !!! ");
		return 1;
	}
	return 0;
}

#if defined (RT6855A_ASIC_BOARD) || defined (RT6855A_FPGA_BOARD)
void rt6855A_gsw_init(void)
{
	u32	i = 0;
	u32	phy_val=0;
	u32     rev=0;
#if defined (RT6855A_FPGA_BOARD)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3000) = 0x5e353;//(P0, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3100) = 0x5e353;//(P1, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	//RALINK_REG(RALINK_ETH_SW_BASE+0x3000) = 0x5e333;//(P0, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
	//RALINK_REG(RALINK_ETH_SW_BASE+0x3100) = 0x5e333;//(P1, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3200) = 0x8000;//P2, link down
	RALINK_REG(RALINK_ETH_SW_BASE+0x3300) = 0x8000;//P3, link down
	RALINK_REG(RALINK_ETH_SW_BASE+0x3400) = 0x8000;//P4, link down
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x8000;//P5, link down

	/* In order to use 10M/Full on FPGA board. We configure phy capable to
	 * 10M Full/Half duplex, so we can use auto-negotiation on PC side */
	for (i=6; i<8; i++) {
		mii_mgr_write(i, 4, 0x07e1);   //Capable of 10M&100M Full/Half Duplex, flow control on/off
		//mii_mgr_write(i, 4, 0x0461);   //Capable of 10M Full/Half Duplex, flow control on/off
		mii_mgr_write(i, 0, 0xB100);   //reset all digital logic, except phy_reg
		mii_mgr_read(i, 9, &phy_val);
                phy_val &= ~(3<<8); //turn off 1000Base-T Advertisement
                mii_mgr_write(i, 9, phy_val);
	}
#elif defined (RT6855A_ASIC_BOARD)

	RALINK_REG(RALINK_ETH_SW_BASE+0x3600) = 0x5e33b;//CPU Port6 Force Link 1G, FC ON
	RALINK_REG(RALINK_ETH_SW_BASE+0x0010) = 0xffffffe0;//CPU exist in port 6
        RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x1ec) = 0x0fffffff;//Set PSE should pause 4 tx ring as default
        RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x1f0) = 0x0fffffff;//switch IOT more stable
	RALINK_REG(RALINK_ETH_SW_BASE+0x30f0) &= ~(3 << 4); ////keep rx/tx port clock ticking, disable internal clock-gating to avoid switch stuck
	/*
	 *  Reg 31: Page Control
	 *  Bit 15     => PortPageSel, 1=local, 0=global
	 *  Bit 14:12  => PageSel, local:0~3, global:0~4
	 *  Reg16~30:Local/Global registers
	 */
	/*correct  PHY  setting J8.0*/
	mii_mgr_read(0, 31, &rev);
        rev &= (0x0f);

	mii_mgr_write(1, 31, 0x4000); //global, page 4

	mii_mgr_write(1, 16, 0xd4cc);
	mii_mgr_write(1, 17, 0x7444);
	mii_mgr_write(1, 19, 0x0112);
	mii_mgr_write(1, 21, 0x7160);
	mii_mgr_write(1, 22, 0x10cf);
	mii_mgr_write(1, 26, 0x0777);

	if(rev == 0){
	        mii_mgr_write(1, 25, 0x0102);
		mii_mgr_write(1, 29, 0x8641);
	}
	else{
	        mii_mgr_write(1, 25, 0x0212);
		mii_mgr_write(1, 29, 0x4640);
        }

	mii_mgr_write(1, 31, 0x2000); //global, page 2
	mii_mgr_write(1, 21, 0x0655);
	mii_mgr_write(1, 22, 0x0fd3);
	mii_mgr_write(1, 23, 0x003d);
	mii_mgr_write(1, 24, 0x096e);
	mii_mgr_write(1, 25, 0x0fed);
	mii_mgr_write(1, 26, 0x0fc4);

	mii_mgr_write(1, 31, 0x1000); //global, page 1  
	mii_mgr_write(1, 17, 0xe7f8);

	mii_mgr_write(1, 31, 0xa000); //local, page 2

	mii_mgr_write(0, 16, 0x0e0e);
	mii_mgr_write(1, 16, 0x0c0c);
	mii_mgr_write(2, 16, 0x0f0f);
	mii_mgr_write(3, 16, 0x1010);
	mii_mgr_write(4, 16, 0x0909);

	mii_mgr_write(0, 17, 0x0000);
	mii_mgr_write(1, 17, 0x0000);
	mii_mgr_write(2, 17, 0x0000);
	mii_mgr_write(3, 17, 0x0000);
	mii_mgr_write(4, 17, 0x0000);

	/*restart AN to make PHY work normal*/
	for (i=0; i<5; i++) {
	    mii_mgr_read(i, 0, &phy_val);
	    phy_val |= 1<<9; //restart AN
	    mii_mgr_write(i, 0, phy_val);
	}
#endif  

#if defined (RT6855A_ASIC_BOARD)
#if defined (P5_RGMII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e33b; ////(P5, Force mode, Link Up, 1000Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x7014) = 0x1f0c000c;//disable port0-port4 internal phy, set phy base address to 12
	RALINK_REG(RALINK_ETH_SW_BASE+0x250c) = 0x000fff10;//disable port5 mac learning
	RALINK_REG(RALINK_ETH_SW_BASE+0x260c) = 0x000fff10;//disable port6 mac learning
#elif defined (P5_MII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e337; ////(P5, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
#elif defined (P5_MAC_TO_PHY_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x7014) = 0xc;//TX/RX CLOCK Phase select
	enable_auto_negotiate();
	if (isICPlusGigaPHY(1)) {
	    printf("ICPLUS Phy1\n");
	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 4, &phy_val);
	    phy_val |= 1<<10; //enable pause ability
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 4, phy_val);

	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &phy_val);
	    phy_val |= 1<<9; //restart AN
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, phy_val);
	}
	if (isMarvellGigaPHY(1)) {
		printf("MARVELL Phy1\n");
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 20, 0x0ce0);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, 0x9140);
	}
	if (isVtssGigaPHY(1)) {
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0x0001); //extended page
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 28, &phy_val);
		printf("GE1 Vitesse Phy reg28 %x --> ",phy_val);
		phy_val |= (0x3<<12); // RGMII RX skew compensation= 2.0 ns
		phy_val &= ~(0x3<<14); // RGMII TX skew compensation= 0 ns
		printf("%x (without reset PHY)\n", phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 28, phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0); //main registers
	}
#elif defined (P5_RMII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e337; ////(P5, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
#else /* Port 5 disabled */
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x8000; ////(P5, Link Down)
#endif // P5_RGMII_TO_MAC_MODE //
#endif
}
#endif



#if defined (RT6855_ASIC_BOARD) || defined (RT6855_FPGA_BOARD) || \
    defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD)
void rt_gsw_init(void)
{
	u32	i = 0;
	u32	phy_val = 0;
	u32     rev = 0;
	u32	is_BGA = 0;
#if defined (P5_RGMII_TO_MAC_MODE)
	u32	regValue;
#endif
#if defined (RT6855_FPGA_BOARD) || defined (MT7620_FPGA_BOARD)
	/*keep dump switch mode */
	RALINK_REG(RALINK_ETH_SW_BASE+0x3000) = 0x5e333;//(P0, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3100) = 0x5e333;//(P1, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3200) = 0x5e333;//(P2, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3300) = 0x5e333;//(P3, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
#if defined (MT7620_FPGA_BOARD)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3400) = 0x5e337;//(P4, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
#else
	RALINK_REG(RALINK_ETH_SW_BASE+0x3400) = 0x5e333;//(P4, Force mode, Link Up, 10Mbps, Full-Duplex, FC ON)
#endif
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e337;//(P5, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	/* In order to use 10M/Full on FPGA board. We configure phy capable to 
	 * 10M Full/Half duplex, so we can use auto-negotiation on PC side */
#if defined (MT7620_FPGA_BOARD)
	for(i=0;i<4;i++){
#else
	for(i=0;i<5;i++){
#endif
	    mii_mgr_write(i, 4, 0x0461);   //Capable of 10M Full/Half Duplex, flow control on/off
	    mii_mgr_write(i, 0, 0xB100);   //reset all digital logic, except phy_reg
	}
#endif  

#if defined (MT7620_ASIC_BOARD)
	is_BGA = (RALINK_REG(RALINK_SYSCTL_BASE + 0xc) >> 16) & 0x1;
	/*
	 *  Reg 31: Page Control
	 *  Bit 15     => PortPageSel, 1=local, 0=global
	 *  Bit 14:12  => PageSel, local:0~3, global:0~4
	 *  Reg 16~30:Local/Global registers
	 */
	/*correct  PHY  setting L3.0 BGA*/

	mii_mgr_write(1, 31, 0x4000); //global, page 4
	mii_mgr_write(1, 17, 0x7444);
	if(is_BGA){
		mii_mgr_write(1, 19, 0x0114);
	}else{
		mii_mgr_write(1, 19, 0x0117);
	}	
	mii_mgr_write(1, 22, 0x10cf);
	mii_mgr_write(1, 25, 0x6212);
	mii_mgr_write(1, 26, 0x0777);
	mii_mgr_write(1, 29, 0x4000);
	mii_mgr_write(1, 28, 0xc077);
	mii_mgr_write(1, 24, 0x0000);

	mii_mgr_write(1, 31, 0x3000); //global, page 3  
	mii_mgr_write(1, 17, 0x4838);

	mii_mgr_write(1, 31, 0x2000); //global, page 2

	if(is_BGA){
	    mii_mgr_write(1, 21, 0x0515);
	    mii_mgr_write(1, 22, 0x0053);
	    mii_mgr_write(1, 23, 0x00bf);
	    mii_mgr_write(1, 24, 0x0aaf);
	    mii_mgr_write(1, 25, 0x0fad);
	    mii_mgr_write(1, 26, 0x0fc1);
	}else{
	    mii_mgr_write(1, 21, 0x0517);
	    mii_mgr_write(1, 22, 0x0fd2);
	    mii_mgr_write(1, 23, 0x00bf);
	    mii_mgr_write(1, 24, 0x0aab);
	    mii_mgr_write(1, 25, 0x00ae);
	    mii_mgr_write(1, 26, 0x0fff);
	}
	mii_mgr_write(1, 31, 0x1000); //global, page 1  
	mii_mgr_write(1, 17, 0xe7f8);

	mii_mgr_write(1, 31, 0x8000); //local, page 0
	mii_mgr_write(0, 30, 0xa000);
	mii_mgr_write(1, 30, 0xa000);
	mii_mgr_write(2, 30, 0xa000);
	mii_mgr_write(3, 30, 0xa000);
#if defined(P4_MAC_TO_NONE_MODE)	
	mii_mgr_write(4, 30, 0xa000);
#endif

	mii_mgr_write(0, 4, 0x05e1);
        mii_mgr_write(1, 4, 0x05e1);
	mii_mgr_write(2, 4, 0x05e1);
	mii_mgr_write(3, 4, 0x05e1);
#if defined(P4_MAC_TO_NONE_MODE)	
	mii_mgr_write(4, 4, 0x05e1);
#endif

	mii_mgr_write(1, 31, 0xa000); //local, page 2
	mii_mgr_write(0, 16, 0x1111);
	mii_mgr_write(1, 16, 0x1010);
	mii_mgr_write(2, 16, 0x1515);
	mii_mgr_write(3, 16, 0x0f0f);
#if defined(P4_MAC_TO_NONE_MODE)	
	mii_mgr_write(4, 16, 0x1313);
#endif

	/*restart AN to make PHY work normal*/
#if defined(P4_MAC_TO_NONE_MODE)	
	for (i=0; i<5; i++) {
#else
	for (i=0; i<4; i++) {
#endif
	    mii_mgr_read(i, 0, &phy_val);
	    phy_val |= 1<<9; //restart AN
	    mii_mgr_write(i, 0, phy_val);
	}
#endif 


#if defined (PDMA_NEW)
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 8); //PCIE_RC_MODE=1
#endif

#if defined (MT7620_FPGA_BOARD) || defined (MT7620_ASIC_BOARD)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3600) = 0x5e33b;//CPU Port6 Force Link 1G, FC ON
	RALINK_REG(RALINK_ETH_SW_BASE+0x0010) = 0x7f7f7fe0;//CPU exist in port 6
#if defined (P5_RGMII_TO_MAC_MODE)
	*(unsigned long *)(0xb0000060) &= ~(3 << 7); //set MDIO to Normal mode

	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e33b; ////(P5, Force mode, Link Up, 1000Mbps, Full-Duplex, FC ON)
	RALINK_REG(RALINK_ETH_SW_BASE+0x7014) = 0x1f0c000c;//disable port0-port4 internal phy, set phy base address to 12
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3<<12); ////GE1_MODE=RGMii Mode
	
	/*HW RESET MT7530*/
	ResetSWusingGPIOx();
	udelay(125000);

	for(i=0;i<=4;i++) 
	{	
	       //turn off PHY
	       mii_mgr_read(i, 0x0 ,&regValue);
	       regValue |= (0x1<<11);
               mii_mgr_write(i, 0x0, regValue);

	}
	mii_mgr_write(31, 0x3500, 0x8000);
	mii_mgr_write(31, 0x3600, 0x8000);//force MAC link down before reset


	/*Init MT7530, we use MT7530 as default external switch*/
	mii_mgr_write(31, 0x7000, 0x3);//reset MT7530
	printf("#Reset_MT7530\n");
	udelay(100);
	
	for(i=0;i<=4;i++) 
	{	
	       //turn on PHY
	       mii_mgr_read(i, 0x0 ,&regValue);
	       regValue &= ~(0x1<<11);
               mii_mgr_write(i, 0x0, regValue);
	}
	mii_mgr_write(31, 0x3600, 0x5e33b);//MT7530 P6 force 1G
	mii_mgr_write(31, 0x7804, 0x1117ccf);//MT7530 P5 disable
#elif defined (P5_MII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e337; ////(P5, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 12);
#elif defined (P5_MAC_TO_PHY_MODE)
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(0xb0000060) &= ~(3 << 7); //set MDIO to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=RGMii Mode
	enable_auto_negotiate();
	if (isICPlusGigaPHY(1)) {
	    printf("ICPLUS Phy1\n");
	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 4, &phy_val);
	    phy_val |= 1<<10; //enable pause ability
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 4, phy_val);

	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &phy_val);
	    phy_val |= 1<<9; //restart AN
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, phy_val);
	}
	if (isMarvellGigaPHY(1)) {
		printf("MARVELL Phy1\n");
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 20, 0x0ce0);
#if defined (MT7620_FPGA_BOARD)
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 9, &phy_val);
		phy_val &= ~(3<<8); //turn off 1000Base-T Advertisement
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 9, phy_val);
#endif
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, 0x9140);
	}
	if (isVtssGigaPHY(1)) {
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0x0001); //extended page
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 28, &phy_val);
		printf("GE1 Vitesse Phy reg28 %x --> ",phy_val);
		phy_val |= (0x3<<12); // RGMII RX skew compensation= 2.0 ns
		phy_val &= ~(0x3<<14); // RGMII TX skew compensation= 0 ns
		printf("%x (without reset PHY)\n", phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 28, phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0); //main registers
	}
#elif defined (P5_RMII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x5e337; ////(P5, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
        RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x2 << 12);
#else /* Port 5 disabled */
	RALINK_REG(RALINK_ETH_SW_BASE+0x3500) = 0x8000; ////(P5, Link Down)
#endif // P5_RGMII_TO_MAC_MODE //
#endif


#if defined (P4_RGMII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3400) = 0x5e33b; ////(P4, Force mode, Link Up, 1000Mbps, Full-Duplex, FC ON)
	RALINK_REG(0xb0000060) &= ~(1 << 10); //set RGMII to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3<<14); ////GE2_MODE=RGMii Mode
#elif defined (P4_MII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3400) = 0x5e337; ////(P4, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	RALINK_REG(0xb0000060) &= ~(1 << 10); //set RGMII2 to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 14); //GE2_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 14);
#elif defined (P4_MAC_TO_PHY_MODE)
	RALINK_REG(0xb0000060) &= ~(1 << 10); //set RGMII2 to Normal mode
	RALINK_REG(0xb0000060) &= ~(3 << 7); //set MDIO to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3<<14); ////GE2_MODE=RGMii Mode
#if defined (MT7620_FPGA_BOARD)
	    mii_mgr_write(4, 4, 0x05e1);   //Capable of 100M Full/Half Duplex, flow control on/off
	    mii_mgr_write(4, 0, 0xB100);   //reset all digital logic, except phy_reg
#endif
	enable_auto_negotiate();
	if (isICPlusGigaPHY(2)) {
	    printf("ICPLUS Phy2\n");
	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR2, 4, &phy_val);
	    phy_val |= 1<<10; //enable pause ability
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 4, phy_val);

	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR2, 0, &phy_val);
	    phy_val |= 1<<9; //restart AN
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 0, phy_val);
	}
	if (isMarvellGigaPHY(2)) {
		printf("MARVELL Phy2\n");
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 20, 0x0ce0);
#if defined (MT7620_FPGA_BOARD)
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR2, 9, &phy_val);
		phy_val &= ~(3<<8); //turn off 1000Base-T Advertisement
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 9, phy_val);
#endif
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 0, 0x9140);
	}
	if (isVtssGigaPHY(2)) {
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 31, 0x0001); //extended page
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR2, 28, &phy_val);
		printf("GE1 Vitesse Phy reg28 %x --> ",phy_val);
		phy_val |= (0x3<<12); // RGMII RX skew compensation= 2.0 ns
		phy_val &= ~(0x3<<14); // RGMII TX skew compensation= 0 ns
		printf("%x (without reset PHY)\n", phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 28, phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR2, 31, 0); //main registers
	}
#elif defined (P4_RMII_TO_MAC_MODE)
	RALINK_REG(RALINK_ETH_SW_BASE+0x3400) = 0x5e337; ////(P4, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
        RALINK_REG(0xb0000060) &= ~(1 << 10); //set RGMII2 to Normal mode
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 14); //GE2_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x2 << 14);
#else /* Port 4 disabled */

#endif // P4_RGMII_TO_MAC_MODE //
}
#endif

#if defined (RT6855A_FPGA_BOARD)
void rt6855A_eth_gpio_reset(void)
{
	u8 ether_gpio = 12;

	/* Load the ethernet gpio value to reset Ethernet PHY */
	ra_or(RALINK_PIO_BASE, 1<<(ether_gpio<<1));
	ra_or(RALINK_PIO_BASE+0x14, 1<<ether_gpio);

	ra_and(RALINK_PIO_BASE+0x4, ~(1<<ether_gpio));
	udelay(100000);

	ra_or(RALINK_PIO_BASE+0x4, 1<<ether_gpio);
	/* must wait for 0.6 seconds after reset*/
	udelay(600000);
}
#endif


#if defined (RT3052_ASIC_BOARD) || defined (RT3052_FPGA_BOARD) || \
    defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD) || \
    defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || \
    defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)
void rt305x_esw_init(void)
{
	u32	i;
	u32	phy_val=0, phy_val2=0;

	/*                                                                               
	 * FC_RLS_TH=200, FC_SET_TH=160
	 * DROP_RLS=120, DROP_SET_TH=80
	 */
	RALINK_REG(RALINK_ETH_SW_BASE+0x0008) = 0xC8A07850;       
	RALINK_REG(RALINK_ETH_SW_BASE+0x00E4) = 0x00000000;
	RALINK_REG(RALINK_ETH_SW_BASE+0x0014) = 0x00405555;
	RALINK_REG(RALINK_ETH_SW_BASE+0x0090) = 0x00007f7f;
	RALINK_REG(RALINK_ETH_SW_BASE+0x0098) = 0x00007f7f; //disable VLAN
	RALINK_REG(RALINK_ETH_SW_BASE+0x00CC) = 0x0002500c;
#ifndef CONFIG_UNH_TEST
	RALINK_REG(RALINK_ETH_SW_BASE+0x009C) = 0x0008a301; //hashing algorithm=XOR48, aging interval=300sec
#else
	/*
	 * bit[30]:1	Backoff Algorithm Option: The latest one to pass UNH test
	 * bit[29]:1	Length of Received Frame Check Enable
	 * bit[8]:0	Enable collision 16 packet abort and late collision abort
	 * bit[7:6]:01	Maximum Packet Length: 1518
	 */
	RALINK_REG(RALINK_ETH_SW_BASE+0x009C) = 0x6008a241;
#endif
	RALINK_REG(RALINK_ETH_SW_BASE+0x008C) = 0x02404040; 

#if defined (RT3052_ASIC_BOARD) || defined (RT3352_ASIC_BOARD) || defined (RT5350_ASIC_BOARD) || defined (MT7628_ASIC_BOARD)
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) = 0x3f502b28; //Ext PHY Addr=0x1F 
	RALINK_REG(RALINK_ETH_SW_BASE+0x0084) = 0x00000000;
	RALINK_REG(RALINK_ETH_SW_BASE+0x0110) = 0x7d000000; //1us cycle number=125 (FE's clock=125Mhz)
#elif defined (RT3052_FPGA_BOARD) || defined (RT3352_FPGA_BOARD) || defined (RT5350_FPGA_BOARD) || defined (MT7628_FPGA_BOARD)

	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) = 0x00f03ff9; //polling Ext PHY Addr=0x0, force port5 as 100F/D (disable auto-polling)
	RALINK_REG(RALINK_ETH_SW_BASE+0x0084) = 0xffdf1f00;
	RALINK_REG(RALINK_ETH_SW_BASE+0x0110) = 0x0d000000; //1us cycle number=13 (FE's clock=12.5Mhz)

	/* In order to use 10M/Full on FPGA board. We configure phy capable to 
	 * 10M Full/Half duplex, so we can use auto-negotiation on PC side */
	for(i=0;i<5;i++){
	    mii_mgr_write(i, 4, 0x0461);   //Capable of 10M Full/Half Duplex, flow control on/off
	    mii_mgr_write(i, 0, 0xB100);   //reset all digital logic, except phy_reg
	}
#ifdef MT7628_FPGA_V6
	    mii_mgr_write(30, 4, 0x0461);   //Capable of 10M Full/Half Duplex, flow control on/off
	    
	    mii_mgr_read(30, 9, &phy_val);
	    phy_val &= ~(3<<8); //turn off 1000Base-T Advertisement
	    mii_mgr_write(30, 9, phy_val);
	    
	    
	    mii_mgr_write(30, 0, 0xB100);   //reset all digital logic, except phy_reg
		printf("MARVELL Phy1\n");
		mii_mgr_write(30, 20, 0x0ce0);
		mii_mgr_write(30, 0, 0x9140);
#endif
#endif

#if defined (P5_RGMII_TO_MAC_MODE)
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(1<<29); //disable port 5 auto-polling
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) |= 0x3fff; //force 1000M full duplex
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(0xf<<20); //rxclk_skew, txclk_skew = 0
#elif defined (P5_MII_TO_MAC_MODE)
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(1<<29); //disable port 5 auto-polling
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(0x3fff);
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) |= 0x3ffd; //force 100M full duplex
#if defined (RT3352_ASIC_BOARD)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 12);
#endif
#elif defined (P5_MAC_TO_PHY_MODE)
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(0xb0000060) &= ~(1 << 7); //set MDIO to Normal mode
#if defined (RT3052_ASIC_BOARD) || defined(RT3352_ASIC_BOARD)
	enable_auto_negotiate();
#endif
	if (isICPlusGigaPHY(1)) {
	    printf("\n ICPLUS Phy\n");
	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 4, &phy_val);
	    phy_val |= 1<<10; //enable pause ability
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 4, phy_val);

	    mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &phy_val);
	    phy_val |= 1<<9; //restart AN
	    mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, phy_val);
	}
	if (isMarvellGigaPHY(1)) {
		printf("\n MARVELL Phy\n");
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 20, 0x0ce0);
#if defined (RT3052_FPGA_BOARD) || defined(RT3352_FPGA_BOARD)
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 9, &phy_val);
		phy_val &= ~(3<<8); //turn off 1000Base-T Advertisement
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 9, phy_val);
#endif
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, 0x9140);
	}
	if (isVtssGigaPHY(1)) {
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0x0001); //extended page
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 28, &phy_val);
		printf("GE1 Vitesse Phy reg28 %x --> ",phy_val);
		phy_val |= (0x3<<12); // RGMII RX skew compensation= 2.0 ns
		phy_val &= ~(0x3<<14); // RGMII TX skew compensation= 0 ns
		printf("%x (without reset PHY)\n", phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 28, phy_val);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0); //main registers
	}
#elif defined (P5_RMII_TO_MAC_MODE)
	/* Reserved */
	RALINK_REG(0xb0000060) &= ~(1 << 9); //set RGMII to Normal mode
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(1<<29); //disable port 5 auto-polling
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(0x3fff);
	RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) |= 0x3ffd; //force 100M full duplex
#if defined (RT3352_ASIC_BOARD)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=RvMii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x2 << 12);
#endif
#else /* Port 5 disabled */

#if defined (RT3052_ASIC_BOARD)
        RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(1 << 29); //port5 auto polling disable
        RALINK_REG(0xb0000060) |= (1 << 7); //set MDIO to GPIO mode (GPIO22-GPIO23)
        RALINK_REG(0xb0000060) |= (1 << 9); //set RGMII to GPIO mode (GPIO41-GPIO50)
        RALINK_REG(0xb0000674) = 0xFFF; //GPIO41-GPIO50 output mode
        RALINK_REG(0xb000067C) = 0x0; //GPIO41-GPIO50 output low
#elif defined (RT3352_ASIC_BOARD)
        RALINK_REG(RALINK_ETH_SW_BASE+0x00C8) &= ~(1 << 29); //port5 auto polling disable
        RALINK_REG(0xb0000060) |= (1 << 7); //set MDIO to GPIO mode (GPIO22-GPIO23)
        RALINK_REG(0xb0000624) = 0xC0000000; //GPIO22-GPIO23 output mode
        RALINK_REG(0xb000062C) = 0xC0000000; //GPIO22-GPIO23 output high

        RALINK_REG(0xb0000060) |= (1 << 9); //set RGMII to GPIO mode (GPIO24-GPIO35)
        RALINK_REG(0xb000064C) = 0xFFF; //GPIO24-GPIO35 output mode
        RALINK_REG(0xb0000654) = 0xFFF; //GPIO24-GPIO35 output high
#endif

#endif // P5_RGMII_TO_MAC_MODE //

#define RSTCTRL_EPHY_RST	(1<<24)
#define MT7628_EPHY_EN	        (0x1f<<16)
	/* We shall prevent modifying PHY registers if it is FPGA mode */
#if defined (RT3052_ASIC_BOARD) || defined (RT3352_ASIC_BOARD) || defined (RT5350_ASIC_BOARD) || defined (MT7628_ASIC_BOARD)
#if defined (RT3052_ASIC_BOARD)

	rw_rf_reg(0, 0, &phy_val);
	phy_val = phy_val >> 4;

	if(phy_val > 0x5) {
	    
	    rw_rf_reg(0, 26, &phy_val);
	    phy_val2 = (phy_val | (0x3 << 5));
	    rw_rf_reg(1, 26, &phy_val2);

	    // reset phy
	    i = RALINK_REG(RT2880_RSTCTRL_REG);
	    i = i | RSTCTRL_EPHY_RST;
	    RALINK_REG(RT2880_RSTCTRL_REG)= i;
	    i = i & ~(RSTCTRL_EPHY_RST);
	    RALINK_REG(RT2880_RSTCTRL_REG)= i;

	    rw_rf_reg(1, 26, &phy_val);

	    //select local register
	    mii_mgr_write(0, 31, 0x8000);
	    for(i=0;i<5;i++){
		mii_mgr_write(i, 26, 0x1600);   //TX10 waveform coefficient //LSB=0 disable PHY
		mii_mgr_write(i, 29, 0x7058);   //TX100/TX10 AD/DA current bias
		mii_mgr_write(i, 30, 0x0018);   //TX100 slew rate control
	    }

	    //select global register
	    mii_mgr_write(0, 31, 0x0);
	    mii_mgr_write(0,  1, 0x4a40); //enlarge agcsel threshold 3 and threshold 2
	    mii_mgr_write(0,  2, 0x6254); //enlarge agcsel threshold 5 and threshold 4
	    mii_mgr_write(0,  3, 0xa17f); //enlarge agcsel threshold 6
//#define ENABLE_LDPS
#if defined (ENABLE_LDPS)
            mii_mgr_write(0, 12, 0x7eaa);
            mii_mgr_write(0, 22, 0x252f); //tune TP_IDL tail and head waveform, enable power down slew rate control
#else
            mii_mgr_write(0, 12, 0x0);
            mii_mgr_write(0, 22, 0x052f);
#endif
	    mii_mgr_write(0, 14, 0x65);   //longer TP_IDL tail length
	    mii_mgr_write(0, 16, 0x0684); //increased squelch pulse count threshold.
	    mii_mgr_write(0, 17, 0x0fe0); //set TX10 signal amplitude threshold to minimum
	    mii_mgr_write(0, 18, 0x40ba); //set squelch amplitude to higher threshold
	    mii_mgr_write(0, 27, 0x2fce); //set PLL/Receive bias current are calibrated
	    mii_mgr_write(0, 28, 0xc410); //change PLL/Receive bias current to internal(RT3350)
	    mii_mgr_write(0, 29, 0x598b); //change PLL bias current to internal(RT3052_MP3)
	    mii_mgr_write(0, 31, 0x8000); //select local register


	    for(i=0;i<5;i++){
		//LSB=1 enable PHY
		mii_mgr_read(i, 26, &phy_val);
		phy_val |= 0x0001;
		mii_mgr_write(i, 26, phy_val);
	    }

	} else {
	    //select local register
	    mii_mgr_write(0, 31, 0x8000);
	    for(i=0;i<5;i++){
		mii_mgr_write(i, 26, 0x1600);   //TX10 waveform coefficient //LSB=0 disable PHY
		mii_mgr_write(i, 29, 0x7058);   //TX100/TX10 AD/DA current bias
		mii_mgr_write(i, 30, 0x0018);   //TX100 slew rate control
	    }
	    
	    //select global register
	    mii_mgr_write(0, 31, 0x0);
	    mii_mgr_write(0,  1, 0x4a40); //enlarge agcsel threshold 3 and threshold 2
	    mii_mgr_write(0,  2, 0x6254); //enlarge agcsel threshold 5 and threshold 4
	    mii_mgr_write(0,  3, 0xa17f); //enlarge agcsel threshold 6
	    mii_mgr_write(0, 14, 0x65);   //longer TP_IDL tail length
	    mii_mgr_write(0, 16, 0x0684); //increased squelch pulse count threshold.
	    mii_mgr_write(0, 17, 0x0fe0); //set TX10 signal amplitude threshold to minimum
	    mii_mgr_write(0, 18, 0x40ba); //set squelch amplitude to higher threshold
	    mii_mgr_write(0, 22, 0x052f); //tune TP_IDL tail and head waveform
	    mii_mgr_write(0, 27, 0x2fce); //set PLL/Receive bias current are calibrated
	    mii_mgr_write(0, 28, 0xc410); //change PLL/Receive bias current to internal(RT3350)
	    mii_mgr_write(0, 29, 0x598b); //change PLL bias current to internal(RT3052_MP3)
	    mii_mgr_write(0, 31, 0x8000); //select local register

	    for(i=0;i<5;i++){
		//LSB=1 enable PHY
		mii_mgr_read(i, 26, &phy_val);
		phy_val |= 0x0001;
		mii_mgr_write(i, 26, phy_val);
	    }
	}

#elif defined (RT3352_ASIC_BOARD)
    //PHY IOT
    // reset phy
    i = RALINK_REG(RT2880_RSTCTRL_REG);
    i = i | RSTCTRL_EPHY_RST;
    RALINK_REG(RT2880_RSTCTRL_REG) = i;
    i = i & ~(RSTCTRL_EPHY_RST);
    RALINK_REG(RT2880_RSTCTRL_REG) = i;

	//select local register
	mii_mgr_write(0, 31, 0x8000);
	for(i=0;i<5;i++){
	    mii_mgr_write(i, 26, 0x1600);   //TX10 waveform coefficient //LSB=0 disable PHY
	    mii_mgr_write(i, 29, 0x7016);   //TX100/TX10 AD/DA current bias
	    mii_mgr_write(i, 30, 0x0038);   //TX100 slew rate control
	}

	//select global register
	mii_mgr_write(0, 31, 0x0);
	mii_mgr_write(0,  1, 0x4a40); //enlarge agcsel threshold 3 and threshold 2
	mii_mgr_write(0,  2, 0x6254); //enlarge agcsel threshold 5 and threshold 4
	mii_mgr_write(0,  3, 0xa17f); //enlarge agcsel threshold 6
	mii_mgr_write(0, 12, 0x7eaa);
	mii_mgr_write(0, 14, 0x65);   //longer TP_IDL tail length
	mii_mgr_write(0, 16, 0x0684); //increased squelch pulse count threshold.
	mii_mgr_write(0, 17, 0x0fe0); //set TX10 signal amplitude threshold to minimum
	mii_mgr_write(0, 18, 0x40ba); //set squelch amplitude to higher threshold
	mii_mgr_write(0, 22, 0x253f); //tune TP_IDL tail and head waveform, enable power down slew rate control
	mii_mgr_write(0, 27, 0x2fda); //set PLL/Receive bias current are calibrated
	mii_mgr_write(0, 28, 0xc410); //change PLL/Receive bias current to internal(RT3350)
	mii_mgr_write(0, 29, 0x598b); //change PLL bias current to internal(RT3052_MP3)
	mii_mgr_write(0, 31, 0x8000); //select local register

	for(i=0;i<5;i++){
	    //LSB=1 enable PHY
	    mii_mgr_read(i, 26, &phy_val);
	    phy_val |= 0x0001;
	    mii_mgr_write(i, 26, phy_val);
	}
#elif defined (RT5350_ASIC_BOARD)
    //PHY IOT
    // reset phy
    i = RALINK_REG(RT2880_RSTCTRL_REG);
    i = i | RSTCTRL_EPHY_RST;
    RALINK_REG(RT2880_RSTCTRL_REG) = i;
    i = i & ~(RSTCTRL_EPHY_RST);
    RALINK_REG(RT2880_RSTCTRL_REG) = i;

	//select local register
	mii_mgr_write(0, 31, 0x8000);
	for(i=0;i<5;i++){
	    mii_mgr_write(i, 26, 0x1600);   //TX10 waveform coefficient //LSB=0 disable PHY
	    mii_mgr_write(i, 29, 0x7015);   //TX100/TX10 AD/DA current bias
	    mii_mgr_write(i, 30, 0x0038);   //TX100 slew rate control
	}

	//select global register
	mii_mgr_write(0, 31, 0x0);
	mii_mgr_write(0,  1, 0x4a40); //enlarge agcsel threshold 3 and threshold 2
	mii_mgr_write(0,  2, 0x6254); //enlarge agcsel threshold 5 and threshold 4
	mii_mgr_write(0,  3, 0xa17f); //enlarge agcsel threshold 6
	mii_mgr_write(0, 12, 0x7eaa);
	mii_mgr_write(0, 14, 0x65);   //longer TP_IDL tail length
	mii_mgr_write(0, 16, 0x0684); //increased squelch pulse count threshold.
	mii_mgr_write(0, 17, 0x0fe0); //set TX10 signal amplitude threshold to minimum
	mii_mgr_write(0, 18, 0x40ba); //set squelch amplitude to higher threshold
	mii_mgr_write(0, 22, 0x253f); //tune TP_IDL tail and head waveform, enable power down slew rate control
	mii_mgr_write(0, 27, 0x2fda); //set PLL/Receive bias current are calibrated
	mii_mgr_write(0, 28, 0xc410); //change PLL/Receive bias current to internal(RT3350)
	mii_mgr_write(0, 29, 0x598b); //change PLL bias current to internal(RT3052_MP3)
	mii_mgr_write(0, 31, 0x8000); //select local register

	for(i=0;i<5;i++){
	    //LSB=1 enable PHY
	    mii_mgr_read(i, 26, &phy_val);
	    phy_val |= 0x0001;
	    mii_mgr_write(i, 26, phy_val);
	}
#elif defined (MT7628_ASIC_BOARD)
/*TODO: Init MT7628 ASIC PHY HERE*/
	i = RALINK_REG(RT2880_AGPIOCFG_REG);
	i = i & ~(MT7628_EPHY_EN);
	RALINK_REG(RT2880_AGPIOCFG_REG) = i;

	printf("RESET MT7628 PHY!!!!!!");
	// reset phy
	i = RALINK_REG(RT2880_RSTCTRL_REG);
	i = i | RSTCTRL_EPHY_RST;
	RALINK_REG(RT2880_RSTCTRL_REG) = i;
	i = i & ~(RSTCTRL_EPHY_RST);
	RALINK_REG(RT2880_RSTCTRL_REG) = i;

#else
#error "Chip is not supported"
#endif // RT3052_ASIC_BOARD //
#endif // RT3052_ASIC_BOARD || RT3352_ASIC_BOARD //

}
#endif


#if defined (RT3883_ASIC_BOARD) && defined (MAC_TO_MT7530_MODE)
void rt3883_gsw_init(void)
{
	printf("\n MT7530 Giga Switch support \n");
	//RALINK_REG(MDIO_CFG)=cpu_to_le32((u32)(0x1F01DC41)); /*GE1 Force 1G/FC ON/MDIO 2Mhz*/
	RALINK_REG(MDIO_CFG)=cpu_to_le32((u32)(0x1F01DC01)); /*GE1 Force 1G/FC ON/MDIO 4Mhz*/
	//enable_auto_negotiate();
	ResetSWusingGPIOx();
	udelay(125000);

	mii_mgr_write(31, 0x3500, 0x5e33b);//MT7530 P5 force 1G
	mii_mgr_write(31, 0x7804, 0x1017d8f);//MT7530 HW TRAP,  P6 disable, P5 RGMII GMAC5
}
#endif
#if defined (MT7621_ASIC_BOARD) || defined (MT7621_FPGA_BOARD)
void setup_internal_gsw(void)
{
	u32	i;
	u32	regValue;

	// reset switch
	regValue = RALINK_REG(RT2880_RSTCTRL_REG);
	regValue = regValue | (1<<2);
	RALINK_REG(RT2880_RSTCTRL_REG) = regValue;
	udelay(1000);
	regValue = regValue & ~(1<<2);
	RALINK_REG(RT2880_RSTCTRL_REG) = regValue;
	udelay(10000);


	/* reduce MDIO PAD driving strength */
	regValue = RALINK_REG(PAD_RGMII2_MDIO_CFG);
	regValue &= ~(0x3<<4);	// reduce Tx driving strength to 2mA (WDT_E4_E2)
	RALINK_REG(PAD_RGMII2_MDIO_CFG) = regValue;
  

	for(i=0;i<=4;i++) 
	{	
	       //turn off PHY
	       mii_mgr_read(i, 0x0 ,&regValue);
	       regValue |= (0x1<<11);
               mii_mgr_write(i, 0x0, regValue);

	}
	mii_mgr_write(31, 0x3500, 0x8000);
	mii_mgr_write(31, 0x3600, 0x8000);//force MAC link down before reset

	mii_mgr_write(31, 0x7000, 0x3);//reset MT7530
	printf("#Reset_MT7530\n");
	udelay(5);
	

#ifdef CONFIG_USE_GE1
#if defined (MT7621_ASIC_BOARD)	
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x2005e33b;//(GE1, Force 1000M/FD, FC ON)
	mii_mgr_write(31, 0x3600, 0x5e30b);//PDMA is not ready,disable FC, Prevent HOL
	mii_mgr_write(31, 0x3500, 0x8000);


#elif defined (MT7621_FPGA_BOARD)
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x2005e337;//(GE1, Force 100M/FD, FC ON)
	mii_mgr_write(31, 0x3600, 0x5e337);
#endif
	RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x00008000;// GE2, down
	
	RALINK_REG(GDMA1_FWD_CFG) = 0x20710000;
	RALINK_REG(GDMA2_FWD_CFG) = 0x20717777;

	/* Enable MT7530 Port 6 */
	regValue = 0x117ccf; //Enable Port 6 only
	mii_mgr_write(31, 0x7804 ,regValue);


#elif defined CONFIG_USE_GE2
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x000008000;//(GE1, Force LinkDown)
	mii_mgr_write(31, 0x3500, 0x56300); //MT7530 P5 AN
	RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x20056300;// GE2, auto-polling
	

	/* Set MT7530 Port 0/4 to PHY mode */
	mii_mgr_read(31, 0x7804 ,&regValue);
#if defined GE_RGMII_INTERNAL_P0_AN 
	regValue &= ~((1<<13)|(1<<6));
	regValue |= ((1<<7)|(1<<16)|(1<<20));
#elif defined GE_RGMII_INTERNAL_P4_AN 
	regValue &= ~((1<<13)|(1<<6)|(1<20));
	regValue |= ((1<<7)|(1<<16));
#endif
	mii_mgr_write(31, 0x7804 ,regValue);

	enable_auto_negotiate();

	RALINK_REG(GDMA1_FWD_CFG) = 0x20717777;
	RALINK_REG(GDMA2_FWD_CFG) = 0x20710000;
#endif

	regValue = RALINK_REG(RALINK_SYSCTL_BASE + 0x10);
	regValue = (regValue >> 6) & 0x7;
	if(regValue >= 6) { //25 Mhz Xtal
		/* do nothing */
	} else if(regValue >= 3) { //40Mhz Xtal
	    mii_mgr_write(0, 13, 0x1f);  // disable MT7530 core clock
	    mii_mgr_write(0, 14, 0x410);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x0);

	    mii_mgr_write(0, 13, 0x1f);  // disable MT7530 PLL
	    mii_mgr_write(0, 14, 0x40d);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x2020);

	    mii_mgr_write(0, 13, 0x1f);  // for MT7530 core clock = 500Mhz
	    mii_mgr_write(0, 14, 0x40e);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x119);

	    mii_mgr_write(0, 13, 0x1f);  // enable MT7530 PLL
	    mii_mgr_write(0, 14, 0x40d);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x2820);

	    udelay(20); //suggest by CD

	    mii_mgr_write(0, 13, 0x1f);  // enable MT7530 core clock
	    mii_mgr_write(0, 14, 0x410);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x1);
	} else { //20 Mhz Xtal

		/* TODO */

	}
/*Tx Driving*/
	mii_mgr_write(31, 0x7a54, 0x44);  //lower driving
	mii_mgr_write(31, 0x7a5c, 0x44);  //lower driving
	mii_mgr_write(31, 0x7a64, 0x44);  //lower driving
	mii_mgr_write(31, 0x7a6c, 0x44);  //lower driving
	mii_mgr_write(31, 0x7a74, 0x44);  //lower driving
	mii_mgr_write(31, 0x7a7c, 0x44);  //lower driving

	for(i=0;i<=4;i++) 
	{	
	       //turn on PHY
	       mii_mgr_read(i, 0x0 ,&regValue);
	       regValue &= ~(0x1<<11);
               mii_mgr_write(i, 0x0, regValue);
	}



#ifdef CONFIG_USE_GE2
#if 1
	mii_mgr_write(31, 0x7b00, 0x102);  //delay detting for 10/1000M
	mii_mgr_write(31, 0x7b04, 0x14);  //delay setting for 10/1000M
#else
	mii_mgr_write(31, 0x7b00, 8);  // for 100M
	mii_mgr_write(31, 0x7b04, 0x14);  // for 100M
#endif
#endif



/*GE2 delay setting only for 1G/10M => turn off 100M for USE_GE2*/
#ifdef CONFIG_USE_GE2
	for(i=0;i<=4;i++) {	
	       mii_mgr_read(i, 4, &regValue);
	       regValue &= ~(3<<7); //turn off 100Base-T Advertisement
               mii_mgr_write(i, 4, regValue);

		//mii_mgr_read(i, 9, &regValue);
                //regValue &= ~(3<<8); //turn off 1000Base-T Advertisement
                //mii_mgr_write(i, 9, regValue);

		//restart AN
		mii_mgr_read(i, 0, &regValue);
		regValue |= (1 << 9);
		mii_mgr_write(i, 0, regValue);
	}
#endif
	mii_mgr_read(31, 0x7808 ,&regValue);
	regValue |= (3<<16); //Enable INTR
	mii_mgr_write(31, 0x7808 ,regValue);
}
#endif

#if defined (MT7623_ASIC_BOARD) || defined (MT7623_FPGA_BOARD) ||\
    defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)
void setup_internal_gsw(void)
{
	u32	i;
	u32	regValue;
	u32     xtal_mode;

	printf("Enter setup_internal_gsw()\n");


#if 0
	/* GE1: RGMII mode setting */	
	*(volatile u_long *)(0xfb110300) = 0x80020000;
	*(volatile u_long *)(0xfb110304) = 0x00980000;
	*(volatile u_long *)(0xfb110300) = 0x40020000;
	*(volatile u_long *)(0xfb110304) = 0xc0980000;
	*(volatile u_long *)(0xfb110310) = 0x00000041;
	*(volatile u_long *)(0xfb110318) = 0x00000044;
	*(volatile u_long *)(0xfb110320) = 0x00000043;
	*(volatile u_long *)(0xfb110328) = 0x00000042;
	*(volatile u_long *)(0xfb110330) = 0x00000042;
	*(volatile u_long *)(0xfb110340) = 0x00020000;
	*(volatile u_long *)(0xfb110390) &= 0xfffffff8; //RGMII mode
#else
	/* GE1: TRGMII mode setting */	
	*(volatile u_long *)(0x1b110390) |= 0x00000002; //TRGMII mode
#endif


	// reset switch
#if defined(MT7623_ASIC_BOARD)
        //regValue = *(volatile u_long *)(0x1b00000c);
        /*MT7530 Reset. Flows for MT7623A and MT7623N are both excuted.*/
        /* Should Modify this section if EFUSE is ready*/
        /*For MT7623N reset MT7530*/
        //if(!(regValue & (1<<16)))
        {
                *(volatile u_long *)(0x10005520) &= ~(1<<1);
                udelay(1000);
                *(volatile u_long *)(0x10005520) |= (1<<1);
                mdelay(100);
        }
        //printf("Assert MT7623 RXC reset\n");
        *(volatile u_long *)(0x1b110300) |= 0x80000000;   // Assert MT7623 RXC reset
        /*For MT7623 reset MT7530*/
        *(volatile u_long *)(RALINK_SYSCTL_BASE + 0x34) |= (0x1 << 2);
        udelay(1000);
        *(volatile u_long *)(RALINK_SYSCTL_BASE + 0x34) &= ~(0x1 << 2);
        //mdelay(100);
        mdelay(1000);	/* NOTE(Nelson): workaround for UBNT RFB */
#endif

	/* Wait for Switch Reset Completed*/
        for(i=0;i<100;i++)
        {
                mdelay(10);
                mii_mgr_read(31, 0x7800 ,&regValue);
                if(regValue != 0){
                        printf("MT7530 Reset Completed!!\n");
                        break;
                }
                if(i == 99)
                        printf("MT7530 Reset Timeout!!\n");
        }

	for(i=0;i<=4;i++) 
	{	
	       //turn off PHY
	       mii_mgr_read(i, 0x0 ,&regValue);
	       regValue |= (0x1<<11);
               mii_mgr_write(i, 0x0, regValue);
        }
        mii_mgr_write(31, 0x7000, 0x3); //reset switch
        udelay(100);

#if defined (MT7623_ASIC_BOARD)
        //sysRegWrite(RALINK_ETH_SW_BASE+0x100, 0x2105e33b);//(GE1, Force 1000M/FD, FC ON)
        RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x2105e33b;  //(GE1, Force 1000M/FD, FC ON)
        mii_mgr_write(31, 0x3600, 0x5e33b);
        mii_mgr_read(31, 0x3600 ,&regValue);
#endif
        //sysRegWrite(RALINK_ETH_SW_BASE+0x200, 0x00008000);//(GE2, Link down)
        RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x00008000;  //(GE2, Link down)

        //regValue = 0x117ccf; //Enable Port 6, P5 as GMAC5, P5 disable*/
        mii_mgr_read(31, 0x7804 ,&regValue);
#if defined (CONFIG_USE_GE1)
        regValue &= ~(1<<8); //Enable Port 6
        regValue |= (1<<6); //Disable Port 5
        regValue &= ~(1<<13); // Port5 connects to internal phy

#elif defined (CONFIG_USE_GE2)
        //RGMII2=Normal mode
        *(volatile u_long *)(RALINK_SYSCTL_BASE + 0x60) &= ~(0x1 << 15);

        //GMAC2= RGMII mode
        *(volatile u_long *)(SYSCFG1) &= ~(0x3 << 14);
        mii_mgr_write(31, 0x3500, 0x56300); //MT7530 P5 AN, we can ignore this setting??????
        //sysRegWrite(RALINK_ETH_SW_BASE+0x200, 0x21056300);//(GE2, auto-polling)
        RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x21056300; //(GE2, auto-polling)
        enable_auto_negotiate();//set polling address

        /* set MT7530 Port 5 to PHY 0/4 mode */
#if defined (GE_RGMII_INTERNAL_P0_AN)
        regValue &= ~((1<<13)|(1<<6));
        regValue |= ((1<<7)|(1<<16)|(1<<20));
#elif defined (GE_RGMII_INTERNAL_P4_AN)
        regValue &= ~((1<<13)|(1<<6)|(1<<20));
        regValue |= ((1<<7)|(1<<16));
#endif
#endif	/* CONFIG_USE_GE1 */

        /*Set MT7530 phy direct access mode**/
        regValue &= ~(1<<5);

        regValue |= (1<<16);//change HW-TRAP
        printf("change HW-TRAP to 0x%x\n",regValue);
        mii_mgr_write(31, 0x7804 ,regValue);

	mii_mgr_read(31, 0x7800, &regValue);
        regValue = (regValue >> 9) & 0x3;
        if(regValue == 0x3)//25Mhz Xtal
                xtal_mode = 1;
        else if(regValue == 0x2) //40Mhz
                xtal_mode = 2;
        else
                xtal_mode = 3;

        if(xtal_mode == 1) { //25Mhz Xtal
                /* do nothing */
        } else if(xtal_mode == 2) { //40Mhz
                mii_mgr_write(0, 13, 0x1f);  // disable MT7530 core clock
                mii_mgr_write(0, 14, 0x410);
                mii_mgr_write(0, 13, 0x401f);
                mii_mgr_write(0, 14, 0x0);

                mii_mgr_write(0, 13, 0x1f);  // disable MT7530 PLL
	    mii_mgr_write(0, 14, 0x40d);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x2020);

	    mii_mgr_write(0, 13, 0x1f);  // for MT7530 core clock = 500Mhz
	    mii_mgr_write(0, 14, 0x40e);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x119);

	    mii_mgr_write(0, 13, 0x1f);  // enable MT7530 PLL
	    mii_mgr_write(0, 14, 0x40d);
	    mii_mgr_write(0, 13, 0x401f);
	    mii_mgr_write(0, 14, 0x2820);

                udelay(20); //suggest by CD

                mii_mgr_write(0, 13, 0x1f);  // enable MT7530 core clock
                mii_mgr_write(0, 14, 0x410);
                mii_mgr_write(0, 13, 0x401f);
        }else {//20MHz
                /*TODO*/
        }

	 mii_mgr_write(0, 14, 0x1);  /*RGMII*/
	/* set MT7530 central align */
        mii_mgr_read(31, 0x7830, &regValue);
        regValue &= ~1;
        regValue |= 1<<1;
        mii_mgr_write(31, 0x7830, regValue);

        mii_mgr_read(31, 0x7a40, &regValue);
        regValue &= ~(1<<30);
        mii_mgr_write(31, 0x7a40, regValue);

        regValue = 0x855;
        mii_mgr_write(31, 0x7a78, regValue);

	
	mii_mgr_write(31, 0x7b00, 0x104);  //delay setting for 10/1000M
	mii_mgr_write(31, 0x7b04, 0x10);  //delay setting for 10/1000M

	/*Tx Driving*/
	mii_mgr_write(31, 0x7a54, 0x88);  //lower GE1 driving
	mii_mgr_write(31, 0x7a5c, 0x88);  //lower GE1 driving
	mii_mgr_write(31, 0x7a64, 0x88);  //lower GE1 driving
	mii_mgr_write(31, 0x7a6c, 0x88);  //lower GE1 driving
	mii_mgr_write(31, 0x7a74, 0x88);  //lower GE1 driving
	mii_mgr_write(31, 0x7a7c, 0x88);  //lower GE1 driving
	mii_mgr_write(31, 0x7810, 0x11);  //lower GE2 driving
	/*Set MT7623A/MT7623N TX Driving*/
	*(volatile u_long *)(0x1b110354) = 0x88;
	*(volatile u_long *)(0x1b11035c) = 0x88;
	*(volatile u_long *)(0x1b110364) = 0x88;
	*(volatile u_long *)(0x1b11036c) = 0x88;
	*(volatile u_long *)(0x1b110374) = 0x88;
	*(volatile u_long *)(0x1b11037c) = 0x88;
#if defined (GE_RGMII_AN)
	*(volatile u_long *)(0x10005f00) = 0xe00; //Set GE2 driving and slew rate
#else
	*(volatile u_long *)(0x10005f00) = 0xa00; //Set GE2 driving and slew rate
#endif
	*(volatile u_long *)(0x100054c0) = 0x5;   //set GE2 TDSEL
	*(volatile u_long *)(0x10005ed0) = 0;	  //set GE2 TUNE

	/* TRGMII Clock */
//      printf("Set TRGMII mode clock stage 1\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x404);
        mii_mgr_write(0, 13, 0x401f);
        if (xtal_mode == 1){ //25MHz
#if defined (CONFIG_GE1_TRGMII_FORCE_2900)
                mii_mgr_write(0, 14, 0x1d00); // 362.5MHz
#elif defined (CONFIG_GE1_TRGMII_FORCE_2600)
                mii_mgr_write(0, 14, 0x1a00); // 325MHz
#elif defined (CONFIG_GE1_TRGMII_FORCE_2000)
                mii_mgr_write(0, 14, 0x1400); //250MHz
#else
                mii_mgr_write(0, 14, 0x0a00); //125MHz
#endif
        }else if(xtal_mode == 2){//40MHz
#if defined (CONFIG_GE1_TRGMII_FORCE_2900)
                mii_mgr_write(0, 14, 0x1220); // 362.5MHz
#elif defined (CONFIG_GE1_TRGMII_FORCE_2600)
                mii_mgr_write(0, 14, 0x1040); // 325MHz
#elif defined (CONFIG_GE1_TRGMII_FORCE_2000)
                mii_mgr_write(0, 14, 0x0c80); //250MHz
#else
                mii_mgr_write(0, 14, 0x0640); //125MHz
#endif
        }
//      printf("Set TRGMII mode clock stage 2\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x405);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0x0);

//      printf("Set TRGMII mode clock stage 3\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x409);
        mii_mgr_write(0, 13, 0x401f);
	if(xtal_mode == 1)	/* 25MHz */
		mii_mgr_write(0, 14, 0x0057);
	else
		mii_mgr_write(0, 14, 0x0087);

//      printf("Set TRGMII mode clock stage 4\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x40a);
        mii_mgr_write(0, 13, 0x401f);
        if(xtal_mode == 1)	/* 25MHz */
		mii_mgr_write(0, 14, 0x0057);
	else
		mii_mgr_write(0, 14, 0x0087);

//      printf("Set TRGMII mode clock stage 5\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x403);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0x1800);

//      printf("Set TRGMII mode clock stage 6\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x403);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0x1c00);

//      printf("Set TRGMII mode clock stage 7\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x401);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0xc020);

//      printf("Set TRGMII mode clock stage 8\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x406);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0xa030);

//      printf("Set TRGMII mode clock stage 9\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x406);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0xa038);

        udelay(120); // for MT7623 bring up test

//      printf("Set TRGMII mode clock stage 10\n");
        mii_mgr_write(0, 13, 0x1f);
        mii_mgr_write(0, 14, 0x410);
        mii_mgr_write(0, 13, 0x401f);
        mii_mgr_write(0, 14, 0x3);

//      printf("Set TRGMII mode clock stage 11\n");

        mii_mgr_read(31, 0x7830 ,&regValue);
        regValue &=0xFFFFFFFC;
        regValue |=0x00000001;
        mii_mgr_write(31, 0x7830, regValue);

//      printf("Set TRGMII mode clock stage 12\n");
        mii_mgr_read(31, 0x7a40 ,&regValue);
        regValue &= ~(0x1<<30);
        regValue &= ~(0x1<<28);
        mii_mgr_write(31, 0x7a40, regValue);

        //mii_mgr_write(31, 0x7a78, 0x855);
        mii_mgr_write(31, 0x7a78, 0x55);
//      printf(" Adjust MT7530 TXC delay\n");
        udelay(100); // for mt7623 bring up test

	printf(" Release MT7623 RXC Reset\n");
        *(volatile u_long *)(0x1b110300) &= 0x7fffffff;   // Release MT7623 RXC reset
	for(i=0;i<=4;i++)
	{
	    mii_mgr_write(i, 13, 0x7);
	    mii_mgr_write(i, 14, 0x3C);
	    mii_mgr_write(i, 13, 0x4007);
	    mii_mgr_write(i, 14, 0x0);
	}

	//Disable EEE 10Base-Te:
	for(i=0;i<=4;i++)
	{
	    mii_mgr_write(i, 13, 0x1f);
	    mii_mgr_write(i, 14, 0x027b);
	    mii_mgr_write(i, 13, 0x401f);
	    mii_mgr_write(i, 14, 0x1177);
	}

	for(i=0;i<=4;i++)
        {
        	//turn on PHY
                mii_mgr_read(i, 0x0 ,&regValue);
                regValue &= ~(0x1<<11);
                mii_mgr_write(i, 0x0, regValue);
        }

        for(i=0;i<=4;i++) {
                mii_mgr_read(i, 4, &regValue);
                regValue |= (3<<7); //turn on 100Base-T Advertisement
                mii_mgr_write(i, 4, regValue);

                mii_mgr_read(i, 9, &regValue);
                regValue |= (3<<8); //turn on 1000Base-T Advertisement
                mii_mgr_write(i, 9, regValue);

                //restart AN
                mii_mgr_read(i, 0, &regValue);
                regValue |= (1 << 9);
                mii_mgr_write(i, 0, regValue);
        }

	mii_mgr_read(31, 0x7808 ,&regValue);
	regValue |= (3<<16); //Enable INTR
	mii_mgr_write(31, 0x7808 ,regValue);
}
#endif

static int rt2880_eth_setup(struct eth_device* dev)
{
	u32	i;
	u32	regValue;
	u16	wTmp;
	uchar	*temp;

	printf("\n Waitting for RX_DMA_BUSY status Start... ");
	while(1)
		if(!isDMABusy(dev))
			break;
	printf("done\n\n");

	// Case1: RT288x/RT3883/MT7621 GE1 + GigaPhy
#if defined (MAC_TO_GIGAPHY_MODE)
	enable_auto_negotiate();
	if (isMarvellGigaPHY(1)) {
#if defined (RT3883_FPGA_BOARD)
		printf("\n Reset MARVELL phy\n");
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 9, &regValue);
		regValue &= ~(3<<8); //turn off 1000Base-T Advertisement
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 9, regValue);

		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 20, &regValue);
		regValue |= 1<<7; //Add delay to RX_CLK for RXD Outputs
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 20, regValue);

		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &regValue);
		regValue |= 1<<15; //PHY Software Reset
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, regValue);
#elif defined (MT7621_FPGA_BOARD) || defined (MT7623_FPGA_BOARD) || defined (MT7623_ASIC_BOARD) ||\
      defined (MT7622_FPGA_BOARD) || defined (MT7622_ASIC_BOARD)
        printf("\n Reset MARVELL phy\n");
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 9, &regValue);
		regValue &= ~(3<<8); //turn off 1000Base-T Advertisement
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 9, regValue);
	    
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &regValue);
		regValue |= 1<<9; //restart AN
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, regValue);
#endif
	}
	if (isVtssGigaPHY(1)) {
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 1);
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 28, &regValue);
		printf("GE1 Vitesse Phy reg28 %x --> ",regValue);
		regValue |= (0x3<<12);
		regValue &= ~(0x3<<14);
		printf("%x (without reset PHY)\n", regValue);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 28, regValue);
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 31, 0);

		/*
		mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &regValue);
		regValue |= 1<<15; //PHY Software Reset
		mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, regValue);
		*/
	}
	
	/* Disable pause ability */
	printf("Disable pause ability\n");
	mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 4, &regValue);
	regValue &= ~(1<<10); //disable pause ability
	mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 4, regValue);

	mii_mgr_read(MAC_TO_GIGAPHY_MODE_ADDR, 0, &regValue);
	regValue |= 1<<9; //restart AN
	mii_mgr_write(MAC_TO_GIGAPHY_MODE_ADDR, 0, regValue);

#if defined (MT7621_FPGA_BOARD) || defined (MT7621_ASIC_BOARD) ||\
    defined (MT7623_FPGA_BOARD) || defined (MT7623_ASIC_BOARD) ||\
    defined (MT7622_FPGA_BOARD) || defined (MT7622_ASIC_BOARD)
#ifdef CONFIG_USE_GE1
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x20056300;//(P0, Auto mode)
	RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x00008000;//(P1, Down)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=RGMII Mode
	//RALINK_REG(0xbe000060) &= ~(1 << 14); //set RGMII1 to Normal mode    
#else
	RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x20056300;//(P1, Auto mode)
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x00008000;//(P0, Down)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 14); //GE2_MODE=RGMII Mode
	//RALINK_REG(0xbe000060) &= ~(1 << 15); //set RGMII2 to Normal mode
#endif
#endif
	// Case2. RT305x/RT335x/RT6856/MT7620 + EmbeddedSW
#elif defined (RT3052_ASIC_BOARD) || defined (RT3052_FPGA_BOARD) || \
      defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD) || \
      defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || \
      defined (RT6855_ASIC_BOARD) || defined (RT6855_FPGA_BOARD) || \
      defined (RT6855A_ASIC_BOARD) || defined (RT6855A_FPGA_BOARD) || \
      defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD) || \
      defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)

#ifdef P5_RGMII_TO_MAC_MODE
#if 0 // we don't have such demo board at this moment
	printf("\n Vitesse giga Mac support \n");
	ResetSWusingGPIOx();
	udelay(125000);
	vtss_init();
#endif
#endif

	// Case3: MT7621 + MT7530 GSW
#elif defined (MAC_TO_MT7530_MODE)
#if defined (MT7621_FPGA_BOARD) || defined (NT7621_ASIC_BOARD)
	//enable MDIO
	RALINK_REG(0xbe000060) &= ~(1 << 12); //set MDIO to Normal mode
	RALINK_REG(0xbe000060) &= ~(1 << 14); //set RGMII1 to Normal mode
	RALINK_REG(0xbe000060) &= ~(1 << 15); //set RGMII2 to Normal mode
#endif
	// Case4: RT288x/RT388x + Vitesse GigaSW
#elif defined (MAC_TO_VITESSE_MODE)
	printf("\n Vitesse giga Mac support \n");
	RALINK_REG(MDIO_CFG)=cpu_to_le32((u32)(0x1F01DC01));
	ResetSWusingGPIOx();
	udelay(125000);
	vtss_init();

	// Case5. RT288x/RT388x/MT7621 + (10/100 Switch or 100PHY)
#elif defined (MAC_TO_100SW_MODE) ||  defined (MAC_TO_100PHY_MODE)

#if defined (RT3883_FPGA_BOARD) || defined (RT3883_ASIC_BOARD)

	regValue = RALINK_REG(RT2880_SYSCFG1_REG);
	regValue &= ~(0xF << 12);

	/* 0=RGMII, 1=MII, 2=RvMii */
#if defined (RT3883_USE_GE2)
#if defined (GE_MII_FORCE_100) || defined (GE_MII_AN)
	regValue |= (0x1 << 14); // GE2 MII Mode
#elif defined (GE_RVMII_FORCE_100)
	regValue |= (0x2 << 14); // GE2 RvMII Mode
#endif

#else //GE1
#if defined (GE_MII_FORCE_100) || defined (GE_MII_AN)
	regValue |= (0x1 << 12); // GE1 MII Mode
#elif defined (GE_RVMII_FORCE_100)
	regValue |= (0x2 << 12); // GE1 RvMII Mode
#endif

#endif // RT3883_USE_GE2 //

	RALINK_REG(RT2880_SYSCFG1_REG)=regValue;

#elif defined (MT7621_FPGA_BOARD) || defined (MT7621_ASIC_BOARD)
#ifdef CONFIG_USE_GE1
#if defined (GE_MII_FORCE_100) || defined (GE_RVMII_FORCE_100)
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x2005e337;//(P0, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 12);
#elif defined (GE_MII_AN)
	RALINK_REG(RALINK_ETH_SW_BASE+0x100) = 0x20056300;//(P0, Auto mode)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 12); //GE1_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 12);
	enable_auto_negotiate();
#endif
#else //CONFIG_USE_GE2
#if defined (GE_MII_FORCE_100) || defined (GE_RVMII_FORCE_100)
	RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x2005e337;//(P0, Force mode, Link Up, 100Mbps, Full-Duplex, FC ON)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 14); //GE2_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 14);
#elif defined (GE_MII_AN)
	RALINK_REG(RALINK_ETH_SW_BASE+0x200) = 0x20056300;//(P0, Auto mode)
	RALINK_REG(RT2880_SYSCFG1_REG) &= ~(0x3 << 14); //GE2_MODE=Mii Mode
	RALINK_REG(RT2880_SYSCFG1_REG) |= (0x1 << 14);
	enable_auto_negotiate();
#endif
#endif

#else // RT288x
	// due to the flaws of RT2880 GMAC implementation (or IC+ SW ?) we use the
	// fixed capability instead of auto-polling.
	RALINK_REG(MDIO_CFG)=cpu_to_le32((u32)(0x1F01BC01));

	//force cpu port is 100F
	mii_mgr_write(29, 22, 0x8420);
#endif
#endif // MAC_TO_GIGAPHY_MODE //
	

#if defined (RT3883_USE_GE2) || defined (CONFIG_USE_GE2)
	wTmp = (u16)dev->enetaddr[0];
	regValue = (wTmp << 8) | dev->enetaddr[1];
	RALINK_REG(GDMA2_MAC_ADRH)=regValue;

	wTmp = (u16)dev->enetaddr[2];
	regValue = (wTmp << 8) | dev->enetaddr[3];
	regValue = regValue << 16;
	wTmp = (u16)dev->enetaddr[4];
	regValue |= (wTmp<<8) | dev->enetaddr[5];
	RALINK_REG(GDMA2_MAC_ADRL)=regValue;

	regValue = RALINK_REG(GDMA2_FWD_CFG);

	regValue = regValue & GDM_UFRC_P_CPU;
	//Broad-cast MAC address frames forward to CPU
	regValue = regValue & GDM_BFRC_P_CPU;
	//Multi-cast MAC address frames forward to CPU
	regValue = regValue & GDM_MFRC_P_CPU;
	//Other MAC address frames forward to CPU
	regValue = regValue & GDM_OFRC_P_CPU;

	RALINK_REG(GDMA2_FWD_CFG)=regValue;
	udelay(500);
	regValue = RALINK_REG(GDMA2_FWD_CFG);
#else // non RT3883_USE_GE2 //
	/* Set MAC address. */
	wTmp = (u16)dev->enetaddr[0];
	regValue = (wTmp << 8) | dev->enetaddr[1];

#if defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)
	RALINK_REG(SDM_MAC_ADRH)=regValue;
	// printf("\n dev->iobase=%08X,SDM_MAC_ADRH=%08X\n",dev->iobase,regValue);
#else
	RALINK_REG(GDMA1_MAC_ADRH)=regValue;
	// printf("\n dev->iobase=%08X,GDMA1_MAC_ADRH=%08X\n ",dev->iobase, regValue);
#endif

	wTmp = (u16)dev->enetaddr[2];
	regValue = (wTmp << 8) | dev->enetaddr[3];
	regValue = regValue << 16;
	wTmp = (u16)dev->enetaddr[4];
	regValue |= (wTmp<<8) | dev->enetaddr[5];
#if defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)
	RALINK_REG(SDM_MAC_ADRL)=regValue;
	// printf("\n dev->iobase=%08X,SDM_MAC_ADRL=%08X\n",dev->iobase,regValue);
#else
	RALINK_REG(GDMA1_MAC_ADRL)=regValue;
	// printf("\n dev->iobase=%08X,GDMA1_MAC_ADRL=%08X\n ",dev->iobase, regValue);
#endif

	//printf("\n rt2880_eth_init,set MAC reg to [%02X:%02X:%02X:%02X:%02X:%02X]\n",
	//	dev->enetaddr[0],dev->enetaddr[1],dev->enetaddr[2],
	//	dev->enetaddr[3],dev->enetaddr[4],dev->enetaddr[5]);

#if ! defined (RT5350_ASIC_BOARD) && ! defined (RT5350_FPGA_BOARD) && !defined (MT7628_ASIC_BOARD) && !defined (MT7628_FPGA_BOARD)
	regValue = RALINK_REG(GDMA1_FWD_CFG);
	//printf("\n old,GDMA1_FWD_CFG = %08X \n",regValue);

#if (defined (MT7620_FPGA_BOARD) || defined (MT7620_ASIC_BOARD))
	//frames destination port = port 0 CPU
	regValue = regValue & ~(0x7);
#else
	//Uni-cast frames forward to CPU
	regValue = regValue & GDM_UFRC_P_CPU;
	//Broad-cast MAC address frames forward to CPU
	regValue = regValue & GDM_BFRC_P_CPU;
	//Multi-cast MAC address frames forward to CPU
	regValue = regValue & GDM_MFRC_P_CPU;
	//Other MAC address frames forward to CPU
	regValue = regValue & GDM_OFRC_P_CPU;
#endif

	RALINK_REG(GDMA1_FWD_CFG)=regValue;
	udelay(500);
	regValue = RALINK_REG(GDMA1_FWD_CFG);

#if (defined (MT7620_FPGA_BOARD) || defined (MT7620_ASIC_BOARD))	
	regValue = 0x80504000;
	RALINK_REG(PSE_FQFC_CFG)=regValue;
#endif    
#endif // RT3883_USE_GE2 //

#endif

	for (i = 0; i < NUM_RX_DESC; i++) {
		temp = memset((void *)&rx_ring[i],0,16);
		rx_ring[i].rxd_info2.DDONE_bit = 0;

		{
			BUFFER_ELEM *buf;
			buf = rt2880_free_buf_entry_dequeue(&rt2880_free_buf_list);
			NetRxPackets[i] = buf->pbuf;
#if defined (RX_SCATTER_GATTER_DMA)
			rx_ring[i].rxd_info2.LS0= 0;
			rx_ring[i].rxd_info2.PLEN0= PKTSIZE_ALIGN;
#else
			rx_ring[i].rxd_info2.LS0= 1;
#endif
			rx_ring[i].rxd_info1.PDP0 = cpu_to_le32(phys_to_bus((u32) NetRxPackets[i]));
		}
	}

	for (i=0; i < NUM_TX_DESC; i++) {
		temp = memset((void *)&tx_ring0[i],0,16);
		tx_ring0[i].txd_info2.LS0_bit = 1;
		tx_ring0[i].txd_info2.DDONE_bit = 1;
		/* PN:
		 *  0:CPU
		 *  1:GE1
		 *  2:GE2 (for RT2883)
		 *  6:PPE
		 *  7:Discard
		 */
#if defined (RT3883_USE_GE2)
		tx_ring0[i].txd_info4.PN = 2;
#elif defined (CONFIG_USE_GE2)
		tx_ring0[i].txd_info4.FPORT=2;
#elif defined (CONFIG_USE_GE1)
		tx_ring0[i].txd_info4.FPORT=1;
#elif defined (MT7620_ASIC_BOARD) || defined (MT7620_FPGA_BOARD)
		tx_ring0[i].txd_info4.FP_BMAP=0x0;
#else
		tx_ring0[i].txd_info4.PN = 1;
#endif

	}
	rxRingSize = NUM_RX_DESC;
	txRingSize = NUM_TX_DESC;

	rx_dma_owner_idx0 = 0;
	rx_wants_alloc_idx0 = (NUM_RX_DESC - 1);
	tx_cpu_owner_idx0 = 0;

	regValue=RALINK_REG(PDMA_GLO_CFG);
	udelay(100);

	{
		regValue &= 0x0000FFFF;

		RALINK_REG(PDMA_GLO_CFG)=regValue;
		udelay(500);
		regValue=RALINK_REG(PDMA_GLO_CFG);
	}

	/* Tell the adapter where the TX/RX rings are located. */
	RALINK_REG(RX_BASE_PTR0)=phys_to_bus((u32) &rx_ring[0]);

	//printf("\n rx_ring=%08X ,RX_BASE_PTR0 = %08X \n",&rx_ring[0],RALINK_REG(RX_BASE_PTR0));
	RALINK_REG(TX_BASE_PTR0)=phys_to_bus((u32) &tx_ring0[0]);

	//printf("\n tx_ring0=%08X, TX_BASE_PTR0 = %08X \n",&tx_ring0[0],RALINK_REG(TX_BASE_PTR0));

	RALINK_REG(RX_MAX_CNT0)=cpu_to_le32((u32) NUM_RX_DESC);
	RALINK_REG(TX_MAX_CNT0)=cpu_to_le32((u32) NUM_TX_DESC);

	RALINK_REG(TX_CTX_IDX0)=cpu_to_le32((u32) tx_cpu_owner_idx0);
	RALINK_REG(PDMA_RST_IDX)=cpu_to_le32((u32)RST_DTX_IDX0);

	RALINK_REG(RX_CALC_IDX0)=cpu_to_le32((u32) (NUM_RX_DESC - 1));
	RALINK_REG(PDMA_RST_IDX)=cpu_to_le32((u32)RST_DRX_IDX0);

	udelay(500);
	START_ETH(dev);
	
	return 1;
}


static int rt2880_eth_send(struct eth_device* dev, volatile void *packet, int length)
{
	int		status = -1;
	int		i;
	int		retry_count = 0, temp;
#if defined (RT3052_FPGA_BOARD) || defined (RT3052_ASIC_BOARD) || \
    defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD) || \
    defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || \
    defined (RT3883_ASIC_BOARD) || defined (RT3883_FPGA_BOARD) || \
    defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)
	char *p=(char *)packet;
#endif

	//printf("Enter rt2880_eth_send()\n");

Retry:
	if (retry_count > 10) {
		return (status);
	}

	if (length <= 0) {
		printf("%s: bad packet size: %d\n", dev->name, length);
		return (status);
	}

#if defined (RT3052_FPGA_BOARD) || defined (RT3052_ASIC_BOARD) || \
    defined (RT3352_ASIC_BOARD) || defined (RT3352_FPGA_BOARD) || \
    defined (RT5350_ASIC_BOARD) || defined (RT5350_FPGA_BOARD) || \
    defined (RT3883_ASIC_BOARD) || defined (RT3883_FPGA_BOARD) || \
    defined (MT7628_ASIC_BOARD) || defined (MT7628_FPGA_BOARD)
#define PADDING_LENGTH 60
	if (length < PADDING_LENGTH) {
		//	print_packet(packet,length);
		for(i=0;i<PADDING_LENGTH-length;i++) {
			p[length+i]=0;
		}
		length = PADDING_LENGTH;
	}
#endif //RT3052

    for(i = 0; tx_ring0[tx_cpu_owner_idx0].txd_info2.DDONE_bit == 0 ; i++)

    {
    	if (i >= TOUT_LOOP) {
    		goto Done;
    	}
    }
	//dump_reg();

	temp = RALINK_REG(TX_DTX_IDX0);

	if(temp == (tx_cpu_owner_idx0+1) % NUM_TX_DESC) {
		puts(" @ ");
		goto Done;
	}

	tx_ring0[tx_cpu_owner_idx0].txd_info1.SDP0 = cpu_to_le32(phys_to_bus((u32) packet));
	tx_ring0[tx_cpu_owner_idx0].txd_info2.SDL0 = length;

#if 0
	printf("==========TX==========(CTX=%d)\n",tx_cpu_owner_idx0);
	printf("tx_ring0[tx_cpu_owner_idx0].txd_info1 =%x\n",tx_ring0[tx_cpu_owner_idx0].txd_info1);
	printf("tx_ring0[tx_cpu_owner_idx0].txd_info2 =%x\n",tx_ring0[tx_cpu_owner_idx0].txd_info2);
	printf("tx_ring0[tx_cpu_owner_idx0].txd_info3 =%x\n",tx_ring0[tx_cpu_owner_idx0].txd_info3);
	printf("tx_ring0[tx_cpu_owner_idx0].txd_info4 =%x\n",tx_ring0[tx_cpu_owner_idx0].txd_info4);
#endif

	tx_ring0[tx_cpu_owner_idx0].txd_info2.DDONE_bit = 0;
	status = length;

	tx_cpu_owner_idx0 = (tx_cpu_owner_idx0+1) % NUM_TX_DESC;
	RALINK_REG(TX_CTX_IDX0)=cpu_to_le32((u32) tx_cpu_owner_idx0);

	return status;
Done:
	udelay(500);
	retry_count++;
	goto Retry;
}


static int rt2880_eth_recv(struct eth_device* dev)
{
	int length = 0,hdr_len=0,bb=0;
	int inter_loopback_cnt =0;
	u32 *rxd_info;
#if !defined (RT3883_FPGA_BOARD) && !defined (RT3883_ASIC_BOARD)
	u8 temp_mac[6];
#endif

	for (; ; ) {
		//rxd_info = (u32 *)KSEG1ADDR(&rx_ring[rx_dma_owner_idx0].rxd_info2);
		rxd_info = (u32 *)(&rx_ring[rx_dma_owner_idx0].rxd_info2);

		if ( (*rxd_info & BIT(31)) == 0 )
		{
			hdr_len =0;
			break;
		}

		udelay(1);
			length = rx_ring[rx_dma_owner_idx0].rxd_info2.PLEN0;

		if(length == 0)
		{
			printf("\n Warring!! Packet Length has error !!,In normal mode !\n");
		}

#if defined (PDMA_NEW)
		if(rx_ring[rx_dma_owner_idx0].rxd_info4.SP == 6)
#else
		if(rx_ring[rx_dma_owner_idx0].rxd_info4.SP == 0)
#endif
		{// Packet received from CPU port
			printf("\n Normal Mode,Packet received from CPU port,plen=%d \n",length);
			//print_packet((void *)KSEG1ADDR(NetRxPackets[rx_dma_owner_idx0]),length);
			inter_loopback_cnt++;
			length = inter_loopback_cnt;//for return
		}
		else {
			//NetReceive((void *)KSEG1ADDR(NetRxPackets[rx_dma_owner_idx0]), length );
			NetReceive((void *)(NetRxPackets[rx_dma_owner_idx0]), length );
		}

#if  defined (RX_SCATTER_GATTER_DMA)
		rx_ring[rx_dma_owner_idx0].rxd_info2.DDONE_bit = 0;
		rx_ring[rx_dma_owner_idx0].rxd_info2.LS0= 0;
		rx_ring[rx_dma_owner_idx0].rxd_info2.PLEN0= PKTSIZE_ALIGN;
#else
		rxd_info = (u32 *)&rx_ring[rx_dma_owner_idx0].rxd_info4;
		*rxd_info = 0;

		rxd_info = (u32 *)&rx_ring[rx_dma_owner_idx0].rxd_info2;
		*rxd_info = 0;
		rx_ring[rx_dma_owner_idx0].rxd_info2.LS0= 1;
#endif

#if 0
		printf("=====RX=======(CALC=%d LEN=%d)\n",rx_dma_owner_idx0, length);
		printf("rx_ring[rx_dma_owner_idx0].rxd_info1 =%x\n",rx_ring[rx_dma_owner_idx0].rxd_info1);
		printf("rx_ring[rx_dma_owner_idx0].rxd_info2 =%x\n",rx_ring[rx_dma_owner_idx0].rxd_info2);
		printf("rx_ring[rx_dma_owner_idx0].rxd_info3 =%x\n",rx_ring[rx_dma_owner_idx0].rxd_info3);
		printf("rx_ring[rx_dma_owner_idx0].rxd_info4 =%x\n",rx_ring[rx_dma_owner_idx0].rxd_info4);
#endif
		/* Tell the adapter where the TX/RX rings are located. */
		RALINK_REG(RX_BASE_PTR0)=phys_to_bus((u32) &rx_ring[0]);

		//udelay(10000);
		/*  Move point to next RXD which wants to alloc*/
		RALINK_REG(RX_CALC_IDX0)=cpu_to_le32((u32) rx_dma_owner_idx0);

		/* Update to Next packet point that was received.
		 */
		rx_dma_owner_idx0 = (rx_dma_owner_idx0 + 1) % NUM_RX_DESC;

		//printf("\n ************************************************* \n");
		//printf("\n RX_CALC_IDX0=%d \n", RALINK_REG(RX_CALC_IDX0));
		//printf("\n RX_DRX_IDX0 = %d \n",RALINK_REG(RX_DRX_IDX0));
		//printf("\n ************************************************* \n");
	}
	return length;
}

void rt2880_eth_halt(struct eth_device* dev)
{
	 STOP_ETH(dev);
	//gmac_phy_switch_gear(DISABLE);
	//printf(" STOP_ETH \n");
	//dump_reg();
}

#if 0
static void print_packet( u8 * buf, int length )
{

	int i;
	int remainder;
	int lines;


	printf("Packet of length %d \n", length );


	lines = length / 16;
	remainder = length % 16;

	for ( i = 0; i < lines ; i ++ ) {
		int cur;

		for ( cur = 0; cur < 8; cur ++ ) {
			u8 a, b;

			a = *(buf ++ );
			b = *(buf ++ );
			printf("%02X %02X ", a, b );
		}
		printf("\n");
	}
	for ( i = 0; i < remainder/2 ; i++ ) {
		u8 a, b;

		a = *(buf ++ );
		b = *(buf ++ );
		printf("%02X %02X ", a, b );
	}
	printf("\n");

}
#endif

#ifdef RT2880_U_BOOT_CMD_OPEN 
#if defined (RT3883_FPGA_BOARD) || defined (RT3883_ASIC_BOARD)
void rt3883_init_gdma(int mode)
{
	u32 reg;
	u16 tmp;
	//mode 0: all pkts to cpu,
	if (mode == 0) {
		reg = RALINK_REG(GDMA1_FWD_CFG);
		reg &= (GDM_UFRC_P_CPU & GDM_BFRC_P_CPU & GDM_MFRC_P_CPU & GDM_OFRC_P_CPU);
		RALINK_REG(GDMA1_FWD_CFG)=cpu_to_le32((u32)reg);

		reg = RALINK_REG(GDMA2_FWD_CFG);
		reg &= (GDM_UFRC_P_CPU & GDM_BFRC_P_CPU & GDM_MFRC_P_CPU & GDM_OFRC_P_CPU);
		RALINK_REG(GDMA2_FWD_CFG)=cpu_to_le32((u32)reg);
	}
	//mode 1: ge1->ge2, ge2->ge1
	else if (mode == 1) {
		reg = RALINK_REG(GDMA1_FWD_CFG);
		reg &= (GDM_UFRC_P_CPU & GDM_BFRC_P_CPU & GDM_MFRC_P_CPU & GDM_OFRC_P_CPU);
		reg |= (GDM_UFRC_P_GDMA2 | GDM_BFRC_P_GDMA2 | GDM_MFRC_P_GDMA2 | GDM_OFRC_P_GDMA2);
		RALINK_REG(GDMA1_FWD_CFG)=cpu_to_le32((u32)reg);

		reg = RALINK_REG(GDMA2_FWD_CFG);
		reg &= (GDM_UFRC_P_CPU & GDM_BFRC_P_CPU & GDM_MFRC_P_CPU & GDM_OFRC_P_CPU);
		reg |= (GDM_UFRC_P_GDMA1 | GDM_BFRC_P_GDMA1 | GDM_MFRC_P_GDMA1 | GDM_OFRC_P_GDMA1);
		RALINK_REG(GDMA2_FWD_CFG)=cpu_to_le32((u32)reg);
	}

	//also set GDMA my MAC
	tmp = (u16)rt2880_pdev->enetaddr[0];
	reg = (tmp << 8) | rt2880_pdev->enetaddr[1];
	RALINK_REG(GDMA1_MAC_ADRH)=reg;

	tmp = (u16)rt2880_pdev->enetaddr[2];
	reg = (tmp << 8) | rt2880_pdev->enetaddr[3];
	reg = reg << 16;
	tmp = (u16)rt2880_pdev->enetaddr[4];
	//reg |= (tmp<<8) | rt2880_pdev->enetaddr[5];
	reg |= (tmp<<8) | 1;
	RALINK_REG(GDMA1_MAC_ADRL)=reg;

	tmp = (u16)rt2880_pdev->enetaddr[0];
	reg = (tmp << 8) | rt2880_pdev->enetaddr[1];
	RALINK_REG(GDMA2_MAC_ADRH)=reg;

	tmp = (u16)rt2880_pdev->enetaddr[2];
	reg = (tmp << 8) | rt2880_pdev->enetaddr[3];
	reg = reg << 16;
	tmp = (u16)rt2880_pdev->enetaddr[4];
	//reg |= (tmp<<8) | rt2880_pdev->enetaddr[5];
	reg |= (tmp<<8) | 2;
	RALINK_REG(GDMA2_MAC_ADRL)=reg;

	//enable auto polling for both GE1 and GE2
	reg = RALINK_REG(MDIO_CFG); 
	reg |= 0x20000000;
	RALINK_REG(MDIO_CFG)=reg;

#define MDIO_CFG2           RALINK_FRAME_ENGINE_BASE + 0x18
	reg = RALINK_REG(MDIO_CFG2);
	reg |= 0x20000000;
	RALINK_REG(MDIO_CFG2)=reg;
}

void rt3883_reset_phy(void)
{
	//Marvell phy: adj skew and reset both phy connected to ge1 and ge2
	mii_mgr_write(31, 20, 0x0ce0);
#ifdef RT3883_FPGA_BOARD
	mii_mgr_write(31, 9, 0);
#endif
	mii_mgr_write(31, 0, 0x9140);
	mii_mgr_write(30, 20, 0x0ce0);
#ifdef RT3883_FPGA_BOARD
	mii_mgr_write(30, 9, 0);
#endif
	mii_mgr_write(30, 0, 0x9140);
}

int do_rt3883_pseloopback(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("RT3883 PSE loopback mode!\n");
	rt3883_init_gdma(1);
	rt3883_reset_phy();
	return 0;
}

U_BOOT_CMD(
	pseloop,	1,	1,	do_rt3883_pseloopback,
	"pseloop   - RT3883 PSE loopback test\n",
	"pseloop   - RT3883 PSE loopback test\n"
);
#endif

#endif

static char erase_seq[] = "\b \b";              /* erase sequence       */
static char   tab_seq[] = "        ";           /* used to expand TABs  */

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
	char *s;

	if (*np == 0) {
		return (p);
	}

	if (*(--p) == '\t') {			/* will retype the whole line	*/
		while (*colp > plen) {
			puts (erase_seq);
			(*colp)--;
		}
		for (s=buffer; s<p; ++s) {
			if (*s == '\t') {
				puts (tab_seq+((*colp) & 07));
				*colp += 8 - ((*colp) & 07);
			} else {
				++(*colp);
				putc (*s);
			}
		}
	} else {
		puts (erase_seq);
		(*colp)--;
	}
	(*np)--;
	return (p);
}

#ifdef RALINK_SWITCH_DEBUG_FUN
#define RALINK_VLAN_ID_BASE	(RALINK_ETH_SW_BASE + 0x50)
#define RALINK_VLAN_MEMB_BASE	(RALINK_ETH_SW_BASE + 0x70)

#define RALINK_TABLE_SEARCH	(RALINK_ETH_SW_BASE + 0x24)
#define RALINK_TABLE_STATUS0	(RALINK_ETH_SW_BASE + 0x28)
#define RALINK_TABLE_STATUS1	(RALINK_ETH_SW_BASE + 0x2c)
#define RALINK_TABLE_STATUS2	(RALINK_ETH_SW_BASE + 0x30)
#define RALINK_WT_MAC_AD0	(RALINK_ETH_SW_BASE + 0x34)
#define RALINK_WT_MAC_AD1	(RALINK_ETH_SW_BASE + 0x38)
#define RALINK_WT_MAC_AD2	(RALINK_ETH_SW_BASE + 0x3C)
#define RALINK_WT_MAC_AD2	(RALINK_ETH_SW_BASE + 0x3C)

void table_dump(void)
{
	int i, j, value, mac;
	int vid[16];

	for (i = 0; i < 8; i++) {
		value = RALINK_REG(RALINK_VLAN_ID_BASE + 4*i);
		vid[2 * i] = value & 0xfff;
		vid[2 * i + 1] = (value & 0xfff000) >> 12;
	}

	RALINK_REG(RALINK_TABLE_SEARCH) = 0x1;
	printf("hash  port(0:6)  vidx  vid  age   mac-address  filt\n");
	for (i = 0; i < 0x400; i++) {
		while(1) {
			value = RALINK_REG(RALINK_TABLE_STATUS0);
			if (value & 0x1) { //search_rdy
				if ((value & 0x70) == 0) {
					printf("found an unused entry (age = 3'b000), please check!\n");
					return;
				}
				printf("%03x:   ", (value >> 22) & 0x3ff); //hash_addr_lu
				j = (value >> 12) & 0x7f; //r_port_map
				printf("%c", (j & 0x01)? '1':'-');
				printf("%c", (j & 0x02)? '1':'-');
				printf("%c", (j & 0x04)? '1':'-');
				printf("%c", (j & 0x08)? '1':'-');
				printf("%c ", (j & 0x10)? '1':'-');
				printf("%c", (j & 0x20)? '1':'-');
				printf("%c", (j & 0x40)? '1':'-');
				printf("   %2d", (value >> 7) & 0xf); //r_vid
				printf("  %4d", vid[(value >> 7) & 0xf]);
				printf("    %1d", (value >> 4) & 0x7); //r_age_field
				mac = RALINK_REG(RALINK_TABLE_STATUS2);
				printf("  %08x", mac);
				mac = RALINK_REG(RALINK_TABLE_STATUS1);
				printf("%04x", (mac & 0xffff));
				printf("     %c\n", (value & 0x8)? 'y':'-');
				if (value & 0x2) {
					printf("end of table %d\n", i);
					return;
				}
				break;
			}
			else if (value & 0x2) { //at_table_end
				printf("found the last entry %d (not ready)\n", i);
				return;
			}
			udelay(5000);
		}
		RALINK_REG(RALINK_TABLE_SEARCH) = 0x2; //search for next address
	}
}

void table_add(int argc, char *argv[])
{
	int i, j, value, is_filter;
	char tmpstr[9];

	is_filter = (argv[1][0] == 'f')? 1 : 0;
	if (!argv[2] || strlen(argv[2]) != 12) {
		printf("MAC address format error, should be of length 12\n");
		return;
	}
	strncpy(tmpstr, argv[2], 8);
	tmpstr[8] = '\0';
	value = simple_strtoul(tmpstr, NULL, 16);
	RALINK_REG(RALINK_WT_MAC_AD2) = value;

	strncpy(tmpstr, argv[2]+8, 4);
	tmpstr[4] = '\0';
	value = simple_strtoul(tmpstr, NULL, 16);
	RALINK_REG(RALINK_WT_MAC_AD1) = value;

	if (!argv[3] || strlen(argv[3]) != 7) {
		if (is_filter)
			argv[3] = "1111111";
		else {
			printf("portmap format error, should be of length 7\n");
			return;
		}
	}
	j = 0;
	for (i = 0; i < 7; i++) {
		if (argv[3][i] != '0' && argv[3][i] != '1') {
			printf("portmap format error, should be of combination of 0 or 1\n");
			return;
		}
		j += (argv[3][i] - '0') * (1 << i);
	}
	value = j << 12; //w_port_map

	if (argc > 4) {
		j = simple_strtoul(argv[4], NULL, 0);
		if (j < 0 || 15 < j) {
			printf("wrong member index range, should be within 0~15\n");
			return;
		}
		value += (j << 7); //w_index
	}

	if (argc > 5) {
		j = simple_strtoul(argv[5], NULL, 0);
		if (j < 1 || 7 < j) {
			printf("wrong age range, should be within 1~7\n");
			return;
		}
		value += (j << 4); //w_age_field
	}
	else
		value += (7 << 4); //w_age_field

	if (is_filter)
		value |= (1 << 3); //sa_filter

	value += 1; //w_mac_cmd
	RALINK_REG(RALINK_WT_MAC_AD0) = value;

	for (i = 0; i < 20; i++) {
		value = RALINK_REG(RALINK_WT_MAC_AD0);
		if (value & 0x2) { //w_mac_done
			printf("done.\n");
			return;
		}
		udelay(1000);
	}
	if (i == 20)
		printf("timeout.\n");
}

void table_del(int argc, char *argv[])
{
	int i, j, value;
	char tmpstr[9];

	if (!argv[2] || strlen(argv[2]) != 12) {
		printf("MAC address format error, should be of length 12\n");
		return;
	}
	strncpy(tmpstr, argv[2], 8);
	tmpstr[8] = '\0';
	value = simple_strtoul(tmpstr, NULL, 16);
	RALINK_REG(RALINK_WT_MAC_AD2) = value;

	strncpy(tmpstr, argv[2]+8, 4);
	tmpstr[4] = '\0';
	value = simple_strtoul(tmpstr, NULL, 16);
	RALINK_REG(RALINK_WT_MAC_AD1) = value;

	value = 0;
	if (argc > 3) {
		j = simple_strtoul(argv[3], NULL, 0);
		if (j < 0 || 15 < j) {
			printf("wrong member index range, should be within 0~15\n");
			return;
		}
		value += (j << 7); //w_index
	}

	value += 1; //w_mac_cmd
	RALINK_REG(RALINK_WT_MAC_AD0) = value;

	for (i = 0; i < 20; i++) {
		value = RALINK_REG(RALINK_WT_MAC_AD0);
		if (value & 0x2) { //w_mac_done
			if (argv[1] != NULL)
				printf("done.\n");
			return;
		}
		udelay(1000);
	}
	if (i == 20)
		printf("timeout.\n");
}

void table_clear(void)
{
	int i, value, mac;
	char v[2][13];
	char *argv[4];

	memset(argv, 0, sizeof(v));
	memset(argv, 0, sizeof(argv));

	RALINK_REG(RALINK_TABLE_SEARCH) = 0x1;
	for (i = 0; i < 0x400; i++) {
		while(1) {
			value = RALINK_REG(RALINK_TABLE_STATUS0);
			if (value & 0x1) { //search_rdy
				if ((value & 0x70) == 0) {
					return;
				}
				sprintf(v[1], "%d", (value >> 7) & 0xf);
				mac = RALINK_REG(RALINK_TABLE_STATUS2);
				sprintf(v[0], "%08x", mac);
				mac = RALINK_REG(RALINK_TABLE_STATUS1);
				sprintf(v[0]+8, "%04x", (mac & 0xffff));
				argv[2] = v[0];
				argv[3] = v[1];
				table_del(4, argv);
				if (value & 0x2) {
					return;
				}
				break;
			}
			else if (value & 0x2) { //at_table_end
				return;
			}
			udelay(5000);
		}
		RALINK_REG(RALINK_TABLE_SEARCH) = 0x2; //search for next address
	}
}

void vlan_dump(void)
{
	int i, vid, value;

	printf("idx   vid  portmap\n");
	for (i = 0; i < 8; i++) {
		vid = RALINK_REG(RALINK_VLAN_ID_BASE + 4*i);
		value = RALINK_REG(RALINK_VLAN_MEMB_BASE + 4*(i/2));
		printf(" %2d  %4d  ", 2*i, vid & 0xfff);
		if (i%2 == 0) {
			printf("%c", (value & 0x00000001)? '1':'-');
			printf("%c", (value & 0x00000002)? '1':'-');
			printf("%c", (value & 0x00000004)? '1':'-');
			printf("%c", (value & 0x00000008)? '1':'-');
			printf("%c", (value & 0x00000010)? '1':'-');
			printf("%c", (value & 0x00000020)? '1':'-');
			printf("%c\n", (value & 0x00000040)? '1':'-');
		}
		else {
			printf("%c", (value & 0x00010000)? '1':'-');
			printf("%c", (value & 0x00020000)? '1':'-');
			printf("%c", (value & 0x00040000)? '1':'-');
			printf("%c", (value & 0x00080000)? '1':'-');
			printf("%c", (value & 0x00100000)? '1':'-');
			printf("%c", (value & 0x00200000)? '1':'-');
			printf("%c\n", (value & 0x00400000)? '1':'-');
		}
		printf(" %2d  %4d  ", 2*i+1, ((vid & 0xfff000) >> 12));
		if (i%2 == 0) {
			printf("%c", (value & 0x00000100)? '1':'-');
			printf("%c", (value & 0x00000200)? '1':'-');
			printf("%c", (value & 0x00000400)? '1':'-');
			printf("%c", (value & 0x00000800)? '1':'-');
			printf("%c", (value & 0x00001000)? '1':'-');
			printf("%c", (value & 0x00002000)? '1':'-');
			printf("%c\n", (value & 0x00004000)? '1':'-');
		}
		else {
			printf("%c", (value & 0x01000000)? '1':'-');
			printf("%c", (value & 0x02000000)? '1':'-');
			printf("%c", (value & 0x04000000)? '1':'-');
			printf("%c", (value & 0x08000000)? '1':'-');
			printf("%c", (value & 0x10000000)? '1':'-');
			printf("%c", (value & 0x20000000)? '1':'-');
			printf("%c\n", (value & 0x40000000)? '1':'-');
		}
	}
}

void vlan_set(int argc, char *argv[])
{
	int i, j, value;
	int idx, vid;

	if (argc != 6) {
		printf("insufficient arguments!\n");
		return;
	}
	idx = simple_strtoul(argv[3], NULL, 0);
	if (idx < 0 || 15 < idx) {
		printf("wrong member index range, should be within 0~15\n");
		return;
	}
	vid = simple_strtoul(argv[4], NULL, 0);
	if (vid < 0 || 0xfff < vid) {
		printf("wrong vlan id range, should be within 0~4095\n");
		return;
	}
	if (strlen(argv[5]) != 7) {
		printf("portmap format error, should be of length 7\n");
		return;
	}
	j = 0;
	for (i = 0; i < 7; i++) {
		if (argv[5][i] != '0' && argv[5][i] != '1') {
			printf("portmap format error, should be of combination of 0 or 1\n");
			return;
		}
		j += (argv[5][i] - '0') * (1 << i);
	}

	//set vlan identifier
	value = RALINK_REG(RALINK_VLAN_ID_BASE + 4*(idx/2));
	if (idx % 2 == 0) {
		value &= 0xfff000;
		value |= vid;
	}
	else {
		value &= 0xfff;
		value |= (vid << 12);
	}
	RALINK_REG(RALINK_VLAN_ID_BASE + 4*(idx/2)) = value;

	//set vlan member
	value = RALINK_REG(RALINK_VLAN_MEMB_BASE + 4*(idx/4));
	if (idx % 4 == 0) {
		value &= 0xffffff00;
		value |= j;
	}
	else if (idx % 4 == 1) {
		value &= 0xffff00ff;
		value |= (j << 8);
	}
	else if (idx % 4 == 2) {
		value &= 0xff00ffff;
		value |= (j << 16);
	}
	else {
		value &= 0x00ffffff;
		value |= (j << 24);
	}
	RALINK_REG(RALINK_VLAN_MEMB_BASE + 4*(idx/4)) = value;
}

int rt3052_switch_command(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	if (argc == 2) {
		if (!strncmp(argv[1], "dump", 5))
			table_dump();
		else if (!strncmp(argv[1], "clear", 6)) {
			table_clear();
			printf("done.\n");
		}
		else {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
		}
	}
	else if (!strncmp(argv[1], "add", 4))
		table_add(argc, argv);
	else if (!strncmp(argv[1], "filt", 5))
		table_add(argc, argv);
	else if (!strncmp(argv[1], "del", 4))
		table_del(argc, argv);
	else if (!strncmp(argv[1], "vlan", 5)) {
		if (argc < 3)
			printf ("Usage:\n%s\n", cmdtp->usage);
		if (!strncmp(argv[2], "dump", 5))
			vlan_dump();
		else if (!strncmp(argv[2], "set", 4))
			vlan_set(argc, argv);
		else
			printf ("Usage:\n%s\n", cmdtp->usage);
	}
	else {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	return 0;
}

U_BOOT_CMD(
 	switch,	6,	1,	rt3052_switch_command,
 	"switch  - rt3052 embedded switch command\n",
 	"switch dump - dump switch table\n"
	"switch clear - clear switch table\n"
 	"switch add [mac] [portmap] - add an entry to switch table\n"
 	"switch add [mac] [portmap] [vlan idx] - add an entry to switch table\n"
 	"switch add [mac] [portmap] [vlan idx] [age] - add an entry to switch table\n"
 	"switch filt [mac] - add an SA filtering entry (with portmap 1111111) to switch table\n"
 	"switch filt [mac] [portmap] - add an SA filtering entry to switch table\n"
 	"switch filt [mac] [portmap] [vlan idx] - add an SA filtering entry to switch table\n"
 	"switch filt [mac] [portmap] [vlan idx] [age] - add an SA filtering entry to switch table\n"
 	"switch del [mac] - delete an entry from switch table\n"
 	"switch del [mac] [vlan idx] - delete an entry from switch table\n"
	"switch vlan dump - dump switch table\n"
	"switch vlan set [vlan idx] [vid] [portmap] - set vlan id and associated member\n"
);
#endif // RALINK_SWITCH_DEBUG_FUN //

#define RT_RDM_DUMP_RANGE	16  // unit=16bytes
u32 register_control = 0;

int rdm_ioctl(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 offset, rtvalue;
	u32 address;
	u32 count=0;

	if(!memcmp(argv[0],"reg.s",sizeof("reg.s")))
	{
		register_control = simple_strtoul(argv[1], NULL, 16);
		printf("switch register base addr to 0x%08x\n", register_control);
	}
	else if(!memcmp(argv[0],"reg.r",sizeof("reg.r")))
	{
		offset = simple_strtoul(argv[1], NULL, 16);
		rtvalue = (*(volatile u32 *)(register_control + offset));
		printf("write offset 0x%x, value 0x%x\n", offset, rtvalue);
	}
	else if(!memcmp(argv[0],"reg.w",sizeof("reg.w")))
	{
		offset = simple_strtoul(argv[1], NULL, 16);
		rtvalue = simple_strtoul(argv[2], NULL, 16);
		*(volatile u32 *)(register_control + offset) = rtvalue;
		printf("write offset 0x%x, value 0x%x\n", offset, *(volatile u32 *)(register_control + offset));
	}
	else if(!memcmp(argv[0],"reg.d",sizeof("reg.d")))
	{
		offset = simple_strtoul(argv[1], NULL, 16);
		for (count=0; count < RT_RDM_DUMP_RANGE ; count++) {
			address = register_control + offset + (count*16);
			printf("%08X: ", address);
			printf("%08X %08X %08X %08X\n", 
				le32_to_cpu(*(volatile u32 *)(address)),
				le32_to_cpu(*(volatile u32 *)(address+4)),
				le32_to_cpu(*(volatile u32 *)(address+8)),
				le32_to_cpu(*(volatile u32 *)(address+12)));
		}
	}
	return 0;
}

U_BOOT_CMD(
 	reg,	4,	1,	rdm_ioctl,
 	"reg   - Ralink PHY register R/W command !!\n",
 	"reg.s [phy_addr(hex)] - set register base \n"
 	"reg.r [phy_addr(hex)] - read register \n"
 	"reg.w [phy_addr(hex)] [data(HEX)] - write register \n"
 	"reg.d [phy register(hex)] - dump registers \n"
);

int EswCntRead(struct seq_file *seq, void *v)
{
	unsigned int pkt_cnt = 0;
	int i = 0;

	printf("\n		  <<CPU>>			 \n");
	printf("		    |				 \n");
	printf("+-----------------------------------------------+\n");
	printf("|		  <<PSE>>		        |\n");
	printf("+-----------------------------------------------+\n");
	printf("		   |				 \n");
	printf("+-----------------------------------------------+\n");
	printf("|		  <<GDMA>>		        |\n");
	printf("| GDMA1_RX_GBCNT  : %010u (Rx Good Bytes)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2400));
	printf("| GDMA1_RX_GPCNT  : %010u (Rx Good Pkts)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2408));
	printf("| GDMA1_RX_OERCNT : %010u (overflow error)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2410));	
	printf("| GDMA1_RX_FERCNT : %010u (FCS error)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2414));	
	printf("| GDMA1_RX_SERCNT : %010u (too short)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2418));	
	printf("| GDMA1_RX_LERCNT : %010u (too long)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x241C));	
	printf("| GDMA1_RX_CERCNT : %010u (checksum error)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2420));	
	printf("| GDMA1_RX_FCCNT  : %010u (flow control)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2424));	
	printf("| GDMA1_TX_SKIPCNT: %010u (about count)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2428));	
	printf("| GDMA1_TX_COLCNT : %010u (collision count)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x242C));	
	printf("| GDMA1_TX_GBCNT  : %010u (Tx Good Bytes)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2430));	
	printf("| GDMA1_TX_GPCNT  : %010u (Tx Good Pkts)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2438));	
	printf("|						|\n");
	printf("| GDMA2_RX_GBCNT  : %010u (Rx Good Bytes)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2440));	
	printf("| GDMA2_RX_GPCNT  : %010u (Rx Good Pkts)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2448));	
	printf("| GDMA2_RX_OERCNT : %010u (overflow error)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2450));	
	printf("| GDMA2_RX_FERCNT : %010u (FCS error)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2454));	
	printf("| GDMA2_RX_SERCNT : %010u (too short)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2458));	
	printf("| GDMA2_RX_LERCNT : %010u (too long)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x245C));	
	printf("| GDMA2_RX_CERCNT : %010u (checksum error)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2460));	
	printf("| GDMA2_RX_FCCNT  : %010u (flow control)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2464));	
	printf("| GDMA2_TX_SKIPCNT: %010u (skip)		|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2468));	
	printf("| GDMA2_TX_COLCNT : %010u (collision)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x246C));	
	printf("| GDMA2_TX_GBCNT  : %010u (Tx Good Bytes)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2470));	
	printf("| GDMA2_TX_GPCNT  : %010u (Tx Good Pkts)	|\n", RALINK_REG(RALINK_FRAME_ENGINE_BASE+0x2478));	
	printf("+-----------------------------------------------+\n");

#define DUMP_EACH_PORT(base)					\
	for(i=0; i < 7;i++) {					\
		mii_mgr_read(31, (base) + (i*0x100), &pkt_cnt); \
		printf("%8u ", pkt_cnt);			\
	}							\
	printf("\n");

	{
		printf("===================== %8s %8s %8s %8s %8s %8s %8s\n","Port0", "Port1", "Port2", "Port3", "Port4", "Port5", "Port6");
		printf("Tx Drop Packet      :"); DUMP_EACH_PORT(0x4000);
		printf("Tx CRC Error        :"); DUMP_EACH_PORT(0x4004);
		printf("Tx Unicast Packet   :"); DUMP_EACH_PORT(0x4008);
		printf("Tx Multicast Packet :"); DUMP_EACH_PORT(0x400C);
		printf("Tx Broadcast Packet :"); DUMP_EACH_PORT(0x4010);
		printf("Tx Collision Event  :"); DUMP_EACH_PORT(0x4014);
		printf("Tx Pause Packet     :"); DUMP_EACH_PORT(0x402C);
		printf("Rx Drop Packet      :"); DUMP_EACH_PORT(0x4060);
		printf("Rx Filtering Packet :"); DUMP_EACH_PORT(0x4064);
		printf("Rx Unicast Packet   :"); DUMP_EACH_PORT(0x4068);
		printf("Rx Multicast Packet :"); DUMP_EACH_PORT(0x406C);
		printf("Rx Broadcast Packet :"); DUMP_EACH_PORT(0x4070);
		printf("Rx Alignment Error  :"); DUMP_EACH_PORT(0x4074);
		printf("Rx CRC Error	    :"); DUMP_EACH_PORT(0x4078);
		printf("Rx Undersize Error  :"); DUMP_EACH_PORT(0x407C);
		printf("Rx Fragment Error   :"); DUMP_EACH_PORT(0x4080);
		printf("Rx Oversize Error   :"); DUMP_EACH_PORT(0x4084);
		printf("Rx Jabber Error     :"); DUMP_EACH_PORT(0x4088);
		printf("Rx Pause Packet     :"); DUMP_EACH_PORT(0x408C);
		mii_mgr_write(31, 0x4fe0, 0xf0);
		mii_mgr_write(31, 0x4fe0, 0x800000f0);
	} 
	printf("\n");

	return 0;
}

U_BOOT_CMD(
 	esw_read,	4,	1,	EswCntRead,
 	"esw_read   - Dump external switch/GMAC status !!\n",
 	"esw_read   - Dump external switch/GMAC status \n"
);

#endif	/* CONFIG_TULIP */
