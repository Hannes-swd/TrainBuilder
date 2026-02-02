#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "gleise.h"
#include "Mouse.h"


void ProcesMaus(Vector2 mausposition) {
    //ORT HERAUSFINDEN MAUSPOSITION
    int MouseGridX = (int)(mausposition.x / 50.0f);
    int MouseGridY = (int)(mausposition.y / 50.0f);

    

    //BEI LINKSKLICK
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LinksGeklickt(mausposition);
    }
    
    

    if (haterstenKlick) {
        // Aktuelle Mausposition mit ErsterKlickPosition vergleichen
        bool horizontal = (MouseGridY == ErsteKlickPosition.y && MouseGridX != ErsteKlickPosition.x);
        bool vertical = (MouseGridX == ErsteKlickPosition.x && MouseGridY != ErsteKlickPosition.y);

        // Wenn horizontal oder vertikal, zeige Vorschau an
        if (horizontal || vertical) {
            // Hier die Vorschau zeichnen (z.B. Linie oder "X")
            DrawText("X", 50, 50, 50, RED);
        }
    }
    /*GleisObjeckt neuesGleis;
    neuesGleis.ObjecktId = 1;
    neuesGleis.GridX = gridX;
    neuesGleis.GridY = gridY;
    neuesGleis.Rotation = 0;

    gleisListe.push_back(neuesGleis);
    GleiseSpeichern();*/
}
void LinksGeklickt(Vector2 mausposition) {
    int KlickGridX = (int)(mausposition.x / 50.0f);
    int KlickGridY = (int)(mausposition.y / 50.0f);

    if (!haterstenKlick) {
        ErsteKlickPosition = { (float)KlickGridX ,(float)KlickGridY };
        haterstenKlick = true;
    }
    else if (haterstenKlick) {
        ZweiteKlickPosition = { (float)KlickGridX ,(float)KlickGridY };
        //zurücksetzen
        haterstenKlick = false;
    }
}
