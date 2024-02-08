#include "battleship.h"
#include <windows.h>

Stats players[2] = {{0, 0, 0, 0.0}, {0, 0, 0, 0.0}};

Cell playerOneGameBoard[ROWS][COLS];
Cell playerTwoGameBoard[ROWS][COLS];

Coordinate target;
Coordinate targetTemp;
Coordinate targetOrigin;
Coordinate targetAI;

WaterCraft ship[8] = {
	{'c', 4, "Carrier 1"}, {'c', 4, "Carrier 2"},
	{'d', 3, "Destroyer 1"}, {'d', 3, "Destroyer 2"},
	{'b', 2, "Battleship 1"}, {'b', 2, "Battleship 2"},
	{'a', 1, "Boat 1"}, {'a', 1, "Boat 2"}
};

Boolean    huntMode       = TRUE;
Boolean    targetMode     = FALSE;
Boolean    flipper        = TRUE;
Boolean    cardinals[4]   = {TRUE, TRUE, TRUE, TRUE};
Boolean    hasAShipSunked = FALSE;


short sunkShip[2][8] = {{4, 4, 3, 3, 2, 2, 1, 1}, {4, 4, 3, 3, 2, 2, 1, 1}};

short player  = 0;
short shot    = 0;
int   option  = 0;
int   north   = 0,
      south   = 0,
      east    = 0,
      west    = 0;
int   i       = 0,
      counter = 1;

char  shipSymbol = '\0';

FILE *outStream = NULL;


