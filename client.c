#include "client.h"

char name[BUFF_MAX];

int connect_to_server()
{
    // Create the client socket
    // Return the file descriptor of the socket, or an error number
    int sock_fd, client_fd;
    struct sockaddr_in server_addr;

    // Create the client socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        return SOCKET_CREATION_FAILED;
    }
    
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Server address is now localhost(127.0.0.1)
    server_addr.sin_port = htons(PORT); // Setting port number

    // Connecting to server
    if((client_fd = connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) != 0)
    {
        return CONNECT_FAILED;
    }else
        printf("Connected to the server.\n");
    
    return sock_fd;
}
 
int close_client(int sock_fd)
{
    // Closing the socket
    if(close(sock_fd) == 0)
    {
        printf("Socket closed successfuly.\n");
        return STATUS_OK;
    }else
        return SOCKET_CLOSING_FAILED;
}

int communication(int sock_fd)
{
    // Client side game handling
    char buff[BUFF_MAX] = {0};
    int game_is_running = 1, row, n = 0;
    char temp;

    // Getting server's name
    if(read(sock_fd, buff, sizeof(buff)) == -1)
        return READ_FAILED;

    printf("You are playing against %s!\n", buff);
    printf("Your color is \033[0;33mO\033[0m, %s's color is \033[0;31mO\033[0m\n", buff);
    printf("Waiting for the opponent's response...\n");

    // Sending our name to the server
    if(send(sock_fd, name, strlen(name), 0) == -1)
        return SEND_FAILED;

    // Initialize grid with zeros
    init_grid(NULL);
    
    while(game_is_running)
    {
        // Server's turn
        printf("\n");
        if(read(sock_fd, buff, sizeof(buff)) == -1)
            return READ_FAILED;

        row = place(buff[0]-49, SERVER_CHARACTER);

        // Printing the grid
        print_grid();

        // Check if we lost
        if(check_win(row, buff[0]-49) == SERVER_CHARACTER)
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
        if(!game_is_running)
            break;

        // Client's turn
        printf("\n");
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
                row = place(buff[0]-49, CLIENT_CHARACTER);
                if(row != -1)
                {
                    if(send(sock_fd, buff, sizeof(buff), 0) == -1)
                        return SEND_FAILED;
                    break;
                }
            }
        }
        printf("\n");
        print_grid();

        // Check if we won
        if(check_win(row, buff[0]-49) == CLIENT_CHARACTER)
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
    }
    
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

        // If can't open the file for writing, return error
        fp = fopen(FILE_NAME, "w");
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
    
    // Connect to server
    int sock_fd = connect_to_server();
    if(sock_fd < STATUS_OK)
    {
        printf("Error: %d\n", sock_fd);
        return sock_fd;
    }
    
    // Game handling
    err = communication(sock_fd);
    if(err != STATUS_OK)
    {
        printf("Error: %d\n", err);
        return err;
    }

    // Close the socket
    err = close_client(sock_fd);
    if(err != STATUS_OK)
    {
        printf("Error: %d\n", err);
        return err;
    }
    
    return 0;
}