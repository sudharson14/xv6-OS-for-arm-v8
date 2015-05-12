xv6-armv7
=========

This is xv6 OS porting on armv7 cpu. It support armv7 vexpress board with cortex-a15 cpu and GICv2 interrupt controller. This kernel has been tested on qemu-system-arm simulator.

This project is a fork from xv6-rpi project; xv6-rpi has ported xv6 kernel to armv6 world.


Procedure
---------

1. install arm binutils and qemu arm simulator

```sh
apt-get install -y gcc-arm-none-eabi 
apt-get install -y qemu-system-arm
```

2. build kernel from source
```sh
cd src
make
```

3. run from simulator
```
sh ./run.sh
```

Done
-------------------
* port to armv7v express a15 board
* support arm gic v2 on a15 board
* scripts to run kernel with logging or gdb tracing

Todo
----
* support file system
