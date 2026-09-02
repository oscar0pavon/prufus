#ifndef DEVICE_H
#define DEVICE_H

#define MAX_USB_DEVICES 16

typedef struct UsbDevice{
    char path[32];   /* e.g. "/dev/sda" */
    char label[96];  /* e.g. "SanDisk 3.2Gen1 114.6 GiB" */
} UsbDevice;

extern UsbDevice usb_devices[MAX_USB_DEVICES];
extern int usb_device_count;

void read_sys_block();

#endif
