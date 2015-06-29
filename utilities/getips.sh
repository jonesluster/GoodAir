#!/bin/bash
#
# (C) 2015 by Kevin W. Jones. All rights reserved.
#
LAN=`/sbin/ifconfig eth0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
WLAN=`/sbin/ifconfig wlan0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
WWAN=`/sbin/ifconfig ppp0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`

if [ "$LAN" == "" ]
then
	LAN="0.0.0.0"
fi 
if [ "$WLAN" == "" ]
then
	WLAN="0.0.0.0"
fi 
if [ "$WWAN" == "" ]
then
	WWAN="0.0.0.0"
fi 
#echo "{'LAN':'$LAN','WLAN':'$WLAN','WWAN':'$WWAN'}"
#echo "{LAN:'$LAN',WLAN:'$WLAN',WWAN:'$WWAN'}"
echo "{\"LAN\":\"$LAN\",\"WLAN\":\"$WLAN\",\"WWAN\":\"$WWAN\"}"
