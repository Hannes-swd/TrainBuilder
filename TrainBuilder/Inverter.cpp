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
#include "zug.h"
#include "Leiter.h"
#include "knoten.h"
#include "Inverter.h"

using json = nlohmann::json;

void InverterPlazieren(int gridX, int gridY) {
	if (schautObSchiene(gridX, gridY)) {
		return;
	}
	for (const auto& Inverter : InverterListe) {
		if (Inverter.GridX == gridX && Inverter.GridY == gridY)
			return;
	}
	InverterObjeckt newInverter;
	newInverter.GridX = gridX;
	newInverter.GridY = gridY;
	newInverter.Rotation = 0;
	newInverter.Status = true;

	int neueID = 1;
	for (const auto& inv : InverterListe) {
		if (inv.eindeutigeId >= neueID) {
			neueID = inv.eindeutigeId + 1;
		}
	}
	newInverter.eindeutigeId = neueID;
	InverterListe.push_back(newInverter);
	InvertorSpeichern();
}

void InvertorSpeichern() {
	json jsonDaten;
	json InvertorArray = json::array();
	for (const auto& Inverter : InverterListe) {
		json InverterJson;
		InverterJson["GridX"] = Inverter.GridX;
		InverterJson["GridY"] = Inverter.GridY;
		InverterJson["Rotation"] = Inverter.Rotation;
		InverterJson["eindeutigeId"] = Inverter.eindeutigeId;
		InverterJson["Status"] = Inverter.Status;
		InvertorArray.push_back(InverterJson);
	}
	jsonDaten["Inverter"] = InvertorArray;
	std::ofstream InverterDatei("resurses/json/Inverter.json");
	if (InverterDatei.is_open()) {
		InverterDatei << jsonDaten.dump(4);
		InverterDatei.close();
	}
}

void InvertorZeichnen() {
	for (const auto& Inverter : InverterListe) {
		
		std::string textureName = Inverter.Status ? "Inverter_Aus" : "Inverter_AN";

		float x = Inverter.GridX * GRID_SIZE;
		float y = Inverter.GridY * GRID_SIZE;

		Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/" + textureName + ".png");

		if (tex.id != 0) {
			Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
			Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };
			float rotation = Inverter.Rotation * 90.0f;
			Vector2 origin = { GRID_SIZE / 2, GRID_SIZE / 2 };
			DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
		}
	}
}

void InvertorCheckInput() {
	for (auto& Inverter : InverterListe) {
		bool eingang = false;

		int CheckX = Inverter.GridX;
		int CheckY = Inverter.GridY;

		if (Inverter.Rotation == 0)      CheckY = Inverter.GridY + 1;
		else if (Inverter.Rotation == 1) CheckX = Inverter.GridX - 1;
		else if (Inverter.Rotation == 2) CheckY = Inverter.GridY - 1;
		else if (Inverter.Rotation == 3) CheckX = Inverter.GridX + 1;

		for (const auto& k : knotenliste) {
			if (k.GridX == CheckX && k.GridY == CheckY && k.Status) {
				eingang = true;
				break;
			}
		}

		if (!eingang) {
			for (const auto& l : LeiterListe) {
				if (l.GridX == CheckX && l.GridY == CheckY && l.Status) {
					eingang = true;
					break;
				}
			}
		}

		Inverter.Status = !eingang;
	}
}

void InvertorCheckOutput() {
	for (const auto& Inverter : InverterListe) {
		if (!Inverter.Status) continue;

		int targetX = Inverter.GridX;
		int targetY = Inverter.GridY;

		if (Inverter.Rotation == 0)      targetY = Inverter.GridY - 1;
		else if (Inverter.Rotation == 1) targetX = Inverter.GridX + 1;
		else if (Inverter.Rotation == 2) targetY = Inverter.GridY + 1;
		else if (Inverter.Rotation == 3) targetX = Inverter.GridX - 1;

		for (auto& k : knotenliste) {
			if (k.GridX == targetX && k.GridY == targetY && !k.modus) {
				k.Status = true;
				break;
			}
		}

		for (auto& l : LeiterListe) {
			if (l.GridX == targetX && l.GridY == targetY) {
				l.Status = true;
				break;
			}
		}
	}
}

bool IstInverterVorhanden(int gridX, int gridY) {
	for (const auto& Inverter : InverterListe) {
		if (Inverter.GridX == gridX && Inverter.GridY == gridY)
			return true;
	}
	return false;
}

void InverterLöschen(int gridX, int gridY) {
	auto it = std::remove_if(InverterListe.begin(), InverterListe.end(),
		[gridX, gridY](const InverterObjeckt& l) {
			return l.GridX == gridX && l.GridY == gridY;
		});

	if (it != InverterListe.end()) {
		InverterListe.erase(it, InverterListe.end());
		InvertorSpeichern();
	}
}