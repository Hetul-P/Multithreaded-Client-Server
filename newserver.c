#include "newserver.h"

int num_of_obs = 0;
int num_of_use = 0;
char* write_to_logfile;

int log_flag = 0; // 1 means no file given 0 means file given

notify_struct all_data[1000];

pthread_mutex_t dataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t countMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condSleep = PTHREAD_COND_INITIALIZER;

/*
This function is for our observer threads
*/
void* handleObserver(void* arguments){

    int client_socket = *(int*) arguments;
    //writeToLogFile("Entering New Observer Thread \n");
    sleep(1);
    pthread_mutex_lock(&countMutex);
    int index = num_of_obs++;
    pthread_mutex_unlock(&countMutex);

    /*
    obs_arguments *parameters = (obs_arguments*) arguments;
    int client_socket = parameters->client_socket;
    int index = parameters->index;
    */
    while(1){

        // deserialize the incoming bytes into our struct
        notify_struct* deserialized_struct = malloc(sizeof(notify_struct));

		uint8_t buff[BUFFER_SIZE];
		ssize_t bufflen = read(client_socket, buff, BUFFER_SIZE);
		deserialized_struct = deserialise(buff, (int) bufflen);

        //printff("number of observer threads: %d \n", num_of_obs);
        // entering critical section
        sleep(1);
        pthread_mutex_lock(&dataMutex);
        all_data[index] = *deserialized_struct;
        pthread_mutex_unlock(&dataMutex);
    }

}

/*
This function is for our user threads
*/
void* handleUser(void* arguments){
    //writeToLogFile("Entering New User Thread \n");
    use_arguments *parameters = (use_arguments*) arguments;
    int client_socket = parameters->client_socket;
    float interval = parameters->interval;


    while(1){

        pthread_mutex_lock(&countMutex);
        int countObs = num_of_obs;
        pthread_mutex_unlock(&countMutex);

        pthread_mutex_lock(&dataMutex);
        pthread_cond_wait(&condSleep, &dataMutex);

        
        for(int i = 0; i < countObs; i++){
            
            notify_struct send_struct = all_data[i];
    
            int* bufflen = malloc(sizeof(int));
            uint8_t* buffer = serialise(&send_struct, bufflen);
            send(client_socket, buffer, *bufflen, 0);

        }

        pthread_mutex_unlock(&dataMutex);

        sleep(1);
    }
}
/*
This function is for our 1 heart beat thread
*/
void* heartBeat(void* interval){
    //writeToLogFile("Entering heartbeat \n");
    float refresh = *(float*)interval;

    while(1){
        //writeToLogFile("Interval Wait Started \n");
        sleep(refresh);
        //writeToLogFile("Done waiting. Now broadcast\n");
        pthread_cond_broadcast(&condSleep);
    }
    
}
/*
This function will printf to a logfile if given one
*/
void writeToLogFile(char* writeline){

    if(log_flag == 1){
        return;
    }
    else{
        // write
        
        FILE *fptr;
        
        fptr = fopen(write_to_logfile,"w");
        fprintf(fptr, writeline);
        fclose(fptr);
        
    }
    
    return;
}
/*
This function creates a server, it takes in the port that it listens on and the interval
that the user function needs to wait before broadcasting
*/
void server(int sport, float interval, char* logfile){
    int port_flag = 0;
    // to check if user input the port
    if(sport == 0){
        port_flag = 1;
    }
    
    // to check if user input log file
    if(strcmp(logfile, "") == 0){
        log_flag = 1;
    }
    else{
        write_to_logfile = logfile;
    }
    
    /////////*** Variables for Server ***/////////
    int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(sport);

    // bind the server to our socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    if(port_flag == 1){
        
        socklen_t len = sizeof(address);
        if (getsockname(server_fd, (struct sockaddr *)&address, &len) == -1)
            perror("getsockname");
        else
            printf("port number %d\n", ntohs(address.sin_port));
    }
    
    // listen on the socket
    if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    writeToLogFile("Server is listening \n");

    while(1){
        // handle a new socket connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // initial handshake process
        read(new_socket, buffer, BUFFER_SIZE);
        
        pthread_t obs, use, heart;
        if(strcmp(buffer, "obs") == 0){
            // go to the observer
            writeToLogFile("New observer detected\n");
            //num_of_obs++;
            
            int *obs_client = malloc(sizeof(int));
            *obs_client = new_socket;
            
            /*
            obs_arguments* param = malloc(sizeof(obs_arguments));
            param->client_socket = new_socket;
            param->index = num_of_obs - 1;
            */
            pthread_create(&obs, NULL, handleObserver, (void*) obs_client);
            //handleObserver(new_socket, num_of_obs);
        }
        else if(strcmp(buffer, "use") == 0){
            // go to user
            writeToLogFile("New user detected \n");
            num_of_use++;
            if(num_of_use == 1){
                writeToLogFile("Creating heartbeat \n");

                float *refresh = malloc(sizeof(float));
                *refresh = interval;
                pthread_create(&heart, NULL, heartBeat, (void*) refresh);
            }
            
            use_arguments* param = malloc(sizeof(use_arguments));
            param->client_socket = new_socket;
            param->interval = interval;

            pthread_create(&use, NULL, handleUser, (void*) param);

        }
    }
}