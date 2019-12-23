// File: ftl.c
// Date: 2014. 12. 03.
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2014
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#include "common.h"
#ifndef VSSIM_BENCH
#include "qemu-kvm.h"
#endif

#ifdef FTL_GET_WRITE_WORKLOAD
FILE* fp_write_workload;
#endif
#ifdef FTL_IO_LATENCY
FILE* fp_ftl_w;
FILE* fp_ftl_r;
#endif

int g_init = 0;
extern double ssd_util;
struct Buffer* compression_buffer;
/*buffer */
#define BUFFER_SIZE_BYTES 4096


typedef struct BufferPageItem{
	int32_t lpn;
	struct BufferPageItem* next;
}BufferPageItem;

typedef struct Buffer{
	int size_in_pages;
	int current_size_bytes;
	int max_size_bytes;
	int32_t buffer_lba;
	struct BufferPageItem *head;
	struct BufferPageItem *tail;
} Buffer;

struct BufferPageItem* createBufferPageItem(int32_t lba){
	struct BufferPageItem* page_item = (struct BufferPageItem*) malloc(sizeof(struct BufferPageItem));
	if(page_item == NULL){
		printf("ERROR[%s] Could not allocate memory for BufferPageItem\n", __FUNCTION__);
	}
	page_item->lpn = lba / (int32_t)SECTORS_PER_PAGE;
	page_item->next = NULL;
	return page_item;
}

struct Buffer* createBuffer(void){
	struct Buffer* buffer = (struct Buffer*) malloc(sizeof(struct Buffer));
	if(buffer == NULL){
		printf("ERROR[%s] Could not allocate memory for Buffer\n", __FUNCTION__);
	}
	buffer->current_size_bytes = 0;
	buffer->max_size_bytes = BUFFER_SIZE_BYTES;
	buffer->buffer_lba = -1;
	buffer->size_in_pages = 0;
	buffer->head = NULL;
	buffer->tail = NULL;
}

void insertBufferPageItem(Buffer* buffer, int32_t lba, unsigned int size_bytes){
	struct BufferPageItem *node;
	node = createBufferPageItem(lba);
	if (buffer->head == NULL){/*meaning empty buffer*/
		buffer->head = node;
		buffer->tail = node;
	} else{
		buffer->tail->next = node;
		buffer->tail = node;
	}
	buffer->size_in_pages++;
	buffer->current_size_bytes += size_bytes;
}

void freeBufferContent(Buffer* buffer){
	struct BufferPageItem* temp, *temp_to_free;
	temp = buffer->head;
	while(temp->next != NULL){
		temp_to_free = temp;
		temp = temp->next;
		free(temp_to_free);
	}
	free(temp);
	buffer->buffer_lba = -1;
	buffer->current_size_bytes = 0;
	buffer->size_in_pages = 0;
	buffer->head = NULL;
	buffer->tail = NULL;
}

void freeBuffer(Buffer* buffer){
	freeBufferContent(buffer);
	free(buffer);
}

void updateMappignsAfterBufferFlush(Buffer* buffer, int32_t lpn){
	struct BufferPageItem *temp;
	temp = buffer->head;
	while(temp != NULL){
		UPDATE_AFTER_FLUSH_BUFFER(lpn, temp->lpn);	
		temp = temp->next;
	}
}

void INIT_COMPRESSION_BUFFER(){
	compression_buffer = createBuffer();
}

void TERM_COMPRESSION_BUFFER(){
	freeBuffer(compression_buffer);
}

void FTL_INIT(void)
{
	if(g_init == 0){
        	printf("[%s] start\n", __FUNCTION__);

		INIT_SSD_CONFIG();
		INIT_MAPPING_TABLE();
		INIT_MAPPING_COUNTER_TABLE();
		INIT_ADVANCED_LPN_MAPPING();
		INIT_INVERSE_MAPPING_TABLE();
		INIT_BLOCK_STATE_TABLE();
		INIT_VALID_ARRAY();
		INIT_EMPTY_BLOCK_LIST();
		INIT_VICTIM_BLOCK_LIST();
		INIT_PERF_CHECKER();
		INIT_COMPRESSION_BUFFER();
		
#ifdef FTL_MAP_CACHE
		INIT_CACHE();
#endif
#ifdef FIRM_IO_BUFFER
		INIT_IO_BUFFER();
#endif
#ifdef MONITOR_ON
#ifndef LOCAL
		INIT_LOG_MANAGER();
#endif
#endif
		g_init = 1;
#ifdef FTL_GET_WRITE_WORKLOAD
		fp_write_workload = fopen("./data/p_write_workload.txt","a");
#endif
#ifdef FTL_IO_LATENCY
		fp_ftl_w = fopen("./data/p_ftl_w.txt","a");
		fp_ftl_r = fopen("./data/p_ftl_r.txt","a");
#endif
		SSD_IO_INIT();
	
		printf("[%s] complete\n", __FUNCTION__);
	}
}

