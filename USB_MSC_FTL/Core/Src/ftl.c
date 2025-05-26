/*
 * ftl.c
 *
 *  Created on: May 26, 2025
 *      Author: User
 */

#include "ftl.h"
#include "nandsim.h"
#include <stdio.h>
#include <string.h>

/* ------------------------
 * Page Mapping Table : LPN -> PPN(block:page)
 * ------------------------
 */
static PPN_t page_map[FTL_LPN_MAX];

void FTL_Init(void) {
	for (int i = 0; i < FTL_LPN_MAX; i++) {
		page_map[i].block = 0xFFFF;
		page_map[i].page = 0xFFFF;
	}
}

int FTL_Read(uint16_t lpn, uint8_t *buf) {
	if (lpn >= FTL_LPN_MAX)
		return -1;

	uint16_t blk = page_map[lpn].block;
	uint16_t pg = page_map[lpn].page;

	if (blk >= NAND_BLOCK_NUM || pg >= NAND_PAGE_PER_BLOCK)
		return -2;

	memcpy(buf, nand_memory[blk][pg], NAND_PAGE_SIZE);
	printf("[READ] LPN=%d -> PPN(%d,%d) Data=%s\r\n", lpn, blk, pg, buf);

	return 0;
}

int FTL_Write(uint16_t lpn, const uint8_t *buf) {

	// 標記舊頁 invalid
	if (lpn < FTL_LPN_MAX) {

		uint16_t old_blk = page_map[lpn].block;
		uint16_t old_pg = page_map[lpn].page;

		if (old_blk < NAND_BLOCK_NUM && old_pg < NAND_PAGE_PER_BLOCK) {

			valid_map[old_blk][old_pg] = 2;
			printf("[FTL] Mark old PPN (%d,%d) invalid (LPN %d)\r\n", old_blk,
					old_pg, lpn);
		}
	}

	uint16_t blk, pg;

	if (Find_Free_Page(&blk, &pg) != 0) {
		printf("[FTL] No free page, trigger GC\r\n");
		FTL_GarbageCollect();

		if (Find_Free_Page(&blk, &pg) != 0) {
			printf("[ERROR] No free page even after GC\r\n");
			return -4;
		}
	}

	memcpy(nand_memory[blk][pg], buf, NAND_PAGE_SIZE);
	valid_map[blk][pg] = 1;
	page_map[lpn].block = blk;
	page_map[lpn].page = pg;
	printf("[WRITE] LPN %d -> PPN(%d,%d) Data=%s\r\n", lpn, blk, pg, buf);

	return 0;
}

// 幫助尋找空白物理頁
static int Find_Free_Page(uint16_t *blk, uint16_t *pg) {

	for (uint16_t b = 0; b < NAND_BLOCK_NUM; b++) {
		for (uint16_t p = 0; p < NAND_PAGE_PER_BLOCK; p++) {
			if (valid_map[b][p] == 0) {
				*blk = b;
				*pg = p;
				return 0;
			}
		}
	}
	return -1;
}

static void FTL_GarbageCollect(void) {

	// 1.找出無效最多的 BLOCK
	int max_invalid = -1, victim = -1;

	for (int b = 0; b < NAND_BLOCK_NUM; b++) {

		int invalid = 0;

		for (int p = 0; p < NAND_PAGE_PER_BLOCK; p++) {
			if (valid_map[b][p] == 2) {
				invalid++;
			}
			if (invalid > max_invalid) {
				max_invalid = invalid;
				victim = b;
			}
		}
	}
	if (victim < 0)
		return;
	printf("[GC] Run. Victim block = %d (invalid pages=%d)\r\n", victim,
			max_invalid);

	// 2.搬移有效頁到新空白頁
	for (int p = 0; p < NAND_PAGE_PER_BLOCK; p++) {

		if (valid_map[victim][p] == 1) {

			uint16_t new_blk, new_pg;

			if (Find_Free_Page(&new_blk, &new_pg) == 0) {

				memcpy(nand_memory[new_blk][new_pg], nand_memory[victim][p],
				NAND_PAGE_SIZE);
				valid_map[new_blk][new_pg] = 1;

				// 更新 page_map 指向新位置
				for (int lpn = 0; lpn < FTL_LPN_MAX; lpn++) {
					if (page_map[lpn].block == victim
							&& page_map[lpn].page == p) {

						page_map[lpn].block = new_blk;
						page_map[lpn].page = new_pg;
					}
				}
				valid_map[victim][p] = 2; // 原頁標無效
				printf("[GC] Move PPN (%d,%d) -> (%d,%d)\r\n", victim, p,
						new_blk, new_pg);
			}
		}
	}

	// 擦除 victim block
	NAND_Erase_Block(victim);
	printf("[GC] Erase block %d\r\n", victim);
}

void FTL_Dump_Status(void) {

	printf("---- FTL Mapping Table ----\r\n");
	printf("LPN | Block | Page | Valid\r\n");

	for (int i = 0; i < FTL_LPN_MAX; i++) {

		uint16_t blk = page_map[i].block;
		uint16_t pg = page_map[i].page;

		if (blk < NAND_BLOCK_NUM && pg < NAND_PAGE_PER_BLOCK)
			printf("%3d | %5d | %4d | %d\n", i, blk, pg, valid_map[blk][pg]);
		else
			printf("%3d |   --  |  --  | --\n", i);
	}
}
