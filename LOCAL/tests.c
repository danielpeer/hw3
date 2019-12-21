#include "ssd.h"
#include "common.h"
#include <math.h>
#include <time.h>
#include <assert.h>

#define RUN_TEST(SETUP, FUNC)   if(SETUP) popen("rm data/*.dat", "r"); \
						        SSD_INIT(); \
 								FUNC; \
								if(SETUP) SSD_TERM()

extern int32_t* mapping_table;

/**
* get a random number from a range lower to upper, that divides by 
*/
int get_random_lba(int lower, int upper) 
{  
	int rand_lba = ((rand() % (upper - lower + 1)) + lower); 
    int rand_lba_modulu_page_sectors = rand_lba % SECTORS_PER_PAGE;
	if (rand_lba_modulu_page_sectors != 0){
		rand_lba = rand_lba - rand_lba_modulu_page_sectors;
	}
	return rand_lba; 
} 

/**
 * simple test that writes up to 70% of sectors in the device sequentially or randomly
 */
int test_access()
{
	int i, lba;
	// SECTORS_PER_PAGE is 8 by deafult and each sector is 512 bytes by default 
	// which means we need 8 sectors for each 4KB write:
	int num_sectors_per_write = SECTORS_PER_PAGE; 
	// we are requested to only write to at most the first 70% of disk sector:
	long int max_number_of_sectors_to_write = 0.7 * SECTOR_NB; 

	// write up to 70% of entire device 
	for(i=0; i + num_sectors_per_write < max_number_of_sectors_to_write; i+=num_sectors_per_write){

			lba = i % SECTOR_NB;
		
		SSD_WRITE(num_sectors_per_write, lba);
	}

	printf("Finished writing workload. \n");

	return 0;
}

int main(int argc, char *argv[]){
	GARBAGE_COLLECTION();
	int setup = 1;
	int req_size_in_4kb_units = atoi(argv[1]);  
	char workload_type = argv[2][0];
	RUN_TEST(setup, test_access(req_size_in_4kb_units, workload_type));

	return 0;
}