void WRITE_TO_BUFFER(int32_t lba, unsigned int length){
	if(compression_buffer->current_size_bytes == 0){ /*either just initiated or just flushed*/
		compression_buffer->buffer_lba = lba;
	}
	insertBufferPageItem(compression_buffer, lba, length);
	UPDATE_PAGES_LOCATION(lba);	
}

void FTL_TERM(void){
	printf("[%s] start\n", __FUNCTION__);

#ifdef FIRM_IO_BUFFER
	TERM_IO_BUFFER();
#endif
	TERM_MAPPING_TABLE();
	TERM_INVERSE_MAPPING_TABLE();
	TERM_ADVANCED_LPN_MAPPING();
	TERM_MAPPING_COUNTER_TABLE();
	TERM_VALID_ARRAY();
	TERM_BLOCK_STATE_TABLE();
	TERM_EMPTY_BLOCK_LIST();
	TERM_VICTIM_BLOCK_LIST();
	TERM_PERF_CHECKER();
	TERM_COMPRESSION_BUFFER();

#ifdef MONITOR_ON
	TERM_LOG_MANAGER();
#endif

#ifdef FTL_IO_LATENCY
	fclose(fp_ftl_w);
	fclose(fp_ftl_r);
#endif
	printf("[%s] complete\n", __FUNCTION__);
}

int READ_FROM_COMPRESSION_BUFFER(void)
{
	return SUCCESS;
}

void FTL_READ(int32_t sector_nb, unsigned int length)
{
	int ret;

#ifdef GET_FTL_WORKLOAD
	FILE* fp_workload = fopen("./data/workload_ftl.txt","a");
	struct timeval tv;
	struct tm *lt;
	double curr_time;
	gettimeofday(&tv, 0);
	lt = localtime(&(tv.tv_sec));
	curr_time = lt->tm_hour*3600 + lt->tm_min*60 + lt->tm_sec + (double)tv.tv_usec/(double)1000000;
	//fprintf(fp_workload,"%lf %d %ld %u %x\n",curr_time, 0, sector_nb, length, 1);
	fprintf(fp_workload,"%lf %d %u %x\n",curr_time, sector_nb, length, 1);
	fclose(fp_workload);
#endif
#ifdef FTL_IO_LATENCY
	int64_t start_ftl_r, end_ftl_r;
	start_ftl_r = get_usec();
#endif
	int32_t lba = sector_nb;
	int amount_of_buffer_attemps;
	int num_of_pages = (int)length/SECTORS_PER_PAGE; /*every page is 4KB by default*/
	int info_location;
	int32_t lpn;
	for (amount_of_buffer_attemps=0; amount_of_buffer_attemps<num_of_pages; amount_of_buffer_attemps++){
		lpn = lba / (int32_t)SECTORS_PER_PAGE;
		info_location = GET_PAGE_LOCATION(lpn);
		if(info_location == -2){ /*lpn is in buffer*/
			ret = READ_FROM_COMPRESSION_BUFFER();	
		}
		else{
			ret = _FTL_READ(lba, SECTORS_PER_PAGE);
		}
		lba += SECTORS_PER_PAGE;
	}

#ifdef FTL_IO_LATENCY
	end_ftl_r = get_usec();
	if(length >= 128)
		fprintf(fp_ftl_r,"%ld\t%u\n", end_ftl_r - start_ftl_r, length);
#endif
}

void FTL_WRITE(int32_t sector_nb, unsigned int length)
{
	int ret;

#ifdef GET_FTL_WORKLOAD
	FILE* fp_workload = fopen("./data/workload_ftl.txt","a");
	struct timeval tv;
	struct tm *lt;
	double curr_time;
	gettimeofday(&tv, 0);
	lt = localtime(&(tv.tv_sec));
	curr_time = lt->tm_hour*3600 + lt->tm_min*60 + lt->tm_sec + (double)tv.tv_usec/(double)1000000;
//	fprintf(fp_workload,"%lf %d %ld %u %x\n",curr_time, 0, sector_nb, length, 0);
	fprintf(fp_workload,"%lf %d %u %x\n",curr_time, sector_nb, length, 0);
	fclose(fp_workload);
#endif
#ifdef FTL_IO_LATENCY
	int64_t start_ftl_w, end_ftl_w;
	start_ftl_w = get_usec();
#endif
	ret = _FTL_WRITE(sector_nb, length);
#ifdef FTL_IO_LATENCY
	end_ftl_w = get_usec();
	if(length >= 128)
		fprintf(fp_ftl_w,"%ld\t%u\n", end_ftl_w - start_ftl_w, length);
#endif
}

