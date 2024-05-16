#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>

typedef enum player_role
{
    STRIKER,
    CENTER_MIDFIELDER,
    DEFENDER
} player_role_enum;

typedef enum team
{
    TEAM_A,
    TEAM_B
} team_enum;

typedef struct stats
{
    int shooting;
    int dribbling;
    int speed;
} stats_t;

typedef struct player
{
    int fd;
    pthread_t tid;
    char name[20];
    player_role_enum player_role;
    stats_t stats;
    team_enum team;
    int is_injured;
    int injury_time;
    int is_expelled;
} player_t;

#endif