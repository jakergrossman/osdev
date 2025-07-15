#include "denton/container_of.h"
#include "denton/heap.h"
#include "denton/klog.h"
#include "denton/list.h"
#include "denton/mm/mm_types.h"
#include <denton/pci.h>
#include <denton/errno.h>
#include <asm/io.h>

enum {
	PCI_CONFIG_ADDRESS = 0xCF8,
	PCI_CONFIG_DATA = 0xCFC,

	PCI_INVALID_VID = 0xFFFF,
};


enum {
	PCI_HEADER_MULTI_FUNC = 0x80,
	PCI_HEADER_BRIDGE = 0x01,
};

struct pci_dev_entry {
	struct pci_dev_info info;
	struct list_head entry;
};

static LIST_HEAD(pci_dev_list);

#define pci_from_entry(dev) \
	container_of(dev, struct pci_dev, pci_entry)

static const char * pci_class_names[] = {
	[PCI_CLASS_NONE] = "No PCI Class",
	[PCI_CLASS_MASS_STORAGE] = "Mass Storage Device",
	[PCI_CLASS_NETWORK] = "Network Controller",
	[PCI_CLASS_DISPLAY] = "Display Controller",
	[PCI_CLASS_MULTIMEDIA] = "Multimedia Controller",
	[PCI_CLASS_MEMORY] = "Memory Controller",
	[PCI_CLASS_BRIDGE] = "PCI Bridge Device",
	[PCI_CLASS_SIMPLE_COMM] = "Simple Communications Controller",
	[PCI_CLASS_BASE_SYSTEM] = "Base System Peripherals",
	[PCI_CLASS_INPUT] = "Input Device",
	[PCI_CLASS_DOCKING] = "Docking Station",
	[PCI_CLASS_PROCESSOR] = "Processor",
	[PCI_CLASS_SERIAL_BUS] = "Serial Bus",
	[PCI_CLASS_WIRELESS] = "Wireless Controller",
	[PCI_CLASS_INTELLIGENT_IO] = "Intelligent I/O Controller",
	[PCI_CLASS_SATELITE] = "Satelite Communication Controller",
	[PCI_CLASS_ENCRYPT_DECRYPT] = "Encryption/Decryption Controller",
	[PCI_CLASS_SIGNAL_PROC] = "Data Acquisition and Signal Processing Controller",
};

static const char * pci_class_storage_names[] = {
	[PCI_CLASS_STORAGE_SCSI] = "SCSI Storage Controller",
	[PCI_CLASS_STORAGE_IDE] = "IDE Interface",
	[PCI_CLASS_STORAGE_FLOPPY] = "Floppy disk controller",
	[PCI_CLASS_STORAGE_IPI] = "IPI bus controller",
	[PCI_CLASS_STORAGE_RAID] = "RAID bus controller",
	[PCI_CLASS_STORAGE_ATA] = "ATA controller",
	[PCI_CLASS_STORAGE_SATA] = "SATA controller",
	[PCI_CLASS_STORAGE_SERIAL_SCSI] = "Serial Attached SCSI controller",
	[PCI_CLASS_STORAGE_NON_VOLATILE] = "Non-Volatile memory controller",
	[PCI_CLASS_STORAGE_MASS_STORAGE] = "Mass storage controller",
};

static const char * pci_class_bridge_names[] = {
	[PCI_CLASS_BRIDGE_HOST] = "Host Bridge",
	[PCI_CLASS_BRIDGE_ISA] = "ISA Bridge",
	[PCI_CLASS_BRIDGE_EISA] = "EISA Bridge",
	[PCI_CLASS_BRIDGE_MCA] = "MCA Bridge",
	[PCI_CLASS_BRIDGE_PCI] = "PCI-to-PCI Bridge",
	[PCI_CLASS_BRIDGE_PCMCIA] = "PCMCIA Bridge",
	[PCI_CLASS_BRIDGE_NUBUS] = "NuBus Bridge",
	[PCI_CLASS_BRIDGE_CARDBUS] = "CardBus Bridge",
	[PCI_CLASS_BRIDGE_RACEWAY] = "RACEway Bridge",
	[PCI_CLASS_BRIDGE_PCI2] = "PCI-to-PCI Bridge",
	[PCI_CLASS_BRIDGE_OTHER] = "Other Bridge",
};

static const char * pci_class_display_names[] = {
	[PCI_CLASS_DISPLAY_VGA] = "VGA Compatible Controller",
	[PCI_CLASS_DISPLAY_XGA] = "XGA Controller",
	[PCI_CLASS_DISPLAY_3D] = "3D Controller",
	[PCI_CLASS_DISPLAY_OTHER] = "Other Controller",
};

