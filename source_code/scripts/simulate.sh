#!/bin/sh
echo "Creating usb...."

mkdir -p /tmp/prufus
#if already exist clean first
rm -rf /tmp/prufus/status

echo 1 > /tmp/prufus/status

sleep 1

echo $1
echo $2

echo "Creating temporal directories.."
echo 2 > /tmp/prufus/status

mkdir -p /tmp/prufus/iso
mkdir -p /tmp/prufus/usb

sleep 1

echo "Mounting USB and ISO"
echo 3 > /tmp/prufus/status

sleep 1

echo "Copying regular installtion files.."
echo 4 > /tmp/prufus/status

sleep 2


echo "Copying big installation file.."
echo 5 > /tmp/prufus/status


sleep 5

echo "Syncronizing disk..."
echo 6 > /tmp/prufus/status

sleep 10

echo "Synconization done"


echo "Cleaning..."
echo 7 > /tmp/prufus/status

sleep 1

echo "Booteable USB created!"
echo 8 > /tmp/prufus/status

#rm -r /tmp/prufus

#watch -d grep -e Dirty: -e Writeback: /proc/meminfo
