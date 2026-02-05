#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"
#include <fstream>
#include "json.hpp"
#include "LoadTexture.h"

void ZeichneGleise() {
    for (const auto& gleis : gleisListe) {
        int ObjecktId = gleis.ObjecktId;
        float x = gleis.GridX * 50.0f;
        float y = gleis.GridY * 50.0f;
        int rotation = gleis.Rotation;

        //zeichnet geraden
        if (ObjecktId == 1) {
            switch (rotation) {
            case 0:
            case 180:
                DrawTexture("GleisVertikal", x, y, 50.0f, 50.0f);
                break;
            case 90:
            case 270:
                DrawTexture("GleisHorizontal", x, y, 50.0f, 50.0f);
                break;
            }
        }
        //kurve
        else if (ObjecktId == 2) {
            switch (rotation) {
            case 0:
                DrawTexture("GleisKurve1", x, y, 50.0f, 50.0f);
                break;
            case 90:
                DrawTexture("GleisKurve2", x, y, 50.0f, 50.0f);
                break;
            case 180:
                DrawTexture("GleisKurve3", x, y, 50.0f, 50.0f);
                break;
            case 270:
                DrawTexture("GleisKurve4", x, y, 50.0f, 50.0f);
                break;
            }
        }
        //kreuzung
        else if (ObjecktId == 3) {
            switch (rotation) {
            case 0:
                DrawTexture("GleisKreuzung1", x, y, 50.0f, 50.0f);
                break;
            case 90:
                DrawTexture("GleisKreuzung2", x, y, 50.0f, 50.0f);
                break;
            case 180:
                DrawTexture("GleisKreuzung3", x, y, 50.0f, 50.0f);
                break;
            case 270:
                DrawTexture("GleisKreuzung4", x, y, 50.0f, 50.0f);
                break;
            }
        }
        //kreuz
        else if (ObjecktId == 4) {
            DrawTexture("GleisKreuz", x, y, 50.0f, 50.0f);
            
        }
    }
}
void GleiseSpeichern() {
	nlohmann::json jsonDaten;

	nlohmann::json gleisArray = nlohmann::json::array();

	for (const auto& gleis : gleisListe) {
		nlohmann::json gleisJson;
		gleisJson["ObjecktId"] = gleis.ObjecktId;
		gleisJson["gridX"] = gleis.GridX;
		gleisJson["gridY"] = gleis.GridY;
		gleisJson["rotation"] = gleis.Rotation;

		gleisArray.push_back(gleisJson);
	}

	jsonDaten["Objeckte"] = gleisArray;

	std::ofstream datei("resurses/json/Gleise.json");
	if (datei.is_open()) {
		datei << jsonDaten.dump(4);
		datei.close();
	}
	else {
		//feler
	}
}

void verbindeSchienen() {
    if (gleisListe.empty()) {
        return;
    }

    for (auto& gleis : gleisListe) {
        int x = gleis.GridX;
        int y = gleis.GridY;

        // Nachbarrichtungen
        bool hatNachbarLinks = false;
        bool hatNachbarRechts = false;
        bool hatNachbarOben = false;
        bool hatNachbarUnten = false;

        
        for (const auto& andererGleis : gleisListe) {
            if (&gleis == &andererGleis) continue;

            int dx = andererGleis.GridX - x;
            int dy = andererGleis.GridY - y;

            
            if (dx == -1 && dy == 0) hatNachbarLinks = true;
            if (dx == 1 && dy == 0) hatNachbarRechts = true;
            if (dx == 0 && dy == -1) hatNachbarOben = true;
            if (dx == 0 && dy == 1) hatNachbarUnten = true;
        }

        // Nachbar zäler
        int anzahlNachbarn = 0;
        if (hatNachbarLinks) anzahlNachbarn++;
        if (hatNachbarRechts) anzahlNachbarn++;
        if (hatNachbarOben) anzahlNachbarn++;
        if (hatNachbarUnten) anzahlNachbarn++;

        
        if (anzahlNachbarn == 4) {
            // Plus-Kreuzung
            gleis.ObjecktId = 4;
            gleis.Rotation = 0; 
        }
        else if (anzahlNachbarn == 3) {
            // T-Kreuzungen (3-Wege)
            gleis.ObjecktId = 3;

            if (hatNachbarOben && hatNachbarUnten && hatNachbarRechts) {
                // 1: oben, unten, rechts
                gleis.Rotation = 0;
            }
            else if (hatNachbarLinks && hatNachbarOben && hatNachbarRechts) {
                // 2: links, oben, rechts
                gleis.Rotation = 90;
            }
            else if (hatNachbarOben && hatNachbarLinks && hatNachbarUnten) {
                // 3: oben, links, unten
                gleis.Rotation = 180;
            }
            else if (hatNachbarLinks && hatNachbarUnten && hatNachbarRechts) {
                // 4: links, unten, rechts
                gleis.Rotation = 270;
            }
        }
        else if (anzahlNachbarn == 2) {
            bool istGeradeLinie = (hatNachbarLinks && hatNachbarRechts) || (hatNachbarOben && hatNachbarUnten);

            if (!istGeradeLinie) {
                // Es ist eine Kurve (ObjecktId = 2)
                gleis.ObjecktId = 2;

                // Bestimme die richtige Rotation für die Kurve
                if (hatNachbarUnten && hatNachbarRechts) {
                    // 1: unten, rechts
                    gleis.Rotation = 0; // GleisKurve1
                }
                else if (hatNachbarOben && hatNachbarRechts) {
                    // 2: oben, rechts
                    gleis.Rotation = 90; // GleisKurve2
                }
                else if (hatNachbarLinks && hatNachbarUnten) {
                    // 3: links, unten
                    gleis.Rotation = 180; // GleisKurve3
                }
                else if (hatNachbarOben && hatNachbarLinks) {
                    // 4: oben, links
                    gleis.Rotation = 270; // GleisKurve4
                }
            }
            else {
                // Gerade Schiene
                gleis.ObjecktId = 1;

                if (hatNachbarLinks && hatNachbarRechts) {
                    // Horizontal
                    gleis.Rotation = 90;
                }
                else if (hatNachbarOben && hatNachbarUnten) {
                    // Vertikal
                    gleis.Rotation = 0;
                }
            }
        }
        else if (anzahlNachbarn == 1) {

            gleis.ObjecktId = 1;

            if (hatNachbarLinks || hatNachbarRechts) {
                // Horizontal
                gleis.Rotation = 90;
            }
            else if (hatNachbarOben || hatNachbarUnten) {
                // Vertikal
                gleis.Rotation = 0;
            }
        }
    }
}

/*-------------------------------------------------
    GLEISE SETZEN
-------------------------------------------------*/


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

