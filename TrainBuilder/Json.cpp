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
    std::ifstream Banhoefe("resurses/json/Banhof.json");
    std::ifstream ZugArten("resurses/json/zugarten.json");
	std::ifstream AktiveZuege("resurses/json/AktiveZuege.json");

    //OBECKTE UND LADEN
    nlohmann::json NutzerDaten;
    if (Nutzer.is_open()) {
        Nutzer >> NutzerDaten;
    }

    nlohmann::json GleiseDaten;
    if (Gleise.is_open()) {
        Gleise >> GleiseDaten;
    }

    nlohmann::json BanhofDaten;
    if (Banhoefe.is_open()) {
        Banhoefe >> BanhofDaten;
    }

    nlohmann::json ZugArtenDaten;
    if (ZugArten.is_open()) {
        ZugArten >> ZugArtenDaten;
    }

	nlohmann::json AktiveZuegeDaten;
	if (AktiveZuege.is_open()) {
		AktiveZuege >> AktiveZuegeDaten;
	}

    //ALLE LISTEN LÖSCHEN
    gleisListe.clear();
    banhofListe.clear();
    zugArtenListe.clear();
    /*-------------------------------------------------
        Gleise
    -------------------------------------------------*/
    if (GleiseDaten.contains("Objeckte")) {
        for (const auto& obj : GleiseDaten["Objeckte"]) {
            GleisObjeckt gleis;
            gleis.ObjecktId = obj["ObjecktId"];
            gleis.GridX = obj["gridX"];
            gleis.GridY = obj["gridY"];
            gleis.Rotation = obj["rotation"];
            gleisListe.push_back(gleis);
        }
    }

    if (Gleise.is_open()) Gleise.close();
    /*-------------------------------------------------
        Banhofe
    -------------------------------------------------*/
    if (BanhofDaten.contains("BanhofObjeckte")) {
        for (const auto& obj : BanhofDaten["BanhofObjeckte"]) {
            BanhofObjeckt banhof;
            banhof.Name = obj["Name"];
            banhof.BanhofId = obj["BanhofId"];
            banhof.GridX = obj["gridX"];
            banhof.GridY = obj["gridY"];
            banhof.Rotation = obj["rotation"];
            banhofListe.push_back(banhof);
        }
    }
    /*-------------------------------------------------
        Zugarten
    -------------------------------------------------*/
    if (ZugArtenDaten.contains("zuege") || ZugArtenDaten.contains(u8"züge")) {
        const auto& zuegeArray = ZugArtenDaten.contains("zuege") ? ZugArtenDaten["zuege"] : ZugArtenDaten[u8"züge"];
        for (const auto& obj : zuegeArray) {
            ZugArt zugart;
			zugart.id = obj["id"];
            zugart.name = obj["name"];
            zugart.geschwindichkeit = obj["geschwindichkeit"];

			zugart.passagiere = obj["passagiere"];
			zugart.gueter = obj["gueter"];
            zugart.biildpfad = obj["biildpfad"];
            zugart.farbe = obj["farbe"];
            zugart.zugtyp = obj["zugtyp"];
            zugArtenListe.push_back(zugart);
        }
    }
    /*-------------------------------------------------
		Aktive Züge
    -------------------------------------------------*/
    if (AktiveZuegeDaten.contains("AktiveZuege")) {
        for (const auto& obj : AktiveZuegeDaten["AktiveZuege"]) {
            Zug zug;
            zug.zugId = obj["zugId"];
            zug.posX = obj["gridX"];
            zug.posY = obj["gridY"];
            zug.rotation = obj["rotation"];
            zug.name = obj["name"];
            zug.passagiere = obj["passagiere"];
            zug.gueter = obj["gueter"];
            zug.Fahrplan = obj["Fahrplan"].get<std::vector<std::string>>();
            zug.zugtyp = obj["zugtyp"];
            zug.farbe = obj["farbe"];
            zug.biildpfad = obj["biildpfad"];
            zug.geschwindichkeit = obj["geschwindichkeit"];
            aktiveZuege.push_back(zug);
        }
    }

	if (AktiveZuege.is_open()) AktiveZuege.close();
    if (Banhoefe.is_open()) Banhoefe.close();
    if (ZugArten.is_open()) ZugArten.close();

    //WERT LADEN
    if (NutzerDaten.contains("SpielerpositionX") && NutzerDaten.contains("SpielerpositionY")) {
        Spielerposition = { NutzerDaten["SpielerpositionX"], NutzerDaten["SpielerpositionY"] };
    }
}

void SpeicherJson() {
    //Daten speichern
    nlohmann::json NutzerDaten;

    NutzerDaten["SpielerpositionX"] = (int)Spielerposition.x;
    NutzerDaten["SpielerpositionY"] = (int)Spielerposition.y;

    //Datei speichern
    std::ofstream datei("resurses/json/User.json");
    if (datei.is_open()) {
        datei << NutzerDaten.dump(4);
        datei.close();
    }
}