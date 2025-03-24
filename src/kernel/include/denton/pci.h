#ifndef __DENTON_PCI_H
#define __DENTON_PCI_H

#include <denton/list.h>
#include <denton/compiler.h>

#include <stdint.h>

#define PCI_ANY_ID (uint16_t)(~0U)
#define PCI_REG_BAR(n) (0x10 + ((n) * 4))

struct pci_dev_info {
	uint8_t bus;
	uint8_t device;
	uint8_t func;
	uint8_t header_type;
	uint8_t klass, subclass;
	uint8_t procif;
	uint16_t vid, pid;
};

struct pci_dev {
	struct pci_dev_info info;
	struct list_head pci_entry;
};



typedef struct pci_address {
	union {
		uint32_t addr_word;
		struct {
			uint32_t reg_off  : 8;
			uint32_t fn_num   : 3;
			uint32_t dev_num  : 5;
			uint32_t bus_num  : 8;
			uint32_t reserved : 7;
			uint32_t en       : 1;
		};
	};
} __packed pci_addr_t;

pci_addr_t pci_addr_create(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg_off);

uint32_t pci_cfg_read32(struct pci_dev * dev, uint8_t reg);
uint16_t pci_cfg_read16(struct pci_dev * dev, uint8_t reg);
uint8_t  pci_cfg_read8 (struct pci_dev * dev, uint8_t reg);

void     pci_cfg_write32(struct pci_dev * dev, uint8_t reg, uint32_t value);
void     pci_cfg_write16(struct pci_dev * dev, uint8_t reg, uint16_t value);
void     pci_cfg_write8 (struct pci_dev * dev, uint8_t reg, uint8_t  value);

void     pci_enumerate(int busno);

void     pci_init(void);

bool     pci_has_interrupt(struct pci_dev * dev);

struct pci_dev * pci_find_device(uint16_t vid, uint16_t pid,
                                 uint16_t klass, uint16_t subclass, 
                                 struct pci_dev * from);

enum pci_class {
	PCI_CLASS_NONE,
	PCI_CLASS_MASS_STORAGE,
	PCI_CLASS_NETWORK,
	PCI_CLASS_DISPLAY,
	PCI_CLASS_MULTIMEDIA,
	PCI_CLASS_MEMORY,
	PCI_CLASS_BRIDGE,
	PCI_CLASS_SIMPLE_COMM,
	PCI_CLASS_BASE_SYSTEM,
	PCI_CLASS_INPUT,
	PCI_CLASS_DOCKING,
	PCI_CLASS_PROCESSOR,
	PCI_CLASS_SERIAL_BUS,
	PCI_CLASS_WIRELESS,
	PCI_CLASS_INTELLIGENT_IO,
	PCI_CLASS_SATELITE,
	PCI_CLASS_ENCRYPT_DECRYPT,
	PCI_CLASS_SIGNAL_PROC,
	PCI_CLASS_UNKNOWN = 0xFF,
};

enum pci_class_storage {
	PCI_CLASS_STORAGE_SCSI = 0,
	PCI_CLASS_STORAGE_IDE = 1,
	PCI_CLASS_STORAGE_FLOPPY = 2,
	PCI_CLASS_STORAGE_IPI = 3,
	PCI_CLASS_STORAGE_RAID = 4,
	PCI_CLASS_STORAGE_ATA = 5,
	PCI_CLASS_STORAGE_SATA = 6,
	PCI_CLASS_STORAGE_SERIAL_SCSI = 7,
	PCI_CLASS_STORAGE_NON_VOLATILE = 8,
	PCI_CLASS_STORAGE_MASS_STORAGE = 80,
};

enum pci_class_bridge {
	PCI_CLASS_BRIDGE_HOST = 0,
	PCI_CLASS_BRIDGE_ISA = 1,
	PCI_CLASS_BRIDGE_EISA = 2,
	PCI_CLASS_BRIDGE_MCA = 3,
	PCI_CLASS_BRIDGE_PCI = 4,
	PCI_CLASS_BRIDGE_PCMCIA = 5,
	PCI_CLASS_BRIDGE_NUBUS = 6,
	PCI_CLASS_BRIDGE_CARDBUS = 7,
	PCI_CLASS_BRIDGE_RACEWAY = 8,
	PCI_CLASS_BRIDGE_PCI2 = 9,
	PCI_CLASS_BRIDGE_OTHER = 0x80,
};

enum pci_class_display {
	PCI_CLASS_DISPLAY_VGA = 0x0,
	PCI_CLASS_DISPLAY_XGA = 0x1,
	PCI_CLASS_DISPLAY_3D = 0x2,
	PCI_CLASS_DISPLAY_OTHER = 0x80,
};

enum pci_class_network {
	PCI_CLASS_NETWORK_ETHERNET,
	PCI_CLASS_NETWORK_TOKEN,
	PCI_CLASS_NETWORK_FDDI,
	PCI_CLASS_NETWORK_ATM,
	PCI_CLASS_NETWORK_ISDN,
	PCI_CLASS_NETWORK_WORLDFIP,
	PCI_CLASS_NETWORK_PICMG,
	PCI_CLASS_NETWORK_INFINIBAND,
	PCI_CLASS_NETWORK_FABRIC,
	PCI_CLASS_NETWORK_OTHER,
};

enum pci_reg {
	PCI_REG_VID = 0x00,
	PCI_REG_PID = 0x02,
	PCI_REG_CMD = 0x04,
	PCI_REG_STATUS = 0x06,
	PCI_REG_REVID = 0x08,
	PCI_REG_PROG_INTF = 0x09,
	PCI_REG_SUBCLASS = 0x0A,
	PCI_REG_CLASS = 0x0B,
	PCI_REG_HEADER_TYPE = 0x0E,

	PCI_REG_PRIMARY_BUS = 0x18,
	PCI_REG_SECONDARY_BUS = 0x19,

	PCI_REG_INTERRUPT = 0x3C,
};

#endif
