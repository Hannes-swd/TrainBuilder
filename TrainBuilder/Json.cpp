#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"

#include <fstream>
#include "json.hpp"
#include "LoadTexture.h"

using json = nlohmann::json;

void LadeJson() {
	//ALLE DATEIEN
	std::ifstream Nutzer("resurses/json/User.json");
	std::ifstream Gleise("resurses/json/Gleise.json");

	//OBECKTE UND LADEN
	nlohmann::json NutzerDaten;
	Nutzer >> NutzerDaten;

	nlohmann::json GleiseDaten;
	Gleise >> GleiseDaten;

	//ALLE LISTEN LÖSCHEN
	gleisListe.clear();
	for (const auto& obj : GleiseDaten["Objeckte"]) {
		GleisObjeckt gleis;
		gleis.ObjecktId = obj["ObjecktId"];
		gleis.GridX = obj["gridX"];
		gleis.GridY = obj["gridY"];
		gleis.Rotation = obj["rotation"];
		gleisListe.push_back(gleis);
	}

	Gleise.close();


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


