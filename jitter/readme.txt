Jitter (igfxcmjit32.so for 32bit, igfxcmjit64.so for 64bit) is an online
compiler to convert VirtualISA into Gen HW instruction, while VirtualISA
is an intermediate representation between CM source code and HW instruction.

Copy the right version of jitter into the directory where cmrt.so locates.

igfxcmjit64.so
build env: Ubuntu 14.04 (64 bit, gcc version 4.8.2)

igfxcmjit32.so
build env: Ubuntu 14.04 (32 bit, gcc version 4.8.2)

Similar systems can re-use the above prebuilt binary.

If you want jitter for other systems, please contact us.
