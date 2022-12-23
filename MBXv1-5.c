/*
 * MBXv1.c
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

 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <regex.h>
#include <errno.h>
#include <termios.h>
#include "sceneCMD.h"
#include "soundCMD.h"
#include "doCMD.h"
#include "statusCMD.h"
#include <vlc/vlc.h>

int processSceneCMD(char*,int);

union 	LightNum{ 
 		int 	LnumI;
		char	LnumC[2];
	};

 
int main(int argc, char **argv)
{
	
	FILE 	*showFH;
	char 	line[1024];
	int 	ser_port;

	int		lineCount;



	char	number;
	
	int		scanEOS;
	char*	scanPTR;
	char*	songNamePTR;
	char*	lightNumPTR;
	union 	LightNum LnumU;
	union 	LightNum rateU;
	char	tempCHR;
	
	unsigned char allOff[]		= {27, 180};
	unsigned char setStyle[]  	= {27, 204, 1};
	unsigned char sceneClear[]	= {27, 201, 0};
	unsigned char sceneOn[]		= {27, 183, 0};
	unsigned char sceneEffect[] = {27, 192, 0};
	unsigned char sceneLevel[]	= {27, 189, 0};
	unsigned char sceneRate[]	= {27, 195, 0, 0, 0};
	unsigned char addLight[]	= {27, 202};
	
	char 	fName[64] 			= "/home/AnimatedLighting/shows/";
	char 	mp3Name[64]			= "/home/AnimatedLighting/shows/";
	clock_t songStart_t, clk_t;
	
	long 	trigger;	
	//long long trigger2;
	
	
	
	
	int 	cIDX;
	char* 	command;
	int		cmdINT;
	char*	paramStrPTR;
	int		paramTypeINT;
	char*	paramValuePTR;
	
	struct termios options;
	if((ser_port = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr,"Unable to openUSB0 %s\n", strerror(errno));
		return 1;
	}
	  
	tcgetattr(ser_port, &options);
	options.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(ser_port,TCIFLUSH);
	
	tcsetattr(ser_port, TCSANOW, &options);

	
	//printf("*********************************@@@@@@@@@@@@@@@@@@@@@@@ser_port={%d}\n", ser_port);
	
	libvlc_instance_t 		*mp3Engine;
	libvlc_media_player_t	*mediaPlayer;
	libvlc_media_t			*media;
	

 
	mp3Engine = libvlc_new(0, NULL);
	if(write(ser_port, allOff ,2) <0){
			printf("UART error on level not sent\n");
		return 1;
	}	  	

	
	printf("%s\n", argv[1]);
	strcat(fName, argv[1]);
	printf("file name = %s\n", fName);
	lineCount = 0;

	
	
	showFH = fopen(fName, "r");
	if(!showFH){
		fprintf(stderr,"  File WTF %s\n", strerror(errno));
		return 1;
	}
	 
		while(1){
		fgets(line, 1024, showFH);
		
		if(feof(showFH)){
			printf("DONE\n");
			break;
		} 
		 
		if ((line[0] == '\'')||(line[0]=='G')||(strlen(line)<3)){
			//printf("SKIP\n");
		 } 
		 else {
			 printf("===================================\n");
			lineCount++;
			//printf("%d...%d, %s\n",lineCount,strlen(line), line);
			cIDX = 0;
/*Scan for start of parameters
 *  Replace the ( with 0x0 to terminate the line string at the end of 
 * the command string
 *  Set the paramStrPTR to &line + cIDX +1 for param processing
 *  Scan for ) and terminate the parameter string 
*/ 
			command = line;
			printf ("%d...%s\n", lineCount,line);
			
			while(line[cIDX] != '('){
				cIDX++;
			}//END while(line[cIDX] != '('
			
			line[cIDX] = 0;
			cIDX++; 
			paramStrPTR = line+cIDX;
			
			while(line[cIDX] != ')'){
				cIDX++;
			}
			
			line[cIDX] = 0;
			//printf ("%d   Command = [%s]\n", lineCount,command);
			//printf ("    Parameter = [%s]\n...\n", paramStrPTR);
