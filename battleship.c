#include "battleship.h"

void firstScreen (void) {
	printf ("Welcome\n");
	printf ("Enter 1 for start game\n");
	printf ("Enter 2 for continue the game\n");
	printf ("Enter 3 for results\n");
	printf ("Enter 4 for scoreboard\n");
	printf ("Enter 5 for save and quit\n");
}


void initializeGameBoard (Cell gameBoard[][COLS]) {
	int i = 0, j = 0;

	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++) {
			gameBoard[i][j].symbol          = WATER;
			gameBoard[i][j].position.row    = i;
			gameBoard[i][j].position.column = j;
		}
}


void printGameBoard (Cell gameBoard [][COLS], Boolean showPegs) {
	int i = 0, j = 0;

	printf ("  0 1 2 3 4 5 6 7 8 9\n");

	for (i = 0; i < ROWS; i++) {
		printf ("%d ", i);

		for (j = 0; j < COLS; j++) {
			if (showPegs == TRUE)
				printf ("%c ", gameBoard [i][j].symbol);
			else {
				switch (gameBoard [i][j].symbol) {
					case HIT:
						printf ("%c ", HIT);
						break;
					case MISS:
						printf ("%c ", MISS);
						break;
					case WATER:
					default:
						printf ("%c ", WATER);
						break;
				}
			}
		}

		putchar ('\n');
	}
}


void putShipOnGameBoard (Cell gameBoard[][COLS], WaterCraft ship,
                         Coordinate position, int direction) {
	int i = ship.length - 1;

	for (i = 0; i < ship.length; i++) {
		if (direction == HORIZONTAL)
			gameBoard [position.row][position.column + i].symbol = ship.symbol;
		else /* VERTICAL */
			gameBoard [position.row + i][position.column].symbol = ship.symbol;
	}
}


void manuallyPlaceShipsOnGameBoard (Cell gameBoard[][COLS], WaterCraft ship[]) {
	char       stringPosition[11] = "";
	int        i = 0, j = 0;

	Coordinate position[5];
	Boolean    isValid = FALSE;

	fflush (stdin);

	for (i = 0; i < NUM_OF_SHIPS; i++) {

		while (TRUE) {
			system ("cls");
			int flag=0;
			printGameBoard (gameBoard, TRUE);
			printf ("> Please enter the %d cells to place the %s across (no spaces):\n", ship[i].length, ship[i].name);
			printf ("> ");
			scanf ("%s", stringPosition);

			if (convertStringtoPosition (position, stringPosition, ship[i].length)) {

				if (isStraightLine(position, ship[i].length) &&
				        areAllPositionsValid(gameBoard, position, ship[i].length)) {
					isValid = TRUE;
					for (int j = 0; j < ship[i].length; j++) {
						gameBoard[position[j].row][position[j].column].symbol = ship[i].symbol;
					}
				} else {
					printf("> Invalid input or ship placement!\n");
					isValid = FALSE;
				}
			} else {
				isValid = FALSE;
				printf ("> Invalid input!\n");
			}


			if (isValid == TRUE) break;
		}

	}
}

Boolean areAllPositionsValid(Cell gameBoard[][COLS], Coordinate position[], int length) {
	for (int i = 0; i < length; i++) {
		if (!isSurroundingWater(gameBoard, position[i].row, position[i].column)) {
			return FALSE;
		}
	}
	return TRUE;
}

Boolean isCellWater(Cell gameBoard[][COLS], int row, int col) {
	return (row >= 0 && row < ROWS && col >= 0 && col < COLS && gameBoard[row][col].symbol == WATER);
}

Boolean isSurroundingWater(Cell gameBoard[][COLS], int row, int col) {
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			int checkRow = row + i;
			int checkCol = col + j;

			if (i == 0 && j == 0) continue;

			if (checkRow < 0 || checkRow >= ROWS || checkCol < 0 || checkCol >= COLS) continue;

			if (gameBoard[checkRow][checkCol].symbol != WATER) return FALSE;
		}
	}
	return TRUE;
}


Boolean isStraightLine(Coordinate position[], int length) {
	int i;
	Boolean isHorizontal = TRUE, isVertical = TRUE;

	for (i = 1; i < length; i++) {
		if (position[i].row != position[0].row+i) {
			isHorizontal = FALSE;
			break;
		}
	}

	for (i = 1; i < length; i++) {
		if (position[i].column != position[0].column+i) {
			isVertical = FALSE;
			break;
		}
	}

	return isHorizontal || isVertical;
}


void randomlyPlaceShipsOnGameBoard (Cell gameBoard[][COLS], WaterCraft ship[]) {
	Coordinate position;
	int direction = -1;
	int i = 0;

	for (i = 0; i < NUM_OF_SHIPS; i++) {
		while (TRUE) {
			direction = getRandomNumber (0, 1); /* 0 -> horizontal, 1 -> vertical */
			position = generatePosition (direction, ship[i].length);

			if (isValidLocation (gameBoard, position, direction, ship[i].length)) break;
		}

		putShipOnGameBoard (gameBoard, ship[i], position, direction);
	}
}


