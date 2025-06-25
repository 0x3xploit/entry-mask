# entry-mask
Simple 64-bit ELF packer for binaries compiled with gcc/g++. XOR-encrypts the .text section and injects a custom stub that decrypts and jumps to the original entry point.
