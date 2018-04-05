file build/btle_und_snes.elf
 target remote localhost:2331
break main
mon semihosting enable
mon semihosting ThumbSWI 0xAB

