#!/bin/bash
lsblk -P -d -n -o HOTPLUG,PATH,MODEL,SIZE | sed '/HOTPLUG="0"/d; s/\w*=//g; s/" /,/g; s/"//g; s/1,/,/g; s/^,\(.*\)/\1/' #add a comma and the last sed remove the first comma