void updateGameBoard (Cell gameBoard[][COLS], Coordinate target) {
	switch (gameBoard[target.row][target.column].symbol) {
		/* miss */
		case WATER:
			gameBoard[target.row][target.column].symbol = MISS;
			break;

		/* hit */
		case CARRIER:
		case BATTLESHIP:
		case BOAT:
		case DESTROYER:
			gameBoard[target.row][target.column].symbol = HIT;
			break;

		case HIT:
		case MISS:
		default:
			break;
	}
}


void checkBoundsOfCardinal (Boolean cardinals[], int bound, int direction) {
	switch (direction) {
		case NORTH:
			if (bound < 0)
				cardinals[0] = FALSE;
			else
				cardinals[0] = TRUE;
			break;

		case SOUTH:
			if (bound > 9)
				cardinals[1] = FALSE;
			else
				cardinals[1] = TRUE;
			break;

		case WEST:
			if (bound < 0)
				cardinals[2] = FALSE;
			else
				cardinals[2] = TRUE;
			break;

		case EAST:
			if (bound > 9)
				cardinals[3] = FALSE;
			else
				cardinals[3] = TRUE;
			break;
	}
}


void systemMessage (char *message) {
	char ch = '\0';

	do {
		printf ("%s", message);
	} while ((ch = getch()) != '\r');
}


Boolean checkSunkShip (short sunkShip[][NUM_OF_SHIPS], short player, char shipSymbol, FILE *stream) {
	Boolean sunked = FALSE;

	switch (shipSymbol) {
		case CARRIER:
			if (--sunkShip[player][0] == 0) {
				printf ("> Player %d's Carrier sunked!\n", player + 1);

				fprintf (stream, "Player %d's Carrier sunked!\n", player + 1);

				sunked = TRUE;
			}
			break;

		case BATTLESHIP:
			if (--sunkShip[player][1] == 0) {
				printf ("> Player %d's Battleship sunked!\n", player + 1);

				fprintf (stream, "Player %d's Battleship sunked!\n", player + 1);

				sunked = TRUE;
			}
			break;

		case BOAT:
			if (--sunkShip[player][2] == 0) {
				printf ("> Player %d's Boat sunked!\n", player + 1);

				fprintf (stream, "Player %d's Boat sunked!\n", player + 1);

				sunked = TRUE;
			}
			break;

		case DESTROYER:
			if (--sunkShip[player][3] == 0) {
				printf ("> Player %d's Destroyer sunked!\n", player + 1);

				fprintf (stream, "Player %d's Destroyer sunked!\n", player + 1);

				sunked = TRUE;
			}
			break;
	}

	return sunked;
}


Boolean isValidLocation (Cell gameBoard[][COLS], Coordinate position,
                         int direction, int length) {
	int i = length - 1;
	Boolean isValid = TRUE;

	for (i = 0; isValid && i < length; i++) {
		if (direction == HORIZONTAL) {
			if (gameBoard [position.row][position.column + i].symbol != WATER
			        && (position.column + i) < COLS
			   )
				isValid = FALSE;
		} else { /* VERTICAL */
			if (gameBoard [position.row + i][position.column].symbol != WATER &&
			        (position.row + i) < ROWS)
				isValid = FALSE;
		}
	}

	return isValid;
}


Boolean convertStringtoPosition (Coordinate position[], char *stringPosition, int length) {
	Boolean flag = TRUE;
	char temp = '\0';
	int i = 0, j = 0, k = 1;

	/* checks if length of input is good */
	if (strlen (stringPosition)/2 == length) {
		/* loops through the length of the ship */
		for (i = 0; i < length && flag; i++) {
			/* checks if each cell is a digit */
			if (isdigit (stringPosition[j]) && isdigit (stringPosition[k])) {
				position[i].row    = stringPosition[j] - '0';
				position[i].column = stringPosition[k] - '0';

				j += 2;
				k += 2;
			} else {
				flag = FALSE;
			}
		}
	} else {
		flag = FALSE;
	}

	return flag;
}


Boolean isWinner (Stats players[], int player) {
	Boolean isWin = FALSE;

	if (players[player].numHits == 20)
		isWin = TRUE;

	return isWin;
}


Coordinate generatePosition (int direction, int length) {
	Coordinate position;

	if (direction == HORIZONTAL) {
		position.row    = getRandomNumber (0, ROWS);
		position.column = getRandomNumber (0, COLS - length);
	} else { /* VERTICAL */
		position.row    = getRandomNumber (0, ROWS - length);
		position.column = getRandomNumber (0, COLS);
	}

	return position;
}


Coordinate getTarget (void) {
	Coordinate target;

	fflush (stdin);

	printf ("> Enter Target (ex. > 3 4):\n");
	printf ("> ");
	scanf ("%d %d", &target.row, &target.column);

	return target;
}


short checkShot (Cell gameBoard[][COLS], Coordinate target) {
	int hit = -2;

	switch (gameBoard[target.row][target.column].symbol) {
		/* miss */
		case WATER:
			hit = 0;
			break;

		/* hit */
		case CARRIER:
		case BATTLESHIP:
		case BOAT:
		case DESTROYER:
			hit = 1;
			break;

		case HIT:
		case MISS:
		default:
			hit = -1;
			break;
	}

	return hit;
}


int getRandomNumber (int lowest, int highest) {
	if (lowest == 0)
		return rand () % ++highest;

	if (lowest > 0)
		return rand () % ++highest + lowest;
}

