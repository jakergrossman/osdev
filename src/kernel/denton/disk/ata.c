#include "asm/sync/spinlock.h"
#include "denton/bits/bits.h"
#include "denton/initcall.h"
#include "denton/klog.h"
#include "denton/pci.h"
#include <denton/errno.h>
#include <asm/instr.h>
#include <asm/io.h>

enum ata_bus {
	ATA_PRIMARY_BUS = 0x1F0,
	ATA_SECONDARY_BUS = 0x170,

	ATA_IO_TO_CTL = 0x206,
};

enum {
	ATA_DEFAULT_PRIMARY_IO = 0x1F0,
	ATA_DEFAULT_PRIMARY_CTL = 0x3F6,
	ATA_DEFAULT_PRIMARY_IRQ = 14,
	ATA_DEFAULT_SECONDARY_IO = 0x170,
	ATA_DEFAULT_SECONDARY_CTL = 0x376,
	ATA_DEFAULT_SECONDARY_IRQ = 15,
};

enum ata_io_reg {
	ATA_IO_DATA_REG_RW = 0x00,
	ATA_IO_ERROR_REG_R = 0x01,
	ATA_IO_FEAT_REG_W  = 0x01,
	ATA_IO_SECTOR_CNT_RW = 0x02,
	ATA_IO_SECTOR_NUM_RW = 0X03,
	ATA_IO_SECTOR_CYL_LO_RW = 0x04,
	ATA_IO_SECTOR_CYL_HI_RW = 0x05,
	ATA_IO_SECTOR_HEAD_RW = 0x06,
	ATA_IO_SECTOR_STATUS_R = 0x07,
	ATA_IO_SECTOR_COMMAND_W = 0x07,
};

struct ata_status {
	bool err : 1;
	bool idx : 1;
	bool corr : 1;
	bool drq : 1;
	bool srv : 1;
	bool df : 1;
	bool rdy : 1;
	bool bsy : 1;
} __packed;

enum {
	ATA_ERROR_ADDR_NOTFOUND = BIT(0),
	ATA_ERROR_TRK0_NOTFOUND = BIT(1),
	ATA_ERROR_ABORT         = BIT(2),
	ATA_ERROR_MEDIA_CHG_REQ = BIT(3),
	ATA_ERROR_ID_NOTFOUND   = BIT(4),
	ATA_ERROR_MEDIA_CHGED   = BIT(5),
	ATA_ERROR_UNCORRECTABLE = BIT(6),
	ATA_ERROR_BAD_BLOCK     = BIT(7),
};

enum ata_ctl_reg {
	ATA_CTL_ALT_STATUS_R = 0x00,
	ATA_CTL_DEVICE_CTL_W = 0x00,
	ATA_CTL_DEVICE_ADDR_R = 0x01,
};

enum {
	ATA_CTL_DEVCTL_IEN_N = BIT(1),
	ATA_CTL_DEVCTL_SRST  = BIT(2),
	ATA_CTL_DEVCTL_HOB   = BIT(7),
};

static inline void ata_write_io(enum ata_bus bus, enum ata_io_reg reg, uint8_t value)
{
	uint16_t target = bus + reg;
	out8(value, target);
}

static inline void ata_write_ctl(enum ata_bus bus, enum ata_ctl_reg reg, uint8_t value)
{
	uint16_t target = bus + reg + ATA_IO_TO_CTL;
	out8(value, target);
}


static inline uint8_t ata_read_io(enum ata_bus bus, enum ata_io_reg reg)
{
	volatile struct ata_status stat;
	uint16_t target = bus + reg;
	return in8(target);
}

static inline uint8_t ata_read_ctl(enum ata_bus bus, enum ata_ctl_reg reg)
{
	uint16_t target = bus + reg + ATA_IO_TO_CTL;
	return in8(target);
}

