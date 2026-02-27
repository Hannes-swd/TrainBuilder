#include <map>
#include <queue>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "untermenü.h"
#include "json.hpp"
#include "Marker.h"
#include "PlatzierungCheck.h"


using json = nlohmann::json;

void MarkerPlazieren(int gridX, int gridY) {
    if (!KannPlatzieren(gridX, gridY, PlatzierTyp::Marker)) return;

	MarkerObjeckt newMarker;
	newMarker.GridX = gridX;
	newMarker.GridY = gridY;
	newMarker.farbe = RED;
	int neueID = 1;
	for (const auto& inv : MarkerListe) {
		if (inv.eindeutigeId >= neueID) {
			neueID = inv.eindeutigeId + 1;
		}
	}
	newMarker.eindeutigeId = neueID;
	MarkerListe.push_back(newMarker);
	MarkerSpeichern();
}

void MarkerSpeichern() {
	json jsonDaten;
	json MarkerArray = json::array();
	for (const auto& Marker : MarkerListe) {
		json MarkerJson;
		MarkerJson["GridX"] = Marker.GridX;
		MarkerJson["GridY"] = Marker.GridY;
		MarkerJson["eindeutigeId"] = Marker.eindeutigeId;

		MarkerJson["farbe_r"] = Marker.farbe.r;
		MarkerJson["farbe_g"] = Marker.farbe.g;
		MarkerJson["farbe_b"] = Marker.farbe.b;

		MarkerArray.push_back(MarkerJson);
	}
	jsonDaten["marker"] = MarkerArray;
	std::ofstream MarkerDatei("resurses/json/Marker.json");
	if (MarkerDatei.is_open()) {
		MarkerDatei << jsonDaten.dump(4);
		MarkerDatei.close();
	}
}

void MarkerZeichnen() {
	for (const auto& Marker : MarkerListe) {
		float centerX = Marker.GridX * GRID_SIZE + GRID_SIZE / 2;
		float centerY = Marker.GridY * GRID_SIZE + GRID_SIZE / 2;
		float halfSize = GRID_SIZE / 4;

		//farbe
		Color farbe = Marker.farbe;

		DrawRectangle(
			centerX - halfSize,
			centerY - halfSize,
			GRID_SIZE / 2,
			GRID_SIZE / 2,
			farbe
		);
	}
}
bool MarkerInSichtfeld(int markerId) {
    MarkerObjeckt* gesuchterMarker = nullptr;
    for (auto& marker : MarkerListe) {
        if (marker.eindeutigeId == markerId) {
            gesuchterMarker = &marker;
            break;
        }
    }

    if (gesuchterMarker == nullptr) {
        return false;
    }

    float viewportBreite = (float)GenaueBreite / Playercam.zoom;
    float viewportHoehe = (float)GenaueHoehe / Playercam.zoom;

    float linkeKante = Playercam.target.x - viewportBreite / 2;
    float rechteKante = Playercam.target.x + viewportBreite / 2;
    float obereKante = Playercam.target.y - viewportHoehe / 2;
    float untereKante = Playercam.target.y + viewportHoehe / 2;

    float markerWeltX = gesuchterMarker->GridX * GRID_SIZE + GRID_SIZE / 2;
    float markerWeltY = gesuchterMarker->GridY * GRID_SIZE + GRID_SIZE / 2;

    bool imSichtfeld = (markerWeltX >= linkeKante &&
        markerWeltX <= rechteKante &&
        markerWeltY >= obereKante &&
        markerWeltY <= untereKante);

    return imSichtfeld;
}
void NichtImFeldZeichnen() {
    float obererRand = 80.0f;
    float untererRand = (float)GenaueHoehe;
    float linkerRand = 0.0f;
    float rechterRand = (float)GenaueBreite;

    float mitteX = GenaueBreite / 2.0f;
    float mitteY = GenaueHoehe / 2.0f;

    float kastenGroesse = 40.0f;
    float randAbstand = kastenGroesse / 2 + 5.0f;

    for (const auto& marker : MarkerListe) {
        if (MarkerInSichtfeld(marker.eindeutigeId)) {
            continue;
        }

        float markerWeltX = marker.GridX * GRID_SIZE + GRID_SIZE / 2;
        float markerWeltY = marker.GridY * GRID_SIZE + GRID_SIZE / 2;

        Vector2 markerBildschirm = GetWorldToScreen2D(
            Vector2{ markerWeltX, markerWeltY },
            Playercam
        );

        float dx = markerBildschirm.x - mitteX;
        float dy = markerBildschirm.y - mitteY;

        if (dx == 0 && dy == 0) continue;

        float randX = mitteX;
        float randY = mitteY;

        float links = linkerRand + randAbstand;
        float rechts = rechterRand - randAbstand;
        float oben = obererRand + randAbstand;
        float unten = untererRand - randAbstand;

        float t = 1e9f;

        if (dx > 0) t = std::min(t, (rechts - mitteX) / dx);
        if (dx < 0) t = std::min(t, (links - mitteX) / dx);
        if (dy > 0) t = std::min(t, (unten - mitteY) / dy);
        if (dy < 0) t = std::min(t, (oben - mitteY) / dy);

        randX = mitteX + t * dx;
        randY = mitteY + t * dy;

        randX = std::max(links, std::min(rechts, randX));
        randY = std::max(oben, std::min(unten, randY));

        DrawRectangle(
            randX - kastenGroesse / 2, randY - kastenGroesse / 2,
            kastenGroesse, kastenGroesse,
            Color{ marker.farbe.r, marker.farbe.g, marker.farbe.b, 180 }
        );

        DrawRectangleLines(
            randX - kastenGroesse / 2, randY - kastenGroesse / 2,
            kastenGroesse, kastenGroesse,
            WHITE
        );
    }
}
void MarkerLöschen(int MouseGridX, int MouseGridY) {
    auto it = std::remove_if(MarkerListe.begin(), MarkerListe.end(),
        [MouseGridX, MouseGridY](const MarkerObjeckt& l) {
            return l.GridX == MouseGridX && l.GridY == MouseGridY;
        });

    if (it != MarkerListe.end()) {
        MarkerListe.erase(it, MarkerListe.end());
        MarkerSpeichern();
    }
}
bool IstMarkerVorhanden(int MouseGridX, int MouseGridY) {
    for (const auto& Marker : MarkerListe) {
        if (Marker.GridX == MouseGridX && Marker.GridY == MouseGridY)
            return true;
    }
    return false;
}
