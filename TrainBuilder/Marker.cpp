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

using json = nlohmann::json;

void MarkerPlazieren(int gridX, int gridY) {
	if (schautObSchiene(gridX, gridY)) {
		return;
	}
	for (const auto& Marker : MarkerListe) {
		if (Marker.GridX == gridX && Marker.GridY == gridY)
			return;
	}
	MarkerObjeckt newMarker;
	newMarker.GridX = gridX;
	newMarker.GridY = gridY;
	newMarker.farbe = "Rot";
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
		MarkerJson["farbe"] = Marker.farbe;
		MarkerJson["eindeutigeId"] = Marker.eindeutigeId;
		MarkerArray.push_back(MarkerJson);
	}
	jsonDaten["Marker"] = MarkerArray;
	std::ofstream MarkerDatei("resurses/json/Marker.json");
	if (MarkerDatei.is_open()) {
		MarkerDatei << jsonDaten.dump(4);
		MarkerDatei.close();
	}
}