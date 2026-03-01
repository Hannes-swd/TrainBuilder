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
#include "Json.h"

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

    std::ofstream GateDatei(GetFullPath("Gate.json").c_str());
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
void GateOutput() {
    for (auto& Gate : GateListe) {
        if (Gate.modus == "AND") {
            Gate.Output = (Gate.input1 && Gate.input2);
        }
        else if (Gate.modus == "XOR") {
            Gate.Output = (Gate.input1 != Gate.input2);
        }
        else {
            Gate.Output = false; 
        }
    }

    for (const auto& Gate : GateListe) {
        int targetX = Gate.GridX;
        int targetY = Gate.GridY;

        if (Gate.Rotation == 0)      targetY = Gate.GridY - 1;
        else if (Gate.Rotation == 1) targetX = Gate.GridX + 1;
        else if (Gate.Rotation == 2) targetY = Gate.GridY + 1; 
        else if (Gate.Rotation == 3) targetX = Gate.GridX - 1;

        for (auto& knoten : knotenliste) {
            if (knoten.GridX == targetX && knoten.GridY == targetY && !knoten.modus) {
                knoten.Status = Gate.Output;
                break;
            }
        }

        for (auto& leiter : LeiterListe) {
            if (leiter.GridX == targetX && leiter.GridY == targetY) {
                leiter.Status = Gate.Output;
                break;
            }
        }

        for (auto& inverter : InverterListe) {
            if (inverter.GridX == targetX && inverter.GridY == targetY) {
                inverter.Status = Gate.Output;
                break;
            }
        }
    }
}

void GateLöschen(int gridX, int gridY) {
    auto it = std::remove_if(GateListe.begin(), GateListe.end(),
        [gridX, gridY](const GateObjeckt& l) {
            return l.GridX == gridX && l.GridY == gridY;
        });

    if (it != GateListe.end()) {
        GateListe.erase(it, GateListe.end());
        GateSpeichern();
    }
}
bool IstGateVorhanden(int gridX, int gridY) {
    for (const auto& gate : GateListe) {
        if (gate.GridX == gridX && gate.GridY == gridY) {
            return true;
        }
    }
    return false;
}