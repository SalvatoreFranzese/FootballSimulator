#include "match.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <string.h>
#include "../utils/colors.h"
#include "../utils/constants.h"
#include "logger.h"

pthread_mutex_t ball = PTHREAD_MUTEX_INITIALIZER;
int goal_team_a = 0;
int goal_team_b = 0;
int failed_shots_counter = 0;
int failed_dribbling_counter = 0;

typedef struct kick_off_data {
    player_t player;
    player_t *opponents;
} kick_off_data_t;

void ask_player_role(player_t *current_player)
{
    char message[] = "\nChe ruolo vuoi giocare?\n1. Attaccante.\n2. Centrocampista.\n3. Difensore.\n";
    int chosen_role;

    write(current_player->fd, message, sizeof(message));
    int bytes_read = read(current_player->fd, &chosen_role, sizeof(chosen_role));
    if (bytes_read < 0)
        perror("Errore in lettura del ruolo del giocatore\n"), exit(1);
    
    switch (chosen_role)
    {
    case 1:
        current_player->player_role = STRIKER;
        current_player->stats.shooting = 2;
        current_player->stats.dribbling = 1;
        current_player->stats.speed = 0;
        break;
    case 2:
        current_player->player_role = CENTER_MIDFIELDER;
        current_player->stats.shooting = 1;
        current_player->stats.dribbling = 1;
        current_player->stats.speed = 1;
        break;
    case 3:
        current_player->player_role = DEFENDER;
        current_player->stats.shooting = 0;
        current_player->stats.dribbling = 0;
        current_player->stats.speed = 3;
        break;
    }
    current_player->is_injured = 0;
    current_player->is_expelled = 0;

    printf("%s ha scelto il ruolo: %s\n", current_player->name, roles[chosen_role-1]);
}

void ask_player_name(player_t *current_player)
{
    char message[] = "Scegli un nickname(max 20 caratteri): ";

    write(current_player->fd, message, sizeof(message));
    int bytes_read = read(current_player->fd, &current_player->name, sizeof(current_player->name));
    if (bytes_read < 0)
        perror("Errore in lettura del nome del giocatore\n"), exit(1);

    printf("Scelto nickname \"%s\"\n", current_player->name);
}

void create_threads(player_t *players, void *function)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        int code_error = pthread_create(&players[i].tid, NULL, function, (void*) &players[i]);
        if (code_error != 0)
            errno = code_error, perror("Errore nella crezione del thread!\n"), exit(1);
    }
}

void wait_threads(player_t *players)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        int cod_err = pthread_join(players[i].tid, NULL);
        if (cod_err != 0)
            errno = cod_err, perror("ptherad_join"), exit(1);
    }
}

void *ask_names_and_role(void *args)
{
    player_t *current_player = (player_t*) args;

    ask_player_name(current_player);
    ask_player_role(current_player);

    pthread_exit(NULL);
}

void initialize_players(player_t *players)
{
   create_threads(players, ask_names_and_role);
   wait_threads(players);
}

void create_teams(player_t *players, player_t *team_a, player_t *team_b)
{
    players[0].team = TEAM_A;
    player_t captain = players[0];
    team_a[0] = captain;
    int id_team_a = ID_TEAM_A;

    show_players_to_captain(players, captain);    
    printf("\nIl capitano %s ha scelto per la ", captain.name);
    printf(ANSI_COLOR_BLUE "SQUADRA A:\n" ANSI_COLOR_RESET);

    for (int i = 1; i < MAX_PLAYERS_TEAM; i++)
    {
        int choice;

        int bytes_read = read(captain.fd, &choice, sizeof(choice));
        if(bytes_read < 0)
            perror("Errore in lettura della scelta del capitano\n"), exit(1);

        players[choice].team = TEAM_A;
        team_a[i] = players[choice];
        printf(ANSI_COLOR_GREEN "%d. %s aggiunto al Team A\n" ANSI_COLOR_RESET, i, team_a[i].name);
        write(team_a[i].fd, &id_team_a, sizeof(id_team_a));
    }

    create_other_team(players, team_a, team_b);
}

