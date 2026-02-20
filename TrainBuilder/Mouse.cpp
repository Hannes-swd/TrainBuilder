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
#include "Zug.h"
#include "untermenü.h"
#include "Ampel.h"
#include "knoten.h"
#include "Leiter.h"



void ProcesMaus(Vector2 mausposition) {
    Vector2 screenMousePos = GetMousePosition();
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

    static int lastTool = -1;
    if (lastTool != aktuellesTool) {
        lastTool = aktuellesTool;
    }

    if (screenMousePos.y < 80.0f) {
        Menuebuttons();
        return;
    }

    if (ausgewahlterBanhof != 0) {
        if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return;
            }
        }
    }
    if (ausgewahlterZug != 0) {
        if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return;
            }
        }
    }
    if (ausgewahlterAmpel != 0) {
        if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return;
            }
        }
    }

    if (ausgewahlterKnoten != 0) {
        if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return;
            }
        }
    }


    if (untermenueOffen) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 screenMousePos = GetMousePosition();

            if (screenMousePos.y >= 80.0f && screenMousePos.y <= 180.0f) {
                UntermenueKlick(screenMousePos);
                return;
            }
        }
    }

    if (aktuellesTool == 5) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            std::cout << "DEBUG: CLICK ERKANNT! ausgewahlterZugArt = " << ausgewahlterZugArt << std::endl;
            zugPlazieren(MouseGridX, MouseGridY, ausgewahlterZugArt);
            return;
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
    if (aktuellesTool == 6 && ausgewählterUntermenuePunkt == 1) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            KnotenPlazieren(MouseGridX, MouseGridY);
    }
    if (aktuellesTool == 6 && ausgewählterUntermenuePunkt == 2) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            LeiterPlazieren(MouseGridX, MouseGridY);
    }


}

