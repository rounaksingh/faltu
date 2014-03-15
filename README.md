# faltu
The project is a **Beginner's level libusb program for sending and receiving SCSI commands to USB mass storage device** that is USB flash drive. The program uses **libusb** for communicating with the USB device in USB layer. It implement the SCSI command library from the **LUFA Library by Dean Camera**, and **FATfs by ChaN**. Now the aim of program is to provide a beginner with basic knowledge of libusb(USB), SCSI commands and FAT filesystem for communicating with a USB flash drive, here communicating means reading, writing a file or folder in the USB flash drive. As a beginner I myself, while implementing a file reading project, experienced a lot of problems, since the three USB, SCSI commands and FAT filesystem are totally exclusive topics, it is not easy job to mix them.

So, finding information for all in one place(or thing) is rare. The one of the rarest place(or destination of search engine journey) is book **USB Mass Storage by Jan Axelson**. The book is a Bible for a person learning "How to interface with USB Mass Storage"? though it has no information about interfacing libusb. For information about libusb, anyone can read the libusb forums which contains huge amount of reading materials.

Let's start with some introduction to all the important things we need to learn about, before using the faltu...

## Aim
The Aim of the project is to provide beginners and enthusiant programmers to introduce the basics of USB, FAT filesystem, and USB flash drives. Since, USB flash drives are very common and useful nowadays. So, there should be a place where wandering (into the darkness of new stuff) persons, like me, can take a rest in the shadows of the USB, FAT filesystem and SCSI protocols. They should get all stuffs and informations about all protocols at one place as when I did the project it was hard for me to find wikis and details about all at a place. However, excellent and complete articles (you can find in Reference section) about aforementioned protocols can be found separately.

When I wrote a simple program for reading/writing from/to USB flash drive, I decided to open-source it, so that somebody do not get confuse and baffle as I did. In this project, I have explained whatever comes in my mind. I have written git wiki about different protocols and softwares dependencies, and how to use them. In addition, I have attached references from the articles and books I have read and wrote the wiki.

Readme have just introductory sections about the protocols and in detail explaination about How to use the program for learning purpose. If you want to read the protocols in *some more details please do read the wikis* with the project.

## Basic Design of program
Now coming to the basic design of the program, that is required to communicate with a USB flash device. For software part, there are three layers which are namely USB host controller driver, SCSI command genrator & SCSI response parser, and filesystem format. Actually the division is on the basis of different protocols, which needs to come together to acomplish our aim, that are USB, SCSI commands and FAT filesystem. Because of mingling of three protocol, the project implementing USB flash devices becomes complicated. Now, below I am providing a visual input for you...

