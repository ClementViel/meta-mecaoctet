#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <signal.h>
#include <syslog.h>
#include "cJSON.h"


#define DEBUG 0
#define EXAMPLE_RX_BUFFER_BYTES (10)
#define MOTOR_MAX  2
#define SIZE_MAX_VALUE 10

/* PROTOTYPES & CALLBACKS */

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );



/*GLOBALS */

static struct lws *web_socket = NULL;
char answer[500];
char sendbuf[500];
int read_ok=0;
int send_ok = 1;

struct lws_context *context; 
int listenfd = 0, connfd = 0;


enum protocols
{
	PROTOCOL_EXAMPLE = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
{
	{
		"example-protocol",
		callback_example,
		0,
		EXAMPLE_RX_BUFFER_BYTES,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};



enum state_producer {
    INIT,
    WAIT_FOR_ARTNET,
    WAIT_FOR_MASTER,
    RUN,
    STOP
};

void process_message(void) {
    int j,i;
    char drive_request[SIZE_MAX_VALUE];
	char big_buffer[MOTOR_MAX * SIZE_MAX_VALUE];
    cJSON *feedback = NULL;
    cJSON *drive = NULL;
    cJSON *motor = NULL;
    cJSON *value = NULL;
    char aux_string[10];
    float final_values[MOTOR_MAX];
    cJSON *from_server=cJSON_Parse(answer);
    memset(drive_request, 0, 10);
    
    if (from_server == NULL) {
        syslog(LOG_NOTICE,"unparsed JSON\n");
    }
#if DEBUG
    syslog(LOG_NOTICE,"Receiving %s \n",answer);
#endif

    feedback = cJSON_GetObjectItemCaseSensitive(from_server, "feedback");
    if (feedback != NULL) {
		memset(big_buffer,0,(MOTOR_MAX * SIZE_MAX_VALUE));
        for (i=0; i< MOTOR_MAX; i++) {

            sprintf(drive_request,"drive-%d",i);
        	drive = cJSON_GetObjectItemCaseSensitive(feedback,drive_request);

			if (drive != NULL) {
            	value = cJSON_GetObjectItemCaseSensitive(drive,"encoder_value");
            
				if (value != NULL){
                	
					sprintf(aux_string,"%s",cJSON_Print(value));
                	aux_string[strlen(aux_string)-1]=0;
                
					for (j=1;j<=strlen(aux_string);j++)
                    	aux_string[j-1]=aux_string[j];
						strncat(big_buffer,aux_string,SIZE_MAX_VALUE);		
#if DEBUG
       syslog(LOG_NOTICE,"value %d = %f\n",i,atof(aux_string));
#endif     	
				}
        	}
	   }

       if (send_ok == 1) {
		   memcpy(sendbuf,big_buffer,(MOTOR_MAX * SIZE_MAX_VALUE));
           send_ok = 2;
       }

    }

    cJSON_Delete(from_server);
    memset(answer,0,500);

}

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
    size_t remaining;
    static int count=0; //count is dirty hack to discard the first packet containing "}"

	switch( reason )
	{
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
            /* send a request to server */
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			/* Handle incomming messages here. */
            count++;
            remaining = lws_remaining_packet_payload(wsi);
            if (!remaining && lws_is_final_fragment(wsi)) {
                /* received last packet, process json. */

                if (count!=0){
                    strncat(answer,in,strlen(in));
                    process_message();
                    count=0;
                }
            }
            if (count !=0) 
                strncat(answer,in,strlen(in));
            
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
            break;
		}

		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			break;

		default:
			break;
	}

	return 0;
}


void handle_signal (int signum) {
	
	unlink("producer");
	close(connfd);
	close(listenfd);
	lws_context_destroy( context );
	exit(0);

}

void socket_setup() {

    struct sockaddr_un serv_addr; 

    time_t ticks;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, "producer");
    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listenfd == -1)
        exit(-1);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_un));
    syslog(LOG_NOTICE,"listening\n");
    listen(listenfd, 10);
}



int main( int argc, char *argv[] )
{
	struct sigaction sa;
    enum state_producer state = INIT;
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );
    struct timeval tv;
	char server_addr[15];
	gettimeofday( &tv, NULL );

	if (argc == 1) {
		syslog(LOG_NOTICE,"usage : wsclient <websocket server IP address>\n");
		exit(-1);
	} else {
		memset(server_addr,0,15);
		memcpy(server_addr,argv[1],strlen(argv[1]));
	}

	sa.sa_handler = handle_signal;
	sigemptyset (&sa.sa_mask);

	sigaction(SIGINT,  &sa, 0);
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGABRT, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);


	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

    connfd = -1;
	time_t old = 0;
	while( 1 ) {

        switch (state) {
            case INIT:
                socket_setup();
                state = WAIT_FOR_ARTNET;
                break;
            case WAIT_FOR_ARTNET:
				syslog(LOG_NOTICE,"waiting for artnet process\n");
                while (connfd < 0) {
                    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
#if DEBUG
                    syslog(LOG_NOTICE,"conn %d\n",connfd);
#endif
                }
                syslog(LOG_NOTICE,"Artnet Connected\n");
                state = WAIT_FOR_MASTER;
                break;

            case WAIT_FOR_MASTER:
		        		        /* Connect if we are not connected to the server. */
	            context = lws_create_context( &info );
		        if( !web_socket && tv.tv_sec != old )
		        {
			        struct lws_client_connect_info ccinfo = {0};
			        ccinfo.context = context;
					syslog(LOG_NOTICE,"server address is %s\n",server_addr);
			        ccinfo.address = server_addr;
			        ccinfo.port = 8080;
			        ccinfo.path = "/info";
			        ccinfo.host = lws_canonical_hostname( context );
			        ccinfo.origin = "origin";
			        ccinfo.protocol = protocols[PROTOCOL_EXAMPLE].name;
			        syslog(LOG_NOTICE,"connecting...");
                    web_socket = lws_client_connect_via_info(&ccinfo);
                    syslog(LOG_NOTICE,"OK web_socket=%x\n",(void *)web_socket);
                    state = RUN;
		        }
                break;

	        case RUN:
                syslog(LOG_NOTICE,"Running...\n");
        	    lws_service( context, /* timeout_ms = */ 1000 );
                if (send_ok == 2) {
#if DEBUG
                    syslog(LOG_NOTICE,"sending %s to artnet\n",sendbuf);
#endif
                    write(connfd, sendbuf, strlen(sendbuf));
                    send_ok = 1;
                }
                break;


        }
    }
	return 0;
}
