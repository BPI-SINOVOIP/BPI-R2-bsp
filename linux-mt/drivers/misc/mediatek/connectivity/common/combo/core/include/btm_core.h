/*! \file
    \brief  Declaration of library functions

    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/



#ifndef _BTM_CORE_H
#define _BTM_CORE_H

#include "stp_wmt.h"
#include "wmt_plat.h"
#include "wmt_idc.h"
#include "osal.h"

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#define STP_BTM_OPERATION_FAIL    (-1)
#define STP_BTM_OPERATION_SUCCESS (0)

#define STP_BTM_OP_BUF_SIZE (64)

#define BTM_THREAD_NAME "mtk_stp_btm"
/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/



/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_STP_BTM_OPID_T {
	STP_OPID_BTM_RETRY = 0x0,
	STP_OPID_BTM_RST = 0x1,
	STP_OPID_BTM_DBG_DUMP = 0x2,
	STP_OPID_BTM_DUMP_TIMEOUT = 0x3,
	STP_OPID_BTM_POLL_CPUPCR = 0x4,
	STP_OPID_BTM_PAGED_DUMP = 0x5,
	STP_OPID_BTM_FULL_DUMP = 0x6,
	STP_OPID_BTM_PAGED_TRACE = 0x7,
	STP_OPID_BTM_FORCE_FW_ASSERT = 0x8,
#if CFG_WMT_LTE_COEX_HANDLING
	STP_OPID_BTM_WMT_LTE_COEX = 0x9,
#endif
	STP_OPID_BTM_EXIT,
	STP_OPID_BTM_NUM
} ENUM_STP_BTM_OPID_T, *P_ENUM_STP_BTM_OPID_T;

typedef OSAL_OP_DAT STP_BTM_OP;
typedef P_OSAL_OP_DAT P_STP_BTM_OP;

typedef struct mtk_stp_btm {
	OSAL_THREAD BTMd;	/* main thread (wmtd) handle */
	OSAL_EVENT STPd_event;
	OSAL_UNSLEEPABLE_LOCK wq_spinlock;

	OSAL_OP_Q rFreeOpQ;	/* free op queue */
	OSAL_OP_Q rActiveOpQ;	/* active op queue */
	OSAL_OP arQue[STP_BTM_OP_BUF_SIZE];	/* real op instances */
	P_OSAL_OP pCurOP;	/* current op */

	/*wmt_notify */
	 INT32 (*wmt_notify)(MTKSTP_BTM_WMT_OP_T);
} MTKSTP_BTM_T;


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/


/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

INT32 stp_btm_notify_wmt_rst_wq(MTKSTP_BTM_T *stp_btm);
INT32 stp_btm_notify_stp_retry_wq(MTKSTP_BTM_T *stp_btm);
INT32 stp_btm_notify_coredump_timeout_wq(MTKSTP_BTM_T *stp_btm);
INT32 stp_btm_notify_wmt_dmp_wq(MTKSTP_BTM_T *stp_btm);
INT32 stp_btm_deinit(MTKSTP_BTM_T *stp_btm);
INT32 stp_btm_reset_btm_wq(MTKSTP_BTM_T *stp_btm);
INT32 stp_btm_sort_btm_wq(MTKSTP_BTM_T *stp_btm);
INT32 stp_notify_btm_dump(MTKSTP_BTM_T *stp_btm);
INT32 stp_notify_btm_do_fw_assert(MTKSTP_BTM_T *stp_btm);
INT32 stp_notify_btm_handle_wmt_lte_coex(MTKSTP_BTM_T *stp_btm);
#if WMT_PLAT_ALPS
#ifdef CONFIG_MTK_SERIAL
extern void dump_uart_history(void);
#else
static inline void dump_uart_history(void) {};
#endif
#endif
INT32 wmt_btm_trigger_reset(VOID);
INT32 stp_btm_set_current_op(MTKSTP_BTM_T *stp_btm, P_OSAL_OP pOp);
P_OSAL_OP stp_btm_get_current_op(MTKSTP_BTM_T *stp_btm);

MTKSTP_BTM_T *stp_btm_init(VOID);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif
