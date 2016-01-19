target extended-remote localhost:3333

file ./build/fml.elf
monitor reset halt
load
monitor reset init
