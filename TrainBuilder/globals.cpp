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
int GenaueHöehe = 0;
int GenaueBreite = 0;

/*-------------------------------------------------
	KARAKTER
-------------------------------------------------*/
Vector2 Spielerposition = { 0, 0 };
float DeltaTime = 0.0f;
Camera2D Playercam = { 0 };

float playerSpeed = 300.0f;

/*-------------------------------------------------
	Mausposition
-------------------------------------------------*/
Vector2 mousePosition = { 0, 0 };
Vector2 gridPosition = { 0, 0 };

/*-------------------------------------------------
	GLEISE
-------------------------------------------------*/
std::vector<GleisObjeckt> gleisListe;