#ifndef LOGGER_H
#define LOGGER_H

#include "../utils/player.h"

void log_match_started(player_t *team_a, player_t *team_b);
void log_time();
void log_header();
void log_teams(player_t *team_a, player_t *team_b);
void log_ball_possesion(player_t *player);
void log_player_injury(player_t *player);
void log_goal_scored(player_t *player, int goal_team_a, int goal_team_b);
void log_attempted_dribbling(player_t *player);
void log_failed_dribbling(player_t *player);
void log_successful_dribbling(player_t *player);
void log_attempted_shot(player_t *player);
void log_failed_shot(player_t *player);
void log_match_ended(int goal_team_a, int goal_team_b, int failed_shots_counter, int dribbling_counter);

#endif