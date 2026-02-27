#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <map>
#include <queue>
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
#include "Gate.h"
#include "PlatzierungCheck.h"
using json = nlohmann::json;

void GatePlazieren(int gridX, int gridY) {
	if (!KannPlatzieren(gridX, gridY, PlatzierTyp::Gate)) return;

	GateObjeckt newGate;
	newGate.GridX = gridX;
	newGate.GridY = gridY;
	newGate.Rotation = 0;
	newGate.modus = "AND";
	newGate.input1 = false;
	newGate.input2 = false;
	newGate.Output = false;
	//eigene id erstellen
	int neieId = 1;
	for (const auto& gate : GateListe) {
		if (gate.eindeutigeId >= neieId) {
			neieId = gate.eindeutigeId + 1;
		}
	}
	newGate.eindeutigeId = neieId;
	GateListe.push_back(newGate);
	GateSpeichern();
}
void GateSpeichern() {
	json jsonDaten;
	json GateArray = json::array();
	for (const auto& gate : GateListe) {
		json gateJson;
		gateJson["GridX"] = gate.GridX;
		gateJson["GridY"] = gate.GridY;
		gateJson["Rotation"] = gate.Rotation;
		gateJson["eindeutigeId"] = gate.eindeutigeId;
		gateJson["modus"] = gate.modus;
		gateJson["input1"] = gate.input1;
		gateJson["input2"] = gate.input2; 
		gateJson["Output"] = gate.Output;

		GateArray.push_back(gateJson);
	}
	jsonDaten["Gate"] = GateArray;
	std::cerr << "Gate gespeichert";
	std::ofstream GateDatei("resurses/json/Gate.json");
	if (GateDatei.is_open()) {
		GateDatei << jsonDaten.dump(4);
		GateDatei.close();

	}
	else {
		std::cerr << "Fehler: Gate.json konnte nicht geöffnet werden!" << std::endl;
	}
}
void GateZeichnen() {
	for (const auto& Gate : GateListe) {
		//grundstrucktur
		DrawTexture("GateGrundstrucktur",
			Gate.GridX * GRID_SIZE,
			Gate.GridY * GRID_SIZE,
			GRID_SIZE, GRID_SIZE, WHITE);
		
		//einzelne angeschaltet
		if (Gate.input1)
			DrawTexture("GateLinks",
				Gate.GridX * GRID_SIZE,
				Gate.GridY * GRID_SIZE,
				GRID_SIZE, GRID_SIZE, WHITE);
		if (Gate.input2)
			DrawTexture("GateRechts",
				Gate.GridX * GRID_SIZE,
				Gate.GridY * GRID_SIZE,
				GRID_SIZE, GRID_SIZE, WHITE);
		if (Gate.Output)
			DrawTexture("GateOutput",
				Gate.GridX * GRID_SIZE,
				Gate.GridY * GRID_SIZE,
				GRID_SIZE, GRID_SIZE, WHITE);
		//gate art anzeigen
		if (Gate.modus == "AND")
			DrawTexture("GateAND",
				Gate.GridX * GRID_SIZE,
				Gate.GridY * GRID_SIZE,
				GRID_SIZE, GRID_SIZE, WHITE);
	}
}