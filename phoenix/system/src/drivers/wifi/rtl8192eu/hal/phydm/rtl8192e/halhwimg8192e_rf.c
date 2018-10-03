/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

/*Image2HeaderVersion: 2.18*/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8192E_SUPPORT == 1)
static boolean
check_positive(
	struct PHY_DM_STRUCT     *p_dm_odm,
	const u32  condition1,
	const u32  condition2,
	const u32  condition3,
	const u32  condition4
)
{
	u8    _board_type = ((p_dm_odm->board_type & BIT(4)) >> 4) << 0 | /* _GLNA*/
		    ((p_dm_odm->board_type & BIT(3)) >> 3) << 1 | /* _GPA*/
		    ((p_dm_odm->board_type & BIT(7)) >> 7) << 2 | /* _ALNA*/
		    ((p_dm_odm->board_type & BIT(6)) >> 6) << 3 | /* _APA */
		    ((p_dm_odm->board_type & BIT(2)) >> 2) << 4;  /* _BT*/

	u32	cond1   = condition1, cond2 = condition2, cond3 = condition3, cond4 = condition4;
	u32    driver1 = p_dm_odm->cut_version       << 24 |
			 (p_dm_odm->support_interface & 0xF0) << 16 |
			 p_dm_odm->support_platform  << 16 |
			 p_dm_odm->package_type      << 12 |
			 (p_dm_odm->support_interface & 0x0F) << 8  |
			 _board_type;

	u32    driver2 = (p_dm_odm->type_glna & 0xFF) <<  0 |
			 (p_dm_odm->type_gpa & 0xFF)  <<  8 |
			 (p_dm_odm->type_alna & 0xFF) << 16 |
			 (p_dm_odm->type_apa & 0xFF)  << 24;

	u32    driver3 = 0;

	u32    driver4 = (p_dm_odm->type_glna & 0xFF00) >>  8 |
			 (p_dm_odm->type_gpa & 0xFF00) |
			 (p_dm_odm->type_alna & 0xFF00) << 8 |
			 (p_dm_odm->type_apa & 0xFF00)  << 16;

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT,
		("===> check_positive (cond1, cond2, cond3, cond4) = (0x%X 0x%X 0x%X 0x%X)\n", cond1, cond2, cond3, cond4));
	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT,
		("===> check_positive (driver1, driver2, driver3, driver4) = (0x%X 0x%X 0x%X 0x%X)\n", driver1, driver2, driver3, driver4));

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT,
		("	(Platform, Interface) = (0x%X, 0x%X)\n", p_dm_odm->support_platform, p_dm_odm->support_interface));
	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT,
		("	(Board, Package) = (0x%X, 0x%X)\n", p_dm_odm->board_type, p_dm_odm->package_type));


	/*============== value Defined Check ===============*/
	/*QFN type [15:12] and cut version [27:24] need to do value check*/

	if (((cond1 & 0x0000F000) != 0) && ((cond1 & 0x0000F000) != (driver1 & 0x0000F000)))
		return false;
	if (((cond1 & 0x0F000000) != 0) && ((cond1 & 0x0F000000) != (driver1 & 0x0F000000)))
		return false;

	/*=============== Bit Defined Check ================*/
	/* We don't care [31:28] */

	cond1   &= 0x00FF0FFF;
	driver1 &= 0x00FF0FFF;

	if ((cond1 & driver1) == cond1) {
		u32 bit_mask = 0;

		if ((cond1 & 0x0F) == 0) /* board_type is DONTCARE*/
			return true;

		if ((cond1 & BIT(0)) != 0) /*GLNA*/
			bit_mask |= 0x000000FF;
		if ((cond1 & BIT(1)) != 0) /*GPA*/
			bit_mask |= 0x0000FF00;
		if ((cond1 & BIT(2)) != 0) /*ALNA*/
			bit_mask |= 0x00FF0000;
		if ((cond1 & BIT(3)) != 0) /*APA*/
			bit_mask |= 0xFF000000;

		if (((cond2 & bit_mask) == (driver2 & bit_mask)) && ((cond4 & bit_mask) == (driver4 & bit_mask)))  /* board_type of each RF path is matched*/
			return true;
		else
			return false;
	} else
		return false;
}
static boolean
check_negative(
	struct PHY_DM_STRUCT     *p_dm_odm,
	const u32  condition1,
	const u32  condition2
)
{
	return true;
}

/******************************************************************************
*                           RadioA.TXT
******************************************************************************/

