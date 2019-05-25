//-----------------------------------------------------------------------------
// \file    evmomapl138_nand.h
// \brief   OMAP-L138 nand defines and function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_NAND_H
#define EVMOMAPL138_NAND_H

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------
#define NAND_PAGE_SIZE        (2048)
#define NAND_NUM_BLOCKS       (4096)
#define NAND_PAGES_PER_BLOCK  (64)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t NAND_init(void);
uint32_t NAND_readPage(uint32_t in_block, uint32_t in_page, uint8_t *dest_buffer);
uint32_t NAND_writePage(uint32_t in_block, uint32_t in_page, uint8_t *src_buffer);
uint32_t NAND_eraseBlock(uint32_t in_block);
uint32_t NAND_isBadBlock(uint32_t in_block);

#endif


