#include <asm/drivers/early_console.h>

#include <denton/stddef.h>
#include <denton/types.h>
#include <denton/bits.h>
#include <denton/errno.h>
#include <denton/klog.h>

#include <asm/instr.h>

#include <stdint.h>

static enum com_port early_comport = COM_NULL;

static bool early_com_test_loopback(enum com_port port)
{
    /* enable loopback */
    uint8_t mcr = inb(port + MCR);
    mcr |= MCR_LOOP;
    outb(mcr, port + MCR);

    uint8_t sent = 0xF5;
    outb(sent, port + TXR);
    uint8_t read = inb(port + RXR);

    /* disable loopback */
    mcr &= ~MCR_LOOP;
    outb(mcr, port + MCR);

    return (sent == read);
}

int early_com_init(enum com_port port, int baud)
{
    /* line/modem settings */
    outb(LCR_BITS_8, port + LCR);   /* 8-n-1 */
    outb(0, port + IER);            /* no interrupts */
    outb(0, port + FCR);            /* no FIFO */
    outb(MCR_DTR | MCR_RTS, port + MCR);

    /* enable div latch */
    uint8_t lcr = inb(port + LCR);
    outb(lcr | LCR_DIV_LATCH_EN, port + LCR);

    /* baud setting */
    uint16_t divisor = 115200 / baud;
    outb(divisor >> 8, port + LCR);
    outb(divisor & 0xFF, port + LCR);

    outb(lcr & ~LCR_DIV_LATCH_EN, port + LCR);

    if (early_com_test_loopback(port)) {
        early_comport = port;
    }

    return 0;
}

static bool early_con_is_tx_ready(enum com_port port)
{
    return (inb(port + LSR) & LSR_THRE);
}

int early_com_putchar(enum com_port port, char ch)
{
    uint16_t timeout = 0xFFFF;

    while (!early_con_is_tx_ready(port)) {
        if (0 == timeout--) {
            return -EBUSY;
        }
    }

    outb(ch, port + TXR);
    return 0;
}

int early_com_getchar(enum com_port port)
{
    uint16_t timeout = 0xFFFF;

    bool read = false;
    while (!((inb(port + LSR) & LSR_DR) == 0) && timeout--)
    {
        read = true;
        pause();
    }

    return read ? inb(port + RXR) : -EBUSY;
}

static void early_console_klog_write(struct klog_sink* sink, const char* buf, size_t buflen)
{
    if (early_comport != COM_NULL) {
        for (size_t i = 0; i < buflen; i++) {
            early_com_putchar(early_comport, buf[i]);
        }
    }
}

static struct klog_sink early_console_klog_sink = {
    .name = "early_console",
    .list = LIST_HEAD_INIT(early_console_klog_sink.list),
    .threshold = KLOG_DEBUG, /* show everything */
    .write = early_console_klog_write,
};

void early_com_register_klog(void)
{
    klog_sink_register(&early_console_klog_sink);
}

void early_com_unregister_klog(void)
{
    klog_sink_deregister(&early_console_klog_sink);
}
