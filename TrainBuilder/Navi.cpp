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
    newNavi.ID = "Navi_" + std::to_string(NaviListe.size() + 1);
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
        NaviJson["ID"] = Navi.ID;

        json haltestellenArray = json::array();
        for (const auto& hs : Navi.Haltestellen) {
            json hsJson;
            hsJson["banhofId"] = hs.banhofId;
            hsJson["modus"] = (hs.modus == NaviHaltestelle::HINZUFUEGEN) ? 0 : 1;
            haltestellenArray.push_back(hsJson);
        }
        NaviJson["Haltestellen"] = haltestellenArray;

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

void NaviSynchronisieren() {
    bool zugGeaendert = false;

    for (const auto& navi : NaviListe) {
        if (!navi.Status) continue;

        for (auto& zug : aktiveZuege) {
            if (zug.ID != navi.ID) continue;

            for (const auto& hs : navi.Haltestellen) {
                std::string banhofName = "";
                for (const auto& banhof : banhofListe) {
                    if (banhof.BanhofId == hs.banhofId) {
                        banhofName = banhof.Name;
                        break;
                    }
                }

                if (banhofName.empty()) continue;

                bool bereitsVorhanden = false;
                for (const auto& eintrag : zug.Fahrplan) {
                    if (eintrag == banhofName) {
                        bereitsVorhanden = true;
                        break;
                    }
                }

                if (hs.modus == NaviHaltestelle::HINZUFUEGEN) {
                    if (!bereitsVorhanden) {
                        zug.Fahrplan.push_back(banhofName);
                        zugGeaendert = true;
                        std::cout << "Navi [" << navi.ID << "]: Bahnhof '"
                            << banhofName << "' zu Zug '"
                            << zug.name << "' hinzugefügt." << std::endl;
                    }
                }
                else if (hs.modus == NaviHaltestelle::ENTFERNEN) {
                    if (bereitsVorhanden) {
                        zug.Fahrplan.erase(
                            std::remove(zug.Fahrplan.begin(), zug.Fahrplan.end(), banhofName),
                            zug.Fahrplan.end()
                        );
                        zugGeaendert = true;
                        std::cout << "Navi [" << navi.ID << "]: Bahnhof '"
                            << banhofName << "' aus Zug '"
                            << zug.name << "' entfernt." << std::endl;
                    }
                }
            }
        }
    }

    if (zugGeaendert) {
        AktiveZuegeSpeichern();
    }
}
void NaviLöschen(int gridX, int gridY) {
    auto it = std::remove_if(NaviListe.begin(), NaviListe.end(),
        [gridX, gridY](const NaviObjeckt& l) {
            return l.GridX == gridX && l.GridY == gridY;
        });

    if (it != NaviListe.end()) {
        NaviListe.erase(it, NaviListe.end());
        NaviSpeichern();
    }
}
bool IstNaviVorhanden(int gridX, int gridY) {
    for (const auto& Navi : NaviListe) {
        if (Navi.GridX == gridX && Navi.GridY == gridY) {
            return true;
        }
    }
    return false;
}