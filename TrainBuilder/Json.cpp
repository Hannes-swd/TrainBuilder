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