
#ifdef CONFIG_RTL8812A
#ifndef _FW_HEADER_8812A_H
#define _FW_HEADER_8812A_H

#ifdef LOAD_FW_HEADER_FROM_DRIVER
#if (defined(CONFIG_AP_WOWLAN) || (DM_ODM_SUPPORT_TYPE & (ODM_AP)))
extern u8 array_mp_8812a_fw_ap[23716];
extern u32 array_length_mp_8812a_fw_ap;
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN)) || (DM_ODM_SUPPORT_TYPE & (ODM_CE))
extern u8 array_mp_8812a_fw_nic[32654];
extern u32 array_length_mp_8812a_fw_nic;
extern u8 array_mp_8812a_fw_nic_bt[29398];
extern u32 array_length_mp_8812a_fw_nic_bt;
extern u8 array_mp_8812a_fw_wowlan[29956];
extern u32 array_length_mp_8812a_fw_wowlan;
#endif
#endif /* end of LOAD_FW_HEADER_FROM_DRIVER*/

#endif
#endif /* end of HWIMG_SUPPORT*/
