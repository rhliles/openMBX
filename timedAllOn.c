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
	unsigned char onLevel[]		= {27,187,255};
	unsigned char onEffect[] 	= {27,190,0};
	unsigned char onRate[]   	= {27,193,0,0,0};
	unsigned char lightOn[]  	= {27,181,0,0};
	unsigned char allOff[]		= {27,180};
	unsigned char face1[]		= {27,181,0x01,0x60,0x01,0x61,0x01,0x62,0x01,0x63,0x01,0x65}; 
	unsigned char face2[]		= {27,181,0x01,0x68,0x01,0x69,0x01,0x6A,0x01,0x6B,0x01,0x6D}; 
	unsigned char face3[]		= {27,181,0x01,0x70,0x01,0x71,0x01,0x72,0x01,0x73,0x01,0x75}; 
	unsigned char face4[]		= {27,181,0x01,0x78,0x01,0x79,0x01,0x7A,0x01,0x7B,0x01,0x7D}; 
	     
	
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
	
	
	

		/*if(write(ser_port, setRange, 7) <0){
			return 1;
			}
		usleep(1000);
		*/
		if(write(ser_port, onLevel,3) <0){
			printf("UART error on level not sent\n");
			return 1;
		}
		
		usleep(1000);
		if(write(ser_port, onEffect,3) <0){
			printf("UART error on effect  not sent\n");
			return 2;
		}
		usleep(1000); 
		if(write(ser_port, onRate,5) <0){
			printf("UART error effect rate not sent\n");
			return 3;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
		}
		usleep(1000);
		for(i=0;i<16;i++){
			lightOn[3] = i;
			if(write(ser_port, lightOn,4) <0){
				printf("UART error Light On not sent\n");
				return 4;
			}
			
		}
		if(write(ser_port, face1,12) <0){
				printf("UART error Light On not sent\n");
				return 4;
		}
		
		if(write(ser_port, face2,12) <0){
				printf("UART error Light On not sent\n");
				return 4;
		}
		if(write(ser_port, face3,12) <0){
				printf("UART error Light On not sent\n");
				return 4;
		}
		if(write(ser_port, face4,12) <0){
				printf("UART error Light On not sent\n");
				return 4;
		}


		
		
		for(i=0; i<seconds; i++){
			usleep(1000000);
			printf("%d\n",i);
	    }
	
	if(write(ser_port, allOff,2) <0){
				printf("UART error All off not sent\n");
				return 4;
			}
	

	return 0;
}//END main