u32 array_mp_8192e_radioa[] = {
	0x07F, 0x00000082,
	0x081, 0x0003FC00,
	0x000, 0x00030000,
	0x008, 0x00008400,
	0x018, 0x00000407,
	0x019, 0x00000012,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x00000064,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x0000394C,
	0xA0000000,	0x00000000,
	0x01B, 0x0000394C,
	0xB0000000,	0x00000000,
	0x01E, 0x00080009,
	0x01F, 0x00000880,
	0x02F, 0x0001A060,
	0x03F, 0x00000000,
	0x042, 0x000060C0,
	0x057, 0x000D0000,
	0x058, 0x000BE180,
	0x067, 0x00001552,
	0x083, 0x00000000,
	0x0B0, 0x000FF9F1,
	0x0B1, 0x00055418,
	0x0B2, 0x0008CC00,
	0x0B4, 0x00043083,
	0x0B5, 0x00008166,
	0x0B6, 0x0000803E,
	0x0B7, 0x0001C69F,
	0x0B8, 0x0000407F,
	0x0B9, 0x00090001,
	0x0BA, 0x00040001,
	0x0BB, 0x00000400,
	0x0BC, 0x00000078,
	0x0BD, 0x000B3333,
	0x0BE, 0x00033340,
	0x0BF, 0x00000000,
	0x0C0, 0x00005999,
	0x0C1, 0x00009999,
	0x0C2, 0x00002400,
	0x0C3, 0x00000009,
	0x0C4, 0x00040C91,
	0x0C5, 0x00099999,
	0x0C6, 0x000000A3,
	0x0C7, 0x00088820,
	0x0C8, 0x00076C06,
	0x0C9, 0x00000000,
	0x0CA, 0x00080000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00000000,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x01C, 0x00000000,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x01C, 0x00000000,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x01C, 0x00000000,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00000000,
	0xA0000000,	0x00000000,
	0x01C, 0x00000000,
	0xB0000000,	0x00000000,
	0x0DF, 0x00000180,
	0x0EF, 0x000001A0,
	0x051, 0x00069545,
	0x052, 0x0007E45E,
	0x053, 0x00000071,
	0x056, 0x00051FF3,
	0x035, 0x000000A8,
	0x035, 0x000001E2,
	0x035, 0x000002A8,
	0x036, 0x00001C24,
	0x036, 0x00009C24,
	0x036, 0x00011C24,
	0x036, 0x00019C24,
	0x018, 0x00000C07,
	0x05A, 0x00048000,
	0x019, 0x000739D0,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0xA0000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0xB0000000,	0x00000000,
	0x000, 0x00030159,
	0x084, 0x00068180,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x0000014E,
	0x087, 0x00049F80,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x0000014E,
	0x087, 0x00049F80,
	0xA0000000,	0x00000000,
	0x086, 0x0000014E,
	0x087, 0x00049F80,
	0xB0000000,	0x00000000,
	0x08E, 0x00065540,
	0x08F, 0x00088000,
	0x0EF, 0x000020A0,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F02B0,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F52B0,
	0xA0000000,	0x00000000,
	0x03B, 0x000F02B0,
	0xB0000000,	0x00000000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000EF7B0,
	0x03B, 0x000D4FB0,
	0x03B, 0x000CF060,
	0x03B, 0x000B0090,
	0x03B, 0x000A0080,
	0x03B, 0x00090080,
	0x03B, 0x0008F780,
	0xA0000000,	0x00000000,
	0x03B, 0x000EF7B0,
	0x03B, 0x000D4FB0,
	0x03B, 0x000CF060,
	0x03B, 0x000B0090,
	0x03B, 0x000A0080,
	0x03B, 0x00090080,
	0x03B, 0x0008F780,
	0xB0000000,	0x00000000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x00078730,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x00070730,
	0xA0000000,	0x00000000,
	0x03B, 0x00078730,
	0xB0000000,	0x00000000,
	0x03B, 0x00060FB0,
	0x03B, 0x0005FFA0,
	0x03B, 0x00040620,
	0x03B, 0x00037090,
	0x03B, 0x00020080,
	0x03B, 0x0001F060,
	0x03B, 0x0000FFB0,
	0x0EF, 0x000000A0,
	0xFFE, 0x00000000,
	0x018, 0x0000FC07,
	0xFFE, 0x00000000,
	0xFFE, 0x00000000,
	0xFFE, 0x00000000,
	0xFFE, 0x00000000,
	0x01E, 0x00000001,
	0x01F, 0x00080000,
	0x000, 0x00033E70,

};

void
odm_read_and_config_mp_8192e_radioa(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
	u32     i         = 0;
	u8     c_cond;
	boolean is_matched = true, is_skipped = false;
	u32     array_len    = sizeof(array_mp_8192e_radioa) / sizeof(u32);
	u32    *array       = array_mp_8192e_radioa;

	u32	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> odm_read_and_config_mp_8192e_radioa\n"));

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 & (BIT(31) | BIT30)) {/*positive & negative condition*/
			if (v1 & BIT(31)) {/* positive condition*/
				c_cond  = (u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) {/*end*/
					is_matched = true;
					is_skipped = false;
					PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("ENDIF\n"));
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("ELSE\n"));
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (is_skipped == false) {
					if (check_positive(p_dm_odm, pre_v1, pre_v2, v1, v2)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else
					is_matched = false;
			}
		} else {
			if (is_matched)
				odm_config_rf_radio_a_8192e(p_dm_odm, v1, v2);
		}
		i = i + 2;
	}
}

