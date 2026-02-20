#include <map>
#include <queue>
#include <fstream>
#include <algorithm>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "untermenü.h"
#include "json.hpp"
#include "zug.h"
#include "Leiter.h"

using json = nlohmann::json;


void LeiterPlazieren(int gridX, int gridY) {
	if (schautObSchiene(gridX, gridY)) {
		return;
	}
	for (const auto& Leiter : LeiterListe) {
		if (Leiter.GridX == gridX && Leiter.GridY == gridY)
			return;
	}
	Leiterobjeckt newLeiter;
	
	newLeiter.GridX = gridX;
	newLeiter.GridY = gridY;
	newLeiter.Rotation = 0;
	newLeiter.Status = false;

	int neueID = 1;
	for (const auto& Leiter : LeiterListe) {
		if (Leiter.eindeutigeId >= neueID) {
			neueID = Leiter.eindeutigeId + 1;
		}
	}
	newLeiter.eindeutigeId = neueID;
	LeiterListe.push_back(newLeiter);
	LeiterSpeichern();
}
void LeiterSpeichern() {
	json jsonDaten;
	json LeiterArray = json::array();
	for (const auto& Leiter : LeiterListe) {
		json LeiterJson;
		LeiterJson["GridX"] = Leiter.GridX;
		LeiterJson["GridY"] = Leiter.GridY;
		LeiterJson["Rotation"] = Leiter.Rotation;
		LeiterJson["eindeutigeId"] = Leiter.eindeutigeId;
		LeiterJson["Status"] = Leiter.Status;

		LeiterArray.push_back(LeiterJson);
	}
	jsonDaten["Leiter"] = LeiterArray;
	std::ofstream Leiterdatei("resurses/json/Leiter.json");
	if (Leiterdatei.is_open()) {
		Leiterdatei << jsonDaten.dump(4);
		Leiterdatei.close();

	}
	else {
	}
}
void LeiterZeichnen() {
	for (const auto& Leiter : LeiterListe) {
		if (Leiter.Status == false) {
			DrawTexture("Leiter_Aus",
				Leiter.GridX * GRID_SIZE,
				Leiter.GridY * GRID_SIZE,
				GRID_SIZE, GRID_SIZE, WHITE);
		}
		else {
			DrawTexture("Leiter_An",
				Leiter.GridX * GRID_SIZE,
				Leiter.GridY * GRID_SIZE,
				GRID_SIZE, GRID_SIZE, WHITE);
		}
	}
}

void LeiterLöschen(int gridX, int gridY) {
	auto it = std::remove_if(LeiterListe.begin(), LeiterListe.end(),
		[gridX, gridY](const Leiterobjeckt& l) {
			return l.GridX == gridX && l.GridY == gridY;
		});

	if (it != LeiterListe.end()) {
		LeiterListe.erase(it, LeiterListe.end());
		LeiterSpeichern();
	}
}
bool IstLeiterVorhanden(int gridX, int gridY) {
	for (const auto& leiter : LeiterListe) {
		if (leiter.GridX == gridX && leiter.GridY == gridY) {
			return true;
		}
	}
	return false;
}