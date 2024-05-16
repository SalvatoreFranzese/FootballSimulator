#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "connection_handler.h"
#include "match.h"
#include "../utils/player.h"
#include "../utils/constants.h"

player_t players[MAX_PLAYERS];

void signal_handler(int signal)
{
    if (signal == SIGALRM)
    {
        end_match(players);
    }
}

int main(int argc, char **argv)
{
    signal(SIGALRM, signal_handler);
    
    int opt = 1;
    int sock_fd;
    player_t team_a[MAX_PLAYERS_TEAM];
    player_t team_b[MAX_PLAYERS_TEAM];
    struct sockaddr_in my_address;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Errore nella creazione della socket\n"), exit(1);
    
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        perror("setsockopt"), exit(1);

    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    my_address.sin_port = htons(PORT);

    bind(sock_fd, (struct sockaddr *) &my_address, sizeof(my_address));
    
    listen(sock_fd, MAX_PLAYERS);
    puts("In attesa dei giocatori...");

    wait_for_all_players(sock_fd, players);
    initialize_players(players);

    create_teams(players, team_a, team_b);
    print_teams(team_a, team_b);
    
    alarm(MAX_MATCH_DURATION);
    start_match(players, team_a, team_b);

    close_player_connection(players);
    close(sock_fd);
}