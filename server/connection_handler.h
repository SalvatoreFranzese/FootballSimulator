#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include "../utils/player.h"

void wait_for_all_players(int sock_fd, player_t *players);
void close_player_connection(player_t *players);

#endif