# vrCPU
Code, documentation, schematics, notes for my [Ben Eater](https://eater.net/8bit "Ben Eater") and [James Bates](https://github.com/jamesbates/jcpu "James Bates") inspired breadboard computer, assembler and web-based [emulator](https://cpu.visualrealmsoftware.com/emu/ "emulator") using a C backend compiled to WASM.

![The real deal](https://cpu.visualrealmsoftware.com/img/computer_gh.jpg "The real deal")

## Structure
### Arduino
* Microcode EEPROM writer
* DecimalDisplay EEPROM writer
* ESP8266 Wi-Fi Program Loader
* Page-write-enabled EEPROM writer library (Tested on Greenliant GLS29EE010)
### Emulator (C library)
* SimLib - The emulator core
* SimInst - A single instance interface of the emulator core
* SimWin - A windows executable around the library (used for testing)
* SimWasm - Emscripten source and scripts to produce WASM output
### Notes
Various files used while building the breadboard computer
### Programs
Various ASM programs used to test the assembler (and the emualtor and physcial computer)
### Web
A copy of most files from [https://cpu.visualrealmsoftware.com](https://cpu.visualrealmsoftware.com "https://cpu.visualrealmsoftware.com") including:
* Assembler derived from [customasm](https://github.com/hlorenzi/customasm "customasm")
* Web-based Emulator (C engine compiled to WASM)

## Videos
##### An overview
[![An overview.](https://img.visualrealmsoftware.com/youtube/thumb/7H-PHc1CNSU)](https://youtu.be/7H-PHc1CNSU "My 8-bit breadboard computer, assembler, emulator and esp8266 program loader. An overview.")

##### Snake!
[![Snake!](https://img.visualrealmsoftware.com/youtube/thumb/xVAOxb0N3pg)](https://youtu.be/xVAOxb0N3pg "Snake! on my Ben Eater inspired 8-bit breadboard computer.")

##### Virtual versus reality. 
[![Virtual versus reality.](https://img.visualrealmsoftware.com/youtube/thumb/90tw-9bhMc0)](http://www.youtube.com/watch?v=90tw-9bhMc0 "Virtual versus reality. My Ben Eater inspired computer and emulator.")

##### Troy's breadboard computer - Triangular numbers
[![Troy's breadboard computer - Triangular numbers](https://img.visualrealmsoftware.com/youtube/thumb/Zj5HfeiyHRU)](http://www.youtube.com/watch?v=Zj5HfeiyHRU "My Ben Eater (and James Bates) inspired 8-bit computer..(Triangular numbers)")

##### Troy's breadboard computer - Emulator
[![Troy's breadboard computer - Emulator](https://img.visualrealmsoftware.com/youtube/thumb/omVyW-ZOdC8)](http://www.youtube.com/watch?v=omVyW-ZOdC8 "Web-based Emulator of my Ben Eater inspired 8-bit computer")

## License
This code is licensed under the [MIT](https://opensource.org/licenses/MIT "MIT") license
