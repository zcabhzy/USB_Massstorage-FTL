/*
 * nandsim.c
 *
 *  Created on: May 26, 2025
 *      Author: User
 */

#include "nandsim.h"
#include <stdio.h>
#include <stdint.h>

uint8_t nand_memory[NAND_BLOCK_NUM][NAND_PAGE_PER_BLOCK][NAND_PAGE_SIZE] = { 0 };
uint8_t valid_map[NAND_BLOCK_NUM][NAND_PAGE_PER_BLOCK];

void NAND_Erase_Block(uint16_t block_idx) {

	if (block_idx >= NAND_BLOCK_NUM)
		return;

	for (int i = 0; i < NAND_PAGE_PER_BLOCK; ++i) {

		memset(nand_memory[block_idx][i], 0xFF, NAND_PAGE_SIZE); // Erase 後狀態設定為 1

		valid_map[block_idx][i] = 0; // free
	}
}
