#ifndef __MTK_PHY_NEW_H
#define __MTK_PHY_NEW_H

/* Choose PHY R/W implementation */
/* Choose PHY version */
//Select your project by defining one of the followings
//#if defined (CONFIG_RALINK_MT7621)
#define CONFIG_PROJECT_7621 //7621
#define CONFIG_U3_PHY_AHB_SUPPORT	//AHB, only on SoC
#define CONFIG_PROJECT_PHY

#define CONFIG_ARCH_MT7623
#define CONFIG_PROJECT_7623 //7623
#define CONFIG_U3_PHY_AHB_SUPPORT       //AHB, only on SoC
#define CFG_DEV_U3H0    1
#define CFG_DEV_U3H1    0 //assume only one port
#define CONFIG_PROJECT_PHY   
#define U3_PHY_PIPE_PHASE_TIME_DELAY    0x0a
//#endif

/* BASE ADDRESS DEFINE, should define this on ASIC */
//#if defined (CONFIG_ARCH_MT7623)
/* for port0 */
#define PHY_BASE                0x1A1C4000 	/* FIXME */
#define SIFSLV_FM_FEG_BASE      (PHY_BASE+0x100)
#define SIFSLV_CHIP_BASE        (PHY_BASE+0x300)
#define SIFSLV_IPPC_BASE	(PHY_BASE+0x700)
#define U2_PHY_BASE             (PHY_BASE+0x800)
#define U3_PHYD_BASE            (PHY_BASE+0x900)
#define U3_PHYD_B2_BASE         (PHY_BASE+0xa00)
#define U3_PHYA_BASE            (PHY_BASE+0xb00)
#define U3_PHYA_DA_BASE         (PHY_BASE+0xc00)

/* for port1 */
#define PHY_BASE_P1		0x1A244000	
#define SIFSLV_FM_FEG_BASE_P1   (PHY_BASE_P1+0x100)
#define SIFSLV_CHIP_BASE_P1     (PHY_BASE_P1+0x300)
#define SIFSLV_IPPC_BASE_P1     (PHY_BASE_P1+0x700)
#define U2_PHY_BASE_P1          (PHY_BASE_P1+0x800)
#define U3_PHYD_BASE_P1         (PHY_BASE_P1+0x900)
#define U3_PHYD_B2_BASE_P1      (PHY_BASE_P1+0xa00)
#define U3_PHYA_BASE_P1         (PHY_BASE_P1+0xb00)
#define U3_PHYA_DA_BASE_P1      (PHY_BASE_P1+0xc00)
//#endif

/*

0x00000100	MODULE	ssusb_sifslv_fmreg	ssusb_sifslv_fmreg
0x00000700	MODULE	ssusb_sifslv_ippc	ssusb_sifslv_ippc
0x00000800	MODULE	ssusb_sifslv_u2phy_com	ssusb_sifslv_u2_phy_com_T28
0x00000900	MODULE	ssusb_sifslv_u3phyd	ssusb_sifslv_u3phyd_T28
0x00000a00	MODULE	ssusb_sifslv_u3phyd_bank2	ssusb_sifslv_u3phyd_bank2_T28
0x00000b00	MODULE	ssusb_sifslv_u3phya	ssusb_sifslv_u3phya_T28
0x00000c00	MODULE	ssusb_sifslv_u3phya_da	ssusb_sifslv_u3phya_da_T28
*/


/* TYPE DEFINE */
typedef unsigned int	PHY_UINT32;
typedef int				PHY_INT32;
typedef	unsigned short	PHY_UINT16;
typedef short			PHY_INT16;
typedef unsigned char	PHY_UINT8;
typedef char			PHY_INT8;

//typedef PHY_UINT32 __bitwise	PHY_LE32;
typedef PHY_UINT32	PHY_LE32;

/* CONSTANT DEFINE */
#define PHY_FALSE	0
#define PHY_TRUE	1

#define RET_SUCCESS 0
#define RET_FAIL 1

/* MACRO DEFINE */
#define DRV_WriteReg32(addr,data)       ((*(volatile PHY_UINT32 *)(addr)) = (unsigned long)(data))
#define DRV_Reg32(addr)                 (*(volatile PHY_UINT32 *)(addr))

#define DRV_MDELAY	mdelay
#define DRV_MSLEEP	mdelay
#define DRV_UDELAY	udelay
#define DRV_USLEEP	udelay

/* PHY FUNCTION DEFINE, implemented in platform files, ex. ahb, gpio */
PHY_INT32 U3PhyWriteReg32(PHY_UINT32 addr, PHY_UINT32 data);
PHY_INT32 U3PhyReadReg32(PHY_UINT32 addr);
PHY_INT32 U3PhyWriteReg8(PHY_UINT32 addr, PHY_UINT8 data);
PHY_INT8 U3PhyReadReg8(PHY_UINT32 addr);

/* PHY GENERAL USAGE FUNC, implemented in mtk-phy.c */
PHY_INT32 U3PhyWriteField8(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask, PHY_INT32 value);
PHY_INT32 U3PhyWriteField32(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask, PHY_INT32 value);
PHY_INT32 U3PhyReadField8(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask);
PHY_INT32 U3PhyReadField32(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask);

