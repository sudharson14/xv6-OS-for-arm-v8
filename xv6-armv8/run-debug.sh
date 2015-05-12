echo run xv6 armv8 in debug mode
export PATH=$PATH:/home/lakshman/Acads/695_osdi/qemu_git/qemu-arm/aarch64-softmmu

clear

qemu-system-aarch64 -machine virt -cpu cortex-a57 \
-machine type=virt -m 128 -nographic \
-singlestep -kernel kernel.elf -s -S \
# -s shorthand for -gdb tcp::1234
# -S freeze at startup
