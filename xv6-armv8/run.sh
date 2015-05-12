echo run armv8 without qemu.log
export PATH=$PATH:/home/lakshman/Acads/695_osdi/qemu_git/qemu-arm/aarch64-softmmu

clear

qemu-system-aarch64 -machine virt -cpu cortex-a57 \
-machine type=virt -m 128 -nographic \
-singlestep -kernel kernel.elf 
# skip: -singlestep
# try skip -cpu, as str r0, [fp,#-8] not write onto mem
# -cpu cortex-a15
# -s              shorthand for -gdb tcp::1234
# -S freeze at startup
