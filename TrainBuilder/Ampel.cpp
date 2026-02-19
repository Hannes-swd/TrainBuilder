#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "untermenü.h"
#include "json.hpp"
#include "zug.h"
#include "Ampel.h"

using json = nlohmann::json;

void AmpelPlazieren(int gridX, int gridY) {
	if (!schautObSchiene(gridX, gridY)) {
		return;
	}

	for (const auto& ampel : ampelListe) {
		if (ampel.GridX == gridX && ampel.GridY == gridY) {
			std::cout << "Ampel bereits vorhanden an Position [" << gridX << ", " << gridY << "]" << std::endl;
			return;
		}
	}

	ampel newAmpel;
	newAmpel.GridX = gridX;
	newAmpel.GridY = gridY;
	newAmpel.isGreen = true;
	

	int newId = 1;
	for (const auto& ampel : ampelListe) {
		if (ampel.AmpelId >= newId) {
			newId = ampel.AmpelId + 1;
		}
	}
	newAmpel.AmpelId = newId;
	newAmpel.Name = "Ampel_" + std::to_string(newId);

	ampelListe.push_back(newAmpel);

	AmpelSpeichern();

	std::cout << "Ampel platziert an Position [" << gridX << ", " << gridY << "] mit ID: " << newId << std::endl;
}

void AmpelZeichnen() {
	for (const auto& ampel : ampelListe) {
		if (ampel.isGreen) {
			DrawTexture("Ampel_Gruen", ampel.GridX * GRID_SIZE, ampel.GridY * GRID_SIZE, GRID_SIZE, GRID_SIZE, WHITE);
		}
		else {
			DrawTexture("Ampel_Rot", ampel.GridX * GRID_SIZE, ampel.GridY * GRID_SIZE, GRID_SIZE, GRID_SIZE, WHITE);
		}
	}
}

void AmpelSpeichern() {
	json jsonDaten;
	json ampelArray = json::array();
	for (const auto& ampel : ampelListe) {
		json ampelJson;
		ampelJson["AmpelId"] = ampel.AmpelId;
		ampelJson["gridX"] = ampel.GridX;
		ampelJson["gridY"] = ampel.GridY;
		ampelJson["isGreen"] = ampel.isGreen;
		ampelJson["Name"] = ampel.Name;

		ampelArray.push_back(ampelJson);
	}
	jsonDaten["Ampeln"] = ampelArray;
	std::ofstream datei("resurses/json/Ampeln.json");
	if (datei.is_open()) {
		datei << jsonDaten.dump(4);
		datei.close();
	}
	else {
		std::cerr << "Fehler: Ampeln.json konnte nicht geöffnet werden!" << std::endl;
	}
}

void AmpelUmschalten(int gridX, int gridY) {
	for (auto& ampel : ampelListe) {
		if (ampel.GridX == gridX && ampel.GridY == gridY) {
			ampel.isGreen = !ampel.isGreen;
			AmpelSpeichern();
			std::cout << "Ampel an [" << gridX << ", " << gridY << "] ist jetzt "
				<< (ampel.isGreen ? "GRUEN" : "ROT") << std::endl;
			return;
		}
	}
	std::cout << "Keine Ampel an Position [" << gridX << ", " << gridY << "] gefunden!" << std::endl;
}

void AmpelUmschaltenNachId(int ampelId) {
	for (auto& ampel : ampelListe) {
		if (ampel.AmpelId == ampelId) {
			ampel.isGreen = !ampel.isGreen;
			AmpelSpeichern();
			std::cout << "Ampel " << ampelId << " ist jetzt "
				<< (ampel.isGreen ? "GRUEN" : "ROT") << std::endl;
			return;
		}
	}
	std::cout << "Ampel mit ID " << ampelId << " nicht gefunden!" << std::endl;
}

// Ampel Grün
void AmpelAufGruen(int gridX, int gridY) {
	for (auto& ampel : ampelListe) {
		if (ampel.GridX == gridX && ampel.GridY == gridY) {
			ampel.isGreen = true;
			AmpelSpeichern();
			return;
		}
	}
}

// Ampel Rot
void AmpelAufRot(int gridX, int gridY) {
	for (auto& ampel : ampelListe) {
		if (ampel.GridX == gridX && ampel.GridY == gridY) {
			ampel.isGreen = false;
			AmpelSpeichern();
			return;
		}
	}
}