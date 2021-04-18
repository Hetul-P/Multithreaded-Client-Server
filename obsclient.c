#include "obsclient.h"

void observer_client(char* saddr, int sport, char* fileordir){
    
    ///////*** variables for structs ***///////
    notify_struct send_struct;
	struct timeval *time_struct = malloc(sizeof(timeval));

    send_struct.ip = strdup(saddr); // FOR TESTING
    
    ///////*** variables for client ***///////
    
    int sock = 0;
	struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(sport);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, send_struct.ip, &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		return -1;
	}
    
    char* handshake = "obs";
    int handshake_len = strlen(handshake);

    send(sock, handshake, handshake_len, 0);

    ///////*** variables for inotify ***///////
	int fd, watch_desc;
    char buffer[OBS_BUFFER_SIZE];

	fd = inotify_init();
	if(fd < 0){
		perror("Notify not initialized");
		exit(EXIT_FAILURE);
	}
    // change with fileordir
	watch_desc = inotify_add_watch(fd, fileordir, IN_CREATE | IN_MODIFY | IN_DELETE);
	printf("watch_desc");
	if(watch_desc < 0){
		printf("Could not add watch to folder\n");
		exit(EXIT_FAILURE);
	}
	else{
		printf("Monitor added to path: %s\n", fileordir);
	}


    while(1){

            int reading = read(fd, buffer, OBS_BUFFER_SIZE);
            if(reading < 0 ){
                perror("read error");
            }

            int i= 0;

            while(i < reading){
                // event that can occur in a directory 

                struct inotify_event *event = (struct inotify_event *)&buffer[i];
                if(event->len ){

                    // if the event was a create
                    gettimeofday(time_struct, NULL);
                    send_struct.seconds = time_struct->tv_sec;
                    send_struct.milliseconds = time_struct->tv_usec;
                    send_struct.dir = strdup(event->name);
                    send_struct.mask = event->mask;
        

                    // SERIALIZE AND SEND
                    
                    int bufflen;
                    uint8_t* buffer = serialise(&send_struct, &bufflen);

                    send(sock, buffer, bufflen, 0);
                    
                    i+=MONITOR_EVENT_SIZE + event->len;
                }
            }
        }
        
    inotify_rm_watch(fd, watch_desc);
    close(fd);
    return(EXIT_SUCCESS);

}