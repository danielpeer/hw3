// File: ftl_mapping_manager.h
// Date: 2014. 12. 03.
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2014
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#ifndef _MAPPING_MANAGER_H_
#define _MAPPING_MANAGER_H_

extern int32_t* mapping_table;
extern void* block_table_start;

extern unsigned int flash_index;
extern unsigned int* plane_index;
extern unsigned int* block_index;

void INIT_MAPPING_TABLE(void);
void TERM_MAPPING_TABLE(void);

int32_t GET_MAPPING_INFO(int32_t lpn);
int GET_NEW_PAGE(int mode, int mapping_index, int32_t* ppn);

int UPDATE_OLD_PAGE_MAPPING(int32_t lpn);
int UPDATE_NEW_PAGE_MAPPING(int32_t lpn, int32_t ppn);

unsigned int CALC_FLASH(int32_t ppn);
unsigned int CALC_BLOCK(int32_t ppn);
unsigned int CALC_PAGE(int32_t ppn);

void INIT_ADVANCED_LPN_MAPPING(void);
void TERM_ADVANCED_LPN_MAPPING(void);
void UPDATE_PAGES_LOCATION(int32_t lba);
int32_t GET_PAGE_LOCATION(int32_t lpn);
void UPDATE_AFTER_FLUSH_BUFFER(int32_t new_lpn, int32_t mapping_location_index);


void INIT_MAPPING_COUNTER_TABLE(void);
void TERM_MAPPING_COUNTER_TABLE(void);
int32_t GET_MAPPING_COUNTER(int32_t lpn);
void SET_MAPPING_COUNTER(int32_t lpn, int counter);
int32_t GET_FREE_LPN();
#endif