u32
odm_get_version_mp_8192e_radioa(void)
{
	return 58;
}

/******************************************************************************
*                           RadioB.TXT
******************************************************************************/

u32 array_mp_8192e_radiob[] = {
	0x07F, 0x00000082,
	0x081, 0x0003FC00,
	0x000, 0x00030000,
	0x008, 0x00008400,
	0x018, 0x00000407,
	0x019, 0x00000012,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x00000064,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x01B, 0x0000146C,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x01B, 0x0000394C,
	0xA0000000,	0x00000000,
	0x01B, 0x0000394C,
	0xB0000000,	0x00000000,
	0x01E, 0x00080009,
	0x01F, 0x00000880,
	0x02F, 0x0001A060,
	0x03F, 0x00000000,
	0x042, 0x000060C0,
	0x057, 0x000D0000,
	0x058, 0x000BE180,
	0x067, 0x00001552,
	0x07F, 0x00000082,
	0x081, 0x0003F000,
	0x083, 0x00000000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00001C00,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x01C, 0x00000000,
	0xA0000000,	0x00000000,
	0x01C, 0x00000000,
	0xB0000000,	0x00000000,
	0x0DF, 0x00000180,
	0x0EF, 0x000001A0,
	0x051, 0x00069545,
	0x052, 0x0007E42E,
	0x053, 0x00000071,
	0x056, 0x00051FF3,
	0x035, 0x000000A8,
	0x035, 0x000001E0,
	0x035, 0x000002A8,
	0x036, 0x00001CA8,
	0x036, 0x00009C24,
	0x036, 0x00011C24,
	0x036, 0x00019C24,
	0x018, 0x00000C07,
	0x05A, 0x00048000,
	0x019, 0x000739D0,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000A093,
	0x034, 0x0000908F,
	0x034, 0x0000808C,
	0x034, 0x0000704D,
	0x034, 0x0000604A,
	0x034, 0x00005047,
	0x034, 0x0000400A,
	0x034, 0x00003007,
	0x034, 0x00002004,
	0x034, 0x00001001,
	0x034, 0x00000000,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0xA0000000,	0x00000000,
	0x034, 0x0000ADD7,
	0x034, 0x00009DD4,
	0x034, 0x00008DD1,
	0x034, 0x00007DCE,
	0x034, 0x00006DCB,
	0x034, 0x00005DC8,
	0x034, 0x00004DC5,
	0x034, 0x000034CC,
	0x034, 0x0000244F,
	0x034, 0x0000144C,
	0x034, 0x00000014,
	0xB0000000,	0x00000000,
	0x000, 0x00030159,
	0x084, 0x00068180,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x000000CE,
	0x087, 0x00049F80,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x086, 0x0000010E,
	0x087, 0x000F5F80,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x086, 0x000000CE,
	0x087, 0x00049F80,
	0xA0000000,	0x00000000,
	0x086, 0x000000CE,
	0x087, 0x00049F80,
	0xB0000000,	0x00000000,
	0x08E, 0x00065540,
	0x08F, 0x00088000,
	0x0EF, 0x000020A0,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F02B0,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000F0730,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000F52B0,
	0xA0000000,	0x00000000,
	0x03B, 0x000F02B0,
	0xB0000000,	0x00000000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000E0730,
	0x03B, 0x000D0020,
	0x03B, 0x000C0020,
	0x03B, 0x000B0760,
	0x03B, 0x000A0010,
	0x03B, 0x00090000,
	0x03B, 0x00080000,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000EF7B0,
	0x03B, 0x000D4FB0,
	0x03B, 0x000CF060,
	0x03B, 0x000B0090,
	0x03B, 0x000A0080,
	0x03B, 0x00090080,
	0x03B, 0x0008F780,
	0xA0000000,	0x00000000,
	0x03B, 0x000EF7B0,
	0x03B, 0x000D4FB0,
	0x03B, 0x000CF060,
	0x03B, 0x000B0090,
	0x03B, 0x000A0080,
	0x03B, 0x00090080,
	0x03B, 0x0008F780,
	0xB0000000,	0x00000000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000003,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000003,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000003,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x00078730,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
	0x03B, 0x000787B0,
	0x90000400,	0x00000000,	0x40000000,	0x00000000,
	0x03B, 0x00070730,
	0xA0000000,	0x00000000,
	0x03B, 0x00078730,
	0xB0000000,	0x00000000,
	0x03B, 0x00060FB0,
	0x03B, 0x0005FFA0,
	0x03B, 0x00040620,
	0x03B, 0x00037090,
	0x03B, 0x00020080,
	0x03B, 0x0001F060,
	0x03B, 0x0000FFB0,
	0x0EF, 0x000000A0,
	0x000, 0x00010159,
	0xFFE, 0x00000000,
	0xFFE, 0x00000000,
	0xFFE, 0x00000000,
	0xFFE, 0x00000000,
	0x01E, 0x00000001,
	0x01F, 0x00080000,
	0x000, 0x00033E70,

};