struct u3phy_info {
	PHY_INT32 phy_version;
	PHY_INT32 phyd_version_addr;
	
//#ifdef CONFIG_PROJECT_PHY
	
#ifdef CONFIG_PROJECT_7623
#if CFG_DEV_U3H0
	struct u2phy_reg *u2phy_regs;
	struct u3phya_reg *u3phya_regs;
	struct u3phya_da_reg *u3phya_da_regs;
	struct u3phyd_reg *u3phyd_regs;
	struct u3phyd_bank2_reg *u3phyd_bank2_regs;
	struct sifslv_chip_reg *sifslv_chip_regs;	
	struct sifslv_fm_feg *sifslv_fm_regs;
#endif
#if CFG_DEV_U3H1
	struct u2phy_reg *u2phy_regs_p1;
	struct u3phya_reg *u3phya_regs_p1;
	struct u3phya_da_reg *u3phya_da_regs_p1;
	struct u3phyd_reg *u3phyd_regs_p1;
	struct u3phyd_bank2_reg *u3phyd_bank2_regs_p1;
	struct sifslv_chip_reg *sifslv_chip_regs_p1;	
	struct sifslv_fm_feg *sifslv_fm_regs_p1;
#endif
#else	
	struct u2phy_reg *u2phy_regs;
	struct u3phya_reg *u3phya_regs;
	struct u3phya_da_reg *u3phya_da_regs;
	struct u3phyd_reg *u3phyd_regs;
	struct u3phyd_bank2_reg *u3phyd_bank2_regs;
	struct sifslv_chip_reg *sifslv_chip_regs;	
	struct sifslv_fm_feg *sifslv_fm_regs;	

#endif
//#endif
};

struct u3phy_operator {
	PHY_INT32 (*init) (struct u3phy_info *info);
	PHY_INT32 (*change_pipe_phase) (struct u3phy_info *info, PHY_INT32 phy_drv, PHY_INT32 pipe_phase);
	PHY_INT32 (*eyescan_init) (struct u3phy_info *info);
	PHY_INT32 (*eyescan) (struct u3phy_info *info, PHY_INT32 x_t1, PHY_INT32 y_t1, PHY_INT32 x_br, PHY_INT32 y_br, PHY_INT32 delta_x, PHY_INT32 delta_y, PHY_INT32 eye_cnt, PHY_INT32 num_cnt, PHY_INT32 PI_cal_en, PHY_INT32 num_ignore_cnt);
	PHY_INT32 (*u2_connect) (struct u3phy_info *info);
	PHY_INT32 (*u2_disconnect) (struct u3phy_info *info);
	PHY_INT32 (*u2_save_current_entry) (struct u3phy_info *info);
	PHY_INT32 (*u2_save_current_recovery) (struct u3phy_info *info);
#ifdef CONFIG_PROJECT_7623
#if CFG_DEV_U3H0
	PHY_INT32 (*u2_slew_rate_calibration) (struct u3phy_info *info);
#endif
#if CFG_DEV_U3H1
	PHY_INT32 (*u2_slew_rate_calibration_p1) (struct u3phy_info *info);
#endif
#endif
};

#ifdef U3_PHY_LIB
#define AUTOEXT
#else
#define AUTOEXT extern
#endif

AUTOEXT struct u3phy_info *u3phy;
//AUTOEXT struct u3phy_info *u3phy_p1;
AUTOEXT struct u3phy_operator *u3phy_ops;

//PHY_INT32 mt7623_phy_init(void);
/*********eye scan required*********/

#define LO_BYTE(x)                   ((PHY_UINT8)((x) & 0xFF))
#define HI_BYTE(x)                   ((PHY_UINT8)(((x) & 0xFF00) >> 8))

typedef enum
{
  SCAN_UP,
  SCAN_DN
} enumScanDir;

struct strucScanRegion
{
  PHY_INT8 bX_tl;
  PHY_INT8 bY_tl;
  PHY_INT8 bX_br;
  PHY_INT8 bY_br;
  PHY_INT8 bDeltaX;
  PHY_INT8 bDeltaY;
};

struct strucTestCycle
{
  PHY_UINT16 wEyeCnt;
  PHY_INT8 bNumOfEyeCnt;
  PHY_INT8 bPICalEn;
  PHY_INT8 bNumOfIgnoreCnt;
};

#define ERRCNT_MAX		128
#define CYCLE_COUNT_MAX	15

/// the map resolution is 128 x 128 pts
#define MAX_X                 127
#define MAX_Y                 127
#define MIN_X                 0
#define MIN_Y                 0

PHY_INT32 u3phy_init(void);
PHY_INT32 mt7628_phy_init(void);
PHY_INT32 mt7628_phy_down(void);

AUTOEXT struct strucScanRegion           _rEye1;
AUTOEXT struct strucScanRegion           _rEye2;
AUTOEXT struct strucTestCycle            _rTestCycle;
AUTOEXT PHY_UINT8                      _bXcurr;
AUTOEXT PHY_UINT8                      _bYcurr;
AUTOEXT enumScanDir               _eScanDir;
AUTOEXT PHY_INT8                      _fgXChged;
AUTOEXT PHY_INT8                      _bPIResult;
/***********************************/
#endif

