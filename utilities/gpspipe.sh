#!/bin/bash
#chmod 666 /dev/ttyUSB0
#sleep 5
gpsdata=$( gpspipe -w | grep -m 1 TPV )
echo $gpsdata
