#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define ROWS 10
#define COLS 10

#define LOG_FILE_NAME "battleship.log"

#define NUM_OF_SHIPS  8

#define HORIZONTAL    0
#define VERTICAL      1

#define PLAYER_ONE    0
#define PLAYER_TWO    1


#define WATER         '~'
#define HIT           '*'
#define MISS          'm'

#define CARRIER       'c'
#define BATTLESHIP    'b'
#define BOAT          'a'
#define DESTROYER     'd'

#define NORTH 0
#define SOUTH 1
#define WEST  2
#define EAST  3

/* ENUMERATED TYPES */

typedef enum {
	FALSE, TRUE
} Boolean;

typedef enum {
	CARRIER_L    = 4, 
	BATTLESHIP_L = 2,
	BOAT_L    = 1, 
	DESTROYER_L  = 3
} ShipType;

typedef struct {
    char username[30];
    int wins;
    int losses;
    int incomplete;
} UserStats;


/* STRUCT TYPES */

typedef struct watercraft {
	char   symbol;
	short  length;
	char   *name;
} WaterCraft;

typedef struct stats {
	int        numHits;
	int        numMisses;
	int        totalShots;
	double     hitMissRatio;
} Stats;

typedef struct coordinate {
	int        row;
	int        column;
} Coordinate;

typedef struct cell {
	char       symbol; 
	Coordinate position;
} Cell;

/* FUNCTION PROTOTYPES */

void         firstScreen                    (void);
void         initializeGameBoard            (Cell gameBoard[][COLS]);
void         printGameBoard                 (Cell gameBoard [][COLS], Boolean showPegs);
void         putShipOnGameBoard             (Cell gameBoard[][COLS], WaterCraft ship, Coordinate position, int direction);
void         manuallyPlaceShipsOnGameBoard  (Cell gameBoard[][COLS], WaterCraft ship[]);
void         randomlyPlaceShipsOnGameBoard  (Cell gameBoard[][COLS], WaterCraft ship[]);
void         updateGameBoard                (Cell gameBoard[][COLS], Coordinate target);
void         checkBoundsOfCardinal          (Boolean cardinals[], int bound, int direction);
void         systemMessage                  (char *message);
void 		 saveGame						(Cell playerOneGameBoard[][COLS], Cell playerTwoGameBoard[][COLS], Stats playerOneStats, Stats playerTwoStats, int currentPlayer,const char *playerOneName, const char *playerTwoName);
void 		 listSavedGames					(void);
void 		 selectAndLoadGame				(void);
void 		 loadGame						(const char *filename);
void 		 saveGameResult					(const char *playerOneName, const char *playerTwoName, const char *winner);
void 		 displayGameResults				(void);
void 		 updateResults					(const char *playerName, const char *result);
void 		 displayScoreboard				(void);

Boolean      checkSunkShip                  (short sunkShip[][NUM_OF_SHIPS], short player, char shipSymbol, FILE *stream);
Boolean      isValidLocation                (Cell gameBoard[][COLS], Coordinate position, int direction, int length);
Boolean      convertStringtoPosition        (Coordinate position[], char *stringPosition, int length);
Boolean      isWinner                       (Stats players[], int player);
Boolean      isStraightLine                 (Coordinate position[], int length);
Boolean 	 areAllPositionsValid			(Cell gameBoard[][COLS], Coordinate position[], int length);
Boolean 	 isCellWater					(Cell gameBoard[][COLS], int row, int col);
Boolean 	 isSurroundingWater				(Cell gameBoard[][COLS], int row, int col);
Coordinate   generatePosition               (int direction, int length);
Coordinate   getTarget                      (void);

short        checkShot                      (Cell gameBoard[][COLS], Coordinate target);
int 		 askForPlayerAgreement			(int currentPlayer);
int          getRandomNumber                (int lowest, int highest);

#endif