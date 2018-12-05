/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <artnet/artnet.h>
#include <netdb.h>
int verbose = 0;
int use_select = 0 ;

int delay = 25000;

#ifndef WIN32
#include <sys/select.h>
#else
#include <windows.h>
#endif

#define MAX_MOTOR 2
#define MAX_TOUR 4000
int sockfd = 0, n = 0;
char recvBuff[500];
struct sockaddr_un serv_addr;

void socket_setup(void) {
    memset(recvBuff, '0',sizeof(recvBuff));

    if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        printf("error connecting client socket\n");

    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path,"producer",8);

     printf("socket connecting\n");

    if( connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        perror("connect\n");
    printf("OK\n");
}


int main(int argc, char *argv[]) {
  artnet_node node ;
  char *ip_addr = NULL ;
  int optc,  subnet_addr = 0, port_addr = 0 ;
  uint8_t data[ARTNET_DMX_LENGTH] ;
  uint8_t aux_string[10];
  struct timeval tv;
  int sd, maxsd,i ;
  int percent=0;
  float aux_float;
  fd_set rset;
#ifndef WIN32
  struct timespec ts;
#endif

  // parse options
  while ((optc = getopt (argc, argv, "s:p:d:a:ve")) != EOF) {
    switch (optc) {
       case 'a':
        ip_addr = (char *) strdup(optarg) ;
            break;
      case 'v':
        verbose = 1 ;
            break;
      case 'e':
        use_select = 1 ;
            break;
      case 's':
        subnet_addr = atoi(optarg) ;

        if(subnet_addr < 0 && subnet_addr > 15) {
          printf("Subnet address must be between 0 and 15\n") ;
          exit(1) ;
        }
        break ;
      case 'd':
        delay = atoi(optarg) ;
        break ;
      case 'p':
        port_addr = atoi(optarg) ;

        if(port_addr < 0 && port_addr > 15) {
          printf("Port address must be between 0 and 15\n") ;
          exit(1) ;
        }
        break ;
          default:
        break;
      }
  }
    memset(data,0,ARTNET_DMX_LENGTH);
    node = artnet_new(ip_addr, verbose) ; ;
    artnet_set_short_name(node, "Artnet -> DMX ") ;
    artnet_set_long_name(node, "ArtNet Flood RX") ;
    artnet_set_node_type(node, ARTNET_NODE) ;

  // set the first port to input dmx data
  artnet_set_port_type(node, 0, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX) ;
  artnet_set_subnet_addr(node, subnet_addr) ;

  // set the universe address of the first port
  artnet_set_port_addr(node, 0, ARTNET_INPUT_PORT, port_addr) ;
  artnet_start(node) ;

  sd = artnet_get_sd(node) ;
  socket_setup();
  printf("socket connected\n");
  if(use_select) {
    while(1) {

  
      FD_ZERO(&rset) ;
      FD_SET(sd, &rset) ;

      tv.tv_usec = delay ;
      tv.tv_sec = 0 ;

      maxsd = sd ;

      switch ( select( maxsd+1, &rset, NULL, NULL, &tv ) ) {
        case 0:
                  artnet_send_dmx(node,0, ARTNET_DMX_LENGTH, data) ;
          artnet_send_dmx(node,0, ARTNET_DMX_LENGTH, data) ;
          artnet_send_dmx(node,0, ARTNET_DMX_LENGTH, data) ;
          artnet_send_dmx(node,0, ARTNET_DMX_LENGTH, data) ;

          break ;
        case -1:
           printf("select error\n") ;
          break ;
        default:
          artnet_read(node,0) ;
          break;
      }
    }
  } else {
    while(1) {

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {



	  for(i=0; i<(MAX_MOTOR); i++){
      	memcpy(aux_string,recvBuff+(i*10),10);
		aux_float = atof(aux_string);
	  	if (aux_float > 0) {
      		percent = (int)(aux_float * 100)/MAX_TOUR + 50;
	  	} else if (aux_float < 0) {
			  percent = (int)(aux_float * 100)/MAX_TOUR;
	  	} else {
				percent = 50;
	  	}
	

	  /* TODO */
	  /* add channel dispatch by motor number. */
      data[i*2] = (char)((percent & 0xFF00) >> 8);
      data[(i*2)+1] = percent & 0xFF;
	  }
#if DEBUG
      printf("received %f - sending %d\n",aux_float,percent);
#endif



      artnet_send_dmx(node,0, ARTNET_DMX_LENGTH, data) ;

      artnet_send_dmx(node,0, ARTNET_DMX_LENGTH, data) ;
      sleep(1);
    }
    }
  }
  return 0 ;
}
