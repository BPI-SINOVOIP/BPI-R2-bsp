/*
* Copyright (C) 2016 MediaTek Inc.
*
* This program is free software: you can redistribute it and/or modify it under the terms of the
* GNU General Public License version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Id: stats.c#1
 */

/*
 * ! \file stats.c
 *  \brief This file includes statistics support.
 */

/*******************************************************************************
 *						C O M P I L E R	 F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *						E X T E R N A L	R E F E R E N C E S
 ********************************************************************************
 */
#include "precomp.h"

#if (CFG_SUPPORT_STATISTICS == 1)

enum EVENT_TYPE {
	EVENT_RX,
	EVENT_TX,
};
/*******************************************************************************
*						C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*						F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*						P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*						P R I V A T E  F U N C T I O N S
********************************************************************************
*/
static VOID statsParsePktInfo(PUINT_8 pucPkt, struct sk_buff *skb, UINT_8 status, UINT_8 eventType)
{
	/* get ethernet protocol */
	UINT_16 u2EtherType = (pucPkt[ETH_TYPE_LEN_OFFSET] << 8) | (pucPkt[ETH_TYPE_LEN_OFFSET + 1]);
	PUINT_8 pucEthBody = &pucPkt[ETH_HLEN];

	switch (u2EtherType) {
	case ETH_P_ARP:
	{
		UINT_16 u2OpCode = (pucEthBody[6] << 8) | pucEthBody[7];

		switch (eventType) {
		case EVENT_RX:
			GLUE_SET_INDEPENDENT_PKT(skb, TRUE);
			if (u2OpCode == ARP_PRO_REQ)
				DBGLOG(RX, TRACE, "<RX> Arp Req From IP: %d.%d.%d.%d\n",
					pucEthBody[14], pucEthBody[15], pucEthBody[16], pucEthBody[17]);
			else if (u2OpCode == ARP_PRO_RSP)
				DBGLOG(RX, TRACE, "<RX> Arp Rsp from IP: %d.%d.%d.%d\n",
					pucEthBody[14], pucEthBody[15], pucEthBody[16], pucEthBody[17]);
			break;
		case EVENT_TX:
			if (u2OpCode == ARP_PRO_REQ)
				DBGLOG(TX, TRACE, "<TX> Arp Req to IP: %d.%d.%d.%d\n",
					pucEthBody[24], pucEthBody[25], pucEthBody[26], pucEthBody[27]);
			else if (u2OpCode == ARP_PRO_RSP)
				DBGLOG(TX, TRACE, "<TX> Arp Rsp to IP: %d.%d.%d.%d\n",
					pucEthBody[24], pucEthBody[25], pucEthBody[26], pucEthBody[27]);
			break;
		}
		break;
	}
	case ETH_P_IPV4:
	{
		UINT_8 ucIpProto = pucEthBody[9]; /* IP header without options */
		UINT_8 ucIpVersion = (pucEthBody[0] & IPVH_VERSION_MASK) >> IPVH_VERSION_OFFSET;
		UINT_16 u2IpId = *(UINT_16 *) &pucEthBody[4];

		if (ucIpVersion != IPVERSION)
			break;
		switch (ucIpProto) {
		case IP_PRO_ICMP:
		{
			/* the number of ICMP packets is seldom so we print log here */
			UINT_8 ucIcmpType;
			UINT_16 u2IcmpId, u2IcmpSeq;
			PUINT_8 pucIcmp = &pucEthBody[20];

			ucIcmpType = pucIcmp[0];
			if (ucIcmpType == 3) /* don't log network unreachable packet */
				break;
			u2IcmpId = *(UINT_16 *) &pucIcmp[4];
			u2IcmpSeq = *(UINT_16 *) &pucIcmp[6];
			switch (eventType) {
			case EVENT_RX:
				GLUE_SET_INDEPENDENT_PKT(skb, TRUE);
				DBGLOG(RX, TRACE, "<RX> ICMP: Type %d, Id BE 0x%04x, Seq BE 0x%04x\n",
							ucIcmpType, u2IcmpId, u2IcmpSeq);
				break;
			case EVENT_TX:
				DBGLOG(TX, TRACE, "<TX> ICMP: Type %d, Id 0x04%x, Seq BE 0x%04x\n",
								ucIcmpType, u2IcmpId, u2IcmpSeq);
				break;
			}
			break;
		}
		case IP_PRO_UDP:
		{
			/* the number of DHCP packets is seldom so we print log here */
			PUINT_8 pucUdp = &pucEthBody[20];
			PUINT_8 pucBootp = &pucUdp[8];
			UINT_16 u2UdpDstPort;
			UINT_16 u2UdpSrcPort;
			UINT_32 u4TransID;

			u2UdpDstPort = (pucUdp[2] << 8) | pucUdp[3];
			u2UdpSrcPort = (pucUdp[0] << 8) | pucUdp[1];
			if ((u2UdpDstPort == UDP_PORT_DHCPS) || (u2UdpDstPort == UDP_PORT_DHCPC)) {
			u4TransID = pucBootp[4]<<24  | pucBootp[5]<<16 | pucBootp[6]<<8  | pucBootp[7];
			switch (eventType) {
			case EVENT_RX:
					GLUE_SET_INDEPENDENT_PKT(skb, TRUE);
					DBGLOG(RX, INFO, "<RX> DHCP: IPID 0x%02x, MsgType 0x%x, TransID 0x%04x\n",
								u2IpId, pucBootp[0], u4TransID);
				break;
			case EVENT_TX:
					DBGLOG(TX, INFO, "<TX> DHCP: IPID 0x%02x, MsgType 0x%x, TransID 0x%04x\n",
								u2IpId, pucBootp[0], u4TransID);
				break;
				}
			} else if (u2UdpSrcPort == UDP_PORT_DNS) { /* tx dns */
				UINT_16 u2TransId = (pucBootp[0] << 8) | pucBootp[1];

				if (eventType == EVENT_RX) {
					DBGLOG(RX, INFO,
						"<RX> DNS: IPID 0x%02x, TransID 0x%04x\n", u2IpId, u2TransId);
				}
			}
			break;
		}
		}
		break;
	}
	case ETH_P_1X:
	{
		PUINT_8 pucEapol = pucEthBody;
		UINT_8 ucEapolType = pucEapol[1];

		switch (ucEapolType) {
		case 0: /* eap packet */
			switch (eventType) {
			case EVENT_RX:
				DBGLOG(RX, INFO, "<RX> EAP Packet: code %d, id %d, type %d\n",
						pucEapol[4], pucEapol[5], pucEapol[7]);
				break;
			case EVENT_TX:
				DBGLOG(TX, INFO, "<TX> EAP Packet: code %d, id %d, type %d\n",
						pucEapol[4], pucEapol[5], pucEapol[7]);
				break;
			}
			break;
		case 1: /* eapol start */
			switch (eventType) {
			case EVENT_RX:
				DBGLOG(RX, INFO, "<RX> EAPOL: start\n");
				break;
			case EVENT_TX:
				DBGLOG(TX, INFO, "<TX> EAPOL: start\n");
				break;
			}
			break;
		case 3: /* key */
			switch (eventType) {
			case EVENT_RX:
				DBGLOG(RX, INFO, "<RX> EAPOL: key, KeyInfo 0x%04x\n",
						*((PUINT_16)(&pucEapol[5])));
				break;
			case EVENT_TX:
				DBGLOG(TX, INFO, "<TX> EAPOL: key, KeyInfo 0x%04x\n",
						*((PUINT_16)(&pucEapol[5])));
				break;
			}

			break;
		}
		break;
	}
	case ETH_WPI_1X:
	{
		UINT_8 ucSubType = pucEthBody[3]; /* sub type filed*/
		UINT_16 u2Length = *(PUINT_16)&pucEthBody[6];
		UINT_16 u2Seq = *(PUINT_16)&pucEthBody[8];

		switch (eventType) {
		case EVENT_RX:
			DBGLOG(RX, INFO, "<RX> WAPI: subType %d, Len %d, Seq %d\n",
					ucSubType, u2Length, u2Seq);
			break;
		case EVENT_TX:
			DBGLOG(TX, INFO, "<TX> WAPI: subType %d, Len %d, Seq %d\n",
					ucSubType, u2Length, u2Seq);
			break;
		}
		break;
	}
	case 0x890d:
		switch (eventType) {
		case EVENT_RX:
			DBGLOG(RX, INFO,
				"<RX> TDLS type %d, category %d, Action %d, Token %d\n",
				pucEthBody[0], pucEthBody[1], pucEthBody[2], pucEthBody[3]);
			break;
		case EVENT_TX:
			DBGLOG(TX, INFO,
				"<TX> TDLS type %d, category %d, Action %d, Token %d\n",
				pucEthBody[0], pucEthBody[1], pucEthBody[2], pucEthBody[3]);
			break;
		}
		break;
	}
}
/*----------------------------------------------------------------------------*/
/*! \brief  This routine is called to display rx packet information.
*
* \param[in] pPkt			Pointer to the packet
* \param[out] None
*
* \retval None
*/
/*----------------------------------------------------------------------------*/
VOID StatsRxPktInfoDisplay(P_SW_RFB_T prSwRfb)
{
	PUINT_8 pPkt = NULL;
	struct sk_buff *skb = NULL;

	if (prSwRfb->u2PacketLen <= ETHER_HEADER_LEN)
		return;

	pPkt = prSwRfb->pvHeader;
	if (!pPkt)
		return;

	skb = (struct sk_buff *)(prSwRfb->pvPacket);
	if (!skb)
		return;

	statsParsePktInfo(pPkt, skb, 0, EVENT_RX);
}

/*----------------------------------------------------------------------------*/
/*! \brief  This routine is called to display tx packet information.
*
* \param[in] pPkt			Pointer to the packet
* \param[out] None
*
* \retval None
*/
/*----------------------------------------------------------------------------*/
VOID StatsTxPktInfoDisplay(UINT_8 *pPkt)
{
	UINT_16 u2EtherTypeLen;

	u2EtherTypeLen = (pPkt[ETH_TYPE_LEN_OFFSET] << 8) | (pPkt[ETH_TYPE_LEN_OFFSET + 1]);
	statsParsePktInfo(pPkt, NULL, 0, EVENT_TX);

}

#endif /* CFG_SUPPORT_STATISTICS */

/* End of stats.c */
