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
void KnotenZeichnen() {
	int fontSize = 15;
	int maxWidth = GRID_SIZE - 4;

	for (const auto& knoten : knotenliste) {
		DrawTexture("knoten",
			knoten.GridX * GRID_SIZE,
			knoten.GridY * GRID_SIZE,
			GRID_SIZE, GRID_SIZE, WHITE);

		std::string text = knoten.Name;
		int textWidth = MeasureText(text.c_str(), fontSize);

		if (textWidth > maxWidth) {
			std::string gekuerzt = text;
			while (MeasureText(gekuerzt.c_str(), fontSize) > maxWidth && gekuerzt.length() > 1) {
				gekuerzt.pop_back();
			}
			if (gekuerzt.length() < text.length()) {
				gekuerzt += "...";
			}
			text = gekuerzt;

			textWidth = MeasureText(text.c_str(), fontSize);
		}

		int xPos = knoten.GridX * GRID_SIZE + (GRID_SIZE - textWidth) / 2;
		int yPos = knoten.GridY * GRID_SIZE + (GRID_SIZE - fontSize) / 2;

		DrawText(text.c_str(), xPos, yPos, fontSize, BLACK);
	}
}
void knotenlöschen(int gridX, int gridY) {

}