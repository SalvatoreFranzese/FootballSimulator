#include "connection_handler.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../utils/constants.h"

void wait_for_all_players(int sock_fd, player_t *players)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].fd = accept(sock_fd, 0, 0);
        printf("Player %d connesso!\n", i + 1);
    }
}

void close_player_connection(player_t *players)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        close(players[i].fd);    
    }
}