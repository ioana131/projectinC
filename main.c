#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
void enable_colors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
}
#else
void enable_colors() {}
#endif

#define MAX_PLAYERS 50
#define MAX_MATCHES 100
#define PLAYER_FILE "players.txt"
#define MATCH_FILE "matches.txt"

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_CYAN "\033[1;36m"
#define COLOR_RED "\033[1;31m"
#define COLOR_PURPLE "\033[1;35m"

typedef struct {
    char name[50];
    int wins;
    int losses;
    int matches_played;
} Player;

typedef struct {
    char player1[50];
    char player2[50];
    int score1;
    int score2;
} Match;

Player players[MAX_PLAYERS];
Match matches[MAX_MATCHES];
int num_players = 0;
int num_matches = 0;

void deletescreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void save_players();
void load_players();
void save_matches();
void load_matches();
void add_match(char player1[], char player2[], int score1, int score2);
void view_rankings();
void view_player_history(char player_name[]);
void display_menu();
void input_string(const char *prompt, char *input, int size);
int input_int(const char *prompt);
void wait_for_user();

void save_players() {
    FILE *file = fopen(PLAYER_FILE, "w");
    if (file == NULL) {
        printf(COLOR_RED "Error opening file to save players.\n" COLOR_RESET);
        return;
    }
    for (int i = 0; i < num_players; i++) {
        fprintf(file, "%s %d %d %d\n", players[i].name, players[i].wins, players[i].losses, players[i].matches_played);
    }
    fclose(file);
}

void load_players() {
    FILE *file = fopen(PLAYER_FILE, "r");
    if (file == NULL) {
        return;
    }
    while (fscanf(file, "%s %d %d %d", players[num_players].name, &players[num_players].wins,
                  &players[num_players].losses, &players[num_players].matches_played) != EOF) {
        num_players++;
    }
    fclose(file);
}

void save_matches() {
    FILE *file = fopen(MATCH_FILE, "w");
    if (file == NULL) {
        printf(COLOR_RED "Error opening file to save matches.\n" COLOR_RESET);
        return;
    }
    for (int i = 0; i < num_matches; i++) {
        fprintf(file, "%s %s %d %d\n", matches[i].player1, matches[i].player2, matches[i].score1, matches[i].score2);
    }
    fclose(file);
}

void load_matches() {
    FILE *file = fopen(MATCH_FILE, "r");
    if (file == NULL) {
        return;
    }
    while (fscanf(file, "%s %s %d %d", matches[num_matches].player1, matches[num_matches].player2,
                  &matches[num_matches].score1, &matches[num_matches].score2) != EOF) {
        num_matches++;
    }
    fclose(file);
}

void add_match(char player1[], char player2[], int score1, int score2) {
    strcpy(matches[num_matches].player1, player1);
    strcpy(matches[num_matches].player2, player2);
    matches[num_matches].score1 = score1;
    matches[num_matches].score2 = score2;

    int i;
    int player1_exists = 0, player2_exists = 0;

    for (i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, player1) == 0) {
            players[i].matches_played++;
            if (score1 > score2) players[i].wins++;
            else players[i].losses++;
            player1_exists = 1;
        }
    }

    if (!player1_exists) {
        strcpy(players[num_players].name, player1);
        players[num_players].wins = (score1 > score2) ? 1 : 0;
        players[num_players].losses = (score1 < score2) ? 1 : 0;
        players[num_players].matches_played = 1;
        num_players++;
    }

    for (i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, player2) == 0) {
            players[i].matches_played++;
            if (score2 > score1) players[i].wins++;
            else players[i].losses++;
            player2_exists = 1;
        }
    }

    if (!player2_exists) {
        strcpy(players[num_players].name, player2);
        players[num_players].wins = (score2 > score1) ? 1 : 0;
        players[num_players].losses = (score2 < score1) ? 1 : 0;
        players[num_players].matches_played = 1;
        num_players++;
    }

    num_matches++;
    save_players();
    save_matches();
}

