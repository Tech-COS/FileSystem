# FileSystem Module

This repository stores a module of the [COS Kernel](https://github.com/Tek-EIP/COS).    
This module's primary purpose is to interact with an EXT4 Filesystem.   
It also includes a standalone CLI tool similar to mkfs.ext4 allowing one to format an image in a minimalistic EXT4 compatible filesystem.

## Documentation

One may find the documentation of the project under the Github Wiki associated with the COS Kernel.

## Dependencies
To build the project, one must install several dependencies first.

* Under Fedora:

    * Install the following packages with dnf: ``make`` and ``gcc``.

    * Download the x86_64 toolchain: https://github.com/lordmilko/i686-elf-tools/releases/download/13.2.0/x86_64-elf-tools-linux.zip

    * Copy all resulting files in /usr.

* Under Arch Linux:

    * Install the following packages with pacman: ``make`` and ``gcc``.

    * Install the following packages from the Arch User Repository (AUR): ``x86_64-elf-gcc`` and ``x86_64-elf-binutils``.

## Building

Then, run the ``make cos_mkfs`` command to build the project.\
To build the unit tests, run ``make test_run`` instead.

## Usage Examples

**A RAW DISK IMAGE MUST BE MADE.**\
To do so, one may use the command: ``dd if=/dev/zero of=test_disk  bs=1M  count=128``\
Using the command ``./cos_mkfs test_disk`` will format the image file to an incomplete version of the EXT4 specification.

## Limitations

- The disk image has to be exactly 128MB in size.
- The inode size is always 256 bytes long.
- The inode ratio is always 16384 inodes per group.

## License

Repository licensed under the MIT License which may be read here: 