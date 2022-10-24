#pragma once
#include "game_utilities.h"
#include "errors.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFF_MAX 100
#define PORT 8080
#define FILE_NAME "server_name.txt"
#define SERVER_CHARACTER 1
#define CLIENT_CHARACTER 2

int create_server();
int close_socket(int conn_fd);
int communication(int conn_fd);
int init_name();