void show_players_to_captain(player_t *players, player_t captain)
{
    int is_captain = IS_CAPTAIN;
    int is_not_captain = IS_NOT_CAPTAIN;

    write(captain.fd, &is_captain, sizeof(is_captain));

    for (int i = 1; i < MAX_PLAYERS; i++)
    {
        write(players[i].fd, &is_not_captain, sizeof(is_not_captain));

        char message[64] = "";
     
        sprintf(message, "%d. %s ruolo: %s", i, players[i].name, roles[players[i].player_role]);
        
        write(captain.fd, message, sizeof(message));
    }   
}

void create_other_team(player_t *players, player_t *team_a, player_t *team_b) 
{
    int id_team_b = ID_TEAM_B;

    int j = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (!is_present(players[i], team_a))
        {
            players[i].team = TEAM_B;
            team_b[j] = players[i];
            write(team_b[j].fd, &id_team_b, sizeof(id_team_b));
            j++;
        }
    }
}

int is_present(player_t player, player_t *team_a) 
{
    for (int i = 0; i < MAX_PLAYERS_TEAM; i++)
    {
        if (player.fd == team_a[i].fd)
        {
            return 1;
        }
    }
    return 0;    
}

void print_teams(player_t *team_a, player_t *team_b) 
{
    int i;

    printf(ANSI_COLOR_BLUE "\n================ SQUADRA A ================\n");
    for (i = 0; i < MAX_PLAYERS_TEAM; i++)
    {
        printf("%s, ruolo: %s\n", team_a[i].name, roles[team_a[i].player_role]);
    }
    
    printf(ANSI_COLOR_RESET ANSI_COLOR_RED "\n================ SQUADRA B ================\n");
    for (i = 0; i < MAX_PLAYERS_TEAM; i++)
    {
        printf("%s, ruolo: %s\n", team_b[i].name, roles[team_b[i].player_role]);
    }
}

void start_match(player_t *players, player_t *team_a, player_t *team_b)
{
    print_status_match(NULL, ANSI_COLOR_YELLOW "\n========== La partita è iniziata ==========\n" ANSI_COLOR_RESET);
    log_match_started(team_a, team_b);
    sleep(1);

    create_kickoff_threads(players, team_a, team_b, kick_off);
    wait_threads(players);
}

void create_kickoff_threads(player_t *players, player_t *team_a, player_t *team_b, void *function)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        kick_off_data_t tmp;
        tmp.player = players[i];
        if (players[i].team == TEAM_A)
            tmp.opponents = team_b;
        else
            tmp.opponents = team_a;

        int code_error = pthread_create(&players[i].tid, NULL, function, (void*) &tmp);
        if (code_error != 0)
            errno = code_error, perror("Errore nella crezione del thread!\n"), exit(1);

        sleep(1);
    }
}

void *kick_off(void *args)
{
    kick_off_data_t *tmp = (kick_off_data_t*) args;
    player_t current_player = tmp->player;
    player_t *opponents = tmp->opponents;
    srand(time(NULL));
    int MIN_WAIT = 3;
    
    while (1)
    {
        sleep(rand() % 5 + MIN_WAIT - current_player.stats.speed);
        
        if (current_player.is_expelled == 1)
        {
            sleep(EXPEL_DURATION); 
            current_player.is_expelled = 0;
        }
        
        if (pthread_mutex_trylock(&ball) == 0)
        {
            ball_possession(&current_player);

            if (current_player.is_injured) 
            {
                expel_player(opponents);
            }

            pthread_mutex_unlock(&ball);

            if (current_player.is_injured) 
            {
                bench_player(&current_player);
            }
        }

        sleep(1);
    }
}

void expel_player(player_t *opponents)
{
    srand(time(NULL));
    int expelled_player = rand() % (MAX_PLAYERS_TEAM);

    if (opponents[expelled_player].is_expelled == 0)
    {
        printf(ANSI_COLOR_GREEN "%s ha commesso fallo, espulso per %d secondi\n" ANSI_COLOR_RESET, opponents[expelled_player].name, EXPEL_DURATION);
        opponents[expelled_player].is_expelled = 1;
    }
}

void bench_player(player_t *player)
{
    sleep(player->injury_time);
    player->is_injured = 0;
}