/*-------------------------------------------------
    AUSWAHLTOOL
-------------------------------------------------*/
void Auswahltool(int gridX, int gridY)
{
    bool bahnhofGeklickt = false;
    bool zugGeklickt = false;
    bool ampelGeklickt = false;
    bool knotenGeklickt = false;
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), Playercam);

    for (const auto& ban : banhofListe) {
        if (ban.GridX == gridX && ban.GridY == gridY) {
            DrawRectangle(gridX * GRID_SIZE, gridY * GRID_SIZE, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ausgewahlterBanhof = ban.BanhofId;
                bahnhofGeklickt = true;
            }
        }
    }
    for (auto& z : aktiveZuege) {
        Rectangle zugBox = {
            z.posX * GRID_SIZE,
            z.posY * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE
        };

        // Kollision mit Mausposition prfen
        if (CheckCollisionPointRec(mouseWorldPos, zugBox)) {
            // Visualisierung des ausgewhlten Zuges
            DrawRectangleRec(zugBox, Color{ 0, 255, 0, 150 });

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ausgewahlterZug = z.zugId;
                zugGeklickt = true;
                std::cout << "Zug ausgewhlt: ID " << z.zugId
                    << " an Position (" << z.posX << ", " << z.posY << ")" << std::endl;
            }
        }
    }
    for (const auto& ampel : ampelListe) {
        if (ampel.GridX == gridX && ampel.GridY == gridY) {
            DrawRectangle(gridX * GRID_SIZE, gridY * GRID_SIZE, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ausgewahlterAmpel = ampel.AmpelId;
                ampelGeklickt = true;
            }
        }
    }
    for (const auto& knoten : knotenliste) {
        if (knoten.GridX == gridX && knoten.GridY == gridY) {
            DrawRectangle(gridX * GRID_SIZE, gridY * GRID_SIZE, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ausgewahlterKnoten = knoten.eindeutigeId;
                knotenGeklickt = true;
            }
        }
    }

    // Fenster schliesen
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !bahnhofGeklickt) {
        ausgewahlterBanhof = 0;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !zugGeklickt) {
        ausgewahlterZug = 0;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ampelGeklickt) {
        ausgewahlterAmpel = 0;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !knotenGeklickt) {
        ausgewahlterKnoten = 0;
    }
}
/*-------------------------------------------------
    Gleise setzen tool
-------------------------------------------------*/
void LinksGeklickt(Vector2 mausposition) {
    Vector2 screenMousePos = GetMousePosition();
    int KlickGridX = (int)floor(mausposition.x / GRID_SIZE);
    int KlickGridY = (int)floor(mausposition.y / GRID_SIZE);
    //BEI GLEISE
    if (aktuellesTool == 1 && untermenueOffen && ausgewählterUntermenuePunkt == 1) {
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
    //AMPEL
    else if (aktuellesTool == 1 && untermenueOffen && ausgewählterUntermenuePunkt == 2) {
        if (screenMousePos.y < 80.0f) {
            return;
        }
        AmpelPlazieren(KlickGridX, KlickGridY);
        std::cout << "Ampel plaziert an [" << KlickGridX << ", " << KlickGridY << "]" << std::endl;
    }
}
/*-------------------------------------------------
    LÖSCHTOOL
-------------------------------------------------*/
void Loeschentool(Vector2 mausposition) {
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

    // Bahnhöfe
    if (IstBanhofBereitsVorhanden(MouseGridX, MouseGridY)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

            std::string geloeschterBanhofName;
            for (const auto& banhof : banhofListe) {
                if (banhof.GridX == MouseGridX && banhof.GridY == MouseGridY) {
                    geloeschterBanhofName = banhof.Name;
                    break;
                }
            }

            auto it = std::remove_if(banhofListe.begin(), banhofListe.end(),
                [MouseGridX, MouseGridY](const BanhofObjeckt& banhof) {
                    return banhof.GridX == MouseGridX && banhof.GridY == MouseGridY;
                });

            if (it != banhofListe.end()) {
                banhofListe.erase(it, banhofListe.end());

                if (!geloeschterBanhofName.empty()) {
                    for (auto& zug : aktiveZuege) {
                        auto& fahrplan = zug.Fahrplan;
                        fahrplan.erase(
                            std::remove(fahrplan.begin(), fahrplan.end(), geloeschterBanhofName),
                            fahrplan.end()
                        );
                    }
                    AktiveZuegeSpeichern();
                }

                BanhofSpeichern();
            }
            return;
        }
    }
    // Knoten
    else if (istKnotenVorhanden(MouseGridX, MouseGridY)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            knotenlöschen(MouseGridX, MouseGridY);
            return;
        }
    }
    // Leiter
    else if (IstLeiterVorhanden(MouseGridX, MouseGridY)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            LeiterLöschen(MouseGridX, MouseGridY);
            return;
        }
    }
    // Gleise
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


    // Löschen tool
    Rectangle loeschenButton = { 10.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, loeschenButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 2;
            untermenueOffen = false;
            aktuellesUntermenue = "";
        }
    }

    // Auswahl tool
    Rectangle auswahlButton = { 80.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, auswahlButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 3;
            untermenueOffen = false;
            aktuellesUntermenue = "";
        }
    }

    //Drehen
    Rectangle DrehenButton = { 150.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, DrehenButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 7;
            untermenueOffen = false;
            aktuellesUntermenue = "";
        }
    }

    // Zeichen tool
    Rectangle zeichenButton = { 220.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, zeichenButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 1;
            untermenueOffen = true;
            aktuellesUntermenue = "gleistool";
        }
    }

    // Banhof tool
    Rectangle banhofButton = { 290.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, banhofButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 4;
            untermenueOffen = false;
            aktuellesUntermenue = "";
        }
    }

    // Zug tool
    Rectangle zugButton = { 360.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, zugButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 5;
            untermenueOffen = true;
            aktuellesUntermenue = "zugtool";
        }
    }

    //technik
    Rectangle technikButton = { 430.0f, 10.0f, 60.0f, 60.0f };
    if (CheckCollisionPointRec(mousePos, technikButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            aktuellesTool = 6;
            untermenueOffen = true;
            aktuellesUntermenue = "technik";
        }
    }
}