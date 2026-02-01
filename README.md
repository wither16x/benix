# The Benix Operating System
Benix is a small 32-bit hobby operating system for x86 I made for fun and learning. It is written in Assembly (Intel syntax) and C. Every part of the OS has been made from scratch, from the bootloader to the user space.

![Benix showcase 0](assets/benix_showcase0.png)

## Features
- Multiboot 2 protocol compatibility
- Limine bootloader compatibility
- Useful tools for building
- Minimalist userspace libc
- Userspace
- Flat binary loader
- VGA text mode support
- FAT12 filesystem support
- ATA PIO disk support
- PS/2 keyboard support
- PIC support

## Build instructions
First, you need to clone the git repository. If you don't have git, you can install it from your distribution packages.
```sh
git clone https://github.com/wither16x/benix
```
Benix does not use BASH, ZSH or any other standard shell scripting language for its build scripts. Instead, it uses [the Amber programming language](https://github.com/amber-lang/amber). Make sure it is installed on your system and run the following command:
```sh
tools/mkimage/run -- build
```
You can also build the system on an existing FAT12 image, so it will not be reformated:
```sh
tools/mkimage/run -- build image path/to/your/image.img
```
It is recommended to build Benix with **the Limine bootloader** (you would get a warning if you build it with the Benix bootloader).
```sh
tools/mkimage/run -- build bootloader limine # or benix
```
**Note:** you need to download and compile Limine before building the OS. To do so, you can use `getdeps`:
```sh
tools/getdeps/run -- limine
```
**Note:** you should verify the version of the OS you are building first:
```sh
tools/mkimage/run -- version
```
### Bootloader: Benix
Now, a floppy image should be available as `images/benix.img`.
### Bootloader: Limine
An ISO file and a floppy image should be available in `images/` as `benix.iso` and `benix.img` (or the name you specified for the floppy).
If you have `qemu` and you want to try Benix on it, execute the command below:
```sh
tools/mkimage/run -- emulate qemu
```
**Note**: if you need more informations about the Benix image builder, run this command:
```sh
tools/mkimage/run -- help
```
**Benix has not been tried on real hardware yet!**

## Programs
Several programs are provided within the Benix disk image. Their source code is located inside the `programs/` directory, **but they will be moved in separated repositories soon**.
- `cash` (Certainly Amazing SHell): the default shell for Benix
- `ls <dir>`: displays a list of the files
- `show <file>`: displays the content of a file in ASCII
- `info [options]`: displays informations about the system (these informations are located in `osinfo.txt`)
- `echo [text]`: displays the given text followed by a newline in the console
- `touch <file>`: creates a new empty file
- `write <file>`: writes text in a file
- `mkdir <dir>`: creates a new directory
- `rm [options] <file>`: removes a file or an empty directory

## Documentation
There is no real documentation. Some of the code has comments, but you still need Assembly and C knowledge to understand what it does if it is not clearly explained or not explained at all...

## Contributing
To contribute to Benix:
1. Fork the git repository
2. Apply your changes
3. Test them carefully
4. Describe them in [CHANGELOG.md](CHANGELOG.md)
5. Make a detailed pull request