void ball_possession(player_t *player) 
{
    print_status_match(player, "%s ha rubato la palla!\n" ANSI_COLOR_RESET, player->name);
    log_ball_possesion(player);
    sleep(1);

    srand(time(NULL));
    int d6 = rand() % 6;

    switch (d6)
    {
    case 0:
        injury(player);
        break;

    case 1:
    case 2:
    case 3:
        dribbling(player);
        break;

    case 4:
    case 5:
        shoot(player);
        break;

    default:
        perror("Errore nel lancio del dado\n"), exit(1);
        break;
    }
}

void injury(player_t *player)
{
    srand(time(NULL));
    player->is_injured = 1;
    player->injury_time = rand() % MAX_INJURY_TIME + MIN_INJURY_TIME;

    print_status_match(player, "%s si è infortunato, resterà fuori per %d secondi\n" ANSI_COLOR_RESET, player->name, player->injury_time);
    log_player_injury(player);
}

void dribbling(player_t *player)
{
    print_status_match(player, "%s prova il dribbling...\n" ANSI_COLOR_RESET, player->name);
    log_attempted_dribbling(player);

    sleep(1);

    srand(time(NULL));
    int d6 = rand() % 6 + player->stats.dribbling;

    switch (d6)
    {
    case 0:
        injury(player);
        break;

    case 1:
    case 2:
    case 3:
        print_status_match(player, "%s ha fallito il dribbling!\n" ANSI_COLOR_RESET, player->name);
        log_failed_dribbling(player);
        failed_dribbling_counter++;
        sleep(1);
        break;
    
    default:
        print_status_match(player, "%s ha dribblato il suo avversario!\n" ANSI_COLOR_RESET, player->name);
        log_successful_dribbling(player);
        shoot(player);
        sleep(1);
        break;
    }
}

void shoot(player_t *player)
{
    print_status_match(player, "%s prova il tiro...\n" ANSI_COLOR_RESET, player->name);
    log_attempted_shot(player);
    sleep(1);

    srand(time(NULL));
    int d6 = rand() % 6 + player->stats.shooting;

    switch (d6)
    {
    case 0:
        injury(player);
        break;
    
    case 1:
    case 2:
    case 3:
        print_status_match(player, "%s ha mancato il bersaglio\n" ANSI_COLOR_RESET, player->name);
        log_failed_shot(player);
        failed_shots_counter++;
        sleep(1);
        break;

    default:
        goal(player);
        sleep(1);
        break;
    }
}

void goal(player_t *player)
{
    if (player->team == TEAM_A)
        goal_team_a++;
    else
        goal_team_b++;

    print_status_match(player, "%s ha segnato!\n" ANSI_COLOR_RESET, player->name);
    print_status_match(NULL, ANSI_COLOR_GREEN "\nRisulato attuale: %d - %d\n\n" ANSI_COLOR_RESET, goal_team_a, goal_team_b);
    log_goal_scored(player, goal_team_a, goal_team_b);
    
    sleep(1);
}

void stop_threads(player_t *players)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        pthread_cancel(players[i].tid);
    }   
}

void end_match(player_t *players)
{
    stop_threads(players);
    print_status_match(NULL, ANSI_COLOR_GREEN "\nRISULATO FINALE: %d - %d\n" ANSI_COLOR_RESET, goal_team_a, goal_team_b);

    if (goal_team_a > goal_team_b)
    {
        printf("Vince la ");
        printf(ANSI_COLOR_BLUE "SQUADRA A!\n" ANSI_COLOR_RESET);
    }
    else if (goal_team_b > goal_team_a)
    {
        printf("Vince la ");
        printf(ANSI_COLOR_RED "SQUADRA B!\n" ANSI_COLOR_RESET);
    }
    else
        printf("Pareggio!\n");

    log_match_ended(goal_team_a, goal_team_b, failed_shots_counter, failed_dribbling_counter);
}

void print_status_match(player_t* player, const char *message, ...)
{
    if (player != NULL)
    {
        if (player->team == TEAM_A)
            printf(ANSI_COLOR_BLUE);
        else
            printf(ANSI_COLOR_RED);
    }

    va_list args;
    va_start(args, message);
    vfprintf(stdout, message, args);
    va_end(args);
}