void view_rankings() {
    printf("\n--- " COLOR_CYAN "Player Rankings" COLOR_RESET " ---\n");
    for (int i = 0; i < num_players; i++) {
        printf("%d. " COLOR_GREEN "%s" COLOR_RESET " - Wins: " COLOR_YELLOW "%d" COLOR_RESET ", Losses: " COLOR_RED "%d" COLOR_RESET ", Matches: " COLOR_PURPLE "%d" COLOR_RESET "\n",
               i + 1, players[i].name, players[i].wins, players[i].losses, players[i].matches_played);
    }
}

void view_player_history(char player_name[]) {
    printf("\n--- " COLOR_CYAN "Match History for %s" COLOR_RESET " ---\n", player_name);
    for (int i = 0; i < num_matches; i++) {
        if (strcmp(matches[i].player1, player_name) == 0 || strcmp(matches[i].player2, player_name) == 0) {
            printf(COLOR_GREEN "%s" COLOR_RESET " vs " COLOR_GREEN "%s" COLOR_RESET " | Score: " COLOR_YELLOW "%d - %d" COLOR_RESET "\n",
                   matches[i].player1, matches[i].player2, matches[i].score1, matches[i].score2);
        }
    }
}

void display_menu() {
    printf("\n--- " COLOR_CYAN "Table Tennis Match Management" COLOR_RESET " ---\n");
    printf("1. Add Match Result\n");
    printf("2. View Rankings\n");
    printf("3. View Player History\n");
    printf("4. Exit\n");
}

void input_string(const char *prompt, char *input, int size) {
    printf(COLOR_YELLOW "%s" COLOR_RESET, prompt);
    fgets(input, size, stdin);
    input[strcspn(input, "\n")] = 0;
    while (strlen(input) == 0) {
        printf(COLOR_RED "Input cannot be empty. Try again: " COLOR_RESET);
        fgets(input, size, stdin);
        input[strcspn(input, "\n")] = 0;
    }
}

int input_int(const char *prompt) {
    int number;
    printf(COLOR_YELLOW "%s" COLOR_RESET, prompt);
    while (scanf("%d", &number) != 1) {
        printf(COLOR_RED "Invalid number. Try again: " COLOR_RESET);
        while (getchar() != '\n');
    }
    getchar(); // consume newline
    return number;
}

void wait_for_user() {
    printf("\nPress Enter to continue...");
    getchar();
    deletescreen();
}

int main() {
    int choice;
    char player1[50], player2[50];
    int score1, score2;
    char player_name[50];

    enable_colors();
    load_players();
    load_matches();

    do {
        display_menu();
        choice = input_int("Choose an option (1-4): ");

        switch (choice) {
            case 1:
                deletescreen();
                input_string("Enter " COLOR_GREEN "Player 1 Name" COLOR_RESET ": ", player1, sizeof(player1));
                input_string("Enter " COLOR_GREEN "Player 2 Name" COLOR_RESET ": ", player2, sizeof(player2));
                score1 = input_int("Enter " COLOR_GREEN "Score for Player 1" COLOR_RESET ": ");
                score2 = input_int("Enter " COLOR_GREEN "Score for Player 2" COLOR_RESET ": ");
                add_match(player1, player2, score1, score2);
                deletescreen();
                break;
            case 2:
                deletescreen();
                view_rankings();
                wait_for_user();
                break;
            case 3:
                deletescreen();
                input_string("Enter " COLOR_GREEN "Player Name" COLOR_RESET " to view history: ", player_name, sizeof(player_name));
                view_player_history(player_name);
                wait_for_user();
                break;
            case 4:
                deletescreen();
                printf(COLOR_CYAN "Exiting program.\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_RED "Invalid choice. Try again.\n" COLOR_RESET);
        }
    } while (choice != 4);

    return 0;
}