void
odm_read_and_config_mp_8192e_radiob(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
	u32     i         = 0;
	u8     c_cond;
	boolean is_matched = true, is_skipped = false;
	u32     array_len    = sizeof(array_mp_8192e_radiob) / sizeof(u32);
	u32    *array       = array_mp_8192e_radiob;

	u32	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> odm_read_and_config_mp_8192e_radiob\n"));

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 & (BIT(31) | BIT30)) {/*positive & negative condition*/
			if (v1 & BIT(31)) {/* positive condition*/
				c_cond  = (u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) {/*end*/
					is_matched = true;
					is_skipped = false;
					PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("ENDIF\n"));
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("ELSE\n"));
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (is_skipped == false) {
					if (check_positive(p_dm_odm, pre_v1, pre_v2, v1, v2)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else
					is_matched = false;
			}
		} else {
			if (is_matched)
				odm_config_rf_radio_b_8192e(p_dm_odm, v1, v2);
		}
		i = i + 2;
	}
}

u32
odm_get_version_mp_8192e_radiob(void)
{
	return 58;
}

/******************************************************************************
*                           TxPowerTrack_AP.TXT
******************************************************************************/

#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
u8 g_delta_swing_table_idx_mp_5gb_n_txpowertrack_ap_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
};
u8 g_delta_swing_table_idx_mp_5gb_p_txpowertrack_ap_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 16, 17, 17, 18, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_5ga_n_txpowertrack_ap_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 13, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11, 12, 13, 14, 14, 15, 15, 16, 16, 16, 16, 16, 16, 16},
};
u8 g_delta_swing_table_idx_mp_5ga_p_txpowertrack_ap_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_2gb_n_txpowertrack_ap_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2gb_p_txpowertrack_ap_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_n_txpowertrack_ap_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_p_txpowertrack_ap_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_ap_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_ap_8192e[] = {0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_ap_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_ap_8192e[] = {0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
#endif

void
odm_read_and_config_mp_8192e_txpowertrack_ap(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm_odm->rf_calibrate_info);

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> ODM_ReadAndConfig_MP_MP_8192E\n"));


	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_p, g_delta_swing_table_idx_mp_2ga_p_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_n, g_delta_swing_table_idx_mp_2ga_n_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_p, g_delta_swing_table_idx_mp_2gb_p_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_n, g_delta_swing_table_idx_mp_2gb_n_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_p, g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_n, g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_p, g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_n, g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_p, g_delta_swing_table_idx_mp_5ga_p_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_n, g_delta_swing_table_idx_mp_5ga_n_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_p, g_delta_swing_table_idx_mp_5gb_p_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_n, g_delta_swing_table_idx_mp_5gb_n_txpowertrack_ap_8192e, DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
*                           TxPowerTrack_PCIE.TXT
******************************************************************************/

#if DEV_BUS_TYPE == RT_PCI_INTERFACE
u8 g_delta_swing_table_idx_mp_5gb_n_txpowertrack_pcie_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
};
u8 g_delta_swing_table_idx_mp_5gb_p_txpowertrack_pcie_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 16, 17, 17, 18, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_5ga_n_txpowertrack_pcie_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 13, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11, 12, 13, 14, 14, 15, 15, 16, 16, 16, 16, 16, 16, 16},
};
u8 g_delta_swing_table_idx_mp_5ga_p_txpowertrack_pcie_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_2gb_n_txpowertrack_pcie_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2gb_p_txpowertrack_pcie_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_n_txpowertrack_pcie_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_p_txpowertrack_pcie_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_pcie_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_pcie_8192e[] = {0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_pcie_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_pcie_8192e[] = {0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
#endif

void
odm_read_and_config_mp_8192e_txpowertrack_pcie(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
#if DEV_BUS_TYPE == RT_PCI_INTERFACE
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm_odm->rf_calibrate_info);

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> ODM_ReadAndConfig_MP_MP_8192E\n"));


	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_p, g_delta_swing_table_idx_mp_2ga_p_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_n, g_delta_swing_table_idx_mp_2ga_n_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_p, g_delta_swing_table_idx_mp_2gb_p_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_n, g_delta_swing_table_idx_mp_2gb_n_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_p, g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_n, g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_p, g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_n, g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_p, g_delta_swing_table_idx_mp_5ga_p_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_n, g_delta_swing_table_idx_mp_5ga_n_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_p, g_delta_swing_table_idx_mp_5gb_p_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_n, g_delta_swing_table_idx_mp_5gb_n_txpowertrack_pcie_8192e, DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
*                           TxPowerTrack_SDIO.TXT
******************************************************************************/

#if DEV_BUS_TYPE == RT_SDIO_INTERFACE
u8 g_delta_swing_table_idx_mp_5gb_n_txpowertrack_sdio_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
};
u8 g_delta_swing_table_idx_mp_5gb_p_txpowertrack_sdio_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 16, 17, 17, 18, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_5ga_n_txpowertrack_sdio_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 13, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11, 12, 13, 14, 14, 15, 15, 16, 16, 16, 16, 16, 16, 16},
};
u8 g_delta_swing_table_idx_mp_5ga_p_txpowertrack_sdio_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_2gb_n_txpowertrack_sdio_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2gb_p_txpowertrack_sdio_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_n_txpowertrack_sdio_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_p_txpowertrack_sdio_8192e[]    = {0, 0, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_sdio_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_sdio_8192e[] = {0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_sdio_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_sdio_8192e[] = {0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 9, 10, 10, 11, 11, 12, 12, 13, 14, 15};
#endif

void
odm_read_and_config_mp_8192e_txpowertrack_sdio(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
#if DEV_BUS_TYPE == RT_SDIO_INTERFACE
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm_odm->rf_calibrate_info);

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> ODM_ReadAndConfig_MP_MP_8192E\n"));


	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_p, g_delta_swing_table_idx_mp_2ga_p_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_n, g_delta_swing_table_idx_mp_2ga_n_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_p, g_delta_swing_table_idx_mp_2gb_p_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_n, g_delta_swing_table_idx_mp_2gb_n_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_p, g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_n, g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_p, g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_n, g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_p, g_delta_swing_table_idx_mp_5ga_p_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_n, g_delta_swing_table_idx_mp_5ga_n_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_p, g_delta_swing_table_idx_mp_5gb_p_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_n, g_delta_swing_table_idx_mp_5gb_n_txpowertrack_sdio_8192e, DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
*                           TxPowerTrack_USB.TXT
******************************************************************************/

#if DEV_BUS_TYPE == RT_USB_INTERFACE
u8 g_delta_swing_table_idx_mp_5gb_n_txpowertrack_usb_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
};
u8 g_delta_swing_table_idx_mp_5gb_p_txpowertrack_usb_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 16, 17, 17, 18, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 19, 20, 20, 20},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_5ga_n_txpowertrack_usb_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 14, 14, 14},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 13, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11, 12, 13, 14, 14, 15, 15, 16, 16, 16, 16, 16, 16, 16},
};
u8 g_delta_swing_table_idx_mp_5ga_p_txpowertrack_usb_8192e[][DELTA_SWINGIDX_SIZE] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
	{0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 21},
};
u8 g_delta_swing_table_idx_mp_2gb_n_txpowertrack_usb_8192e[]    = {0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5, 5, 5, 6, 6, 7, 8, 8, 8, 9, 10, 10, 10, 10, 11, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2gb_p_txpowertrack_usb_8192e[]    = {0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 5, 5, 6, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_n_txpowertrack_usb_8192e[]    = {0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5, 5, 5, 6, 6, 7, 8, 8, 8, 9, 10, 10, 10, 10, 11, 11, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2ga_p_txpowertrack_usb_8192e[]    = {0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 5, 5, 6, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_usb_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9, 9, 10, 11, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_usb_8192e[] = {0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_usb_8192e[] = {0, 0, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9, 9, 10, 11, 11, 11, 12, 12, 13, 14, 15};
u8 g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_usb_8192e[] = {0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15};
#endif

void
odm_read_and_config_mp_8192e_txpowertrack_usb(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
#if DEV_BUS_TYPE == RT_USB_INTERFACE
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm_odm->rf_calibrate_info);

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> ODM_ReadAndConfig_MP_MP_8192E\n"));


	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_p, g_delta_swing_table_idx_mp_2ga_p_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2ga_n, g_delta_swing_table_idx_mp_2ga_n_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_p, g_delta_swing_table_idx_mp_2gb_p_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2gb_n, g_delta_swing_table_idx_mp_2gb_n_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_p, g_delta_swing_table_idx_mp_2g_cck_a_p_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_n, g_delta_swing_table_idx_mp_2g_cck_a_n_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_p, g_delta_swing_table_idx_mp_2g_cck_b_p_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_n, g_delta_swing_table_idx_mp_2g_cck_b_n_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE);

	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_p, g_delta_swing_table_idx_mp_5ga_p_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5ga_n, g_delta_swing_table_idx_mp_5ga_n_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_p, g_delta_swing_table_idx_mp_5gb_p_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE * 3);
	odm_move_memory(p_dm_odm, p_rf_calibrate_info->delta_swing_table_idx_5gb_n, g_delta_swing_table_idx_mp_5gb_n_txpowertrack_usb_8192e, DELTA_SWINGIDX_SIZE * 3);
#endif
}

/******************************************************************************
*                           TXPWR_LMT.TXT
******************************************************************************/

const char *array_mp_8192e_txpwr_lmt[] = {
	"FCC", "2.4G", "20M", "CCK", "1T", "01", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "01", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "01", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "02", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "02", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "02", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "03", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "03", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "03", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "04", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "04", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "05", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "05", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "06", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "06", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "07", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "07", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "08", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "08", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "09", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "09", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "09", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "10", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "10", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "10", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "11", "34",
	"ETSI", "2.4G", "20M", "CCK", "1T", "11", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "11", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "12", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "12", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "12", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "13", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "13", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "13", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "14", "63",
	"MKK", "2.4G", "20M", "CCK", "1T", "14", "32",
	"FCC", "2.4G", "20M", "CCK", "2T", "01", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "01", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "01", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "02", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "02", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "02", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "03", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "03", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "03", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "04", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "04", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "04", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "05", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "05", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "05", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "06", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "06", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "06", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "07", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "07", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "07", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "08", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "08", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "08", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "09", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "09", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "09", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "10", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "10", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "10", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "11", "26",
	"ETSI", "2.4G", "20M", "CCK", "2T", "11", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "11", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "CCK", "2T", "12", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "12", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "CCK", "2T", "13", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "13", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "CCK", "2T", "14", "63",
	"MKK", "2.4G", "20M", "CCK", "2T", "14", "26",
	"FCC", "2.4G", "20M", "OFDM", "1T", "01", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "01", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "01", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "02", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "02", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "03", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "03", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "03", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "04", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "05", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "06", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "07", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "08", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "09", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "09", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "09", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "10", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "10", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "11", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "11", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "11", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "12", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "12", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "12", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "13", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "13", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "13", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"MKK", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"FCC", "2.4G", "20M", "OFDM", "2T", "01", "26",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "01", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "01", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "02", "26",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "02", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "02", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "03", "34",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "03", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "03", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "04", "34",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "04", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "04", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "05", "34",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "05", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "05", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "06", "34",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "06", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "06", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "07", "34",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "07", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "07", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "08", "34",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "08", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "08", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "09", "30",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "09", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "09", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "10", "30",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "10", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "10", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "11", "30",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "11", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "11", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "12", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "12", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "13", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "13", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "14", "63",
	"MKK", "2.4G", "20M", "OFDM", "2T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "1T", "01", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "01", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "01", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "02", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "02", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "03", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "03", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "03", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "04", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "04", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "05", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "05", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "06", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "06", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "07", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "07", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "08", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "08", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "09", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "09", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "09", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "10", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "10", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "11", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "11", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "11", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "12", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "12", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "12", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "13", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "13", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "13", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "2T", "01", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "01", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "01", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "02", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "02", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "02", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "03", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "04", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "05", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "06", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "07", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "08", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "09", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "10", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "10", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "11", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "12", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "12", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "13", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "13", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "2T", "01", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "01", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "01", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "02", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "02", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "02", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "03", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "04", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "05", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "06", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "07", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "08", "34",
	"ETSI", "2.4G", "20M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "09", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "09", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "10", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "10", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "11", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "11", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "12", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "12", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "13", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "13", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "03", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "03", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "03", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "04", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "04", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "04", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "05", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "05", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "05", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "06", "32",
	"ETSI", "2.4G", "40M", "HT", "1T", "06", "32",
	"MKK", "2.4G", "40M", "HT", "1T", "06", "30",
	"FCC", "2.4G", "40M", "HT", "1T", "07", "32",
	"ETSI", "2.4G", "40M", "HT", "1T", "07", "32",
	"MKK", "2.4G", "40M", "HT", "1T", "07", "30",
	"FCC", "2.4G", "40M", "HT", "1T", "08", "32",
	"ETSI", "2.4G", "40M", "HT", "1T", "08", "32",
	"MKK", "2.4G", "40M", "HT", "1T", "08", "30",
	"FCC", "2.4G", "40M", "HT", "1T", "09", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "09", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "09", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "10", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "10", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "10", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "11", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "11", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "11", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "03", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "03", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "04", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "04", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "05", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "05", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "06", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "07", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "07", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "08", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "08", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "09", "22",
	"ETSI", "2.4G", "40M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "09", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "10", "22",
	"ETSI", "2.4G", "40M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "10", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "11", "22",
	"ETSI", "2.4G", "40M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "11", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "03", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "03", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "04", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "04", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "05", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "05", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "06", "30",
	"ETSI", "2.4G", "40M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "06", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "07", "30",
	"ETSI", "2.4G", "40M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "07", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "08", "30",
	"ETSI", "2.4G", "40M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "08", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "09", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "10", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "10", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "11", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "14", "63"
};

void
odm_read_and_config_mp_8192e_txpwr_lmt(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
	u32     i           = 0;
	u32     array_len    = sizeof(array_mp_8192e_txpwr_lmt) / sizeof(u8 *);
	u8 **array      = (u8 **)array_mp_8192e_txpwr_lmt;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter = p_dm_odm->adapter;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(adapter);

	PlatformZeroMemory(p_hal_data->BufOfLinesPwrLmt, MAX_LINES_HWCONFIG_TXT * MAX_BYTES_LINE_HWCONFIG_TXT);
	p_hal_data->nLinesReadPwrLmt = array_len / 7;
#endif

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> odm_read_and_config_mp_8192e_txpwr_lmt\n"));

	for (i = 0; i < array_len; i += 7) {
		u8 *regulation = array[i];
		u8 *band = array[i + 1];
		u8 *bandwidth = array[i + 2];
		u8 *rate = array[i + 3];
		u8 *rf_path = array[i + 4];
		u8 *chnl = array[i + 5];
		u8 *val = array[i + 6];

		odm_config_bb_txpwr_lmt_8192e(p_dm_odm, regulation, band, bandwidth, rate, rf_path, chnl, val);
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		rsprintf((char *)p_hal_data->BufOfLinesPwrLmt[i / 7], 100, "\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\",",
			 regulation, band, bandwidth, rate, rf_path, chnl, val);
#endif
	}

}

/******************************************************************************
*                           TXPWR_LMT_8192E_SAR_5mm.TXT
******************************************************************************/

const char *array_mp_8192e_txpwr_lmt_8192e_sar_5mm[] = {
	"FCC", "2.4G", "20M", "CCK", "1T", "01", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "01", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "01", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "02", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "02", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "03", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "03", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "03", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "04", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "04", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "04", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "05", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "05", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "05", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "06", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "06", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "06", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "07", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "07", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "07", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "08", "30",
	"ETSI", "2.4G", "20M", "CCK", "1T", "08", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "08", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "09", "28",
	"ETSI", "2.4G", "20M", "CCK", "1T", "09", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "09", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "10", "28",
	"ETSI", "2.4G", "20M", "CCK", "1T", "10", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "10", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "11", "28",
	"ETSI", "2.4G", "20M", "CCK", "1T", "11", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "11", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "12", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "12", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "12", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "13", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "13", "32",
	"MKK", "2.4G", "20M", "CCK", "1T", "13", "32",
	"FCC", "2.4G", "20M", "CCK", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "14", "63",
	"MKK", "2.4G", "20M", "CCK", "1T", "14", "32",
	"FCC", "2.4G", "20M", "CCK", "2T", "01", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "01", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "01", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "02", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "02", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "02", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "03", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "03", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "03", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "04", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "04", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "04", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "05", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "05", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "05", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "06", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "06", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "06", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "07", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "07", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "07", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "08", "24",
	"ETSI", "2.4G", "20M", "CCK", "2T", "08", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "08", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "09", "22",
	"ETSI", "2.4G", "20M", "CCK", "2T", "09", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "09", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "10", "22",
	"ETSI", "2.4G", "20M", "CCK", "2T", "10", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "10", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "11", "22",
	"ETSI", "2.4G", "20M", "CCK", "2T", "11", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "11", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "CCK", "2T", "12", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "12", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "CCK", "2T", "13", "22",
	"MKK", "2.4G", "20M", "CCK", "2T", "13", "26",
	"FCC", "2.4G", "20M", "CCK", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "CCK", "2T", "14", "63",
	"MKK", "2.4G", "20M", "CCK", "2T", "14", "26",
	"FCC", "2.4G", "20M", "OFDM", "1T", "01", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "01", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "01", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "02", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "02", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "03", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "03", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "03", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "04", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "04", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "05", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "05", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "06", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "06", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "07", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "07", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "08", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "08", "32",
	"MKK", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "09", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "09", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "09", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "10", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "10", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "11", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "11", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "11", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "12", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "12", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "12", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "13", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "13", "28",
	"MKK", "2.4G", "20M", "OFDM", "1T", "13", "28",
	"FCC", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"MKK", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"FCC", "2.4G", "20M", "OFDM", "2T", "01", "26",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "01", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "01", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "02", "26",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "02", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "02", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "03", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "03", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "03", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "04", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "04", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "04", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "05", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "05", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "05", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "06", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "06", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "06", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "07", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "07", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "07", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "08", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "08", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "08", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "09", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "09", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "09", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "10", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "10", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "10", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "11", "28",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "11", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "11", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "12", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "12", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "13", "26",
	"MKK", "2.4G", "20M", "OFDM", "2T", "13", "28",
	"FCC", "2.4G", "20M", "OFDM", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "OFDM", "2T", "14", "63",
	"MKK", "2.4G", "20M", "OFDM", "2T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "1T", "01", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "01", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "01", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "02", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "02", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "03", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "03", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "03", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "04", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "04", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "04", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "05", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "05", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "05", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "06", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "06", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "06", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "07", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "07", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "07", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "08", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "08", "32",
	"MKK", "2.4G", "20M", "HT", "1T", "08", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "09", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "09", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "09", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "10", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "10", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "11", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "11", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "11", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "12", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "12", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "12", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "13", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "13", "28",
	"MKK", "2.4G", "20M", "HT", "1T", "13", "28",
	"FCC", "2.4G", "20M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "2T", "01", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "01", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "01", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "02", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "02", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "02", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "03", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "04", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "05", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "07", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "08", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "09", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "10", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "10", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "11", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "12", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "12", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "13", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "13", "28",
	"FCC", "2.4G", "20M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "2T", "01", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "01", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "01", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "02", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "02", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "02", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "03", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "04", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "05", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "07", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "08", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "09", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "10", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "10", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "11", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "12", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "12", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "13", "26",
	"MKK", "2.4G", "20M", "HT", "2T", "13", "26",
	"FCC", "2.4G", "20M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "03", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "03", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "03", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "04", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "04", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "04", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "05", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "05", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "05", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "06", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "06", "32",
	"MKK", "2.4G", "40M", "HT", "1T", "06", "30",
	"FCC", "2.4G", "40M", "HT", "1T", "07", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "07", "32",
	"MKK", "2.4G", "40M", "HT", "1T", "07", "30",
	"FCC", "2.4G", "40M", "HT", "1T", "08", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "08", "32",
	"MKK", "2.4G", "40M", "HT", "1T", "08", "30",
	"FCC", "2.4G", "40M", "HT", "1T", "09", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "09", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "09", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "10", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "10", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "10", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "11", "28",
	"ETSI", "2.4G", "40M", "HT", "1T", "11", "28",
	"MKK", "2.4G", "40M", "HT", "1T", "11", "28",
	"FCC", "2.4G", "40M", "HT", "1T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "03", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "03", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "04", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "04", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "05", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "05", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "06", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "07", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "07", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "08", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "08", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "09", "22",
	"ETSI", "2.4G", "40M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "09", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "10", "22",
	"ETSI", "2.4G", "40M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "10", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "11", "22",
	"ETSI", "2.4G", "40M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "11", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "03", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "03", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "03", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "04", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "04", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "04", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "05", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "05", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "05", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "06", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "06", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "07", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "07", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "07", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "08", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "08", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "08", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "09", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "09", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "10", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "10", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "10", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "11", "26",
	"MKK", "2.4G", "40M", "HT", "2T", "11", "26",
	"FCC", "2.4G", "40M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "14", "63"
};

void
odm_read_and_config_mp_8192e_txpwr_lmt_8192e_sar_5mm(
	struct PHY_DM_STRUCT  *p_dm_odm
)
{
	u32     i           = 0;
	u32     array_len    = sizeof(array_mp_8192e_txpwr_lmt_8192e_sar_5mm) / sizeof(u8 *);
	u8 **array      = (u8 **)array_mp_8192e_txpwr_lmt_8192e_sar_5mm;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter = p_dm_odm->adapter;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(adapter);

	PlatformZeroMemory(p_hal_data->BufOfLinesPwrLmt, MAX_LINES_HWCONFIG_TXT * MAX_BYTES_LINE_HWCONFIG_TXT);
	p_hal_data->nLinesReadPwrLmt = array_len / 7;
#endif

	PHYDM_DBG(p_dm_odm, ODM_COMP_INIT, ("===> odm_read_and_config_mp_8192e_txpwr_lmt_8192e_sar_5mm\n"));

	for (i = 0; i < array_len; i += 7) {
		u8 *regulation = array[i];
		u8 *band = array[i + 1];
		u8 *bandwidth = array[i + 2];
		u8 *rate = array[i + 3];
		u8 *rf_path = array[i + 4];
		u8 *chnl = array[i + 5];
		u8 *val = array[i + 6];

		odm_config_bb_txpwr_lmt_8192e(p_dm_odm, regulation, band, bandwidth, rate, rf_path, chnl, val);
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		rsprintf((char *)p_hal_data->BufOfLinesPwrLmt[i / 7], 100, "\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\",",
			 regulation, band, bandwidth, rate, rf_path, chnl, val);
#endif
	}

}

#endif /* end of HWIMG_SUPPORT*/
