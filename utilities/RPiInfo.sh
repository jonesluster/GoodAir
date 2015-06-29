#!/bin/bash
#
# (C) 2015 by Kevin W. Jones. All rights reserved.
#
hardware=`cat /proc/cpuinfo | grep Hardware | cut -d: -f2 | tr -d '[[:space:]]'`
revision=`cat /proc/cpuinfo | grep Revision | cut -d: -f2 | tr -d '[[:space:]]'`
serialno=`cat /proc/cpuinfo | grep Serial | cut -d: -f2 | tr -d '[[:space:]]'`
ethernetMAC=`cat /sys/class/net/eth0/address`
#
echo "{\"hardware\":\"$hardware\",\"revision\":\"$revision\",\"serialno\":\"$serialno\",\"ethernetMAC\":\"$ethernetMAC\"}"
