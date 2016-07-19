
Build Env, Ubuntu 
====

The dev tools for ubuntu are not quite right, so I used this PPA:

https://launchpad.net/~terry.guo/+archive/ubuntu/gcc-arm-embedded

    sudo apt-get remove binutils-arm-none-eabi gcc-arm-none-eabi
    sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded
    sudo apt-get update

To install, on 15.10 Wily, I did:

    sudo apt-get install gcc-arm-none-eabi=4.9.3.2015q3-1wily1


Requirements
====

requires the file libarm_cortexM4lf_math.a which is from the 'cmsis' distribution.

You must register then go to downloads in the self-service portal here:
https://silver.arm.com/

* Downloads
	* ARM and AMBA Architecture
		* CMSIS MCU Software Standard

or direct:
https://silver.arm.com/browse/CMSIS



Info from the project I forked for the arm stuff
====

stm32f4xx GCC Barebones Project
----

Very basic project to get started with GCC and the stm32f4xx (Discovery board, in this case)
It includes the 1.3.0 STM32F4xx Standard Peripheral Driver Library, but DOES NOT include USB support (it is a 'barebones' project after all)

If you want a slightly more useful(but also complex) project, check out the other branches.
usb-device - Simple usb device example using CDC. 
usb-console - Same as usb-device, but includes some command parsing code to make adding functionality simpler.

Build instructions
----

Make sure you have the arm-none-eabi(gcc, etc...) tools installed: https://launchpad.net/gcc-arm-embedded

Running make on the top directory should build both the driver library, support files, and main.c
Output files are in the build/ directory


Programming instructions (using openOCD and gdb)
----

Connect using openOCD and the included configuration file.

    $ openocd -f stm32f4xx-openOCD.cfg

On a separate terminal window, run arm-none-eabi-gdb

    $ arm-none-eabi-gdb 

Connect to openOCD

    (gdb) target extended-remote localhost:3333

Load program the stm32f4xx

    (gdb) file build/stm32f4xx-gcc-barebones.elf
    (gdb) monitor reset halt
    (gdb) load
    (gdb) monitor reset init

To run you program:

    (gdb) continue
