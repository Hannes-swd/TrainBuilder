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
            zugart.name = obj["name"];
            zugart.geschwindichkeit = obj["geschwindichkeit"];

			zugart.passagiere = obj["passagiere"];
			zugart.güter = obj["gueter"];
            zugart.biildpfad = obj["biildpfad"];
            zugart.farbe = obj["farbe"];
            zugart.zugtyp = obj["zugtyp"];
            zugArtenListe.push_back(zugart);
        }
    }



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