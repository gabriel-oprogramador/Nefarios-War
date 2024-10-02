## Platform Support Status
![Windows](https://img.shields.io/badge/Windows-OK-green) => 
![Windows](https://img.shields.io/badge/Win32-Suported-green)
![Windows](https://img.shields.io/badge/WGL-Suported-green)
![Windows](https://img.shields.io/badge/OpenGL-Suported-green)  

![Linux](https://img.shields.io/badge/Linux-BROKEN-red) =>
![Linux](https://img.shields.io/badge/X11-NotSuported-red) 
![Linux](https://img.shields.io/badge/EGL-NotSuported-red) 
![Linux](https://img.shields.io/badge/OpenGL-NotSuported-red)  

# Note
Sorry for any errors, this was all translated using Google Translate.  
To compile this project use the tools listed in the Dependencies part.  
To use GCC, change the Variable CC = clang to CC = gcc in the Makefile file using a text editor.  
My channel -> [Gabriel O Programador](https://www.youtube.com/@gabriel-oprogramador)

# About
This is a 2D turn-based combat game between underwater ships inspired by 'Thorvarium' where a ship can move, attack, protect itself, drop mines and other things one at a time while observing the state of its ship and enemies through the Sonar. your own underwater ship.

# Synopsis
In a post-World War world where the atmosphere is no longer habitable, the survivors went to live in the depths of the sea where this is only possible with the discovery and mining of a peculiar ore called 'Nefarium' where with it humanity can manufacture structures that surpass the high pressure and corrosion of the seabed.  

You will be in a scenario where each city has to seek its own survival by searching for resources at the bottom of the sea and mainly Nefarium where it is possible to create and maintain structures necessary for everyone's survival.  

Many years have passed since going into the depths and many technologies have already been lost and many no longer master the surrounding technologies.  

Once you have Nefarium, it is possible to refine it, extracting only the best parts of this ore. With this you obtain the 'Varium' ore and as this it is possible to enrich various types of already existing ores such as Iron, Zinc, Copper and others.  

Nefarium is the ore in raw form with which it is possible to manufacture various things, however in this state its greatest capacity is to destroy or degrade other ores, hence its name in Latin.  

# Dependencies
If you use Visual Studio code, it is not necessary to use Clang.  
With Visual Studio code use the 'Makefile Tool' extension to manage the project.  
To use Clang on Windows Add the below to the Vs Code settings file.  
    "makefile.defaultLaunchConfiguration": {  
        "MIMode": "gdb",  
        "miDebuggerPath": "Add the path to W64devkit Path/bin/"  
    }  
* Windows
    + Port of Linux Tools 'make, gcc, mkdir, rm, ...'. Used v1.22.0 -> [W64devkit](https://github.com/skeeto/w64devkit/releases)
    + Clang with format for GDB. Used v14.0.0 -> [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases)  
    I switched to clang 14 because it is the version used on Linux and the versions above for Windows are executable and are accused of viruses.
* Linux
    + Gnu-Make
    + Clang/GCC
    + libgl-dev
    + libegl-dev
    + libx11-dev


