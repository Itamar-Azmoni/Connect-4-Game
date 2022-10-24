#pragma once
#include "game_utilities.h"
#include "errors.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFF_MAX 100
#define PORT 8080
#define FILE_NAME "client_name.txt"
#define SERVER_CHARACTER 1
#define CLIENT_CHARACTER 2
#define SERVER_IP "127.0.0.1"

int connect_to_server();
int close_client(int sock_fd);
int communication(int sock_fd);
int init_name();