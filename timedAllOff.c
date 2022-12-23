/*
 * SerialTest.c
 * 
 * Copyright 2022  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <wiringPi.h>
#include <wiringSerial.h>


int main(int argc, char **argv)
{
	int ser_port;
	int i;
	unsigned int seconds = atoi(argv[1]);
	unsigned char allOff[]		= {27,180};
	     
	
	struct termios options;
	printf("Delay of %d\n" , seconds);
	
	if((ser_port = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr,"Unable to open %s\n", strerror(errno));
		return 1;
	}
	  
	tcgetattr(ser_port, &options);
	options.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(ser_port,TCIFLUSH);
	
	tcsetattr(ser_port, TCSANOW, &options);
	
	
	
	if(write(ser_port, allOff,2) <0){
				printf("UART error All off not sent\n");
				return 4;
			}



		
		
		for(i=0; i<seconds; i++){
			usleep(1000000);
			printf("%d\n",i);
	    }
	
	

	return 0;
	
}//END main

