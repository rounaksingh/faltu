#faltu
The project is a Beginner's level libusb program for sending and receiving SCSI commands to USB mass storage device that is USB flash drive. The program uses libusb for communicating with the USB device in USB layer. It implement the SCSI command library from the LUFA Library by Dean Camera, and Petit FATfs by ChaN. Now the aim of this program is to provide a beginner with basic knowledge of libusb(USB), SCSI commands and FAT filesystem for communicating with a USB flash drive, here communicating means reading, writing a file or folder in the USB flash drive. As a beginner I myself, while implementing a file reading project, experienced a lot of problems, since the three USB, SCSI commands and FAT filesystem are totally exclusive topics. 

So, finding information for all in one place(or thing) is rare. The one of the rare place(or thing) is book USB Mass Storage by Jan Axelson. The book is a Bible for a person learning "How to interface with USB Mass Storage"? though it has no information about interfacing libusb. For information about libusb, anyone can read the forums which contains huge amount of reading materials.

Let's start with some introduction to all the important things we need to learn about, before using the faltu...

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



with the 

## How to interface with a USB mass storage device?
The project consists of a source folder having all source files and a Makefile. Just clone the project and change to faltu directory and do a make main or make all, to compile the project. TO clean the compiled and build documents, do a make clean.

### Dependencies:
Project depends on:

1. libusb 1.0 : 

libusb is the


##References:
1. 

