#include "server.h"

char name[BUFF_MAX];

int create_server()
{
    // Create the server
    // Return the file descriptor of the socket with the client, or an error number
    int server_fd, conn_fd, len, opt = 1;

    struct sockaddr_in server_addr;

    // Create the server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        return SOCKET_CREATION_FAILED;
    }

    // Used to prevent errors
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
		return SETSOCKOPT_FAILED;
	}

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET; // Ipv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY to bind to all interfaces
    server_addr.sin_port = htons(PORT); // Setting port number

    // Binding
    if((bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) != 0)
    {
        return SOCKET_BINDING_FAILED;
    }

    // Listening
    if((listen(server_fd, 3)) == -1)
    {
        return LISTEN_FAILED;
    }else
        printf("Waiting for clients...\n");


    // Accepting the client
    len = sizeof(server_addr);
    conn_fd = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)&len);
    if(conn_fd < 0)
    {
        return SERVER_ACCEPT_FAILED;
    }else
        printf("Server accept the client.\n");
    
    // Close the server socket, we are not going to accept other clients
    shutdown(server_fd, SHUT_RDWR);

    return conn_fd;
}

int close_socket(int conn_fd)
{
    // Closing the socket
    if(close(conn_fd) == 0)
    {
        printf("Socket closed successfuly.\n");
        return STATUS_OK;
    }else
        return SOCKET_CLOSING_FAILED;
}

int communication(int conn_fd)
{
    // Server side game handling
    char buff[BUFF_MAX] = {0};
    int game_is_running = 1, row, n = 0;
    char temp;
    
    // Sending our name to the client
    if(send(conn_fd, name, strlen(name), 0) == -1)
        return SEND_FAILED;

    // Getting client's name
    if(read(conn_fd, buff, sizeof(buff)) == -1)
        return READ_FAILED;

    printf("You are playing against %s!\n", buff);
    printf("Your color is \033[0;31mO\033[0m, %s's color is \033[0;33mO\033[0m\n", buff);

    init_grid(NULL); // Initialize grid with zeros
    printf("\n");
    print_grid(); // Printing the grid
    while(game_is_running)
    {   
        // Server's turn
        bzero(buff, sizeof(buff));
        while(1)
        {
            bzero(buff, sizeof(buff));
            n = 0;
            printf("Choose a column (1-7): ");
            while(n < BUFF_MAX)
            {
                temp = getchar();
                if(temp == '\n')
                    break;
                buff[n] = temp;
                n++;
            }
            if(n == BUFF_MAX)
                buff[n] = '\n';

            if(buff[0] >= 49 && buff[0] <= 55)
            {
                row = place(buff[0]-49, SERVER_CHARACTER);
                if(row != -1)
                {
                    if(send(conn_fd, buff, sizeof(buff), 0) == -1)
                        return SEND_FAILED;
                    break;
                }
            }
        }
        printf("\n");
        print_grid(); // Printing the grid

        // Check if we won
        if(check_win(row, buff[0]-49) == SERVER_CHARACTER)
        {
            printf("You have WON!\n");
            game_is_running = 0;
        }
        
        // If the grid is full, stop the game
        if(check_grid_full())
        {
            printf("The grid is full! Game over!\n");
            return STATUS_OK;
        }
        if(game_is_running)
            printf("Waiting for the opponent's response...\n");
        else
            break;

        // Client's turn
        bzero(buff, sizeof(buff));
        if(read(conn_fd, buff, sizeof(buff)) == -1)
            return READ_FAILED;
        row = place(buff[0]-49, CLIENT_CHARACTER);
        print_grid();

        // Check if we lost
        if(check_win(row, buff[0]-49) == CLIENT_CHARACTER)
        {
            printf("You LOST!\n");
            game_is_running = 0;
        }

        // If the grid is full, stop the game
        if(check_grid_full())
        {
            printf("The grid is full! Game over!\n");
            return STATUS_OK;
        }
        printf("\n");
    }

    return STATUS_OK;
}

int init_name()
{
    int n = 0;
    char temp;
    FILE* fp = fopen(FILE_NAME, "r");

    // If its the first time running the game, user should insert his name
    if(fp == NULL)
    {
        printf("Please enter you name: ");
        while(n < BUFF_MAX)
        {
            temp = getchar();
            if(temp == '\n')
                break;
            name[n] = temp;
            n++;
        }

        if(n == BUFF_MAX)
            name[n] = '\n';

        fp = fopen(FILE_NAME, "w");
        // If can't open the file for writing, return error
        if(fp == NULL)
        {
            printf("Could not open %s", FILE_NAME);
            return CANNOT_OPEN_FILE;
        }

        fwrite(name, sizeof(char), strlen(name), fp);
        fclose(fp);
        // After creating the file with the user's name, call init_name() to read the name
        return init_name();
    }
    fread(name, sizeof(char), BUFF_MAX, fp);

    fclose(fp);

    return STATUS_OK;
}

int main(int argc, char **argv)
{
    // Will contain the error type (if exist)
    int err = STATUS_OK;

    printf("Welcome to CONNECT 4!\n");

    // Get the name
    err = init_name();
    if(err != STATUS_OK)
    {
        printf("Error: %d\n", err);
        return err;
    }
        
    printf("Hello %s\n", name);
    
    // Create the server
    int conn_fd = create_server();
    if(conn_fd < STATUS_OK)
    {
        printf("Error: %d\n", conn_fd);
        return conn_fd;
    }

    // Game handling
    err = communication(conn_fd);
    if(err != STATUS_OK)
    {
        printf("Error: %d\n", err);
        return err;
    }
    
    // Close the socket
    err = close_socket(conn_fd);
    if(err != STATUS_OK)
    {
        printf("Error: %d\n", err);
        return err;
    }
        
    return 0;
}