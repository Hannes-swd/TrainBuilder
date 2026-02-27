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
#include "PlatzierungCheck.h"


using json = nlohmann::json;


void LeiterPlazieren(int gridX, int gridY) {
	if (!KannPlatzieren(gridX, gridY, PlatzierTyp::Leiter)) return;

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
		std::string textureName = Leiter.Status ? "Leiter_An" : "Leiter_Aus";

		float x = Leiter.GridX * GRID_SIZE;
		float y = Leiter.GridY * GRID_SIZE;

		Texture2D tex = LoadTextureFromPath("resurses/Texturen/Technik/" + textureName + ".png");

		if (tex.id != 0) {
			Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };

			Rectangle dest = { x + GRID_SIZE / 2, y + GRID_SIZE / 2, GRID_SIZE, GRID_SIZE };

			float rotation = Leiter.Rotation * 90.0f;

			Vector2 origin = { GRID_SIZE / 2, GRID_SIZE / 2 };

			DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
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
void CheckInput() {

	for (auto& Leiter : LeiterListe) {
		if (Leiter.Status) continue;

		int CheckX = Leiter.GridX;
		int CheckY = Leiter.GridY;

		if (Leiter.Rotation == 0)      CheckY = Leiter.GridY + 1;
		else if (Leiter.Rotation == 1) CheckX = Leiter.GridX - 1;
		else if (Leiter.Rotation == 2) CheckY = Leiter.GridY - 1;
		else if (Leiter.Rotation == 3) CheckX = Leiter.GridX + 1;

		bool knotenFound = false;

		for (const auto& knoten : knotenliste) {
			if (knoten.GridX == CheckX && knoten.GridY == CheckY && knoten.Status) {
				knotenFound = true;
				break;
			}
		}

		if (!knotenFound) {
			for (const auto& leiter : LeiterListe) {
				if (leiter.GridX == CheckX && leiter.GridY == CheckY && leiter.Status) {
					knotenFound = true;
					break;
				}
			}
		}

		if (knotenFound) {
			Leiter.Status = true;
		}
	}
}
void CheckOutput() {
	std::map<int, bool> leiterAktiv;
	for (const auto& Leiter : LeiterListe) {
		if (Leiter.Status) {
			int targetX = Leiter.GridX;
			int targetY = Leiter.GridY;

			if (Leiter.Rotation == 0)      targetY = Leiter.GridY - 1;
			else if (Leiter.Rotation == 1) targetX = Leiter.GridX + 1;
			else if (Leiter.Rotation == 2) targetY = Leiter.GridY + 1;
			else if (Leiter.Rotation == 3) targetX = Leiter.GridX - 1;

			for (auto& knoten : knotenliste) {
				if (knoten.GridX == targetX && knoten.GridY == targetY && !knoten.modus) {
					knoten.Status = true;
					leiterAktiv[knoten.eindeutigeId] = true;
					break;
				}
			}

			for (auto& Gate : GateListe) {
				if (Gate.GridX == targetX && Gate.GridY == targetY) {
					int relX = Leiter.GridX - Gate.GridX;
					int relY = Leiter.GridY - Gate.GridY;

					for (int r = 0; r < Gate.Rotation; r++) {
						int tempX = relX;
						int tempY = relY;
						relX = tempY;
						relY = -tempX;
					}

					if (relX == -1 && relY == 0) {
						Gate.input1 = true;
						leiterAktiv[Gate.eindeutigeId + 1000] = true;
					}
					else if (relX == 1 && relY == 0) {
						Gate.input2 = true;
						leiterAktiv[Gate.eindeutigeId + 2000] = true;
					}
					break;
				}
			}
		}
	}

	for (auto& knoten : knotenliste) {
		if (!knoten.modus && leiterAktiv.find(knoten.eindeutigeId) == leiterAktiv.end()) {
			knoten.Status = false;
		}
	}

	for (auto& Gate : GateListe) {
		if (leiterAktiv.find(Gate.eindeutigeId + 1000) == leiterAktiv.end()) {
			Gate.input1 = false;
		}
		if (leiterAktiv.find(Gate.eindeutigeId + 2000) == leiterAktiv.end()) {
			Gate.input2 = false;
		}
	}
}