#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"

void zeichneUI() {
	DrawRectangle(0, 0, (float)GenaueBreite, 80.0f, LIGHTGRAY);
	
	//hervorheben des aktiven tools
	if (aktuellesTool == 1) {
		DrawRectangle(10.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
	}
    else if (aktuellesTool == 2) {
        DrawRectangle(80.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
    else if (aktuellesTool == 3) {
        DrawRectangle(150.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
	else if (aktuellesTool == 4) {
		DrawRectangle(220.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
	}

	//zeichnet tools nebeneinander
	DrawTexture("zeichnen", 10.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Löschen", 80.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Auswahl", 150.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Banhof", 220.0f, 10.0f, 60.0f, 60.0f);

}