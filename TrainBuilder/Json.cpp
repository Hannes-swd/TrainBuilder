#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"

#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

void LadeJson() {
	//ALLE DATEIEN
	std::ifstream Nutzer("resurses/json/User.json");

	//OBECKTE UND LADEN
	nlohmann::json NutzerDaten;
	Nutzer >> NutzerDaten;


	//WERT LADEN
	Spielerposition = { NutzerDaten["SpielerpositionX"], NutzerDaten["SpielerpositionY"] };
}
void SpeicherJson() {
	//Daten speichern
	nlohmann::json NutzerDaten;

	NutzerDaten["SpielerpositionX"] = (int)Spielerposition.x;
	NutzerDaten["SpielerpositionY"] = (int)Spielerposition.y;


	//Datei speichern
	std::ofstream datei("resurses/json/User.json");
	datei << NutzerDaten.dump(4);
}