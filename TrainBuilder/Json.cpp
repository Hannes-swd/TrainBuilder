#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"
#include <fstream>
#include "json.hpp"
#include "LoadTexture.h"
#include "Signal.h"


using json = nlohmann::json;

void LadeJson() {
    //ALLE DATEIEN
    std::ifstream Nutzer("resurses/json/User.json");
    std::ifstream Gleise("resurses/json/Gleise.json");
    std::ifstream Banhoefe("resurses/json/Banhof.json");
    std::ifstream ZugArten("resurses/json/zugarten.json");
	std::ifstream AktiveZuege("resurses/json/AktiveZuege.json");
    std::ifstream Ampeln("resurses/json/Ampeln.json");
    std::ifstream Knoten("resurses/json/Knoten.json");
    std::ifstream Leiter("resurses/json/Leiter.json"); 
    std::ifstream Inverter("resurses/json/Inverter.json");
    std::ifstream marker("resurses/json/marker.json");
    std::ifstream Gate("resurses/json/Gate.json");

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

	nlohmann::json AmpelnDaten;
	if (Ampeln.is_open()) {
		Ampeln >> AmpelnDaten;
	}

    nlohmann::json KnotenDaten;
    if (Knoten.is_open()) {
        Knoten >> KnotenDaten;
    }

    nlohmann::json LeiterDaten;
    if (Leiter.is_open()) {
        Leiter >> LeiterDaten;
    }

    nlohmann::json InverterDaten;
    if (Inverter.is_open()) {
        Inverter >> InverterDaten;
    }

    nlohmann::json markerDaten;
    if (marker.is_open()) {
        marker >> markerDaten;
    }

    nlohmann::json gateDaten;
    if (Gate.is_open()) {
        Gate >> gateDaten;
    }
    
    
    //ALLE LISTEN LÖSCHEN
    gleisListe.clear();
    banhofListe.clear();
    zugArtenListe.clear();
    ampelListe.clear();
    LeiterListe.clear();
    InverterListe.clear();
    MarkerListe.clear();
    GateListe.clear();
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
            banhof.ID = obj["ID"];
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
            zug.posX = obj["posX"];
            zug.posY = obj["posY"];
            zug.rotation = obj["rotation"];
            zug.name = obj["name"];
            zug.Fahrplan = obj["Fahrplan"].get<std::vector<std::string>>();
            zug.zugtyp = obj["zugtyp"];
            zug.farbe = obj["farbe"];
            zug.biildpfad = obj["biildpfad"];
            zug.geschwindichkeit = obj["geschwindichkeit"];
            aktiveZuege.push_back(zug);
        }
    }
    /*-------------------------------------------------
        Ampeln
    -------------------------------------------------*/
    if (AmpelnDaten.contains("Ampeln")) {
        for (const auto& obj : AmpelnDaten["Ampeln"]) {
            ampel ampel;
            ampel.AmpelId = obj["AmpelId"];
            ampel.GridX = obj["gridX"];
            ampel.GridY = obj["gridY"];
            ampel.isGreen = obj["isGreen"];
            ampel.Name = obj["Name"];
            ampelListe.push_back(ampel);

            if (!ampel.Name.empty()) {
                SignalTeilHinzufuegen(ampel.AmpelId, ampel.Name, ampel.isGreen, Ampel);
            }
        }
    }
    /*-------------------------------------------------
        Knoten
    -------------------------------------------------*/
    if (KnotenDaten.contains("Knoten")) {
        for (const auto& obj : KnotenDaten["Knoten"]) {
            knoten Knoten;
            Knoten.GridX = obj["GridX"];
            Knoten.GridY = obj["GridY"];
            Knoten.eindeutigeId = obj["eindeutigeId"];
            Knoten.Status = obj["Status"];
            Knoten.Name = obj["Name"];
            //modus: Lesen = true / schreiben = false
            Knoten.modus = obj["modus"];
            knotenliste.push_back(Knoten);
        }
    }
    /*-------------------------------------------------
        Leiter
    -------------------------------------------------*/
    if (LeiterDaten.contains("Leiter")) {
        for (const auto& obj : LeiterDaten["Leiter"]) {
            Leiterobjeckt Leiter;
            Leiter.GridX = obj["GridX"];
            Leiter.GridY = obj["GridY"];
            Leiter.Rotation = obj["Rotation"];
            Leiter.eindeutigeId = obj["eindeutigeId"];
            Leiter.Status = obj["Status"];

            LeiterListe.push_back(Leiter);
        }
    }

    /*-------------------------------------------------
        Inverter
    -------------------------------------------------*/
    if (InverterDaten.contains("Inverter")) {
        for (const auto& obj : InverterDaten["Inverter"]) {
            InverterObjeckt Inverter;
            Inverter.GridX = obj["GridX"];
            Inverter.GridY = obj["GridY"];
            Inverter.Rotation = obj["Rotation"];
            Inverter.eindeutigeId = obj["eindeutigeId"];
            Inverter.Status = obj["Status"];

            InverterListe.push_back(Inverter);
        }
    }


    /*-------------------------------------------------
        Marker
    -------------------------------------------------*/
    if (markerDaten.contains("marker")) {
        for (const auto& obj : markerDaten["marker"]) {
            MarkerObjeckt marker; 
            marker.GridX = obj["GridX"];
            marker.GridY = obj["GridY"];
            marker.eindeutigeId = obj["eindeutigeId"];
            if (obj.contains("farbe_r") && obj.contains("farbe_g") && obj.contains("farbe_b")) {
                marker.farbe.r = obj["farbe_r"];
                marker.farbe.g = obj["farbe_g"];
                marker.farbe.b = obj["farbe_b"];
                marker.farbe.a = 255;
            }
            else {
                marker.farbe = RED;
            }

            MarkerListe.push_back(marker);
        }
    }

    /*-------------------------------------------------
        Gate
    -------------------------------------------------*/
    if (gateDaten.contains("Gate")) {
        for (const auto& obj : gateDaten["Gate"]) {
            GateObjeckt gate;
            gate.GridX = obj["GridX"];
            gate.GridY = obj["GridY"];
            gate.Rotation = obj["Rotation"];
            gate.eindeutigeId = obj["eindeutigeId"];
            gate.modus = obj["modus"];

            GateListe.push_back(gate);
        }
    }

	if (AktiveZuege.is_open()) AktiveZuege.close();
    if (Banhoefe.is_open()) Banhoefe.close();
    if (ZugArten.is_open()) ZugArten.close();
	if (Ampeln.is_open()) Ampeln.close();
    if (Knoten.is_open()) Knoten.close();
    if (Leiter.is_open()) Leiter.close();
    if (Inverter.is_open()) Inverter.close();
    if (marker.is_open()) marker.close();
    if (Gate.is_open()) Gate.close();


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