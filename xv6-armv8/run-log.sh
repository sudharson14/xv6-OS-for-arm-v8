echo run armv8 with qemu logging feature 'qemu.log'
export PATH=$PATH:/home/lakshman/Acads/695_osdi/qemu_git/qemu-arm/aarch64-softmmu

clear

qemu-system-aarch64 -machine virt -cpu cortex-a57 \
-machine type=virt -m 128 -nographic \
-singlestep \
-d exec,cpu,guest_errors,in_asm -D qemu.log -kernel kernel.elf 
