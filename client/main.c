#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "../utils/player.h"
#include "../utils/colors.h"
#include "../utils/constants.h"

int previous_choises[MAX_PLAYERS_TEAM - 1];

void choose_nickname(int sock_fd);
void choose_role(int sock_fd);
int check_is_captain(int sock_fd);
void create_your_team(int sock_fd);
void clean_socket_buffer(int sock_fd);
void wait_your_team(int sock_fd);

int main(int argc, char **argv)
{
    int sock_fd;
    struct sockaddr_in my_address;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Errore nella creazione della socket\n"), exit(1);

    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    my_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP_ADDRESS, &my_address.sin_addr) <= 0)
        perror("Invalid address/Address not supported\n"), exit(1);

    if (connect(sock_fd, (struct sockaddr *) &my_address, sizeof(my_address)) < 0)
        perror("Errore nella connessione al server\n"), exit(1);

    printf(ANSI_COLOR_GREEN "\n███╗   ███╗ █████╗ ██████╗ ██╗ ██████╗ ███████╗████████╗██████╗ ██╗██╗  ██╗███████╗██████╗ ███████╗ ██████╗███╗   ███╗██████╗ \n████╗ ████║██╔══██╗██╔══██╗██║██╔═══██╗██╔════╝╚══██╔══╝██╔══██╗██║██║ ██╔╝██╔════╝██╔══██╗██╔════╝██╔════╝████╗ ████║██╔══██╗\n██╔████╔██║███████║██████╔╝██║██║   ██║███████╗   ██║   ██████╔╝██║█████╔╝ █████╗  ██████╔╝███████╗██║     ██╔████╔██║██║  ██║\n██║╚██╔╝██║██╔══██║██╔══██╗██║██║   ██║╚════██║   ██║   ██╔══██╗██║██╔═██╗ ██╔══╝  ██╔══██╗╚════██║██║     ██║╚██╔╝██║██║  ██║\n██║ ╚═╝ ██║██║  ██║██║  ██║██║╚██████╔╝███████║   ██║   ██║  ██║██║██║  ██╗███████╗██║  ██║███████║╚██████╗██║ ╚═╝ ██║██████╔╝\n╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝     ╚═╝╚═════╝\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "Aspetta che gli altri giocatori si connettino!\n" ANSI_COLOR_RESET);

    choose_nickname(sock_fd);
    choose_role(sock_fd);

    int is_captain = check_is_captain(sock_fd);

    if (is_captain)
        create_your_team(sock_fd);
    else
        wait_your_team(sock_fd);
    
    close(sock_fd);
}

void choose_nickname(int sock_fd)
{
    char message[64] = "";
    char answer[20] = "";

    int bytes_read = read(sock_fd, message, sizeof(message));
    if (bytes_read < 0)
        perror("Errore in lettura dalla socket\n"), exit(1);

    printf("%s", message);
    fflush(STDIN_FILENO);
    scanf("%s", answer);

    write(sock_fd, answer, sizeof(answer));
}

void choose_role(int sock_fd)
{
    char message[72] = "";
    int answer;

    int bytes_read = read(sock_fd, message, sizeof(message));
    if (bytes_read < 0)
        perror("Errore in lettura dalla socket\n"), exit(1);

    printf("%s", message);
    fflush(STDIN_FILENO);
    do
    {
        printf("\nInserisci il valore corrispondente al ruolo: ");
        scanf("%d", &answer);
        if (answer < 1 || answer > 3)
            printf("\nInserisci un valore valido!\n");
    } while(answer < 1 || answer > 3);
    
    write(sock_fd, &answer, sizeof(answer));
    printf(ANSI_COLOR_RED "\nGli altri giocatori non hanno ancora finito, attendi...\n" ANSI_COLOR_RESET);
}

int check_is_captain(int sock_fd)
{
    clean_socket_buffer(sock_fd);
    
    int is_captain = 0;
    
    int bytes_read = read(sock_fd, &is_captain, sizeof(is_captain));
    if (bytes_read < 0)
        perror("Errore in lettura dalla socket\n"), exit(1);
    
    return (is_captain == IS_CAPTAIN) ? 1 : 0;
}

void create_your_team(int sock_fd)
{
    printf("\nSei il capitano della ");
    printf(ANSI_COLOR_BLUE "SQUADRA A" ANSI_COLOR_RESET);
    printf(", scegli i tuoi giocatori:\n");

    for (int i = 0; i < MAX_PLAYERS - 1; i++)
    {
        char player_list[64] = "";

        int bytes_read = read(sock_fd, player_list, sizeof(player_list));
        if (bytes_read < 0)
            perror("Errore in lettura dalla socket\n"), exit(1);
        
        printf("%s\n", player_list);
    }   

    int choice;
    for (int i = 0; i < MAX_PLAYERS_TEAM - 1; i++) {
        do {
            fflush(STDIN_FILENO);
            printf("Scegli il %d° giocatore: ", i + 1);
            scanf("%d", &choice);

            if (choice < 1 || choice > MAX_PLAYERS) 
            {
                printf("Inserisci un valore valido!\n");
                choice = -1;
                continue;
            }

            for (int j = 0; j < i; j++) {
                if (choice == previous_choises[j]) {
                    printf("Il giocatore %d è già stato scelto.\n", choice);
                    choice = -1; 
                    break;
                }
            }            
        } while (choice == -1 ); 

        previous_choises[i] = choice;
        write(sock_fd, &choice, sizeof(choice));
    }
}

void wait_your_team(int sock_fd)
{
    int your_team;

    printf(ANSI_COLOR_RED "\nAspetta la formazione delle squadre...\n" ANSI_COLOR_RESET);
    int read_bytes = read(sock_fd, &your_team, sizeof(your_team));
    if (read_bytes < 0)
        perror("Errore in lettura dalla socket\n"), exit(1);

    if (your_team == ID_TEAM_A)
        printf(ANSI_COLOR_BLUE "Sei nella squadra A!\n" ANSI_COLOR_RESET);
    else if (your_team == ID_TEAM_B) 
        printf(ANSI_COLOR_RED "Sei nella squadra B!\n" ANSI_COLOR_RESET);
}

void clean_socket_buffer(int sock_fd)
{
    char trubbish[1024];
    read(sock_fd, trubbish, sizeof(trubbish));
}