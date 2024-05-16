#include "logger.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "../utils/colors.h"
#include "../utils/constants.h"

int log_fd;

void log_match_started(player_t *team_a, player_t *team_b)
{
    log_fd = open("/tmp/log_match.txt", O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR );
    if (log_fd < 0)
        perror("Errore nella creazione del file di log\n"), exit(1);
    
    log_time();
    log_header();
    log_teams(team_a, team_b);
}

void log_time()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    int len = strftime(buffer, sizeof(buffer), ANSI_COLOR_MAGENTA "\n\nData e ora partita: %Y-%m-%d %H:%M:%S" ANSI_COLOR_RESET, timeinfo);
    if (write(log_fd, buffer, len) < 0) { 
        perror("Errore durante la scrittura della data e ora nel file di log");
        exit(1);
    }
}

void log_header()
{
    const char title[] = ANSI_COLOR_GREEN "\n███╗   ███╗ █████╗ ██████╗ ██╗ ██████╗ ███████╗████████╗██████╗ ██╗██╗  ██╗███████╗██████╗ ███████╗ ██████╗███╗   ███╗██████╗ \n████╗ ████║██╔══██╗██╔══██╗██║██╔═══██╗██╔════╝╚══██╔══╝██╔══██╗██║██║ ██╔╝██╔════╝██╔══██╗██╔════╝██╔════╝████╗ ████║██╔══██╗\n██╔████╔██║███████║██████╔╝██║██║   ██║███████╗   ██║   ██████╔╝██║█████╔╝ █████╗  ██████╔╝███████╗██║     ██╔████╔██║██║  ██║\n██║╚██╔╝██║██╔══██║██╔══██╗██║██║   ██║╚════██║   ██║   ██╔══██╗██║██╔═██╗ ██╔══╝  ██╔══██╗╚════██║██║     ██║╚██╔╝██║██║  ██║\n██║ ╚═╝ ██║██║  ██║██║  ██║██║╚██████╔╝███████║   ██║   ██║  ██║██║██║  ██╗███████╗██║  ██║███████║╚██████╗██║ ╚═╝ ██║██████╔╝\n╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝     ╚═╝╚═════╝\n" ANSI_COLOR_RESET;
    const char match_started[] = "\nMatch iniziato!\n";

    if (write(log_fd, title, sizeof(title)) < 0)
        perror("Errore durante la scrittura del titolo nel file di log"), exit(1);

    if (write(log_fd, match_started, sizeof(match_started)) < 0)
        perror("Errore durante la scrittura del messaggio: \"inizio match\" nel file di log"), exit(1);
}

void log_teams(player_t *team_a, player_t *team_b)
{
    int i;
    
    char message_team_a[] = ANSI_COLOR_BLUE "\n================ SQUADRA A ================\n";
    if (write(log_fd, message_team_a, sizeof(message_team_a)) < 0)
        perror("Errore durante la scrittura del messaggio: \"SQUADRA A\" nel file di log"), exit(1);

    for (i = 0; i < MAX_PLAYERS_TEAM; i++)
    {
        char message[1024] = "";
        sprintf(message, "%s, ruolo: %s.\n", team_a[i].name, roles[team_a[i].player_role]);

        if (write(log_fd, message, sizeof(message)) < 0)
            perror("Errore durante la scrittura dei player della squadra A nel file di log"), exit(1);
    }

    char message_team_b[] = ANSI_COLOR_RESET ANSI_COLOR_RED "\n================ SQUADRA B ================\n";
    if (write(log_fd, message_team_b, sizeof(message_team_b)) < 0)
        perror("Errore durante la scrittura del messaggio: \"SQUADRA B\" nel file di log"), exit(1);

    for (i = 0; i < MAX_PLAYERS_TEAM; i++)
    {
        char message[1024] = "";
        sprintf(message, "%s, ruolo: %s.\n", team_b[i].name, roles[team_b[i].player_role]);

        if (write(log_fd, message, sizeof(message)) < 0)
            perror("Errore durante la scrittura dei player della squadra B nel file di log"), exit(1);
    }
}

void log_ball_possesion(player_t *player)
{
    char message[1024] = "";
    sprintf(message, ANSI_COLOR_RESET "%s ha rubato la palla!\n", player->name);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del possesso palla nel file di log"), exit(1);
}

void log_player_injury(player_t *player)
{
    char message[1024] = "";
    sprintf(message, "%s si è infortunato, resterà fuori per %d secondi\n", player->name, player->injury_time);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del player infortunato nel file di log"), exit(1);
}

void log_goal_scored(player_t *player, int goal_team_a, int goal_team_b)
{
    char message[1024] = "";
    sprintf(message, "%s ha segnato!\n", player->name);
    sprintf(message, "Risulato attuale: %d - %d\n", goal_team_a, goal_team_b);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del risultato attuale nel file di log"), exit(1);
}

void log_match_ended(int goal_team_a, int goal_team_b, int failed_shots_counter, int failed_dribbling_counter)
{   
    char message[1024] = "";
    sprintf(message, "RISULATO FINALE: %d - %d\nTiri falliti: %d\nDribbling falliti: %d\n", goal_team_a, goal_team_b, failed_shots_counter, failed_dribbling_counter);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del risultato finale nel file di log"), exit(1);

    close(log_fd);
}

void log_attempted_dribbling(player_t *player)
{
    char message[1024] = "";
    sprintf(message, "%s prova il dribbling...\n", player->name);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del tentativo di dribling nel file di log"), exit(1);
}

void log_failed_dribbling(player_t *player)
{
    char message[1024] = "";
    sprintf(message, "%s ha fallito il dribbling!\n", player->name);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del fallimento del dribling nel file di log"), exit(1);
}

void log_successful_dribbling(player_t *player)
{
    char message[1024] = "";
    sprintf(message, "%s ha dribblato il suo avversario!\n", player->name);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del successo di dribling nel file di log"), exit(1);
}

void log_attempted_shot(player_t *player)
{
    char message[1024] = "";
    sprintf(message, "%s prova il tiro...\n", player->name);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del tentativo di gol nel file di log"), exit(1);
}

void log_failed_shot(player_t *player)
{
    char message[1024] = "";
    sprintf(message, "%s ha mancato il bersaglio\n", player->name);
    if (write(log_fd, message, sizeof(message)) < 0)
        perror("Errore durante la scrittura del fallimento di gol nel file di log"), exit(1);
}