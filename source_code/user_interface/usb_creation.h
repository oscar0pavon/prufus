#ifndef USB_CREATION_H
#define USB_CREATION_H

#include <stdbool.h>
#include <time.h>

extern bool usb_creation_running;
extern bool usb_creation_success;
extern bool usb_creation_failed;
extern char usb_creation_status_text[64];
extern time_t usb_creation_start_time;

void usb_creation_start(const char* iso_path, const char* device_path);

void usb_creation_cancel();

void usb_creation_poll();

#endif
