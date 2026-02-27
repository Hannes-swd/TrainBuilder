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