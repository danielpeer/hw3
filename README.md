VSSIM: Virtual machine based SSD SIMulator
-----
* Maintainer : Jinsoo Yoo (jedisty@hanyang.ac.kr)
* Contributor : Joongwoo Hwang, Haesung Kim, Joohyun Kim 

VSSIM is an SSD Simulator. Originally it is run as performed as a full virtualized system based on QEMU. For the purpose of this course it is run outside of a virtual machine context with its own main() and direct issuing of calls to its latency simulator module.

What is the merit?
-----
VSSIM can flexibly model the various hardware components, e.g. the number of channels, the number of ways, block size, page size, the number of planes per chip, program, erase, read latency of NAND cells, channel switch delay, and way switch delay. VSSIM can also facilitate the implementation of the SSD firmware algorithms. 

Publication
-----
* Jinsoo Yoo, Youjip Won, Joongwoo Hwang, Sooyong Kang, Jongmoo Choi, Sungroh Yoon and Jaehyuk Cha, VSSIM: Virtual Machine based SSD Simulator In Proc. of Mass Storage Systems and Technologies (MSST), 2013 IEEE 29th Symposium on, Long Beach, California, USA, May 6-10, 2013

* Joohyun Kim, Haesung Kim, Seongjin Lee, Youjip Won, FTL Design for TRIM Command In Proc. of Software Support for Portable Storage (IWSSPS), 2010 15th International Workshop on, Scottsdale, AZ, USA, October 28, 2010

Acknowledgement
-----
* This work is sponsored by IT R&D program MKE/KEIT. [No.10035202, Large Scale hyper-MLC SSD Technology Development].

User Guide
-----

#### Folder Composition

![Folder Composition]( http://dmclab.hanyang.ac.kr/wikidata/img/folder_arch_git.jpg)

1. CONFIG: In CONFIG folder, there is ssd.conf file, which is used to configurate virtual SSD, and a source code that uses this file to design virtual SSD.

2. FIRMWARE: In FIRMWARE folder, there is the wrapper for calls to the latency manager and a firmware(IO Buffer) source code (disabled in this version). 

3. FTL: In FTL folder, subsequent folders of FTL_SOURCE folder are COMMON, PAGE_MAP, PERF_MODULE, and QEMU_MAKEFILE folder.

    * COMMON: There is ‘common.h’ file that includes FTL header file.

    * PAGE_MAP: There is Page Mapping FTL Code and Garbage Collection Code.

    * PERF_MODULE: There is a source code of VSSIM Performance Module, which manages information on VSSIM’s SSD behavior and transfers this to monitor.

    * QEMU_MAKER: There is Makefile, which QEMU uses to compile FTL code. Not used in this version.

4. SSD_MODULE: There is SSD IO Manager related source code that emulates SSD’s NAND IO operation, and also SSD Log Manager related code, which is a communication source code that transfers virtual SSD’s operation to SSD Monitor. 

5. LOCAL: Code for running the SSD latency manager outside of the context of a virtual machine. All tests will be run using this directory.

Other folders not relevant in this version include:

1. MONITOR: There is a source code of SSD Monitor, which is a graphic user interface. It was disabled in this version.

2. OS: This is a folder where iso files of necessary OS are located when VSSIM installs Guest OS. Not necessary in this version.

3. QEMU: QEMU related source code is located. Not necessary in this version.

4. RAMDISK: There is a Shell script that creates Ramdisk and executes mount. Not necessary in this version.

#### Virtual SSD Setting

This section explains about the structure of virtual SSD, in other words, the section will describe how to set the number of flash memories, the number of channels, the number of ways, page size, and etc. Virtual SSD Setting is done by editing ‘ssd.conf file in VSSIM_V/CONFIG. What each parameter refers to in the file is shown as follow.

1. ssd.conf File:

    - PAGE_SIZE: the size of one page (Byte)
    - SECTOR_SIZE: the size of one sector (Byte)
    - FLASH_NB: the number of flash memories in a whole SSD (unit)
    - BLOCK_NB: the number of blocks per flash memory (unit)
    - PLANES_PER_FLASH: the number of planes per flash memory (unit)
    - REG_WRITE_DELAY: delay in register write (usec)
    - CELL_PROGRAM_DELAY: delay in nand page write (usec)
    - REG_READ_DELAY: delay in register read (usec)
    - CELL_READ_DELAY: delay in nand page read (usec)
    - BLOCK_ERASE_DELAY: delay in nand block erase (usec)
    - CHANNEL_SWITCH_DELAY_R: delay in channel switch during read operation (usec)
    - CHANNEL_SWITCH_DELAY_W: delay in channel switch during write operation (usec)
    - CHANNEL_NB: the number of channels in SSD (usec)
    - WRITE_BUFFER_FRAME_NB: the number of buffer frame for write operation (sector)
    - READ_BUFFER_FRAME_NB: the number of buffer frame for read operation (sector)
    - OVP: Over provisioning percentage (%)


#### Compile / Execution

This section explains the entire process of actually compiling VSSIM (without QEMU).

1. FTL Setting

    VSSIM is modularized to make it easy for the user to easily change FTL. One can change FTL using simple link/unlink script. To link our FTL code

        $ cd VSSIM-1/CONFIG/FTL_SETTING
        $ ./unlink_pm_local
        $ ./link_pm_local

2. VSSIM Execution

    Change tests in VSSIM-1/LOCAL/tests.c as you see fit. To run your code

        $ cd VSSIM-1/LOCAL
        $ make
		$ ./tests

