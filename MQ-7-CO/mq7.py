#!/usr/bin/env python
#
# (C) 2015. Kevin W. Jones. All rights reserved.
#
# Interface to MQ-7 (Carbon Monoxide/Gas) Sensor via mcp3008 ADC
# Program by Kevin W Jones with inspiration from multiple web resources
# 
# Written June 2015
# 
#
import argparse
import time
import os
import sys
import spidev    # import the spidev module

DEBUG = 0
CHANNEL = 0

parser = argparse.ArgumentParser(description='Process command line.')
parser.add_argument("channel", type=int, default=0, help="ADC channel for this MQ7 (CO) sensor")
parser.add_argument('--debug', action='store_const', const=1, default=0, help='Add debugging output to this program')

args = parser.parse_args()
CHANNEL = args.channel
DEBUG = args.debug

# Open SPI bus
spi = spidev.SpiDev()
spi.open(0,0)
 
# Function to read SPI data from MCP3008 chip 
# Channel must be an integer 0-7
def ReadChannel(channel):
  adc = spi.xfer2([1,(8+channel)<<4,0])
  data = ((adc[1]&3) << 8) + adc[2]
  return data
 
# Function to convert data to voltage level, rounded to specified number of decimal places.
def ConvertVolts(data,places):
  volts = (data * 3.3) / float(1023)
  volts = round(volts,places)
  return volts

# Function to calculate carbon monoxide PPM from MQ7 data, rounded to specified number of decimal places.
def ConvertCO(data,places):
  if data < 38: data = float(38)
  else: data = float(data)
  data = data - float(38)
  carbon_monoxide = 10.009 * data
  carbon_monoxide = round(carbon_monoxide,places)  
  return carbon_monoxide

# Define sensor channels
CHANNEL = 0

if __name__ == '__main__':
	# Read the MQ7 (CO) sensor data
	MQ7_level = ReadChannel(CHANNEL)
	MQ7_volts = ConvertVolts(MQ7_level,2)
	carbon_monixide_ppm = ConvertCO(MQ7_level,2);
	# Print out results
	if DEBUG:
		print "--------------------------------------------"
		print("Carbon Monoxide: {}PPM ({}V)".format(carbon_monixide_ppm,MQ7_volts))
	else:
		print carbon_monixide_ppm
	spi.close() 
	sys.exit(0)


