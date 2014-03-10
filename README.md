#faltu
The project is a Beginner's level libusb program for sending and receiving SCSI commands to USB mass storage device that is USB flash drive. The program uses libusb for communicating with the USB device in USB layer. It implement the SCSI command library from the LUFA Library by Dean Camera, and Petit FATfs by ChaN. Now the aim of this program is to provide a beginner with basic knowledge of libusb(USB), SCSI commands and FAT filesystem for communicating with a USB flash drive, here communicating means reading, writing a file or folder in the USB flash drive. As a beginner I myself, while implementing a file reading project, experienced a lot of problems, since the three USB, SCSI commands and FAT filesystem are totally exclusive topics. 

So, finding information for all in one place(or thing) is rare. The one of the rare place(or thing) is book USB Mass Storage by Jan Axelson. The book is a Bible for a person learning "How to interface with USB Mass Storage"? though it has no information about interfacing libusb. For information about libusb, anyone can read the forums which contains huge amount of reading materials.

Let's start with some introduction to all the important things we need to learn about, before using the faltu...

## Aim
The Aim of the project is to provide beginners and enthusiant programmers to introduce the basics of USB, FAT filesystem, and USB flash drives. Since, USB flash drives are very common and useful nowadays. So, there should be a place where wandering (into the darkness of new stuff) persons, like me, can take a rest in the shadows of the USB, FAT filesystem and SCSI protocols. They should get all stuffs and informations about all protocols at one place as when I did the project it was hard for me to find wikis and details about all at a place. However, excellent and complete articles (you can find in Reference section) about aforementioned protocols can be found separately.

When I wrote a simple program for reading/writing from/to USB flash drive, I decided to open-source it, so that somebody do not get confuse and baffle as I did. In this project, I have explained whatever comes in my mind. I have written git wiki about different protocols and softwares dependencies, and how to use them. In addition, I have attached references from the articles and books I have read and wrote the wiki.

Readme have just introductory sections about the protocols and in detail explaination about How to use the program for learning purpose. If you want to read the protocols in *some more details please do read the wikis* with the project.

##Modules
Now coming to the macro-scale design of the program, which is required to communicate with a USB flash device. For software part, there are three layers which are namely USB host controller driver, SCSI command genrator & SCSI response parser, and filesystem format. Actually the division is on the basis of different protocols, which needs to come together to acomplish our aim, that are USB, SCSI commands and FAT filesystem. Because of mingling of three protocol, the project implementing USB flash devices becomes complicated. Now, below I am providing a visual input for you...

				 ___________________________________________________________________
				|																	|
				|						Petit FAT filesystem						|
				|___________________________________________________________________|
				|																	|
				|							SCSI Commands							|
				|___________________________________________________________________|
				|																	|
				|							libusb (USB)							|
				|___________________________________________________________________|

1. Petit FAT filesystem module: 

The FAT layer manages the files and folders of the filesystem. It basically maintains File Allocation Tables (oh yeah that's the full form of FAT). This tables are nothing but a specific memory location on the disk where the memory locations of files and folders are stored. FAT filesystem are introduced around 70s-80s as a filesystem when there were no like protocols. The Microsoft took it from its initial form and goes on adding new features, removing bugs trying to make it a nice filesystem. But as the size of the storage devices goes on waxing, the FAT filesystem waned. Now, the FAT filesystem is a primitive filesystem though we use it day today in USB flash devices, SD memory cards, and other similar memory storage devices. 

The FAT filesystem started with the FAT 8, then FAT12, FAT16 and now FAT32. Now there are reasons and drawbacks which made the professionals reluctant to advance to FAT64. And therefore they changed the whole filesystem format and moved to others formats like NTFS, ext2, ext3, ext4. Now, I am not going to explain about the drawbacks and why professionals do not love it, because after reading something about it you will understand, just search about drawbacks of FAT on some decent and fancy search engines.

Here I am not going into the depths of FAT filesystem, though I will give you some reference where you can find the details in simple words.


2. SCSI Commands:

SCSI commands originated as a protocol for devices that use the Small Computer Systems Interface (SCSI) parallel interface. The commands provide a framework for obtaining information about a storage device, controlling the device’s operation, and reading and writing blocks of data in the storage media. 

3. USB

The USB is 


## How to interface with a USB mass storage device?
The project consists of a source folder having all source files and a Makefile. Just clone the project and change to faltu directory and do a make main or make all, to compile the project. TO clean the compiled and build documents, do a make clean.

### Platform
The Platform for compiling and running the program is linux based OS that is debian, ubuntu, redhat, fedora, etc. For the project, the platform dependency is because of the requirement of libusb 1.0.8 or later. Since, the libusb 1.0 is not ported for windows, so I am afraid that windows cannot use the program. While all other parts (such as SCSI commands and FAT filesysytem programs) are dependency free. Therefore anyone can port the parts (save libusb) of the program for any other platform.

### Dependencies:
Project depends on:

1. libusb library version 1.0.8 or later: 

To download and install the library and development packages, please use the source respository and download using apt or yum facility. 

**Debian and Debian-based OS**

	sudo apt-get install libusb-1.0-0-dev
This command when run terminal download the development packages and binary library required for compiling and running the program.

2. gcc version 4 or later

### What things are required other than Software dependencies
As a primary requirement, we need a USB flash drive. Also, the computer on which you want to run 

### How to know the VID and PID of a USB device
Most of the linux box has "lsusb" as a built-in command to access the list of connected USB devices.

	$ lsusb

The same commands can be used to print the PID and VID of the USB devices. 

The command also provide the functionality for printing decriptors such as device, configuration, interface and endpoint descriptors of a particular connected USB device. The aforementioned functionality can be achieved using below command:

	$ lsusb -v -d aaaa:bbbb

where aaaa is the hexadecimal VID and bbbb is the hexadecimal PID.

For more information about lsusb, please read its man page by using the following command:

	$ man lsusb

### How to compile and run the program
Please change the VID and PID of device in main.h. Also, the starting number of sectors(starting LBA) and no of bytes to read (for now provide no of bytes to read should be multiple of Block or sector size) should be provided in main.h. Moreover, note that if you have install libusb-1.0-0-dev package in folder other than /usr/local/lib, then update the folder location of libusb by changing the LIBUSB_DIR variable in Makefile.

To compile the program, do:

	$ make all

To clean the project:

	$ make clean

The compilation creates build files, which are stored in build folders, and binary executable file which is stored in bin folder. All folders are created automatically while building the project using Make utility. Moreover, while cleaning the project, build and bin folders including files, inside them, are removed.

To run the program, please execute the "main" in bin folder. 

Important Note: If the USB device has not have user level access permission, then please execute the "main" binary file with super user priviledge.
From project folder, run:

	$ sudo ./bin/main 

### Brief about Errors

##References:
1. 

