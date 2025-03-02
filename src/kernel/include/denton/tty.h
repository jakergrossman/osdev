#ifndef __DENTON_TTY_H
#define __DENTON_TTY_H

#include <stddef.h>
#include <stdint.h>

enum {
    EARLY_BOOT_VGA = 0xC03FF000, // VGA mem after bootloader
    INIT_VGA = 0xC00B8000, // VGA mem after cmain
};

void terminal_initialize(uint32_t);
void terminal_update_base(uint32_t);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color); 
void terminal_clear(void); 
void terminal_flush(void);

#endif
