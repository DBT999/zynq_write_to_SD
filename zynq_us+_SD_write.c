/*
*
*	Below are defines, data structs, and include libs needed for your .h header file
*
*/
#include <stdio.h>
#include <stdarg.h>

#include "xparameters.h"
#include "xil_io.h"
#include "sleep.h"
#include "xstatus.h"
#include "xsdps.h"
#include "ff.h"
#include "xil_printf.h"
#include "xil_types.h"

XSdPs SdIntf;													// SD interface driver instance

// Generic FAT file system data structures
FATFS 							Fatfs;							// FATFS struct
FIL 							Fil;							// File struct
MKFS_PARM 						MkfsParm;						// Format parameter


#define SD_DEVICE_ID			XPAR_XSDPS_0_DEVICE_ID			// Your hardware's SD slot ID/address

/*
*
*	Below is the code and the variables needed
*
*/

const TCHAR *sd_file = "SD.bin";
const TCHAR *path = "0:/";
s8 buf_work[FF_MAX_SS]; 										// FF_MAX_SS is the max buffer size defined by your hardware. It is usually = 512.
u32 bytes_written;
FRESULT sd_step; 												// This keeps track of the step currently during the being undertaken
int status;
	
void sdStartup() {

	XSdPs_Config *SdConfig;										// Xilinx SD interface configuration struct
	int status;

	// Initialize SD interface driver by getting SD configuration struct
	// then use that to initialize Xilinx driver.
	SdConfig = XSdPs_LookupConfig(SD_DEVICE_ID);
	if (NULL == SdConfig) {
		return XST_FAILURE;
	}
	status = XSdPs_CfgInitialize(&SdIntf, SdConfig, SdConfig->BaseAddress);
	if (status != XST_SUCCESS){
		return XST_FAILURE;
	}
	status = XSdPs_CardInitialize(&SdIntf);
	if (status != XST_SUCCESS){
		return XST_FAILURE;
	}

	return;
}

void sdWrite() {
	xil_printf("\n\rBegin open, write, and close of SD file.");
	
	// Logical mounting of SD card
	sdMount();

	// Open file with proper permissions
	sd_step = f_open(&Fil, sd_file, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	if (sd_step){
		xil_printf("Error. Opening SD card failed. sd_step = %d.\n\r", sd_step);
		return XST_FAILURE;
	}

	// Set pointer to beginning of file. The "i" can be removed if you are not storing multiple packets in a loop.
	// the second argument with the "i" gives the pointer where writing will begin in the next sd_step.
	sd_step = f_lseek(&Fil, i * words_per_packet * packets * sizeof(s16));
	if(sd_step){
		xil_printf("Error. Cannot find beginning of file.\n\r");
		return XST_FAILURE;
	}

	// Write data from system mem to file. 2nd arg is pointer to where your unstored data lives. 3rd arg is number of bytes to write.
	// 4th arg is the function returning number of bytes that have been written to SD.
	sd_step = f_write(&Fil, (const void*)(s16 *)buf_acq_to_dma, words_per_packet * packets * sizeof(s16), &bytes_written);
	if (sd_step){
		xil_printf("Error. Failed to write to file.\n\r");
		return XST_FAILURE;
	}

	// Close file
	sd_step = f_close(&Fil);
	if (sd_step){
		xil_printf("Error. Could not close the file.\n\r");
		return XST_FAILURE;
	}

	xil_printf("\n\rData written to SD card successfully.");
}