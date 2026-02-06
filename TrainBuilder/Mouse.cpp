#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <cmath>
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "gleise.h"
#include "Mouse.h"
#include "Banhof.h"
#include "untermenü.h"

void ProcesMaus(Vector2 mausposition) {
    Vector2 screenMousePos = GetMousePosition();
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

    if (screenMousePos.y < 80.0f) {
        Menuebuttons();
    }

    if (ausgewahlterBanhof != 0) {
        if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return;
            }
        }
    }

	//Untermenü für Zug tool
    if (aktuellesTool == 5) {
		untermenueOffen = true;
	}
    else {
        if (untermenueOffen) {
            untermenueOffen = false;
		}
    }

    if (untermenueOffen) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			//bricht ab wen es im untermenü geklickt wird
            if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
                UntermenueKlick(mausposition);
                return;
			}
		}
	}

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LinksGeklickt(mausposition);
    }

    if (aktuellesTool == 1) {
        if (haterstenKlick) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                haterstenKlick = false;
                return;
            }
            ZeichnePriviou(mausposition);
        }
    }

    if (aktuellesTool == 2) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Loeschentool(mausposition);
        }
    }

    if (aktuellesTool == 3) {
        Auswahltool(MouseGridX, MouseGridY);
    }

    if (aktuellesTool == 4) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            plaziereBanhof(mausposition);
    }
    if (aktuellesTool == 5) {
		aktuellesUntermenue = "zugtool";
	}
    else {
        aktuellesUntermenue = "";
    }
}

/*-------------------------------------------------
    AUSWAHLTOOL 
-------------------------------------------------*/
void Auswahltool(int gridX, int gridY)
{
    bool bahnhofGeklickt = false;

    for (const auto& ban : banhofListe) {
        if (ban.GridX == gridX && ban.GridY == gridY) {
            DrawRectangle(gridX * GRID_SIZE, gridY * GRID_SIZE, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ausgewahlterBanhof = ban.BanhofId;
                bahnhofGeklickt = true;
            }
        }
    }

    // Fenster schliesen
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !bahnhofGeklickt) {
        ausgewahlterBanhof = 0;
    }
}
/*-------------------------------------------------
	Gleise setzen tool
-------------------------------------------------*/
void LinksGeklickt(Vector2 mausposition) {
    Vector2 screenMousePos = GetMousePosition();
    int KlickGridX = (int)floor(mausposition.x / GRID_SIZE);
    int KlickGridY = (int)floor(mausposition.y / GRID_SIZE);

    if (aktuellesTool == 1) {
        if (screenMousePos.y < 80.0f) {
            return;
        }

        if (!haterstenKlick) {
            ErsteKlickPosition = { (float)KlickGridX ,(float)KlickGridY };
            haterstenKlick = true;
        }
        else if (haterstenKlick) {
            ZweiteKlickPosition = { (float)KlickGridX ,(float)KlickGridY };

            if ((int)ErsteKlickPosition.x == (int)ZweiteKlickPosition.x &&
                (int)ErsteKlickPosition.y == (int)ZweiteKlickPosition.y) {
                PlatziereEinzelneSchiene(KlickGridX, KlickGridY);
            }
            else {
                PlatziereSchienenZwischenPunkten(
                    { (float)(int)ErsteKlickPosition.x, (float)(int)ErsteKlickPosition.y },
                    { (float)(int)ZweiteKlickPosition.x, (float)(int)ZweiteKlickPosition.y }
                );
            }

            haterstenKlick = false;
        }
    }
}
/*-------------------------------------------------
    LÖSCHTOOL
-------------------------------------------------*/
void Loeschentool(Vector2 mausposition) {
    // Lösct als erstes banhof und wen nicht dan gleis
    if (IstBanhofBereitsVorhanden(
        (int)floor(mausposition.x / GRID_SIZE),
        (int)floor(mausposition.y / GRID_SIZE))) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Löscht banhof
            int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
            int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

            auto it = std::remove_if(banhofListe.begin(), banhofListe.end(),
                [MouseGridX, MouseGridY](const BanhofObjeckt& banhof) {
                    return banhof.GridX == MouseGridX && banhof.GridY == MouseGridY;
                });

            if (it != banhofListe.end()) {
                banhofListe.erase(it, banhofListe.end());
                BanhofSpeichern();
            }
            return;
        }
    }
    else {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 screenMousePos = GetMousePosition();
            int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
            int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

            auto it = std::remove_if(gleisListe.begin(), gleisListe.end(),
                [MouseGridX, MouseGridY](const GleisObjeckt& gleis) {
                    return gleis.GridX == MouseGridX && gleis.GridY == MouseGridY;
                });

            if (it != gleisListe.end()) {
                gleisListe.erase(it, gleisListe.end());
                GleiseSpeichern();
            }
        }
    }
}

/*-------------------------------------------------
    TOOLSWITCHEN
-------------------------------------------------*/
void Menuebuttons() {
    Vector2 mousePos = GetMousePosition();

    // Zeichen tool
    Rectangle zeichenButton = { 10.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, zeichenButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 1;
        }
    }

    // Löschen tool
    Rectangle loeschenButton = { 80.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, loeschenButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 2;
        }
    }

    // Auswahl tool
    Rectangle auswahlButton = { 150.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, auswahlButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 3;
        }
    }

    // Banhof tool
    Rectangle banhofButton = { 220.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, banhofButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 4;
        }
    }

	// Zug tool
    Rectangle zugButton = { 290.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, zugButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 5;
        }
	}
}