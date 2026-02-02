#pragma once
#include "raylib.h"
#include <vector>

/*-------------------------------------------------
	GRUNDLAGEN
-------------------------------------------------*/
extern int GenaueHöehe;
extern int GenaueBreite;

/*-------------------------------------------------
	KARAKTER
-------------------------------------------------*/
extern Vector2 Spielerposition;
extern float DeltaTime;
extern Camera2D Playercam;
extern float playerSpeed;


extern Vector2 mousePosition;
extern Vector2 gridPosition;

/*-------------------------------------------------
	GLEISE
-------------------------------------------------*/

struct GleisObjeckt {
	int ObjecktId;
	int GridX;
	int GridY;
	int Rotation;
};

extern std::vector<GleisObjeckt> gleisListe;