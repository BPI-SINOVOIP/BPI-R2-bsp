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
******************************************************************************/

/*Image2HeaderVersion: 2.3.1*/
#if (RTL8723D_SUPPORT == 1)
#ifndef __INC_MP_MAC_HW_IMG_8723D_H
#define __INC_MP_MAC_HW_IMG_8723D_H


/******************************************************************************
*                           MAC_REG.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8723D_MAC_REG(/* TC: Test Chip, MP: MP Chip*/
	IN   PDM_ODM_T  pDM_Odm
);
u4Byte ODM_GetVersion_MP_8723D_MAC_REG(void);

#endif
#endif /* end of HWIMG_SUPPORT*/

