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
	newInverter.Status = false;

	int neueID = 1;
	for (const auto& newInverter : InverterListe) {
		if (newInverter.eindeutigeId >= neueID) {
			neueID = newInverter.eindeutigeId + 1;
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
	else {
	}
}
void InvertorZeichnen() {
	for (const auto& Inverter : InverterListe) {
		std::string textureName = Inverter.Status ? "Inverter_AN" : "Inverter_Aus";

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
	int CheckX = 0;
	int CheckY = 0;

	for (auto& Leiter : LeiterListe) {
		Leiter.Status = false;
	}

	for (auto& Leiter : LeiterListe) {
		bool knotenFound = false;

		if (Leiter.Rotation == 0) {
			CheckX = Leiter.GridX;
			CheckY = Leiter.GridY + 1;
		}
		else if (Leiter.Rotation == 1) {
			CheckX = Leiter.GridX - 1;
			CheckY = Leiter.GridY;
		}
		else if (Leiter.Rotation == 2) {
			CheckX = Leiter.GridX;
			CheckY = Leiter.GridY - 1;
		}
		else if (Leiter.Rotation == 3) {
			CheckX = Leiter.GridX + 1;
			CheckY = Leiter.GridY;
		}

		for (const auto& knoten : knotenliste) {
			if (knoten.GridX == CheckX && knoten.GridY == CheckY) {
				if (knoten.Status) {
					knotenFound = true;

				}
				break;
			}
		}
		for (const auto& leiter : LeiterListe) {
			if (leiter.GridX == CheckX && leiter.GridY == CheckY) {
				if (leiter.Status) {
					knotenFound = true;

				}
				break;
			}
		}

		Leiter.Status = knotenFound;
	}
}
void InvertorCheckOutput() {

}

bool IstInverterVorhanden(int gridX, int gridY) {
	for (const auto& Inverter : InverterListe) {
		if (Inverter.GridX == gridX && Inverter.GridY == gridY) {
			return true;
		}
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