#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "untermenü.h"
#include "json.hpp"
#include "zug.h"
#include "knoten.h"

using json = nlohmann::json;

void KnotenPlazieren(int gridX, int gridY) {
	//kan nur auf blöcken ohne plazeren
	if (schautObSchiene(gridX, gridY)) {
		return;
	}
	for (const auto& knoten : knotenliste) {
		if (knoten.GridX == gridX && knoten.GridY == gridY)
			return;
	}
	knoten newKnoten;
	newKnoten.GridX = gridX;
	newKnoten.GridY = gridY;
	newKnoten.Status = false;
	//eigene id erstellen
	int neieId = 1;
	for (const auto& knoten : knotenliste) {
		if (knoten.eindeutigeId >= neieId) {
			neieId = knoten.eindeutigeId + 1;
		}
	}
	newKnoten.eindeutigeId = neieId;
	knotenliste.push_back(newKnoten);
	KnotenSpeichern();
	std::cerr << "knoiten hinzugefügt";
}

void KnotenSpeichern() {
	json jsonDaten;
	json knotenlArray = json::array();
	for (const auto& knoten : knotenliste) {
		json knotenJson;
		knotenJson["GridX"] = knoten.GridX;
		knotenJson["GridY"] = knoten.GridY;
		knotenJson["eindeutigeId"] = knoten.eindeutigeId;
		knotenJson["Name"] = knoten.Name;
		knotenJson["Status"] = knoten.Status;

		knotenlArray.push_back(knotenJson);
	}
	jsonDaten["Knoten"] = knotenlArray;
	std::cerr << "knoiten gespeichert";
	std::ofstream Knotendatei("resurses/json/Knoten.json");
	if (Knotendatei.is_open()) {
		Knotendatei << jsonDaten.dump(4);
		Knotendatei.close();

	}
	else {
		std::cerr << "Fehler: Knoten.json konnte nicht geöffnet werden!" << std::endl;
	}
}