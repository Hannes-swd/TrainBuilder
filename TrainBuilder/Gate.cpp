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
        float x = Gate.GridX * GRID_SIZE;
        float y = Gate.GridY * GRID_SIZE;

        float rotation = Gate.Rotation * 90.0f;
        Vector2 origin = { GRID_SIZE / 2, GRID_SIZE / 2 };

        DrawTexture("GateGrundstrucktur", x, y, GRID_SIZE, GRID_SIZE, WHITE);

        if (Gate.input1) {
            Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/Gate/GateLinks.png");
            if (tex.id != 0) {
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };
                DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
            }
        }

        if (Gate.input2) {
            Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/Gate/GateRechts.png");
            if (tex.id != 0) {
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };
                DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
            }
        }

        if (Gate.Output) {
            Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/Gate/GateOutput.png");
            if (tex.id != 0) {
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };
                DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
            }
        }

        if (Gate.modus == "AND") {
            Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/Gate/AND.png");
            if (tex.id != 0) {
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };
                DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
            }
        }
        if (Gate.modus == "XOR") {
            Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/Gate/XOR.png");
            if (tex.id != 0) {
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };
                DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
            }
        }
    }
}