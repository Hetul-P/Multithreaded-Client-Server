#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "newserver.h"
#include "obsclient.h"
#include "useclient.h"

int main(int argc, char const *argv[]) {

    //char* inputs;
    int sflag = 0; // flag to see if server
    int oflag = 0; // flag to see if observer
    int uflag = 0; // flag to see if user

    int c;
    //int index;

    char* saddr; // for address
    int sport = 0; // for port
    char* fileordir; // for observer file or directory
    char* logfile = ""; // for server logfile
    float interval; //for server interval

    opterr = 0;

    while((c = getopt(argc, argv, "sout:p:l:")) != -1)
        switch (c){
            case 's':
                sflag = 1;
                break;
            case 'o':
                oflag = 1;
                break;
            case 'u':
                uflag = 1;
                break;
            case 't':
                interval = atof(optarg);
                break;
            case 'p':
                sport = atoi(optarg);
                break;
            case 'l':
                logfile = optarg;
                break;
        }

    
    if(oflag == 1 || uflag == 1){
        saddr = argv[optind++];
        sport = atoi(argv[optind++]);
    }
    if(oflag == 1){
        fileordir = argv[optind++];
        printf(" saddr = %s\n, sport = %d\n, fileordir =  %s\n", saddr, sport, fileordir);
    }
    
    
    if(sflag == 1){
        // run server
        server(sport, interval, logfile);
    }
    else if(uflag == 1){
        // run user
        user_client(saddr, sport);
    }
    else if(oflag == 1){
        // run observer
        observer_client(saddr, sport, fileordir);
    }

    return 0;
}