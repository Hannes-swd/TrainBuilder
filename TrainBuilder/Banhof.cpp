#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"
#include <fstream>
#include "json.hpp"
#include "LoadTexture.h"
#include "Banhof.h"

void plaziereBanhof(Vector2 position) {
	int gridX = (int)floor(position.x / GRID_SIZE);
	int gridY = (int)floor(position.y / GRID_SIZE);
	
	if (IstBanhofBereitsVorhanden(gridX, gridY)) {
		return;
	}
	
	if (!schautObSchiene(gridX, gridY)) {
		//bricht ab wen keine schiene da ist
		return;
	}
	BanhofObjeckt neuerBanhof;
	neuerBanhof.Name = "Banhof_" + std::to_string(banhofListe.size() + 1);
	neuerBanhof.BanhofId = (int)banhofListe.size() + 1;
	neuerBanhof.GridX = gridX;
	neuerBanhof.GridY = gridY;
	neuerBanhof.Rotation = 0;
	neuerBanhof.ID = "Banhof_" + std::to_string(banhofListe.size() + 1);
	banhofListe.push_back(neuerBanhof);

	BanhofSpeichern();

	
}

void BanhofSpeichern() {
	nlohmann::json jsonDaten;
	nlohmann::json banhofArray = nlohmann::json::array();
	for (const auto& banhof : banhofListe) {
		nlohmann::json banhofJson;
		banhofJson["Name"] = banhof.Name;
		banhofJson["BanhofId"] = banhof.BanhofId;
		banhofJson["gridX"] = banhof.GridX;
		banhofJson["gridY"] = banhof.GridY;
		banhofJson["rotation"] = banhof.Rotation;
		banhofJson["ID"] = banhof.ID;
		banhofArray.push_back(banhofJson);
	}
	jsonDaten["BanhofObjeckte"] = banhofArray;
	std::ofstream datei("resurses/json/Banhof.json");
	if (datei.is_open()) {
		datei << jsonDaten.dump(4);
		datei.close();
	}
	else {
		//feler
	}
}
bool IstBanhofBereitsVorhanden(int gridX, int gridY) {
	for (const auto& banhof : banhofListe) {
		if (banhof.GridX == gridX && banhof.GridY == gridY) {
			return true;
		}
	}
	return false;
}
bool schautObSchiene(int gridX, int gridY) {
	for (const auto& gleis : gleisListe) {
		if (gleis.GridX == gridX && gleis.GridY == gridY) {
			return true;
		}
	}
	return false;
}

void ZeichneBanhof() {
	for (const auto& banhof : banhofListe) {
		float pixelX = (float)(banhof.GridX * GRID_SIZE);
		float pixelY = (float)(banhof.GridY * GRID_SIZE);
		DrawTexture("Banhof",
			pixelX, pixelY,
			GRID_SIZE, GRID_SIZE,
			WHITE);
	}
}