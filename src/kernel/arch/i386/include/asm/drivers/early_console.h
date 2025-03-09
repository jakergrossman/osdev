#ifndef __ARCH_I386_DRIVERS_COM_SERIAL_H
#define __ARCH_I386_DRIVERS_COM_SERIAL_H

#include <stdint.h>

enum com_port {
	COM_NULL = 0x000,
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,
	COM5 = 0x5F8,
	COM6 = 0x4F8,
	COM7 = 0x5E8,
	COM8 = 0x4E8,
};

#define TXR 0
#define RXR 0
#define IER 1
#define   IER_RX                BIT(0)
#define   IER_TX                BIT(1)
#define   IER_RX_STAT           BIT(2)
#define   IER_MODEM_STAT        BIT(3)
#define IIR 2
#define   IIR_PENDING           BIT(0)
#define   IIR_STATE_SHIFT       1U
#define   IIR_STATE_WIDTH       2U
#define     IIR_STATE_MASK      GENMASK(IIR_STATE_WIDTH+IIR_STATE_SHIFT, IIR_STATE_SHIFT)
#define     IIR_STATE_MODEM     (0U << IIR_STATE_SHIFT)
#define     IIR_STATE_TX_EMPTY  (1U << IIR_STATE_SHIFT)
#define     IIR_STATE_RX_READY  (2U << IIR_STATE_SHIFT)
#define     IIR_STATE_RX_STAT   (3U << IIR_STATE_SHIFT)
#define   IIR_TIMEOUT           BIT(3)
#define   IIR_FIFO_SHIFT        6U
#define   IIR_FIFO_WIDTH        2U
#define     IIR_FIFO_MASK       GENMASK(IIR_FIFO_WIDTH+IIR_FIFO_SHIFT, IIR_FIFO_SHIFT)
#define     IIR_FIFO_MODEM      (0U << IIR_FIFO_SHIFT)
#define     IIR_FIFO_NONE       (1U << IIR_FIFO_SHIFT)
#define     IIR_FIFO_UNUSABLE   (2U << IIR_FIFO_SHIFT)
#define     IIR_FIFO_ENABLED    (3U << IIR_FIFO_SHIFT)
#define FCR 2
#define   FCR_EN BIT            BIT(0)
#define   FCR_RX_CLR            BIT(1)
#define   FCR_TX_CLR            BIT(2)
#define   FCR_DMA_MODE          BIT(3)
#define   FCR_IRQ_TRG_SHIFT     6U
#define   FCR_IRQ_TRG_WIDTH     2U
#define   FCR_IRQ_TRG_MASK      GENMASK(IIR_IRQ_TRG_WIDTH+IIR_IRQ_TRG_SHIFT, IIR_IRQ_TRG_SHIFT)
#define     FCR_IRQ_TRG_1BYTE   (0U << FCR_IRQ_TRG_MASK)
#define     FCR_IRQ_TRG_4BYTE   (1U << FCR_IRQ_TRG_MASK)
#define     FCR_IRQ_TRG_8BYTE   (2U << FCR_IRQ_TRG_MASK)
#define     FCR_IRQ_TRG_14BYTE  (3U << FCR_IRQ_TRG_MASK)
#define LCR 3
#define   LCR_BITS_SHIFT        0U
#define   LCR_BITS_WIDTH        2U
#define   LCR_BITS_MASK         GENMASK(LCR_BITS_WIDTH+LCR_BITS_SHIFT, LCR_BITS_SHIFT)
#define     LCR_BITS_5          (0 << LCR_BITS_SHIFT)
#define     LCR_BITS_6          (1 << LCR_BITS_SHIFT)
#define     LCR_BITS_7          (2 << LCR_BITS_SHIFT)
#define     LCR_BITS_8          (3 << LCR_BITS_SHIFT)
#define   LCR_TWO_STOP_BITS     BIT(2)
#define   LCR_PARITY_EN         BIT(3)
#define   LCR_PARITY_SHIFT      4U
#define   LCR_PARITY_WIDTH      2U
#define   LCR_PARITY_MASK       GENMASK(LCR_PARITY_WIDTH+LCR_PARITY_SHIFT, LCR_PARITY_SHIFT)
#define     LCR_PARITY_ODD      (0 << LCR_PARITY_SHIFT)
#define     LCR_PARITY_EVEN     (1 << LCR_PARITY_SHIFT)
#define     LCR_PARITY_MARK     (2 << LCR_PARITY_SHIFT)
#define     LCR_PARITY_SPACE    (3 << LCR_PARITY_SHIFT)
#define   LCR_BREAK_EN          BIT(6)
#define   LCR_DIV_LATCH_EN      BIT(7)
#define MCR 4
#define   MCR_DTR               BIT(0)
#define   MCR_RTS               BIT(1)
#define   MCR_OUT1              BIT(2)
#define   MCR_OUT2              BIT(3)
#define   MCR_LOOP              BIT(4)
#define LSR 5
#define   LSR_DR                BIT(0)
#define   LSR_OE                BIT(1)
#define   LSR_PE                BIT(2)
#define   LSR_FE                BIT(3)
#define   LSR_BI                BIT(4)
#define   LSR_THRE              BIT(5)
#define   LSR_TEMT              BIT(6)
#define   LSR_IMPENDING_ERR     BIT(7)
#define MSR 6
#define   MSR_DCTS              BIT(0)
#define   MSR_DDSR              BIT(1)
#define   MSR_TERI              BIT(2)
#define   MSR_DDCD              BIT(3)
#define   MSR_CTS               BIT(4)
#define   MSR_DSR               BIT(5)
#define   MSR_RI                BIT(6)
#define   MSR_DCD               BIT(7)
#define SCRATCH 7
#define DLL 0
#define DLH 1

/**
 * initialize the early boot console
 * @port: the base of the port
 * @baud: the baud rate of the port
 */
int early_com_init(enum com_port port, int baud);

/**
 * early_com_putchar() - write a character to the serial COM
 * @port: port to write on
 * @ch: character to write
 *
 * Returns 0 on success and a negative error code on failure.
 */
int early_com_putchar(enum com_port port, char ch);

/**
 * early_com_putchar() - read a character from the serial COM
 * @port: port to write on
 *
 * Returns 0 on success and a negative error code on failure.
 */
int early_com_getchar(enum com_port port);

/** register the early console with the kernel logger */
void early_com_register_klog(void);

/**
 * deregister the early console with the kernel logger,
 * presumable to replace with a later stage driver
 */
void early_com_unregister_klog(void);

#endif
