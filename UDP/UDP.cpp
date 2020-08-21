#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <array>

int main()
{
    std::string local_ip = "192.168.6.100";
    std::uint16_t local_port = 8001;
    std::string listen_ip = "192.168.6.104";
    std::uint16_t listen_port = 4001;
    std::string send_ip = "192.168.6.104";
    std::uint16_t send_port = 4002;
    std::cout << "Local IP " << local_ip << " Port " << local_port << std::endl;
    std::cout << "Listen IP " << listen_ip << " Port " << listen_port << std::endl;
    std::cout << "Send IP " << send_ip << " Port " << send_port << std::endl;

    sockaddr_in local_address;
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(local_port);
    local_address.sin_addr.s_addr = inet_addr(local_ip.c_str());
    socklen_t local_addr_length = sizeof(local_address);

    sockaddr_in listen_address;
    listen_address.sin_family = AF_INET;
    listen_address.sin_port = htons(listen_port);
    listen_address.sin_addr.s_addr = inet_addr(listen_ip.c_str());
    socklen_t listen_addr_length = sizeof(listen_address);

    sockaddr_in send_address;
    send_address.sin_family = AF_INET;
    send_address.sin_port = htons(send_port);
    send_address.sin_addr.s_addr = inet_addr(send_ip.c_str());
    socklen_t send_addr_length = sizeof(send_address);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_fd < 0)
    {
	perror("socket");
    }

    int flags = fcntl(socket_fd, F_GETFL, 0);
    if(flags < 0)
    {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    if(fcntl(socket_fd, F_SETFL, flags) < 0)
    {
        perror("fcntl");
        return -1;
    }

    struct sockaddr* addr = nullptr;
    socklen_t size = 0;
    addr = (struct sockaddr*)&local_address;
    size = sizeof(local_address);
    if (bind(socket_fd, addr, size) < 0 ) 
    {
        std::cout << "Failed to bind socket." << std::endl;
	perror("bind");
	return -1;
    }
    else
    {
	std::cout << "Succeeded to bind socket." << std::endl;
    }

    uint8_t rec_buffer[13];
    int rec_length;
    int send_length;

    while(1)
    {
	rec_length = recvfrom(socket_fd, rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *) &listen_address, &listen_addr_length);
 
        if(rec_length < 0)
        {
	    //perror("recvfrom");
	    //return -1;
	    continue;
        }
        printf("rec_length = %d\n", rec_length);
        for(int i = 0;i < rec_length;i++)
        {
	    printf("rec buffer[%d] %x\n",i,rec_buffer[i]);
        }

	send_length = sendto(socket_fd, rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *) &send_address, send_addr_length);
	printf("send_length = %d \n", send_length);	

	printf("\n");
	sleep(0.02);
    }

    return 0;
}
