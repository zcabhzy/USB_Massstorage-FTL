/*
 * ftl.h
 *
 *  Created on: May 26, 2025
 *      Author: User
 */

#ifndef INC_FTL_H_
#define INC_FTL_H_

#include <stdint.h>

#define FTL_LPN_MAX    (NAND_BLOCK_NUM * NAND_PAGE_PER_BLOCK)

typedef struct {
	uint16_t block;
	uint16_t page;
} PPN_t;

void FTL_Init(void);
int FTL_Read(uint16_t lpn, uint8_t *buf);
int FTL_Write(uint16_t lpn, const uint8_t *buf);
void FTL_Dump_Status(void);
static int Find_Free_Page(uint16_t *blk, uint16_t *pg);
static void FTL_GarbageCollect(void);

#endif /* INC_FTL_H_ */
