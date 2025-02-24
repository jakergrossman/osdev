/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text : ALIGN(4K)
	{
		_TEXT_START_ = .;
		*(.multiboot*)
		*(.text*)
		_TEXT_END_ = .;
	}

	/* Read-only data. */
	.rodata : ALIGN(4K)
	{
		_RODATA_START_ = .;
		*(.rodata.*)
		_RODATA_END_ = .;
	}

	/* Read-write data (initialized) */
	.data : ALIGN(4K)
	{
		_DATA_START_ = .;
		*(.data*)
		_DATA_END_ = .;
	}

	/* Read-write data (uninitialized) and stack */
	.bss : ALIGN(4K)
	{
		_BSS_START_ = .;
		*(COMMON*)
		*(.bss*)
		_BSS_END_ = .;
	}

	/* The compiler may produce other sections, put them in the proper place in
	   in this file, if you'd like to include them in the final kernel. */
}
