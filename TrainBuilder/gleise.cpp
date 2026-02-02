#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"

#include <fstream>
#include "json.hpp"
#include "LoadTexture.h"

void ZeichneGleise() {

	for (const auto& gleis : gleisListe) {
		//ladet alle werte
		int ObjecktId = gleis.ObjecktId;
		float x = gleis.GridX * 50.0f;
		float y = gleis.GridY * 50.0f;
		int rotation = gleis.Rotation;


		//zeichnet geraden
		if (ObjecktId == 1) {
			switch (rotation) {
				case 0:
					DrawTexture("GleisVertikal", x, y, 50.0f, 50.0f);
					break;
				case 180:
					DrawTexture("GleisVertikal", x, y, 50.0f, 50.0f);
					break;
				case 90:
					DrawTexture("GleisHorizontal", x, y, 50.0f, 50.0f);
					break;
				case 270:
					DrawTexture("GleisHorizontal", x, y, 50.0f, 50.0f);
					break;
			}
		}
		else if (ObjecktId == 2) {
			switch (rotation) {
			case 0:
				DrawTexture("GleisKurve1", x, y, 50.0f, 50.0f);
				break;
			case 180:
				DrawTexture("GleisKurve3", x, y, 50.0f, 50.0f);
				break;
			case 90:
				DrawTexture("GleisKurve2", x, y, 50.0f, 50.0f);
				break;
			case 270:
				DrawTexture("GleisKurve4", x, y, 50.0f, 50.0f);
				break;
			}
		}
	}
}
void GleiseSpeichern() {
	nlohmann::json jsonDaten;

	nlohmann::json gleisArray = nlohmann::json::array();

	for (const auto& gleis : gleisListe) {
		nlohmann::json gleisJson;
		gleisJson["ObjecktId"] = gleis.ObjecktId;
		gleisJson["gridX"] = gleis.GridX;
		gleisJson["gridY"] = gleis.GridY;
		gleisJson["rotation"] = gleis.Rotation;

		gleisArray.push_back(gleisJson);
	}

	jsonDaten["Objeckte"] = gleisArray;

	std::ofstream datei("resurses/json/Gleise.json");
	if (datei.is_open()) {
		datei << jsonDaten.dump(4);
		datei.close();
	}
	else {
		//feler
	}
}