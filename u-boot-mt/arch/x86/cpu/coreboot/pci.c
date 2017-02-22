/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2008,2009
 * Graeme Russ, <graeme.russ@gmail.com>
 *
 * (C) Copyright 2002
 * Daniel Engström, Omicron Ceti AB, <daniel@omicron.se>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <pci.h>
#include <asm/pci.h>

static struct pci_controller coreboot_hose;

static void config_pci_bridge(struct pci_controller *hose, pci_dev_t dev,
			      struct pci_config_table *table)
{
	u8 secondary;
	hose->read_byte(hose, dev, PCI_SECONDARY_BUS, &secondary);
	hose->last_busno = max(hose->last_busno, secondary);
	pci_hose_scan_bus(hose, secondary);
}

static struct pci_config_table pci_coreboot_config_table[] = {
	/* vendor, device, class, bus, dev, func */
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_PCI,
		PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, &config_pci_bridge },
	{}
};

void pci_init_board(void)
{
	coreboot_hose.config_table = pci_coreboot_config_table;
	coreboot_hose.first_busno = 0;
	coreboot_hose.last_busno = 0;

	pci_set_region(coreboot_hose.regions + 0, 0x0, 0x0, 0xffffffff,
		PCI_REGION_MEM);
	coreboot_hose.region_count = 1;

	pci_setup_type1(&coreboot_hose);

	pci_register_hose(&coreboot_hose);

	pci_hose_scan(&coreboot_hose);
}
