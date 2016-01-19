target extended-remote localhost:3333

file ./build/stm32f4xx-gcc-barebones.elf
monitor reset halt
load
monitor reset init