int main (void) {

	outStream = fopen (LOG_FILE_NAME, "w");

	srand ((unsigned int) time (NULL));

	firstScreen ();
	scanf ("%d", &option);
	switch (option) {
		case 2:
			selectAndLoadGame();
			goto game;
			break;
		case 3:
			displayGameResults();
			systemMessage("Press ENTER to continue\n");
			system("cls");
			main();
			break;
		case 4:
			system("cls");
			displayScoreboard();
			break;
		case 5: {
			fclose (outStream);
			return 1;
		}
		break;
	}
	system ("cls");

	int gameMode;
	printf("Enter 1 for play against computer\nEnter 2 for multiplayer\n");
	scanf("%d",&gameMode);
	system ("cls");

	initializeGameBoard (playerOneGameBoard);
	initializeGameBoard (playerTwoGameBoard);

	char playerOneUsername[30];
	char playerTwoUsername[30];
	while(TRUE) {
		printf("Enter your username for Player 1:\n");
		scanf("%29s", playerOneUsername);
		system("cls");
		if(strcmp(playerOneUsername, "COMPUTER") != 0) {
			break;
		}
	}

	manuallyPlaceShipsOnGameBoard(playerOneGameBoard, ship);

	system("cls");

	printf("Player 2 Board\n");
	switch (gameMode) {
		case 1:
			randomlyPlaceShipsOnGameBoard (playerTwoGameBoard, ship);
			break;
		case 2:
			while(TRUE) {
				printf("Enter your username for Player 2:\n");
				scanf("%29s", playerTwoUsername);
				system("cls");
				if(strcmp(playerOneUsername, "COMPUTER") != 0) {
					break;
				}
			}
			manuallyPlaceShipsOnGameBoard (playerTwoGameBoard, ship);
			break;
	}

	system ("cls");

game:
	while (TRUE) {

		fprintf (outStream, "Player %d's turn.\n", player + 1);

		switch (player) {

			case PLAYER_ONE: {
				printf("Player %d's turn. Choose an option:\n", player + 1);
				printf("1. Take a shot\n");
				printf("2. Give up\n");
				printf("3. Save game and continue later\n");

				int choice;
				scanf("%d", &choice);

				switch (choice) {
					case 1: {
						printf ("> Your Board:\n");
						printGameBoard (playerOneGameBoard, TRUE);
						printf ("> Player 2's' Board:\n");
						printGameBoard (playerTwoGameBoard, FALSE);
						printf ("> player 1's' TURN\n");

						do {
							target = getTarget ();
							shot = checkShot (playerTwoGameBoard, target);

							if (shot == -1)
								printf ("> Try inputting another target!\n");

						} while (shot == -1);

						shipSymbol = playerTwoGameBoard[target.row][target.column].symbol;
						break;
					}
					break;
					case 2:
						// Handle giving up (maybe end the game and declare the other player the winner)
						break;
					case 3:
						if(gameMode==2) {

							if (askForPlayerAgreement(player)) {
								saveGame(playerOneGameBoard,playerTwoGameBoard,players[0],players[1],0,playerOneUsername,playerTwoUsername);
								updateResults(playerOneUsername, "incomplete");
								updateResults(playerTwoUsername, "incomplete");
								printf("Game saved. Returning to main menu.\n");
								return 0;
							} else {
								printf("Other player did not agree. Continue with your turn.\n");
							}
						} else {
							saveGame(playerOneGameBoard,playerTwoGameBoard,players[0],players[1],0,playerOneUsername,playerTwoUsername);
							updateResults(playerOneUsername, "incomplete");
							updateResults(playerTwoUsername, "incomplete");
							printf("Game saved. Returning to main menu.\n");
							return 0;
						}
						break;
				}
			}

			case PLAYER_TWO: {

				if (gameMode==2) {
					printf("Player %d's turn. Choose an option:\n", player + 1);
				printf("1. Take a shot\n");
				printf("2. Give up\n");
				printf("3. Save game and continue later\n");

				int choice;
				scanf("%d", &choice);

				switch (choice){
				case 1:
					printf ("> Your Board:\n");
					printGameBoard (playerTwoGameBoard, TRUE);
					printf ("> Player 1's' Board:\n");
					printGameBoard (playerOneGameBoard, FALSE);
					printf ("> Player 2's TURN\n");

					do {
						target = getTarget ();
						shot = checkShot (playerOneGameBoard, target);

						if (shot == -1)
							printf ("> Try inputting another target!\n");

					} while (shot == -1);

					shipSymbol = playerOneGameBoard[target.row][target.column].symbol;
					break;
				case 2:
				//
				break;
				case 3:
					if (askForPlayerAgreement(player)) {
								saveGame(playerOneGameBoard,playerTwoGameBoard,players[0],players[1],0,playerOneUsername,playerTwoUsername);
								updateResults(playerOneUsername, "incomplete");
								updateResults(playerTwoUsername, "incomplete");
								printf("Game saved. Returning to main menu.\n");
								return 0;
							} else {
								printf("Other player did not agree. Continue with your turn.\n");
							}break;}
				} else if(gameMode==1) {



					printf ("> Player 1's Board:\n");
					printGameBoard (playerOneGameBoard, TRUE);
					printf ("> COMPUTER'S TURN\n");


					if (hasAShipSunked) {
						hasAShipSunked = FALSE;
						targetMode = FALSE;
						huntMode = TRUE;
					}

					if (targetMode) {
						target = targetAI;

						do {
							if (cardinals[NORTH]) {
								target.row = north;
							} else if (cardinals[SOUTH]) {
								target.row = south;
							} else if (cardinals[WEST]) {
								target.column = west;
							} else if (cardinals[EAST]) {
								target.column = east;
							} else if (!cardinals[NORTH] && !cardinals[SOUTH] &&
							           !cardinals[WEST]  && !cardinals[EAST]  &&
							           !hasAShipSunked) {

								target = targetOrigin;
								targetTemp = target;

								north = target.row - counter;
								targetTemp.row = north;

								if (checkShot (playerOneGameBoard, targetTemp) != -1 && north >= 0) {
									cardinals[NORTH] = TRUE;
								}

								targetTemp = target;
								south = target.row + counter;
								targetTemp.row = south;

								if (checkShot (playerOneGameBoard, targetTemp) != -1 && south <= 9) {
									cardinals[SOUTH] = TRUE;
								}

								targetTemp = target;
								west = target.column - counter;
								targetTemp.column = west;

								if (checkShot (playerOneGameBoard, targetTemp) != -1 && west >= 0) {
									cardinals[WEST] = TRUE;
								}

								targetTemp = target;
								east = target.column + counter;
								targetTemp.column = east;

								if (checkShot (playerOneGameBoard, targetTemp) != -1 && east <= 9) {
									cardinals[EAST] = TRUE;
								}

								counter++;

							} else  {
								targetMode = FALSE;
								huntMode = TRUE;
								break;
							}

							shot = checkShot (playerOneGameBoard, target);

						} while (shot == -1 && targetMode == TRUE);

						if (shot == 1 && huntMode == FALSE) {
							for (i = 0; i < 4; i++) {
								if (flipper == FALSE)
									cardinals[i] = FALSE;

								if (cardinals[i] == flipper)
									flipper = FALSE;
							}
						} else {
							for (i = 0; i < 4; i++) {
								if (flipper == TRUE && cardinals[i] != FALSE) {
									cardinals[i] = FALSE;
									break;
								}
							}
						}

						flipper = TRUE;
					}

					if (huntMode) {

						counter = 1;
						flipper = TRUE;
						for (i = 0; i < 4; i++)
							cardinals[i] = TRUE;

						do {
							target.row = getRandomNumber (0, 9);
							target.column = getRandomNumber (0, 9);

							shot = checkShot (playerOneGameBoard, target);
						} while (shot == -1);

						if (shot == 1) targetOrigin = target;
					}

					if (shot == 1) {

						if (!cardinals[NORTH] && !cardinals[SOUTH] &&
						        !cardinals[WEST]  && !cardinals[EAST]  &&
						        !hasAShipSunked) {
							target = targetOrigin;
						}

						huntMode = FALSE;
						targetMode = TRUE;
						targetAI = target;

						if (cardinals[NORTH] == TRUE) {
							north = (targetAI.row - 1);
							checkBoundsOfCardinal (cardinals, north, NORTH);
							targetTemp = target;
							targetTemp.row = north;
							if (checkShot (playerOneGameBoard, targetTemp) == -1)
								cardinals[NORTH] = FALSE;
						}

						if (cardinals[SOUTH] == TRUE) {
							south = targetAI.row + 1;
							checkBoundsOfCardinal (cardinals, south, SOUTH);
							targetTemp = target;
							targetTemp.row = south;
							if (checkShot (playerOneGameBoard, targetTemp) == -1)
								cardinals[SOUTH] = FALSE;
						}

						if (cardinals[WEST] == TRUE) {
							west  = targetAI.column - 1;
							checkBoundsOfCardinal (cardinals, west, WEST);
							targetTemp = target;
							targetTemp.column = west;
							if (checkShot (playerOneGameBoard, targetTemp) == -1)
								cardinals[WEST] = FALSE;
						}

						if (cardinals[EAST] == TRUE) {
							east  = targetAI.column + 1;
							checkBoundsOfCardinal (cardinals, east, EAST);
							targetTemp = target;
							targetTemp.column = east;
							if (checkShot (playerOneGameBoard, targetTemp) == -1)
								cardinals[EAST] = FALSE;
						}
					}

					shipSymbol = playerOneGameBoard[target.row][target.column].symbol;
					break;
				}
			}
		}


		if (shot == 1) {
			printf ("> %d, %d is a hit!\n", target.row, target.column);

			fprintf (outStream, "%d, %d is a hit!\n", target.row, target.column);

			players[player].numHits++;

			if (player == 1)
				hasAShipSunked = checkSunkShip (sunkShip, !player, shipSymbol, outStream);
			else
				checkSunkShip (sunkShip, !player, shipSymbol, outStream);

		} else {
			printf ("> %d, %d is a miss!\n", target.row, target.column);

			fprintf (outStream, "%d, %d is a miss!\n", target.row, target.column);
			players[player].numMisses++;
		}

		if (player == 0)
			updateGameBoard (playerTwoGameBoard, target);
		else
			updateGameBoard (playerOneGameBoard, target);


		if (isWinner (players, player)) {
			printf ("\n> Player %d wins!\n", player + 1);

			fprintf (outStream, "\n>>>>> Player %d wins! <<<<<\n", player + 1);
			break;
		}

		systemMessage ("> Hit <ENTER> to continue!\n");

		if(shot != 1) {
			player = !player;
		}

		system ("cls");
	}

	players[0].totalShots = players[0].numHits + players[0].numMisses;
	players[0].hitMissRatio = ((double) players[0].numHits/(double) players[0].numMisses) * 100;
	players[1].totalShots = players[1].numHits + players[1].numMisses;
	players[1].hitMissRatio = ((double) players[1].numHits/(double) players[1].numMisses) * 100;
	fprintf (outStream, "+===================================================\n");
	fprintf (outStream, "|                    PLAYER STATS                   \n");
	fprintf (outStream, "+---------------------------------------------------\n");
	fprintf (outStream, "| PLAYER 1 : %d hits                                \n", players[0].numHits);
	fprintf (outStream, "|            %d misses                              \n", players[0].numMisses);
	fprintf (outStream, "|            %d total shots                         \n", players[0].totalShots);
	fprintf (outStream, "|            %.2lf%% hit/miss ratio                 \n", players[0].hitMissRatio);
	fprintf (outStream, "| PLAYER 2 : %d hits                                \n", players[1].numHits);
	fprintf (outStream, "|            %d misses                              \n", players[1].numMisses);
	fprintf (outStream, "|            %d total shots                         \n", players[1].totalShots);
	fprintf (outStream, "|            %.2lf%% hit/miss ratio                 \n", players[1].hitMissRatio);
	fprintf (outStream, "+===================================================");

	fclose (outStream);
	return 0;
}

