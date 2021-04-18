#include "useclient.h"

void user_client(char* saddr, int sport){
    
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
	if(inet_pton(AF_INET, saddr, &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		return -1;
	}
    
    char* handshake = "use";
    int handshake_len = strlen(handshake);

    send(sock, handshake, handshake_len, 0);

	while(1){

		// deserialize the incoming bytes into our struct

        notify_struct* print_struct = malloc(sizeof(notify_struct));

		uint8_t buff[1024];
		ssize_t bufflen = read(sock, buff, 1024);
		sleep(1);
		print_struct = deserialise(buff, (int) bufflen);

		char* printMask;
		switch (print_struct->mask){
                case 256:
                    printMask = "IN_CREATE";
                    break;
                case 2:
                    printMask = "IN_MODIFY";
                    break;
                case 512:
                    printMask = "IN_DELETE";
                    break;
            }
            printf("'%-17s' '%-14s' '%-10s' %-25s \n", "TIME", "HOST", "MONITORED", "EVENT");
            printf(" %-10ld.%-6ld   %-14s   %-10s   %-25s \n", print_struct->seconds, print_struct->milliseconds, print_struct->ip, print_struct->dir, printMask);
	}
}