static const char * pci_class_network_names[] = {
	[PCI_CLASS_NETWORK_ETHERNET] = "Ethernet Controller",
	[PCI_CLASS_NETWORK_TOKEN] = "Token Ring Controller",
	[PCI_CLASS_NETWORK_FDDI] = "FDDI Controller",
	[PCI_CLASS_NETWORK_ATM] = "ATM Controller",
	[PCI_CLASS_NETWORK_ISDN] = "ISDN Controller",
	[PCI_CLASS_NETWORK_WORLDFIP] = "WorldFip Controller",
	[PCI_CLASS_NETWORK_PICMG] = "PICMG 2.14 Multi Computing Controller",
	[PCI_CLASS_NETWORK_INFINIBAND] = "Infiniband Controller",
	[PCI_CLASS_NETWORK_FABRIC] = "Fabric Controller",
	[PCI_CLASS_NETWORK_OTHER] = "Other",
};

static const char* * pci_class_device_names[PCI_CLASS_UNKNOWN] = {
	[PCI_CLASS_MASS_STORAGE] = pci_class_storage_names,
	[PCI_CLASS_BRIDGE] = pci_class_bridge_names,
	[PCI_CLASS_DISPLAY] = pci_class_display_names,
	[PCI_CLASS_NETWORK] = pci_class_network_names,
};

static bool pci_known_class(enum pci_class klass)
{
	switch (klass) {
		case PCI_CLASS_MASS_STORAGE:
		case PCI_CLASS_NETWORK:
		case PCI_CLASS_DISPLAY:
		case PCI_CLASS_MULTIMEDIA:
		case PCI_CLASS_MEMORY:
		case PCI_CLASS_BRIDGE:
		case PCI_CLASS_SIMPLE_COMM:
		case PCI_CLASS_BASE_SYSTEM:
		case PCI_CLASS_INPUT:
		case PCI_CLASS_DOCKING:
		case PCI_CLASS_PROCESSOR:
		case PCI_CLASS_SERIAL_BUS:
		case PCI_CLASS_WIRELESS:
		case PCI_CLASS_INTELLIGENT_IO:
		case PCI_CLASS_SATELITE:
		case PCI_CLASS_ENCRYPT_DECRYPT:
		case PCI_CLASS_SIGNAL_PROC:
			return true;

		case PCI_CLASS_NONE:
		case PCI_CLASS_UNKNOWN:
		default:
			return false;
		}
	}

static void pci_get_class_names(enum pci_class klass, uint8_t subclass,
                                const char* * klass_name, const char* * subclass_name)
{
	*klass_name = NULL;
	*subclass_name = NULL;

	if (pci_known_class(klass)) {
		*klass_name = pci_class_names[klass];
		const char** subclass_names = pci_class_device_names[klass];
		if (*klass_name && subclass_names) {
			const char* name = pci_class_device_names[klass][subclass];
			if (name) {
				*subclass_name = name;
			}
		}
	}
}

static void pci_get_dev_vendor(struct pci_dev * dev, uint16_t * vendor, uint16_t * device)
{
	*vendor = pci_cfg_read16(dev, 0);
	*device = pci_cfg_read16(dev, 2);
}

static int pci_init_dev_info(struct pci_dev * dev)
{
	pci_get_dev_vendor(dev, &dev->info.vid, &dev->info.pid);

	if (dev->info.vid == PCI_INVALID_VID) {
		return -EINVAL;
	}

	dev->info.header_type = pci_cfg_read8(dev, PCI_REG_HEADER_TYPE);
	dev->info.klass = pci_cfg_read8(dev, PCI_REG_CLASS);
	dev->info.subclass = pci_cfg_read8(dev, PCI_REG_SUBCLASS);
	dev->info.procif = pci_cfg_read8(dev, PCI_REG_PROG_INTF);

	return 0;
}

static int pci_dev_info_cmp(struct pci_dev_info * a, struct pci_dev_info * b)
{
	/* concat for easy cmp */
	uint32_t aval = (a->func   << 0 ) |
	                (a->device << 8 ) |
	                (a->bus    << 16);
	uint32_t bval = (b->func   << 0 ) |
	                (b->device << 8 ) |
	                (b->bus    << 16);

	if (aval < bval) {
		return -1;
	} else if (aval > bval) {
		return 1;
	} else {
		return 0;
	}
}

static int pci_dev_cmp(struct list_head * lista, struct list_head * listb)
{
	struct pci_dev* a = container_of(lista, struct pci_dev, pci_entry);
	struct pci_dev* b = container_of(listb, struct pci_dev, pci_entry);
	return pci_dev_info_cmp(&a->info, &b->info);
}

static void pci_add_dev(struct pci_dev_info * info)
{
	struct pci_dev* dev = kmalloc(sizeof(*dev), PGF_KERNEL);
	list_init(&dev->pci_entry);
	dev->info = *info;

	const char* klassname;
	const char* subklassname;
	pci_get_class_names(info->klass, info->subclass, &klassname, &subklassname);

	klog_debug("PCI %02d:%02d.%1d: %04X:%04X - %s%s%s\n",
	          dev->info.bus, dev->info.device,
	          dev->info.func, info->vid, info->pid,
	          (klassname ? klassname : "UNKNOWN_CLASS"),
	          (subklassname ? ", " : ""),
	          (subklassname ? subklassname : "")
	);

	list_add_sorted(&dev->pci_entry, &pci_dev_list, pci_dev_cmp);

	if (info->header_type & PCI_HEADER_BRIDGE) {
		pci_enumerate(pci_cfg_read8(dev, PCI_REG_SECONDARY_BUS));
	}
}

