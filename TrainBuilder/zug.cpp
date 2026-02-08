#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "untermenü.h"
#include "json.hpp"
#include "zug.h"

using json = nlohmann::json;

void zugPlazieren(int gridX, int gridY, int zugArtId) {
    if (zugArtId == 0)
        return;
    if (IstZugBereitsVorhanden(gridX, gridY))
        return;

    if (!schautObSchiene(gridX, gridY)) {
        return;
    }

    const ZugArt* gefundeneZugArt = nullptr;
    for (const auto& zugArt : zugArtenListe) {
        if (zugArt.id == zugArtId) {
            gefundeneZugArt = &zugArt;
            break;
        }
    }

    if (!gefundeneZugArt) {
        return;
    }

    Zug neuerZug;
    neuerZug.zugId = (int)aktiveZuege.size() + 1;
    neuerZug.posX = gridX;
    neuerZug.posY = gridY;
    neuerZug.rotation = 0;

    neuerZug.name = gefundeneZugArt->name;
    neuerZug.passagiere = gefundeneZugArt->passagiere;
    neuerZug.gueter = gefundeneZugArt->gueter;
    neuerZug.zugtyp = gefundeneZugArt->zugtyp;
    neuerZug.farbe = gefundeneZugArt->farbe;
    neuerZug.biildpfad = gefundeneZugArt->biildpfad;
    neuerZug.geschwindichkeit = gefundeneZugArt->geschwindichkeit;

    neuerZug.Fahrplan = {};

    aktiveZuege.push_back(neuerZug);
    AktiveZuegeSpeichern();

    std::cout << "Zug platziert: " << neuerZug.name
        << " an Position [" << gridX << ", " << gridY << "]"
        << " mit ID: " << neuerZug.zugId << std::endl;
}

bool IstZugBereitsVorhanden(int gridX, int gridY) {
    for (const auto& zug : aktiveZuege) {
        if (zug.posX == gridX && zug.posY == gridY) {
            return true;
        }
    }
    return false;
}

void AktiveZuegeSpeichern() {
    json jsonDaten;
    json zuegeArray = json::array();

    for (const auto& zug : aktiveZuege) {
        json zugJson;
        zugJson["zugId"] = zug.zugId;
        zugJson["posX"] = zug.posX;
        zugJson["posY"] = zug.posY;
        zugJson["rotation"] = zug.rotation;
        zugJson["name"] = zug.name;
        zugJson["passagiere"] = zug.passagiere;
        zugJson["gueter"] = zug.gueter;
        zugJson["Fahrplan"] = zug.Fahrplan;
        zugJson["zugtyp"] = zug.zugtyp;
        zugJson["farbe"] = zug.farbe;
        zugJson["biildpfad"] = zug.biildpfad;
        zugJson["geschwindichkeit"] = zug.geschwindichkeit;
        zuegeArray.push_back(zugJson);
    }

    jsonDaten["AktiveZuege"] = zuegeArray;

    std::ofstream datei("resurses/json/AktiveZuege.json");
    if (datei.is_open()) {
        datei << jsonDaten.dump(4);
        datei.close();
    }
    else {
        std::cerr << "Fehler: AktiveZuege.json konnte nicht geöffnet werden!" << std::endl;
    }
}
void ZeichneZuege() {
    for (const auto& zug : aktiveZuege) {
        DrawRectangle(
            zug.posX * GRID_SIZE + GRID_SIZE / 4,
            zug.posY * GRID_SIZE + GRID_SIZE / 4,
            GRID_SIZE / 2,
            GRID_SIZE / 2,
            Color{ 0, 0, 255, 150 }
		);
	}
}