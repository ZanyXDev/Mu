# Palm m515/OS 4.1 emulator(Mu)

This is a complete restart of my Palm OS emulator, with the last one the code got too messy and the goal too ambitious(to emulate every palm API in software and for compatibility with all Palm OS versions and devices).


# The goal of this project

To emulate every part of the Palm m515 that is used by the OS perfectly.
This means no hacks like POSE where API calls are intercepted and replaced with those that dont use the actual hardware.
It is also written in C for RetroArch so it will run on everything at the start, not just Win95 like POSE and Copilot and is being developed with modern emulator features in mind, save/load state and overclocking are available from the start.

## Why the m515?

It is the best Palm brand OS 4 device that has no special hardware builtin(cellphone, barcode scanner, keyboard attachment), it has a color screen, and available ROM dumps.

## What about accessories?

The Palm keyboard attachment will likely be emulated later on so the PC keyboard can be used on text fields.

## OS improvements

If there is something the OS does not support(like an OS 5 API) I may add it manually, this will be in the form of a custom firmware as a ROM image or a .bps patch, not a patch to the emulator with only 1 exception "PceNativeCall" which would require an ARM interpreter, it would also be an option to not apply this patch leaving you with an accurately emulated stock Palm m515.

These patches will not be worked on until the emulator is actually running.

## But you shouldnt waste your time on OS 4, OS 5 can do more.

I have made a video of OS 5 doing that "more", it is extremely incompatible with OS 4 games which most games are, very few games used the ARM CPU and those that did are available on other platforms, some used the 320x320 display mode.

On Clie PEG-SL10:  
ArgonV:      Normal  
Galax:        Normal  
Zap!20XX:  Normal  
Invaders:    Normal  
Platypus:    No ARM CPU, Cant Run

On Palm T|X:  
ArgonV:      Runs At Warpspeed  
Galax:         Audio Cuts Out  
Zap!20XX:  Audio Cuts Out  
Invaders:    Reboots System  
Platypus:    Normal, Used To Prove Speaker Isnt Defective  
Bonus:        Has Adware(Addit)

[Palm T|X vs OS 4 Games](https://youtu.be/ithVLI_M6i0)

The lack of ARM CPU will be fixed in my emulator see "OS improvements".

## Credits
uARM (ARMv5 Core)  
Musashi v3.4 (last version that builds outside of MAME)(68k Core)  
blip_buf 1.1.0 (Audio Resampler)  
http://www.iconarchive.com/show/crystal-clear-icons-by-everaldo/App-palm-icon.html (Desktop Icon)  
http://tango.freedesktop.org/Tango_Icon_Library (Palm Action Buttons, All UI buttons)  
https://www.iconfinder.com/icons/2317746/chip_computer_cpu_device_frequency_microchip_processor_icon (hwTestSuite Icon)  
https://www.flaticon.com/free-icon/cow_235371#term=cow&page=1&position=13 (muExpDriver Icon)  
https://findicons.com/icon/164302/cursor (Libretro Port Joystick Cursor)

## Building
#### For RetroArch
Make sure you have done all the steps here https://docs.libretro.com/ under "For Developers/Compilation" so you build environment works.  

    cd ./libretroBuildSystem
    make

#### For Qt
Install Qt 5.11.1 and Qt Creator(optional) if not installed  
Open the .pro file in Qt Creator and click "Run" or build from command line

##### To build without Qt Creator(untested)

    cd ./qtBuildSystem/Mu
    qmake
    make

#### TestSuite for Palm OS
Install prc-tools from the below link(self compiled or prepackaged VM)  

    cd ./tools/palm/hwTestSuite
    ./make.sh

#### MuExpDriver for Palm OS
Install prc-tools from the below link(self compiled or prepackaged VM)  

    cd ./tools/palm/muExpansionDriver
    ./make.sh

## Running
#### Files
palmos41-en-m515.rom(all euro languages, from Palm dev program): 412557a221933a8be12622de7a21320a (MD5)  
palmos41-en-m515.rom(English only, verified authentic from device): 83cb1d1c76e568b916dc2e7c0bf669f6 (MD5)  
bootloader-en-m515.rom(v1): 9da101cd2317830649a31f8fa46debec (MD5)  
bootloader-en-m515.rom(v2): e64c755205bc3e79d1bc5b70128f688d (MD5)  

If you can read English or intend to install your own language pack use the verified version:  
I understand not everyone speaks English so I list both ROMs, I do all my testing on the English only build because I have verified it with an actual device, but will do my best to support the Palm dev version as well(it runs on the same hardware anyway).

#### For Qt
 1. Run once to create the directory tree
 2. Copy "palmos41-en-m515.rom" to "~/Mu"
 3. Copy "bootloader-en-m515.rom" to "~/Mu"
 4. Run emu and press start button

 #### For RetroArch
 1. Download "Palm OS(Mu)" from "Online Updater->Core Updater"
 2. Go back, select "Load Core", select "Palm OS(Mu)"
 3. Run "palmos41-en-m515.rom"

## Tools
[Prc-tools, Palm OS SDKs, pilrc, pilot-link](https://github.com/meepingsnesroms/prc-tools-remix)

## License
<a rel="license" href="http://creativecommons.org/licenses/by-nc/3.0/us/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/3.0/us/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/3.0/us/">Creative Commons Attribution-NonCommercial 3.0 United States License</a>.

## Links
[Website](https://meepingsnesroms.github.io/)  
[Discord](https://discord.gg/hWSz8VN)

## Also see Mu's sister project
[🐿SquirrelJME](https://multiphasicapps.net)