int ata_identify(enum ata_bus bus)
{
	ata_write_ctl(bus, ATA_CTL_DEVICE_CTL_W, 0xA0);
	ata_write_io(bus, ATA_IO_SECTOR_NUM_RW, 0);
	ata_write_io(bus, ATA_IO_SECTOR_CYL_LO_RW, 0);
	ata_write_io(bus, ATA_IO_SECTOR_CYL_HI_RW, 0);

	ata_write_io(bus, ATA_IO_SECTOR_COMMAND_W, 0xEC);
	uint8_t val;
	val = ata_read_io(bus, ATA_IO_SECTOR_STATUS_R);
	if (val == 0) {
		return -ENODEV;
	}

	while ((val = ata_read_io(bus, ATA_IO_SECTOR_STATUS_R)) & 0x80) 
		;

	val = ata_read_io(bus, ATA_IO_SECTOR_CYL_LO_RW);
	if (val) {
		return -ENODEV;
	}

	if ((val = ata_read_io(bus, ATA_IO_SECTOR_CYL_HI_RW)))
	{
		return -ENODEV;
	}

	uint8_t status;
	while (!((status = ata_read_io(bus, ATA_IO_SECTOR_STATUS_R)) & 0x5))
		;

	if (status & 0x1) {
		return -1;
	}

	uint16_t id[256];
	for (int i = 0; i < 256; i++) {
		id[i] = in16(bus + ATA_IO_DATA_REG_RW);
	}

	return 0;
}

static const struct pci_dev_info ata_dev_info = {
	.vid = PCI_ANY_ID,
	.pid = PCI_ANY_ID,
	.klass = PCI_CLASS_MASS_STORAGE,
	.subclass = PCI_CLASS_STORAGE_IDE,
};

struct ata_dev {
	spinlock_t lock;
	uint16_t io_base;
	uint16_t ctl_base;
	uint16_t dma_base;
	int interrupt;

	struct pci_dev* dev;
};

static void
ata_create_disk(struct pci_dev* bus, uint16_t io, uint16_t ctl, uint16_t dma, int interrupt)
{
	klog_debug("Creating PCI ATA Device, IO Base: 0x%04X, IO Ctrl: 0x%04X, DMA: 0x%04X, IRQ: %d\n",
	          io, ctl, dma, interrupt);
}

static void __ata_detect(struct pci_dev * ata_pci, bool primary)
{
	int io_bar = primary ? 0 : 2;
	int ctl_bar = primary ? 1 : 3;
	int dma_bar = 4;

	uint16_t io_base = pci_cfg_read32(ata_pci, PCI_REG_BAR(io_bar)) & 0xFFF0;
	uint16_t ctl_base = pci_cfg_read32(ata_pci, PCI_REG_BAR(ctl_bar)) & 0xFFF0;
	uint16_t dma_base = pci_cfg_read32(ata_pci, PCI_REG_BAR(dma_bar)) & 0xFFF0;
	if (dma_base && !primary) {
		dma_base += 8;
	}
	int irq = pci_cfg_read8(ata_pci, PCI_REG_INTERRUPT);

	if (io_base <= 1) {
		io_base = primary ? ATA_DEFAULT_PRIMARY_IO : ATA_DEFAULT_SECONDARY_IO;
	}

	if (ctl_base <= 1) {
		ctl_base = primary ? ATA_DEFAULT_PRIMARY_CTL : ATA_DEFAULT_SECONDARY_CTL;
	}

	if (!pci_has_interrupt(ata_pci) || !irq) {
		irq = primary ? ATA_DEFAULT_PRIMARY_IRQ : ATA_DEFAULT_SECONDARY_IRQ;
	}

	ata_create_disk(ata_pci, io_base, ctl_base, dma_base, irq);
}

void ata_detect(void)
{
	struct pci_dev* ata_pci =
	    pci_find_device(ata_dev_info.vid, ata_dev_info.pid,
	                    ata_dev_info.klass, ata_dev_info.subclass,
	                    NULL);

	__ata_detect(ata_pci, true);
	__ata_detect(ata_pci, false);
}

static void ata_init(void)
{
	klog_debug("HI IM HERE\n");
}
initcall_device(ata, ata_init);

