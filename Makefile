notapp:
	gcc -g -pthread  notapp.c newserver.c obsclient.c useclient.c serialdeserial.c -o notapp
clean:
	rm *.o notapp