int _FTL_READ(int32_t sector_nb, unsigned int length)
{
#ifdef FTL_DEBUG
	printf("[%s] Start\n", __FUNCTION__);
#endif

	if(sector_nb + length > SECTOR_NB){
		printf("Error[%s] Exceed Sector number\n", __FUNCTION__); 
		return FAIL;	
	}
	
	int32_t lpn;
	int32_t ppn;
	int32_t lba = sector_nb;
	unsigned int remain = length;
	unsigned int left_skip = sector_nb % SECTORS_PER_PAGE;
	unsigned int right_skip;
	unsigned int read_sects;

	unsigned int ret = FAIL;
	int read_page_nb = 0;
	int io_page_nb;
	
	

#ifdef FIRM_IO_BUFFER
	INCREASE_RB_FTL_POINTER(length);
#endif

	while(remain > 0){

		if(remain > SECTORS_PER_PAGE - left_skip){
			right_skip = 0;
		}
		else{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}
		read_sects = SECTORS_PER_PAGE - left_skip - right_skip;

		lpn = lba / (int32_t)SECTORS_PER_PAGE;
		ppn = GET_MAPPING_INFO(lpn);

		if(ppn == -1){
#ifdef FIRM_IO_BUFFER
			INCREASE_RB_LIMIT_POINTER();
#endif
			return FAIL;
		}

		lba += read_sects;
		remain -= read_sects;
		left_skip = 0;
	}

	io_alloc_overhead = ALLOC_IO_REQUEST(sector_nb, length, READ, &io_page_nb);

	remain = length;
	lba = sector_nb;
	left_skip = sector_nb % SECTORS_PER_PAGE;

	while(remain > 0){

		if(remain > SECTORS_PER_PAGE - left_skip){
			right_skip = 0;
		}
		else{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}
		read_sects = SECTORS_PER_PAGE - left_skip - right_skip;

		lpn = lba / (int32_t)SECTORS_PER_PAGE;

#ifdef FTL_MAP_CACHE
		ppn = CACHE_GET_PPN(lpn);
#else
		ppn = GET_MAPPING_INFO(lpn);
#endif

		if(ppn == -1){
#ifdef FTL_DEBUG
			printf("ERROR[%s] No Mapping info\n", __FUNCTION__);
#endif
		}

		ret = SSD_PAGE_READ(CALC_FLASH(ppn), CALC_BLOCK(ppn), CALC_PAGE(ppn), read_page_nb, READ, io_page_nb);

#ifdef FTL_DEBUG
		if(ret == SUCCESS){
			printf("\t read complete [%u]\n",ppn);
		}
		else if(ret == FAIL){
			printf("ERROR[%s] %u page read fail \n",__FUNCTION__, ppn);
		}
#endif
		read_page_nb++;

		lba += read_sects;
		remain -= read_sects;
		left_skip = 0;

	}

	INCREASE_IO_REQUEST_SEQ_NB();

#ifdef FIRM_IO_BUFFER
	INCREASE_RB_LIMIT_POINTER();
#endif

#ifdef MONITOR_ON
	char szTemp[1024];
	sprintf(szTemp, "READ PAGE %d ", length);
	WRITE_LOG(szTemp);
#endif

#ifdef FTL_DEBUG
	printf("[%s] Complete\n", __FUNCTION__);
#endif

	return ret;
}

