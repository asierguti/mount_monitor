# mount_monitors

This is just a simple mount monitor. It consists of 2 parts, a Linux Kernel module that hooks up a custom code to monitor mount calls. The second parts is just a simple C++ program that opens a NETLINK socket to the kernel, and then paciently waits for the name of the devices mounted. Once a device name is received from the kernel, it is dumped to a log file.