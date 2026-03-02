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
#include "PlatzierungCheck.h"
#include "Navi.h"
using json = nlohmann::json;


void NaviPlazieren(int gridX, int gridY) {
	if (!KannPlatzieren(gridX, gridY, PlatzierTyp::Navi)) return;

	NaviObjeckt newNavi;
	newNavi.GridX = gridX;
	newNavi.GridY = gridY;
    newNavi.Status = false;
	int neueID = 1;
	for (const auto& inv : NaviListe) {
		if (inv.eindeutigeId >= neueID) {
			neueID = inv.eindeutigeId + 1;
		}
	}
	newNavi.eindeutigeId = neueID;
	NaviListe.push_back(newNavi);
	NaviSpeichern();
}
void NaviSpeichern() {
    json jsonDaten;
    json NaviArray = json::array();
    for (const auto& Navi : NaviListe) {
        json NaviJson;
        NaviJson["GridX"] = Navi.GridX;
        NaviJson["GridY"] = Navi.GridY;
        NaviJson["eindeutigeId"] = Navi.eindeutigeId;
        NaviJson["Status"] = Navi.Status;
        NaviArray.push_back(NaviJson);
    }
    jsonDaten["Navi"] = NaviArray;

    std::ofstream NaviDatei(GetFullPath("Navi.json").c_str());
    if (NaviDatei.is_open()) {
        NaviDatei << jsonDaten.dump(4);
        NaviDatei.close();
    }
}
void NaviZeichnen() {
    for (const auto& Navi : NaviListe) {
        float pixelX = (float)(Navi.GridX * GRID_SIZE);
        float pixelY = (float)(Navi.GridY * GRID_SIZE);

        if (!Navi.Status)
            DrawTexture("Navi_An",
                pixelX, pixelY,
                GRID_SIZE, GRID_SIZE,
                WHITE);
        else
            DrawTexture("Navi_Aus",
                pixelX, pixelY,
                GRID_SIZE, GRID_SIZE,
                WHITE);
    }
}