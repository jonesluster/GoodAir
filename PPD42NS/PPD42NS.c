#include <wiringPi.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/*
 Interface to Shinyei Model PPD42NS Particle Sensor
 Program by Kevin W Jones with inspiration from multiple web resources regarding using wiringPi and PWM-based sensor reading
 
 Written June 2015
 
 To compile, cJSON.c must be in root of dustsensor directory, and wiringPi must be installed.  Install mercurial (apt-get) in order to load jsmn (JSON for C) for the dustsensor.c code. hg clone http://bitbucket.org/zserge/jsmn jsmn 
 
 gcc -Wall PPD42NS.c -o PPD42NS -l wiringPi -lm

 PPD42NS Pin 1 (Black Wire)  => Raspberry GND
 PPD42NS Pin 3 (Red wire)    => Raspberry 5VDC
 PPD42NS Pin 4 (Yellow wire) => Raspberry GPIO
 */

//int pin = 7; // default GPIO 4 on RPi B+ models. Run "gpio readall" from command line to get pin/BCM/GPIO mapping numbering
//int timeout = 50000; // zero means no timeout
int debug = FALSE;
//unsigned long sampletime_ms = 30000;

void setup(int pin);
//float reading(int timeout);
float reading(int pin, int timeout, unsigned long sampletime_ms);

long pulseIn(int pin, int level, int timeout);
double pow(double x, double y);

 int main(int argc,char *argv[]) {
	char *ptr;
	if (argc < 4) {
		printf("Usage: %s pin-number sample-time (in milliseconds) time-out (numeric  - 0 = unlimited) -d (debug)\n",argv[0]);
		return 1;
	}
	int pin = atoi(argv[1]); /* convert string to integer */
	unsigned long sampletime_ms = strtoul(argv[2], &ptr, 10); /* convert string to unsigned long */
	int timeout = atoi(argv[3]); /* convert string to integer */
	if (argc == 5) {
		if (strcmp(argv[4],"-d") == 0) {
			debug = TRUE; // accept anything for debugging
			printf("Debug on... pin:%d, sample-time: %lu (ms), time-out: %d (ms) -d (debug)\n",pin,sampletime_ms,timeout);
		}
		else {
			printf("Usage: %s sample-time (in milliseconds) time-out (numeric  - 0 = unlimited) -d (debug)\n",argv[0]);
			return 1;
		}
	}
	float concentration;
	setup(pin);
	concentration = reading(pin, timeout, sampletime_ms);
	// anything over 10K particles per .01 cubic feet. Not actually particles per million, but particles per area.
	if (concentration > 10000) fprintf(stdout,"0");
	else fprintf(stdout,"%f",concentration);
	return 0;
}

void setup(int pin) {
	wiringPiSetup();
	pinMode(pin,INPUT);
	return;
}

float reading(int pin, int timeout, unsigned long sampletime_ms) {
	/*
	int previous_reading = 0;
	int reading = 0;
	int start_time = millis();
	*/
	unsigned long LPOTime = 0;
	float ratio = 0;
	float concentration = 0;

	long duration;
	unsigned long starttime;
	starttime = millis();
	if (debug) fprintf(stderr,"starttime:%lu\n",starttime);

	while(1) {
		duration = pulseIn(pin, LOW, timeout);
		LPOTime = LPOTime + duration;

		if ((millis() - starttime) > sampletime_ms) {
			ratio = LPOTime /(sampletime_ms * 10.0);  // Integer percentage 0=>100
			concentration = 1.1 * pow(ratio,3) - 3.8 * pow(ratio,2) + 520 * ratio + 0.62; // using spec sheet curve

			if (debug) fprintf(stderr,"Lo Pulse Occupancy:%ld\n",LPOTime);
			//fprintf(stdout,",");
			if (debug) fprintf(stderr,"Ratio:%f\n",ratio);
			//fprintf(stdout,",");
			if (debug) fprintf(stderr,"Concentration:%f\n",concentration);
			LPOTime = 0;
			//gettimeofday(&tv,NULL);
			//time_in_millis = 1000 * tv.tv_sec + tv.tv_usec;
			return concentration;
		}
	}
}

long pulseIn(int pin, int level, int timeout)
{
	struct timeval tn, t0, t1;
	long micros;
	gettimeofday(&t0, NULL);
	micros = 0;
	// Wait until the GPIO Pin goes to the requested level (state)
	while (digitalRead(pin) != level)  {
		gettimeofday(&tn, NULL);
		if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
		micros += (tn.tv_usec - t0.tv_usec);
		if ((timeout != 0) && (micros > timeout)) return 0;
	}
	// Once the requested level (state) is reached, start to count the time it takes to change to another level (state)
	gettimeofday(&t1, NULL);
	while (digitalRead(pin) == level) {
		gettimeofday(&tn, NULL);
		if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
		micros = micros + (tn.tv_usec - t0.tv_usec);
		if ((timeout != 0) && (micros > timeout)) return 0;
		//fprintf(stderr,"pin == LOW - micros:%ld\n",micros);
	}
	if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
	micros = micros + (tn.tv_usec - t1.tv_usec);
	//micros = (tn.tv_usec - t1.tv_usec);
	if (debug) fprintf(stderr,"pulseIn:micros:%ld\n",micros);
	return micros;
}