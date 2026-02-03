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

    //ALLE LISTEN LÖSCHEN
    gleisListe.clear();
    banhofListe.clear();

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

    if (Banhoefe.is_open()) Banhoefe.close();

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