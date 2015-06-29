#!/usr/bin/env python
#
# (C) 2015. Kevin W. Jones. All rights reserved.
#
# Interface to MQ-4 (Methane/Gas) Sensor via mcp3008 ADC
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
parser.add_argument("channel", type=int, default=0, help="ADC channel for this MQ4 (CH4) sensor")
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
 
# MQ7 = 83 @ .25 V
# MQ4 = 60 @ .16V

# Function to calculate temperature from TMP36 data, rounded to specified number of decimal places.
def ConvertTemp(data,places):
  # ADC Value
  # (approx)  Temp  Volts
  #    0      -50    0.00
  #   78      -25    0.25
  #  155        0    0.50
  #  233       25    0.75
  #  310       50    1.00
  #  465      100    1.50
  #  775      200    2.50
  # 1023      280    3.30

  temp = ((data * 330)/float(1023))-50
  temp = round(temp,places)
  return temp

# Function to calculate Methane / Natural Gas from MQ4 data, rounded to specified number of decimal places.
def ConvertCH4(data,places):
  if data < 38: data = float(38)
  else: data = float(data)
  data = data - float(38)
  methane = 10.009 * data
  methane = round(methane,places)
  return methane

# Define sensor channels
CHANNEL = 0


if __name__ == '__main__':
	# Read the MQ4 (CH4) sensor data
	MQ4_level = ReadChannel(CHANNEL)
	MQ4_volts = ConvertVolts(MQ4_level,2)
	methane_ppm = ConvertCH4(MQ4_level,2);
	# Print out results
	if DEBUG:
		print "--------------------------------------------"
		print("Methane: {}PPM ({}V)".format(methane_ppm,MQ4_volts))
	else:
		print methane_ppm
	spi.close() 
	sys.exit(0)