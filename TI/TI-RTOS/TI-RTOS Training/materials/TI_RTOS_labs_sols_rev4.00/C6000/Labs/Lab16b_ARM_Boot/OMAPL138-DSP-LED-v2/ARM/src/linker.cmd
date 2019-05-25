/*
 *  Linker command file
 *
 */
-
-stack           0x00000800      /* Stack Size */
-heap            0x00000800      /* Heap Size */


MEMORY
{
    ARMRAM:      o = 0xFFFF0000  l = 0x00002000
    SHAREDRAM:   o = 0x80010000  l = 0x00010000
    SDRAM:       o = 0xC0000000  l = 0x20000000
}

SECTIONS
{
    .bss        >   SHAREDRAM
    .cinit      >   SHAREDRAM
    .cio        >   SHAREDRAM
    .const      >   SHAREDRAM
    .stack      >   SHAREDRAM
    .sysmem     >   SHAREDRAM
    .text       >   SHAREDRAM
    .switch     >   SHAREDRAM
    .far        >   SHAREDRAM
	.svcstack	>	SHAREDRAM		/* Supervisor Mode Stack			*/
	.irqstack	>	SHAREDRAM		/* IRQ Interrupt Stack				*/
	.fiqstack	>	SHAREDRAM		/* FIQ Interrupt Stack				*/
	.intvecs	>	ARMRAM			/* Interrupt Vectors				*/
}
