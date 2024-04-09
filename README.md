# zynq_us-_write_to_SD
This C code can be used in any Xilinx FPGA with a Zynq PS. It uses a generic FatFs to mount and write to an SD card slot on your FPGA board.

Make sure to include your xilffs in your BSP in Vitis or else the libraries needed to use FPGA to store to SD won't work. 

Here are the fatfs API docs for further reference.
http://elm-chan.org/fsw/ff/