int WRITE_BUFFER_TO_FTL(void)
{
	int io_page_nb;
	/*we flush only 4k at a time - which is SECTORS_PER_PAGE*/
	io_alloc_overhead = ALLOC_IO_REQUEST(compression_buffer->buffer_lba, SECTORS_PER_PAGE, WRITE, &io_page_nb);
	int32_t lpn;
	int32_t new_ppn;
	int32_t old_ppn;

	unsigned int remain = SECTORS_PER_PAGE;
	unsigned int left_skip = compression_buffer->buffer_lba % SECTORS_PER_PAGE;
	unsigned int right_skip;
	unsigned int write_sects;

	unsigned int ret = FAIL;
	int write_page_nb = 0;

	while(remain > 0){
		if(remain > SECTORS_PER_PAGE - left_skip){
			right_skip = 0;
		}
		else{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}

		write_sects = SECTORS_PER_PAGE - left_skip - right_skip;

		#ifdef FIRM_IO_BUFFER
				INCREASE_WB_FTL_POINTER(write_sects);
		#endif

		#ifdef WRITE_NOPARAL
				ret = GET_NEW_PAGE(VICTIM_NOPARAL, empty_block_table_index, &new_ppn);
		#else
				ret = GET_NEW_PAGE(VICTIM_OVERALL, EMPTY_TABLE_ENTRY_NB, &new_ppn);
		#endif
		if(ret == FAIL){
			printf("ERROR[%s] Get new page fail \n", __FUNCTION__);
			return FAIL;
		}

		lpn = GET_FREE_LPN();
		old_ppn = GET_MAPPING_INFO(lpn);

		if((left_skip || right_skip) && (old_ppn != -1)){
			ret = SSD_PAGE_PARTIAL_WRITE(
				CALC_FLASH(old_ppn), CALC_BLOCK(old_ppn), CALC_PAGE(old_ppn),
				CALC_FLASH(new_ppn), CALC_BLOCK(new_ppn), CALC_PAGE(new_ppn),
				write_page_nb, WRITE, io_page_nb);
		} else{
			ret = SSD_PAGE_WRITE(CALC_FLASH(new_ppn), CALC_BLOCK(new_ppn), CALC_PAGE(new_ppn), write_page_nb, WRITE, io_page_nb);
		}
		
		write_page_nb++;

		UPDATE_OLD_PAGE_MAPPING(lpn);
		UPDATE_NEW_PAGE_MAPPING(lpn, new_ppn);

		#ifdef FTL_DEBUG
						if(ret == SUCCESS){
								printf("\twrite complete [%d, %d, %d]\n",CALC_FLASH(new_ppn), CALC_BLOCK(new_ppn),CALC_PAGE(new_ppn));
						}
						else if(ret == FAIL){
								printf("ERROR[%s] %d page write fail \n",__FUNCTION__, new_ppn);
						}
		#endif
		compression_buffer->buffer_lba += write_sects;
		remain -= write_sects;
		left_skip = 0;
	}
	updateMappignsAfterBufferFlush(compression_buffer ,lpn);
	freeBufferContent(compression_buffer);
	INCREASE_IO_REQUEST_SEQ_NB();

#ifdef GC_ON
	GC_CHECK(CALC_FLASH(new_ppn), CALC_BLOCK(new_ppn));
#endif

#ifdef FIRM_IO_BUFFER
	INCREASE_WB_LIMIT_POINTER();
#endif

#ifdef MONITOR_ON
	char szTemp[1024];
	sprintf(szTemp, "WRITE PAGE %d ", SECTORS_PER_PAGE);
	WRITE_LOG(szTemp);
	sprintf(szTemp, "WB CORRECT %d", write_page_nb);
	WRITE_LOG(szTemp);
#endif

#ifdef FTL_DEBUG
	printf("[%s] End\n", __FUNCTION__);
#endif

	return ret;
}	

int _FTL_WRITE(int32_t sector_nb, unsigned int length)
{
#ifdef FTL_DEBUG
	printf("[%s] Start\n", __FUNCTION__);
#endif

#ifdef FTL_GET_WRITE_WORKLOAD
	fprintf(fp_write_workload,"%d\t%u\n", sector_nb, length);
#endif

	unsigned int ret = SUCCESS;
	int32_t lba = sector_nb;
	int num_of_pages = (int)length/SECTORS_PER_PAGE; /*every page is 4KB by default*/
	int compress_rounds, not_all_request_in_buffer = 0;
	float compressed_info_size;
	float compression_ratio;
	if(sector_nb + length > SECTOR_NB){
   		printf("ERROR[%s] Exceed Sector number\n", __FUNCTION__);
        return FAIL;   
	}
	for (compress_rounds=0; compress_rounds<num_of_pages; compress_rounds++){
		compression_ratio = (double)rand() / (double)RAND_MAX;
		compressed_info_size = compression_ratio * PAGE_SIZE; 
		if(compression_buffer->max_size_bytes - compression_buffer->current_size_bytes >= compressed_info_size){
			WRITE_TO_BUFFER(lba, compressed_info_size); /*a single page is compressed in to the buffer at a time*/
			lba += SECTORS_PER_PAGE;
		} else{ /* no space in buffer - so we flush him and then continue */ 
			ret = WRITE_BUFFER_TO_FTL();
			WRITE_TO_BUFFER(lba, compressed_info_size);
			lba += SECTORS_PER_PAGE;	
		}
	}	
	return ret;
}