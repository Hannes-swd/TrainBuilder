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

void ProcesMaus(Vector2 mausposition) {
    Vector2 screenMousePos = GetMousePosition();
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

    // Im menü
    if (screenMousePos.y < 80.0f) {
        Menuebuttons();
    }

    // Wenn man im seitenmenü klickt nicht abbrechen
    if(aktuellesTool != 0)
        if (screenMousePos.x > GenaueBreite - 250.0f && screenMousePos.y > 80.0f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return;
            }
        }
    else {
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
    GLEISE SETZEN
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

bool IstGleisBereitsVorhanden(int gridX, int gridY) {
    for (const auto& gleis : gleisListe) {
        if (gleis.GridX == gridX && gleis.GridY == gridY) {
            return true;
        }
    }
    return false;
}

void ZeichnePriviou(Vector2 mausposition) {
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

    int ersteGridX = (int)ErsteKlickPosition.x;
    int ersteGridY = (int)ErsteKlickPosition.y;

    if (MouseGridX == ersteGridX && MouseGridY == ersteGridY) {
        float pixelX = (float)(ersteGridX * GRID_SIZE);
        float pixelY = (float)(ersteGridY * GRID_SIZE);

        if (IstGleisBereitsVorhanden(ersteGridX, ersteGridY)) {
            DrawRectangle(pixelX, pixelY, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 255, 0, 0, 150 });
        }
        else {
            DrawRectangle(pixelX, pixelY, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });
        }
        return;
    }

    int diffX = MouseGridX - ersteGridX;
    int diffY = MouseGridY - ersteGridY;

    bool isHorizontal = (diffY == 0 && diffX != 0);
    bool isVertical = (diffX == 0 && diffY != 0);

    if (isHorizontal || isVertical) {
        if (isHorizontal) {
            int startX = std::min(MouseGridX, ersteGridX);
            int endX = std::max(MouseGridX, ersteGridX);

            for (int x = startX; x <= endX; x++) {
                float pixelX = (float)(x * GRID_SIZE);
                float pixelY = (float)(ersteGridY * GRID_SIZE);

                if (IstGleisBereitsVorhanden(x, ersteGridY)) {
                    DrawRectangle(pixelX, pixelY, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 255, 0, 0, 150 });
                }
                else {
                    DrawRectangle(pixelX, pixelY, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });
                }
            }
        }
        else if (isVertical) {
            int startY = std::min(MouseGridY, ersteGridY);
            int endY = std::max(MouseGridY, ersteGridY);

            for (int y = startY; y <= endY; y++) {
                float pixelX = (float)(ersteGridX * GRID_SIZE);
                float pixelY = (float)(y * GRID_SIZE);

                if (IstGleisBereitsVorhanden(ersteGridX, y)) {
                    DrawRectangle(pixelX, pixelY, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 255, 0, 0, 150 });
                }
                else {
                    DrawRectangle(pixelX, pixelY, (float)GRID_SIZE, (float)GRID_SIZE, Color{ 0, 255, 0, 150 });
                }
            }
        }
    }
}

void PlatziereEinzelneSchiene(int gridX, int gridY) {
    if (IstGleisBereitsVorhanden(gridX, gridY)) {
        return;
    }

    GleisObjeckt neuesGleis;
    neuesGleis.ObjecktId = 1;
    neuesGleis.GridX = gridX;
    neuesGleis.GridY = gridY;
    neuesGleis.Rotation = 90;

    gleisListe.push_back(neuesGleis);

    GleiseSpeichern();
}

void PlatziereSchienenZwischenPunkten(Vector2 start, Vector2 end) {
    int startX = (int)start.x;
    int startY = (int)start.y;
    int endX = (int)end.x;
    int endY = (int)end.y;

    bool isHorizontal = (startY == endY && startX != endX);
    bool isVertical = (startX == endX && startY != endY);

    if (!isHorizontal && !isVertical) {
        return;
    }

    bool allePositionenFrei = true;

    if (isHorizontal) {
        int step = (startX < endX) ? 1 : -1;
        for (int x = startX; x != endX + step; x += step) {
            if (IstGleisBereitsVorhanden(x, startY)) {
                allePositionenFrei = false;
                break;
            }
        }
    }
    else if (isVertical) {
        int step = (startY < endY) ? 1 : -1;
        for (int y = startY; y != endY + step; y += step) {
            if (IstGleisBereitsVorhanden(startX, y)) {
                allePositionenFrei = false;
                break;
            }
        }
    }

    if (!allePositionenFrei) {
        return;
    }

    if (isHorizontal) {
        int step = (startX < endX) ? 1 : -1;

        for (int x = startX; x != endX + step; x += step) {
            GleisObjeckt neuesGleis;
            neuesGleis.ObjecktId = 1;
            neuesGleis.GridX = x;
            neuesGleis.GridY = startY;
            neuesGleis.Rotation = 90;
            gleisListe.push_back(neuesGleis);
        }
    }
    else if (isVertical) {
        int step = (startY < endY) ? 1 : -1;

        for (int y = startY; y != endY + step; y += step) {
            GleisObjeckt neuesGleis;
            neuesGleis.ObjecktId = 1;
            neuesGleis.GridX = startX;
            neuesGleis.GridY = y;
            neuesGleis.Rotation = 0;
            gleisListe.push_back(neuesGleis);
        }
    }

    GleiseSpeichern();
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
}