void pci_enumerate(int busno)
{
	for (int slot = 0; slot < 32; slot++) {
		/** dummy PCI device for query */
		struct pci_dev dev = {
			.info.bus = busno,
			.info.device = slot,
		};

		if (pci_init_dev_info(&dev)) {
			continue;
		}

		pci_add_dev(&dev.info);

		if (dev.info.header_type & PCI_HEADER_MULTI_FUNC) {
			for(dev.info.func = 1; dev.info.func < 8; dev.info.func++) {
				if (pci_init_dev_info(&dev)) {
					continue;
				}

				pci_add_dev(&dev.info);
			}
		}
	}
}

pci_addr_t pci_addr_create(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg_off)
{
	return (pci_addr_t) {
		.bus_num = bus,
		.dev_num = dev,
		.fn_num = func,
		.reg_off = reg_off,
		.en = 1,
	};
}

uint32_t pci_cfg_read32(struct pci_dev * dev, uint8_t reg)
{
	pci_addr_t addr = pci_addr_create(dev->info.bus, dev->info.device, dev->info.func, reg);
	out32(addr.addr_word, PCI_CONFIG_ADDRESS);
	return in32(PCI_CONFIG_DATA);
}

uint16_t pci_cfg_read16(struct pci_dev * dev, uint8_t reg)
{
	pci_addr_t addr = pci_addr_create(dev->info.bus, dev->info.device, dev->info.func, reg);
	out32(addr.addr_word, PCI_CONFIG_ADDRESS);
	return in16(PCI_CONFIG_DATA);
}

uint8_t pci_cfg_read8(struct pci_dev * dev, uint8_t reg)
{
	pci_addr_t addr = pci_addr_create(dev->info.bus, dev->info.device, dev->info.func, reg);
	out32(addr.addr_word, PCI_CONFIG_ADDRESS);
	return in8(PCI_CONFIG_DATA);
}

void pci_cfg_write32(struct pci_dev * dev, uint8_t reg, uint32_t value)
{
	pci_addr_t addr = pci_addr_create(dev->info.bus, dev->info.device, dev->info.func, reg);
	out32(addr.addr_word, PCI_CONFIG_ADDRESS);
	out32(value, PCI_CONFIG_DATA);
}

void pci_cfg_write16(struct pci_dev * dev, uint8_t reg, uint16_t value)
{
	pci_addr_t addr = pci_addr_create(dev->info.bus, dev->info.device, dev->info.func, reg);
	out32(addr.addr_word, PCI_CONFIG_ADDRESS);
	out16(value, PCI_CONFIG_DATA);
}

void pci_cfg_write8(struct pci_dev * dev, uint8_t reg, uint8_t  value)
{
	pci_addr_t addr = pci_addr_create(dev->info.bus, dev->info.device, dev->info.func, reg);
	out32(addr.addr_word, PCI_CONFIG_ADDRESS);
	out8(value, PCI_CONFIG_DATA);
}

static bool pci_match(struct pci_dev_info * query, struct pci_dev_info * target)
{
	if ((query->vid != PCI_ANY_ID) && (query->vid != target->vid)) {
		return false;
	}

	if ((query->pid != PCI_ANY_ID) && (query->pid != target->pid)) {
		return false;
	}

	if ((query->klass != (uint8_t)PCI_ANY_ID) && (query->klass != target->klass)) {
		return false;
	}

	if ((query->subclass != (uint8_t)PCI_ANY_ID) && (query->subclass != target->subclass)) {
		return false;
	}

	return true;
}

struct pci_dev * pci_find_device(
	uint16_t vid, uint16_t pid,
	uint16_t klass, uint16_t subclass,
	struct pci_dev * from
) {
	struct list_head* iter = from ? &from->pci_entry : &pci_dev_list;
	struct pci_dev_info query = {
		.vid = vid,
		.pid = pid,
		.klass = klass,
		.subclass = subclass,
	};
	list_for_each(iter, &pci_dev_list) {
		struct pci_dev* target = pci_from_entry(iter);
		if (pci_match(&query, &target->info)) {
			return target;
		}
	}
	return NULL;
}

bool pci_has_interrupt(struct pci_dev * dev)
{
    uint8_t int_line = pci_cfg_read8(dev, PCI_REG_INTERRUPT);
    uint8_t result;

    pci_cfg_write8(dev, PCI_REG_INTERRUPT, 0xFE);
    result = pci_cfg_read8(dev, PCI_REG_INTERRUPT);
    pci_cfg_write8(dev, PCI_REG_INTERRUPT, int_line);

    return result == 0xFE;
}

void pci_init(void)
{
	pci_enumerate(0);
}
