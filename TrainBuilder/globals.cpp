#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include "raylib.h"
#include "globals.h"


/*-------------------------------------------------
	GRUNDLAGEN
-------------------------------------------------*/
int GenaueHoehe = 0;
int GenaueBreite = 0;
const float GRID_SIZE = 50.0f;

/*-------------------------------------------------
	KARAKTER
-------------------------------------------------*/
Vector2 Spielerposition = { 0, 0 };
float DeltaTime = 0.0f;
Camera2D Playercam = { 0 };

float playerSpeed = 700.0f;

int aktuellesTool = 1; 
//1 = zeichnen
//2 = löschen
//3 = auswahl

/*-------------------------------------------------
	Mausposition
-------------------------------------------------*/
Vector2 mousePosition = { 0, 0 };
Vector2 gridPosition = { 0, 0 };

/*-------------------------------------------------
	MOUSEKLICK/SCHIENEN PLAZIEREN
-------------------------------------------------*/

Vector2 ErsteKlickPosition = { 0, 0 };
Vector2 ZweiteKlickPosition = { 0, 0 };
bool haterstenKlick = false;


/*-------------------------------------------------
	GLEISE
-------------------------------------------------*/
std::vector<GleisObjeckt> gleisListe;

/*-------------------------------------------------
	Banhöfe
-------------------------------------------------*/
std::vector<BanhofObjeckt> banhofListe;