![Block diagram showing the different protocols required.](https://docs.google.com/drawings/d/1ULt5r62bR9rdLrx0l5VQ5kh6L-N4IdeHjZTlaNEqLBQ/pub?w=495&h=751 "Block Diagram of Project")

1. **FAT filesystem library**: 
	The FAT layer manages the files and folders of the filesystem. It basically maintains File Allocation Tables (oh yeah that's the full form of FAT). This tables are nothing but a specific memory location on the disk where the memory locations of files and folders are stored. FAT filesystem are introduced around 70s-80s as a filesystem when there were no like protocols. The Microsoft took it from its initial form and goes on adding new features, removing bugs trying to make it a nice filesystem. But as the size of the storage devices goes on waxing, the FAT filesystem waned. Now, the FAT filesystem is a primitive filesystem though we use it day today in USB flash devices, SD memory cards, and other similar memory storage devices. 

	The FAT filesystem started with the FAT 8, then FAT12, FAT16 and now FAT32. Now there are reasons and drawbacks which made the professionals reluctant to advance to FAT64. And therefore they changed the whole filesystem format and moved to others formats like NTFS, ext2, ext3, ext4. Now, I am not going to explain about the drawbacks and why professionals do not love it, because after reading something about it you will understand, just search about drawbacks of FAT on some decent and fancy search engines.

	Here I am not going into the depths of FAT filesystem, though I will give you some reference where you can find the details in simple words. There are tutorials about FAT filesystem on the web, links are given in Reference section.

2. **SCSI Commands**:
	SCSI commands originated as a protocol for devices that use the "Small Computer Systems Interface" (SCSI) parallel interface. The commands provide a framework for obtaining information from a storage device, controlling the device’s operation, and reading and writing blocks of data in the storage media. The SCSI commands are standarized by technical committee of T10. Also, the USB-IF has provided a specification (with the help of T10), the specification is released under the class of "USB Mass Storage" with name "UFI command specification". 

	The UFI specification from USB-IF consisits of a list of SCSI commands, with full detail and sequence of command, which can be implemented for USB flash drive. The specification uses a simple and easily understandable which can be helpful to persons who have an intermediate level knowledge of the USB 2.0 specification (or just how the USB works and how the communication takes place between USB host and USB device).

	Please download the specifications for USB Mass Storage class from the link in Reference section.

3. **Universal Serial Bus (USB)**:
	The USB is a serial communication protocol, defined with an aim to provide every possible ease and flexiblity to the end-user only. The USB is now-a-days is also one of the popular protocol, that can be used with a wide range of devices for data communication. The major part of USB is not the use but implementation, because of its aim, the whole burden is on the developer and device. Developer need to do the rough work of implementation, so that the end user can use it easily.

	To begin with, USB is a host-controlled bus communication, that means there is just one host(master) and several slaves(according to USB specification 2.0, a host can connect to 127 slaves). Now, host is the main player which detect the slave connected, initiate transfers, send/receive data, keeps track of data packets, and other tasks such as attaching a 7-bit address on the bus. For slaves, a specific terminology is used that is "USB devices". The USB devices wait for the host to detect them(enumeration step), also they waits for data transfer, until the host starts transfer. The devices are functions they just do specific functions, while controlled by some higher authority.

	Please read the wiki with the project. Also, I would like to recommend USB Mass Storage book by Jan Axelson. The book explains the introduction of USB, details about the SCSI commands and FAT filesystem; it is basically everything for a beginner to start with. Moreover, if you want to learn USB protocol in depth you can refer to USB Complete by the same author. It is good to read a book before reading a specification directly, since reading specification directly at first seems confusing and sometimes, blow one's mind (as it blowed mine at first). I am not going to recommend specification, at first, to a beginner.

	**Lakeview Research** is a website (created by Jan Axelson) which has books, links and articles for anything related to USB. It has links to nice projects and articles on the Internet. If you want to do anything with USB, I would ask you to just pay a visit to this site. Link is provided in the Reference section.

## About the Project
The project consists of source files (in folder src) and a Makefile. Just clone the project and change to faltu directory and do a "make main" or "make all", to compile the project. To clean the compiled and build documents, do a make clean. "How to compile the project" is expained below.

### Project Branches
The project consists of three branches based on the complexity and level of implementation, below are the names of branches with description:

1. master: 
	code using FATfs, USB Mass Storage commands and libusb code. The code implements FATfs latest version R0.10a which has a lots of improvements (however most APIs of FATfs are almost compatible in working with Petit FATfs) comparing with the Petit FATfs. However, the improvements increases the complexities for implemention. So, usage of this branch is not recommended to the beginner at first, without having basic knowledge of APIs and 

2. usingPetitFATfs: 
	code using Petit FATfs(which is small footprint of FAT than FATfs), USB Mass Storage commands and libusb. Petit FATfs implement less features than FATfs, but complexity is less and that's good for a beginner to comprehend the APIs of FATfs. Petit FATfs has some restrictions such as low write features means it can only write to an existing file and cannot create a new file, however it can perform basic functions such as reading a file, listing files and folders under a folder.

3. simple_direct_SCSI: 
	code with no implementation of FAT filesystem, but only SCSI commands and libusb. This branch can give a feel to the lower level of protocol that is SCSI commands. The user can do testing with the SCSI commands and understand the working of the commands and USB.

### Platform
The Platform for compiling and running the program is linux based OS that is debian, ubuntu, redhat, fedora, etc. For the project, the platform dependency is because of the requirement of libusb 1.0.8 or later. Since, the libusb 1.0 is not ported for windows, so I am afraid that windows cannot use the program. While all other parts (such as SCSI commands and FAT filesysytem programs) are dependency free. Therefore anyone can port the parts (save libusb) of the program for any other platform.

### Project Dependencies:
Project depends on:

1. GNU linux OS :
	The project is based on library and APIs which donot work on the non-linux systems. Therefore the project is dependent on linux.

2. **libusb library version 1.0.8 or later**: 
	To download and install the library and development packages, please use the source respository and download using apt or yum facility. 

	**Debian and Debian-based OS**

		sudo apt-get install libusb-1.0-0-dev
	This command when run terminal download the development packages and binary library required for compiling and running the program.

3. **gcc version 4 or later**: 
	GCC compiler is required for compiling and building the project.

### What things are required other than Software dependencies
As a primary requirement, we need a USB flash drive (Pen drive).

### How to know the VID and PID of a USB device
Most of the linux box has "lsusb" as a built-in command to access the list of connected USB devices. So, connect the USB device and run the command given below.

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

##References:
1. libusb:
	* [Home Page of libusb](http://libusb.org/)
	* [Documentations of APIs of libusb](http://libusb.sourceforge.net/api-1.0/index.html)

2. USB
	* [Lakeview Research -- USB](http://www.lvr.com/usb.htm)
	* [Lakeview Research -- Mass Storage](http://www.lvr.com/mass_storage.htm)
	* [USB.org developers download section](http://www.usb.org/developers/devclass_docs)

3. FAT filesystem
	* [Lakeview Research -- Mass Storage](http://www.lvr.com/mass_storage.htm)
	* [Understanding FAT32 Filesystems](http://www.pjrc.com/tech/8051/ide/fat32.html)
	* [The FAT File Systems](http://www.ntfs.com/fat-systems.htm)
	* [Home Page for FATfs](http://elm-chan.org/fsw/ff/00index_e.html)

4. SCSI commands
	* [USB.org developers download section](http://www.usb.org/developers/devclass_docs)
