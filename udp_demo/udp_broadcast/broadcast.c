#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

int main(int argc, char const *argv[])
{
	printf("udp broadcast client\n");

	int iSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
	checkError(iSocketFd);

	int on = 1;
	int broadcastEnable = 1;//the permissions of broadcast
	struct timeval timeout={2,5};//timeout
	checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_BROADCAST, (char *)&broadcastEnable, sizeof(broadcastEnable)));
	checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)));
	checkError(setsockopt(iSocketFd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)));

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family 	= AF_INET; 
	server_addr.sin_port 	= 7878;
	server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	
	checkError(bind(iSocketFd, (struct sockaddr*)&server_addr, sizeof(server_addr)));

	struct sockaddr_in client_addr;//create a client addr
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	char aRecv[2048] = {0};
	const char *aSend = "This is udp brocast client";	
	
	while(1)
	{
		printf("send hello to server\n");
		checkError(sendto(iSocketFd, aSend, strlen(aSend), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)));

		while(1){
			printf("wait client data...\n");
			memset(aRecv, 0, sizeof(aRecv));
			int irecv = recvfrom(iSocketFd, aRecv, sizeof(aRecv), 0, (struct sockaddr*)&client_addr, &client_len);
			if(-1 == irecv){
				printf("recvfrom err:%s\n", strerror(errno));
				if(errno == EAGAIN){
					usleep(100);
					continue;
				} else {
					exit(1);
				}
			}
			printf("client ipaddr:%s, data:%s\n", inet_ntoa(client_addr.sin_addr), aRecv);
			if(!strcmp(aRecv, "This is udp server")){
				break;
			}
		}

		sleep(1);
	}
	
	return 0;
}