int askForPlayerAgreement(int currentPlayer) {
	printf("Player %d wants to save the game and continue later. Does Player %d agree? (1 for Yes, 0 for No)\n",
	       currentPlayer + 1, (currentPlayer == 0 ? 2 : 1));
	int agreement;
	scanf("%d", &agreement);
	return agreement;
}

void saveGame(Cell playerOneGameBoard[][COLS], Cell playerTwoGameBoard[][COLS],
              Stats playerOneStats, Stats playerTwoStats, int currentPlayer,const char *playerOneName, const char *playerTwoName) {
	FILE *file;
	char filename[100];

	// Create a unique filename using the current time
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	snprintf(filename, sizeof(filename), "battleship_save_%d-%02d-%02d_%02d-%02d-%02d.dat",
	         t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	// Open the file for writing
	file = fopen(filename, "wb");
	if (file == NULL) {
		perror("Error opening file for saving");
		return;
	}

	// Write player game boards
	fwrite(playerOneGameBoard, sizeof(Cell), ROWS * COLS, file);
	fwrite(playerTwoGameBoard, sizeof(Cell), ROWS * COLS, file);

	// Write player stats
	fwrite(&playerOneStats, sizeof(Stats), 1, file);
	fwrite(&playerTwoStats, sizeof(Stats), 1, file);

	// Write the current player
	fwrite(&currentPlayer, sizeof(int), 1, file);
	saveGameResult(playerOneName, playerTwoName, "Incomplete");

	fclose(file);
	printf("Game saved as %s\n", filename);
}


void listSavedGames() {
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile("*.battleship_save", &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		system("cls");
		printf("No saved games found.\n");
		main();
	} else {
		do {
			printf("%s\n", findFileData.cFileName);
		} while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
	}
}


void selectAndLoadGame() {
	listSavedGames();
	char filename[100];

	printf("Enter the filename of the game you want to load: ");
	scanf("%99s", filename);

	loadGame(filename);
}

void loadGame(const char *filename) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Error opening file.\n");
		goto *0;
	}

	fread(playerOneGameBoard, sizeof(Cell), ROWS * COLS, file);
	fread(playerTwoGameBoard, sizeof(Cell), ROWS * COLS, file);
	fread(player, sizeof(int), 1, file);

	char playerOneName[30], playerTwoName[30];
	fread(playerOneName, sizeof(char), 30, file);
	fread(playerTwoName, sizeof(char), 30, file);
	fread(&players[0], sizeof(Stats), 1, file);
	fread(&players[1], sizeof(Stats), 1, file);

	fclose(file);


	printf("Resuming game between %s and %s...\n", playerOneName, playerTwoName);

}

