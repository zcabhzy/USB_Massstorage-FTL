/*
 * nandsim.h
 *
 *  Created on: May 26, 2025
 *      Author: User
 */

#ifndef INC_NANDSIM_H_
#define INC_NANDSIM_H_

#include <stdint.h>

/* ------------------------
 * 32F411EDISCOVERY 模擬參數
 * ------------------------
 * BLOCK_NUM : 16
 * PAGE_SIZE : 每 BLOCK 的 PAGE 數量 12
 * PAGE_PER_BLOCK : 512 bytes
 * TOTAL SIZE : BLOCK_NUM * PAGE_SIZE * PAGE_PER_BLOCK
 */
#define NAND_BLOCK_NUM 16
#define NAND_PAGE_SIZE 512
#define NAND_PAGE_PER_BLOCK 12

#define NAND_TOTAL_PAGE (NAND_BLOCK_NUM * NAND_PAGE_PER_BLOCK)
#define NAND_TOTAL_SIZE (NAND_TOTAL_PAGE * NAND_PAGE_SIZE)

/* ------------------------
 * nand_memory
 * ------------------------
 *
 * ------------------------
 * valid_map
 * ------------------------
 * 0 = free / 1 = valid /2 = invalid
 */
extern uint8_t nand_memory[NAND_BLOCK_NUM][NAND_PAGE_PER_BLOCK][NAND_PAGE_SIZE];
extern uint8_t valid_map[NAND_BLOCK_NUM][NAND_PAGE_PER_BLOCK];

void NAND_Erase_Block(uint16_t block_idx);

#endif /* INC_NANDSIM_H_ */
