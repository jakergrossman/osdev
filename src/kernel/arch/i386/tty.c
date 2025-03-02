#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <denton/tty.h>
#include <stdlib.h>

#include "vga.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT  25
// static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static uint16_t* VGA_MEMORY = (uint16_t*) 0xC03FF000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t terminal_buffer[VGA_HEIGHT*VGA_WIDTH];

void terminal_flush(void)
{
	memcpy(VGA_MEMORY, terminal_buffer, sizeof(terminal_buffer));
}

void terminal_clear(void)
{
	terminal_row = 0;
	terminal_column = 0;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_initialize(uint32_t base) {
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_update_base(base);
	terminal_clear();
}

void terminal_update_base(uint32_t base)
{
	VGA_MEMORY = (void*)base;
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

static void
terminal_scroll(size_t rows)
{
	size_t affected_rows = VGA_HEIGHT - rows;
	size_t cleared_start = affected_rows * VGA_WIDTH;
	size_t affected = cleared_start * sizeof(uint16_t);
	size_t start = VGA_WIDTH * rows;
	memcpy(terminal_buffer, &terminal_buffer[start], affected);
	terminal_row = affected_rows;

	for (int row = terminal_row; row < VGA_HEIGHT; row++) {
		for (int i = 0; i < VGA_WIDTH; i++) {
			terminal_putentryat(' ', terminal_color, i, row);
		}
	}

}

void terminal_putchar(char c) {
	unsigned char uc = c;
	switch (uc) {
		case '\r':
			terminal_column = 0;
			break;

		case '\n':
			terminal_column = 0;
			terminal_row += 1;
			if (terminal_row == VGA_HEIGHT) {
				terminal_scroll(1);
			}
			break;

		default:
			terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) {
				abort();
				terminal_column = 0;
				if (++terminal_row == VGA_HEIGHT) {
					terminal_scroll(1);
				}
			}
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