void saveGameResult(const char *playerOneName, const char *playerTwoName, const char *winner) {
	FILE *file = fopen("game_results.txt", "a"); // Open for appending
	if (file == NULL) {
		perror("Error opening results file");
		return;
	}

	if (winner == NULL) {
		fprintf(file, "Game between %s and %s was incomplete.\n", playerOneName, playerTwoName);
	} else {
		fprintf(file, "Game between %s and %s was won by %s.\n", playerOneName, playerTwoName, winner);
	}

	fclose(file);
}

void displayGameResults() {
	FILE *file = fopen("game_results.txt", "r");
	if (file == NULL) {
		perror("Error opening results file");
		return;
	}

	char buffer[256];
	while (fgets(buffer, sizeof(buffer), file)) {
		printf("%s", buffer);
	}

	fclose(file);
}

void updateResults(const char *playerName, const char *result) {
    // Open the file for appending
    FILE *file = fopen("user_results.csv", "a");
    if (file == NULL) {
        perror("Error opening results file");
        return;
    }

    // Write the result
    fprintf(file, "%s,%s\n", playerName, result);
    fclose(file);
}

void displayScoreboard() {
    FILE *file = fopen("user_results.csv", "r");
    if (file == NULL) {
        perror("Error opening results file");
        main();
    }


    UserStats users[20];
    int userCount = 0;

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char *username = strtok(line, ",");
        char *result = strtok(NULL, "\n");

        
    }

    // Display the stats
    for (int i = 0; i < userCount; i++) {
        printf("User: %s, Wins: %d, Losses: %d, Incomplete: %d\n",
               users[i].username, users[i].wins, users[i].losses, users[i].incomplete);
    }

    fclose(file);
}


