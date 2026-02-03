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


	//seitenmenü
	if (ausgewahlterBanhof != 0) {

		//zeichnet breite seitenleiste
		DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
		DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);

		//banhof informationen
		for (const auto& banhof : banhofListe) {
			if (banhof.BanhofId == ausgewahlterBanhof) {
				DrawText("Banhof Info:", GenaueBreite - 240, 90, 20, BLACK);
				DrawText(TextFormat("Name: %s", banhof.Name.c_str()), GenaueBreite - 240, 120, 20, BLACK);
				DrawText(TextFormat("ID: %d", banhof.BanhofId), GenaueBreite - 240, 150, 20, BLACK);
				DrawText(TextFormat("Position: [%d, %d]", banhof.GridX, banhof.GridY), GenaueBreite - 240, 180, 20, BLACK);
				DrawText(TextFormat("Rotation: %d", banhof.Rotation), GenaueBreite - 240, 210, 20, BLACK);
				break;
			}
		}
	}

}