#ifndef MATCH_H
#define MATCH_H

#include "../utils/player.h"

void *ask_names_and_role(void *args);
void ask_player_name(player_t *current_player);
void ask_player_role(player_t *current_player);
void create_threads(player_t *players, void *function);
void wait_threads(player_t *players);
void initialize_players(player_t *players);
void create_teams(player_t *players, player_t *team_a, player_t *team_b);
void show_players_to_captain(player_t *players, player_t captain);
void create_other_team(player_t *players, player_t *team_a, player_t *team_b);
int is_present(player_t player, player_t *team_a);
void print_teams(player_t *team_a, player_t *team_b);
void start_match(player_t *players, player_t *team_a, player_t *team_b);
void *kick_off(void *args);
void bench_player(player_t *player);
void ball_possession(player_t *player);
void injury(player_t *player);
void dribbling(player_t *player);
void shoot(player_t *player);
void goal(player_t *player);
void end_match(player_t *players);
void print_status_match(player_t* player, const char *message, ...);

void create_kickoff_threads(player_t *players, player_t *team_a, player_t *team_b, void *function);
void expel_player(player_t *opponents);

#endif