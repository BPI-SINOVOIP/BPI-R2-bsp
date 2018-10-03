/******************************************************************************
 *
 * Copyright(c) 2017 Realtek Corporation. All rights reserved.
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

#ifndef _HALMAC_STATE_MACHINE_H_
#define _HALMAC_STATE_MACHINE_H_

enum halmac_dlfw_state {
	HALMAC_DLFW_NONE = 0,
	HALMAC_DLFW_DONE = 1,
	HALMAC_GEN_INFO_SENT = 2,

	/* Data CPU firmware download framework */
	HALMAC_DLFW_INIT = 0x11,
	HALMAC_DLFW_START = 0x12,
	HALMAC_DLFW_CONF_READY = 0x13,
	HALMAC_DLFW_CPU_READY = 0x14,
	HALMAC_DLFW_MEM_READY = 0x15,
	HALMAC_DLFW_SW_READY = 0x16,
	HALMAC_DLFW_OFLD_READY = 0x17,

	HALMAC_DLFW_UNDEFINED = 0x7F,
};

enum halmac_gpio_cfg_state {
	HALMAC_GPIO_CFG_STATE_IDLE = 0,
	HALMAC_GPIO_CFG_STATE_BUSY = 1,
	HALMAC_GPIO_CFG_STATE_UNDEFINED = 0x7F,
};

enum halmac_rsvd_pg_state {
	HALMAC_RSVD_PG_STATE_IDLE = 0,
	HALMAC_RSVD_PG_STATE_BUSY = 1,
	HALMAC_RSVD_PG_STATE_UNDEFINED = 0x7F,
};

enum halmac_efuse_cmd_construct_state {
	HALMAC_EFUSE_CMD_IDLE = 0,
	HALMAC_EFUSE_CMD_BUSY = 1,
	HALMAC_EFUSE_CMD_H2C_SENT = 2,
	HALMAC_EFUSE_CMD_STATE_NUM = 3,
	HALMAC_EFUSE_CMD_UNDEFINED = 0x7F,
};

enum halmac_cfg_para_cmd_construct_state {
	HALMAC_CFG_PARA_CMD_IDLE = 0,
	HALMAC_CFG_PARA_CMD_CONSTRUCT = 1,
	HALMAC_CFG_PARA_CMD_H2C_SENT = 2,
	HALMAC_CFG_PARA_CMD_NUM = 3,
	HALMAC_CFG_PARA_CMD_UNDEFINED = 0x7F,
};

enum halmac_scan_cmd_construct_state {
	HALMAC_SCAN_CMD_IDLE = 0,
	HALMAC_SCAN_CMD_BUFFER_CLEARED = 1,
	HALMAC_SCAN_CMD_CONSTRUCT = 2,
	HALMAC_SCAN_CMD_H2C_SENT = 3,
	HALMAC_SCAN_CMD_NUM = 4,
	HALMAC_SCAN_CMD_CONSTRUCT_UNDEFINED = 0x7F,
};

enum halmac_fw_snding_cmd_construct_state {
	HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE = 0,
	HALMAC_FW_SNDING_CMD_CONSTRUCT_SNDING = 1,
	HALMAC_FW_SNDING_CONSTRUCT_UNDEFINED = 0x7F,
};

enum halmac_api_state {
	HALMAC_API_STATE_INIT = 0,
	HALMAC_API_STATE_HALT = 1,
	HALMAC_API_STATE_UNDEFINED = 0x7F,
};

enum halmac_cmd_process_status {
	HALMAC_CMD_PROCESS_IDLE = 0x01,                 /* Init status */
	HALMAC_CMD_PROCESS_SENDING = 0x02,              /* Wait ack */
	HALMAC_CMD_PROCESS_RCVD = 0x03,                 /* Rcvd ack */
	HALMAC_CMD_PROCESS_DONE = 0x04,                 /* Event done */
	HALMAC_CMD_PROCESS_ERROR = 0x05,                /* Return code error */
	HALMAC_CMD_PROCESS_UNDEFINE = 0x7F,
};

enum halmac_mac_power {
	HALMAC_MAC_POWER_OFF = 0x0,
	HALMAC_MAC_POWER_ON = 0x1,
	HALMAC_MAC_POWER_UNDEFINE = 0x7F,
};

enum halmac_wlcpu_mode {
	HALMAC_WLCPU_ACTIVE = 0x0,
	HALMAC_WLCPU_ENTER_SLEEP = 0x1,
	HALMAC_WLCPU_SLEEP = 0x2,
	HALMAC_WLCPU_UNDEFINE = 0x7F,
};

struct halmac_efuse_state_set {
	enum halmac_efuse_cmd_construct_state efuse_cmd_construct_state;
	enum halmac_cmd_process_status process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_cfg_para_state_set {
	enum halmac_cfg_para_cmd_construct_state cfg_param_cmd_state;
	enum halmac_cmd_process_status process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_scan_state_set {
	enum halmac_scan_cmd_construct_state scan_cmd_construct_state;
	enum halmac_cmd_process_status process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_update_packet_state_set {
	enum halmac_cmd_process_status process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_iqk_state_set {
	enum halmac_cmd_process_status process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_power_tracking_state_set {
	enum halmac_cmd_process_status	process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_psd_state_set {
	enum halmac_cmd_process_status process_status;
	u16 data_size;
	u16 segment_size;
	u8 *data;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_fw_snding_state_set {
	enum halmac_fw_snding_cmd_construct_state fw_snding_cmd_construct_state;
	enum halmac_cmd_process_status process_status;
	u8 fw_return_code;
	u16 seq_num;
};

struct halmac_state {
	struct halmac_efuse_state_set efuse_state_set; /* State machine + cmd process status */
	struct halmac_cfg_para_state_set cfg_para_state_set; /* State machine + cmd process status */
	struct halmac_scan_state_set scan_state_set; /* State machine + cmd process status */
	struct halmac_update_packet_state_set update_packet_set; /* cmd process status */
	struct halmac_iqk_state_set iqk_set; /* cmd process status */
	struct halmac_power_tracking_state_set power_tracking_set; /* cmd process status */
	struct halmac_psd_state_set psd_set; /* cmd process status */
	struct halmac_fw_snding_state_set fw_snding_set; /* cmd process status */
	enum halmac_api_state api_state; /* Halmac api state */
	enum halmac_mac_power mac_power; /* 0 : power off, 1 : power on*/
	enum halmac_dlfw_state dlfw_state; /* download FW state */
	enum halmac_wlcpu_mode wlcpu_mode;
	enum halmac_gpio_cfg_state gpio_cfg_state; /* gpio state */
	enum halmac_rsvd_pg_state rsvd_pg_state; /* download rsvd page state */
};

#endif