// 	END input parsing Command  and Params are now two 
//	0x0 terminated strings


			cmdINT 	= 0;
			cIDX	= 0;
			while(command[cIDX]){
				cmdINT += command[cIDX];
				cIDX++;
			}
			//printf("%s == [%d]\n",command,cmdINT);
	    
	        switch(cmdINT){
//Set On Effect				
				case 1618 :	   
					paramTypeINT 	= 0;
					cIDX 			= 0;
					while(paramStrPTR[cIDX]){
						paramTypeINT += paramStrPTR[cIDX];
						cIDX++;
					}
					
					//printf("%s == [%d]\n",paramStrPTR,paramTypeINT);				
					switch(paramTypeINT){
						case 938  :   	//RAMP
							sceneEffect[2] =alEFFECTRAMP;
						break;
						
						case 1145 :		//POPFADE
							sceneEffect[2] = alEFFECTPOPFADE;
						break;
						
						case 1002 :		//BLINK
							sceneEffect[2] = alEFFECTBLINK;
						break;
						
						case 1164 :		//SPARKLE
							sceneEffect[2] = alEFFECTSPARKLE;
						break;
						
						case 906  :		//FADE
							sceneEffect[2] = alEFFECTFADE;
						break;
						
						case 1110 :		//REVPOP
							sceneEffect[2] = alEFFECTREVPOP;
						break;
						
						case 1146 :		//FLICKER
							sceneEffect[2] = alEFFECTFLICKER;
						break;
						
						case 1167 :		//SHIMMER
							sceneEffect[2] = alEFFECTSHIMMER;
						break;
												
						default:
						break; 
					}//END effect type swtich
						//printf("Sending  onEffect\n");
					if(write(ser_port, sceneEffect ,3) <0){
						fprintf(stderr,"UART error on effect not sent %s\n", strerror(errno));
						return 1;
					}	
					tcdrain(ser_port);					
						
				break;
//Set On Level				
				case 1533 : 
					number = (char)atoi(paramStrPTR);	 
					//printf("sending on level (%X)\n",number);  
					sceneLevel[2] = number;
					if(write(ser_port, sceneLevel ,3) <0){
						printf("UART error on level not sent\n");
						return 1;
					}	
					tcdrain(ser_port);		
				
				break;
// Set Rate				
				case 1236 : 
					cIDX = 0;
					while(paramStrPTR[cIDX] != ','){
						cIDX++;
					}	
					paramStrPTR[cIDX] = 0;
					cIDX++;
					paramValuePTR = paramStrPTR + cIDX;
					//printf("Parameter[%s] Value[%s]\n",paramStrPTR,paramValuePTR);
					paramTypeINT 	= 0;
					cIDX 			= 0;
					while(paramStrPTR[cIDX]){
						paramTypeINT += paramStrPTR[cIDX];
						cIDX++;
					}
					//printf("[%s] = [%d]\n\n",paramStrPTR,paramTypeINT);
					switch(paramTypeINT){
						case 809  :   	//RAMP
							sceneRate[2] =alEFFECTRAMP;
						break;
						
						case 1016 :		//POPFADE
							sceneRate[2] = alEFFECTPOPFADE;
						break;
						
						case 873 :		//BLINK
							sceneRate[2] = alEFFECTBLINK;
						break;
						
						case 1035 :		//SPARKLE
							sceneRate[2] = alEFFECTSPARKLE;
						break;
						
						case 777  :		//FADE
							sceneRate[2] = alEFFECTFADE;
						break;
						
						case 981 :		//REVPOP
							sceneRate[2] = alEFFECTREVPOP;
						break;
						
						case 1017 :		//FLICKER
							sceneRate[2] = alEFFECTFLICKER;
						break;
						
						case 1038 :		//SHIMMER
							sceneRate[2] = alEFFECTSHIMMER;
						break;
												
						default:
						break; 
					}//END effect type swtich
				
				
					rateU.LnumI = atoi(paramValuePTR);
					sceneRate[3] = rateU.LnumC[1];
					sceneRate[4] = rateU.LnumC[0];
					if(write(ser_port, sceneRate ,5) <0){
						printf("UART error on level not sent\n");
						return 1;
					}	
					tcdrain(ser_port);
				
				break;
// Add Light				
				case 1309 : // Add Light
					if(write(ser_port, addLight ,2) <0){
						printf("UART error on addlight not sent\n");
						return 1;
					}	
					tcdrain(ser_port);
					
					scanPTR = paramStrPTR; 
					//printf(" SCAN == [%c]\n", *scanPTR);
					scanEOS =0;
					do{
						while((*scanPTR != ',') && (*scanPTR)){
							scanPTR++;		
							//printf(" SCAN == [%c]...", *scanPTR);				
						}
						
						//trap end of paramString
						if(!*scanPTR){
							scanEOS =1;
						}
						
						//set ',' to 0x0 for atoi
					    *scanPTR = 0;
						scanPTR++;
						lightNumPTR =scanPTR;
						lightNumPTR -= 2;
						
						//scan back to beginning of the light number 
						while((*lightNumPTR >= '0') && (*lightNumPTR <= '9')){
							lightNumPTR--;
							//printf("LN scan [%c]\n",*lightNumPTR);
						}					
						lightNumPTR++;		//lightNumPTR now points to start of the number string
											// Convert to a big endian char[2]
						//printf("LightNumber = [%s]\n" , lightNumPTR);
												
						LnumU.LnumI = atoi(lightNumPTR);
						tempCHR = LnumU.LnumC[0];
						LnumU.LnumC[0] = LnumU.LnumC[1];
						LnumU.LnumC[1] = tempCHR;
				//Transmit the light numbers
						//printf("LIGHTNUMBER ==[%X..%X]\n",LnumU.LnumC[0],LnumU.LnumC[1]);						
						if(write(ser_port, &LnumU.LnumC ,2) <0){
							printf("UART error LIght Number not sent\n");
							return 1;
						}	   
						tcdrain(ser_port);
						
						  
						
				}
						
					while(!scanEOS);		
				break;
//Set Song				
				case 1274 : 
					//printf("SET SONG\n");
					songNamePTR = paramStrPTR;
					songNamePTR += 10;
					scanPTR = songNamePTR;
					
					while(*scanPTR != '"'){
						scanPTR++;
					}
					*scanPTR = 0;
			
					printf("Song name =[%s]\n",songNamePTR);
					strcat(mp3Name, songNamePTR);
					media 	= libvlc_media_new_path(mp3Engine, mp3Name);
					mediaPlayer = libvlc_media_player_new_from_media(media);
					
				break;
//Sound Play				
				case 973  : 
					printf("SOUND PLAY\n");
					libvlc_audio_set_volume( mediaPlayer, 125 );
					libvlc_media_player_play(mediaPlayer);				
					songStart_t = clock()/1000;
					printf("Start time = [%ld]\n", songStart_t);
				break;
// Set Time Interval				
				case 1565 : 
					paramStrPTR += 4;
					printf("SOUND SET TIME INT [%s]\n",paramStrPTR);
					//trigger = songStart_t+(atoi(paramStrPTR));
					trigger = (atoi(paramStrPTR));
					printf("Trigger = [%ld]\n", trigger);
				break;
// Do Wile wait for time interval  				
				case 748  : 
					printf("DO LOOP\n");
					
					clk_t = clock()/1000;
					printf("Start time %ld\n",clk_t);	 
				printf("TIME 1 ...[%lld}\n" , libvlc_media_player_get_time(mediaPlayer));	
					//while(clk_t < trigger){ 
						//clk_t = clock()/1000;
									
					//}
					while((long)libvlc_media_player_get_time(mediaPlayer)<trigger){
						if(libvlc_media_player_get_state(mediaPlayer) == libvlc_Ended){
							trigger = 0;
						}
					}
					
				printf("TIME 2 ..[%lld}\n" , libvlc_media_player_get_time(mediaPlayer));		
					printf("END time %ld\n",clk_t);
				break;
 			
				
				
				case 1027 : // Scene Clear
					printf("Scene.Clear\n");
					if(write(ser_port, sceneClear,3) <0){
						printf("UART error on Scene Clear not sent\n");
						return 1;
					} 
					tcdrain(ser_port);
					break;
							
				case 729 :
					printf("Scene.On\n");
					if(write(ser_port, sceneOn,3) <0){
						printf("UART error on Scene Clear not sent\n");
						return 1;
					} 
					tcdrain(ser_port);
					//return 0;
					break;
				
				case 1369 :
				printf("Scene.Style\n");
					if(write(ser_port, setStyle,3) <0){
						printf("UART error SET STYLE  not sent\n");   
					} 
					tcdrain(ser_port);				
					break;
					
					
			
				default:
					break;
			
			}//END switch
			
        }//END else
    }  

	libvlc_media_release(media);
	libvlc_media_player_release(mediaPlayer);
	libvlc_release(mp3Engine);
	close(ser_port);
	fclose(showFH);
	//fclose(logfile);
	


    
	return 0;
}

  



 
