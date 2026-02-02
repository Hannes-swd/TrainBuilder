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


void ProcesMaus(Vector2 mausposition) {
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);



    //BEI LINKSKLICK
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LinksGeklickt(mausposition);
    }



    if (haterstenKlick) {


        ZeichnePriviou(mausposition);
    }
}

void LinksGeklickt(Vector2 mausposition) {
    int KlickGridX = (int)floor(mausposition.x / GRID_SIZE);
    int KlickGridY = (int)floor(mausposition.y / GRID_SIZE);

    if (!haterstenKlick) {
        ErsteKlickPosition = { (float)KlickGridX ,(float)KlickGridY };
        haterstenKlick = true;
    }
    else if (haterstenKlick) {
        ZweiteKlickPosition = { (float)KlickGridX ,(float)KlickGridY };
        haterstenKlick = false;
    }
}

void ZeichnePriviou(Vector2 mausposition) {
    int MouseGridX = (int)floor(mausposition.x / GRID_SIZE);
    int MouseGridY = (int)floor(mausposition.y / GRID_SIZE);

    int ersteGridX = (int)ErsteKlickPosition.x;
    int ersteGridY = (int)ErsteKlickPosition.y;


    int diffX = MouseGridX - ersteGridX;
    int diffY = MouseGridY - ersteGridY;


    bool isHorizontal = (diffY == 0 && diffX != 0);
    bool isVertical = (diffX == 0 && diffY != 0);

    if (isHorizontal || isVertical) {
        // Horizontal
        if (isHorizontal) {
            int startX = std::min(MouseGridX, ersteGridX);
            int endX = std::max(MouseGridX, ersteGridX);

            for (int x = startX; x <= endX; x++) {
                float pixelX = x * GRID_SIZE;
                float pixelY = ersteGridY * GRID_SIZE;
                DrawRectangle(pixelX, pixelY, GRID_SIZE, GRID_SIZE, Color{ 255, 0, 0, 150 });
            }
        }
        // Vertikal
        else if (isVertical) {
            int startY = std::min(MouseGridY, ersteGridY);
            int endY = std::max(MouseGridY, ersteGridY);

            for (int y = startY; y <= endY; y++) {
                float pixelX = ersteGridX * GRID_SIZE;
                float pixelY = y * GRID_SIZE;
                DrawRectangle(pixelX, pixelY, GRID_SIZE, GRID_SIZE, Color{ 255, 0, 0, 150 });
            }
        }
    }
}