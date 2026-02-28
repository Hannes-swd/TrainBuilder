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
#include "Json.h"

using json = nlohmann::json;

static std::map<int, int> zugFahrplanIndex;
static std::map<int, std::vector<Vector2>> zugWege;
static std::map<int, int> zugWegIndex;
static std::map<int, float> zugMoveProgress;
static std::map<int, float> zugWaitTime;
static std::map<int, float> zugWaitTimer;

bool IstRoteAmpelAnPosition(int gridX, int gridY) {
    for (const auto& ampel : ampelListe) {
        if (ampel.GridX == gridX && ampel.GridY == gridY && !ampel.isGreen) {
            return true;
        }
    }
    return false;
}

bool ZugIdExistiert(int id) {
    for (const auto& zug : aktiveZuege) {
        if (zug.zugId == id) {
            return true;
        }
    }
    return false;
}

int GenerateUniqueZugId() {
    int id = 1;
    while (ZugIdExistiert(id)) {
        id++;
    }
    return id;
}

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
    neuerZug.zugId = GenerateUniqueZugId();

    if (ZugIdExistiert(neuerZug.zugId)) {
        std::cerr << "FEHLER: zugId " << neuerZug.zugId << " existiert bereits!" << std::endl;
        return;
    }

    neuerZug.posX = gridX;
    neuerZug.posY = gridY;
    neuerZug.rotation = 0;

    neuerZug.name = "Zug " + std::to_string(neuerZug.zugId);
    neuerZug.zugtyp = gefundeneZugArt->zugtyp;
    neuerZug.farbe = gefundeneZugArt->farbe;
    neuerZug.biildpfad = gefundeneZugArt->biildpfad;
    neuerZug.geschwindichkeit = gefundeneZugArt->geschwindichkeit;

    neuerZug.Fahrplan = {};

    aktiveZuege.push_back(neuerZug);

    zugFahrplanIndex[neuerZug.zugId] = 0;
    zugWege[neuerZug.zugId] = {};
    zugWegIndex[neuerZug.zugId] = 0;
    zugMoveProgress[neuerZug.zugId] = 0.0f;
    zugWaitTime[neuerZug.zugId] = 0.0f;
    zugWaitTimer[neuerZug.zugId] = 0.0f;

    AktiveZuegeSpeichern();

    std::cout << "Zug platziert: " << neuerZug.name
        << " an Position [" << gridX << ", " << gridY << "]"
        << " mit eindeutiger ID: " << neuerZug.zugId << std::endl;
}

bool IstZugBereitsVorhanden(int gridX, int gridY) {
    for (const auto& zug : aktiveZuege) {
        if ((int)zug.posX == gridX && (int)zug.posY == gridY) {
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
        zugJson["Fahrplan"] = zug.Fahrplan;
        zugJson["zugtyp"] = zug.zugtyp;
        zugJson["farbe"] = zug.farbe;
        zugJson["biildpfad"] = zug.biildpfad;
        zugJson["geschwindichkeit"] = zug.geschwindichkeit;
        zuegeArray.push_back(zugJson);
    }

    jsonDaten["AktiveZuege"] = zuegeArray;

    std::ofstream datei(GetFullPath("AktiveZuege.json").c_str());
    if (datei.is_open()) {
        datei << jsonDaten.dump(4);
        datei.close();
        std::cout << "DEBUG: " << aktiveZuege.size() << " Zuege gespeichert" << std::endl;
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
            zug.farbe == "rot" ? RED :
            zug.farbe == "blau" ? BLUE :
            zug.farbe == "gruen" ? GREEN :
            zug.farbe == "gelb" ? YELLOW :
            zug.farbe == "violet" ? PURPLE :
            zug.farbe == "orange" ? ORANGE :
            GRAY
        );
    }
}

std::vector<Vector2> FindPathBFS(Vector2 start, Vector2 end) {
    std::queue<Vector2> q;
    std::map<std::string, Vector2> parent;
    std::map<std::string, bool> visited;

    q.push(start);
    visited[std::to_string((int)start.x) + "," + std::to_string((int)start.y)] = true;

    while (!q.empty()) {
        Vector2 current = q.front();
        q.pop();

        if ((int)current.x == (int)end.x && (int)current.y == (int)end.y) {
            std::vector<Vector2> path;
            Vector2 node = end;

            while (!((int)node.x == (int)start.x && (int)node.y == (int)start.y)) {
                path.insert(path.begin(), node);
                std::string key = std::to_string((int)node.x) + "," + std::to_string((int)node.y);
                node = parent[key];
            }
            path.insert(path.begin(), start);
            return path;
        }

        Vector2 neighbors[4] = {
            {current.x + 1, current.y},
            {current.x - 1, current.y},
            {current.x, current.y + 1},
            {current.x, current.y - 1}
        };

        for (int i = 0; i < 4; i++) {
            Vector2 neighbor = neighbors[i];
            std::string key = std::to_string((int)neighbor.x) + "," + std::to_string((int)neighbor.y);

            if (!visited[key] && schautObSchiene((int)neighbor.x, (int)neighbor.y)) {
                visited[key] = true;
                parent[key] = current;
                q.push(neighbor);
            }
        }
    }

    return { start };
}

void BewegeZuege() {
    for (auto& zug : aktiveZuege) {
        if (zug.Fahrplan.empty()) {
            continue;
        }

        if (zugFahrplanIndex.find(zug.zugId) == zugFahrplanIndex.end()) {
            zugFahrplanIndex[zug.zugId] = 0;
            zugWege[zug.zugId] = {};
            zugWegIndex[zug.zugId] = 0;
            zugMoveProgress[zug.zugId] = 0.0f;
            zugWaitTime[zug.zugId] = 0.0f;
            zugWaitTimer[zug.zugId] = 0.0f;
        }

        int stationIndex = zugFahrplanIndex[zug.zugId];

        if (stationIndex >= (int)zug.Fahrplan.size()) {
            zugFahrplanIndex[zug.zugId] = 0;
            zugWege[zug.zugId] = {};
            zugWegIndex[zug.zugId] = 0;
            zugMoveProgress[zug.zugId] = 0.0f;
            continue;
        }

        std::string targetStationName = zug.Fahrplan[stationIndex];
        BanhofObjeckt* targetStation = nullptr;

        for (auto& banhof : banhofListe) {
            if (banhof.Name == targetStationName) {
                targetStation = &banhof;
                break;
            }
        }

        if (!targetStation) {
            zugFahrplanIndex[zug.zugId]++;
            continue;
        }

        if (zugWege[zug.zugId].empty()) {
            Vector2 start = { zug.posX, zug.posY };
            Vector2 end = { (float)targetStation->GridX, (float)targetStation->GridY };
            zugWege[zug.zugId] = FindPathBFS(start, end);
            zugWegIndex[zug.zugId] = 0;
            zugMoveProgress[zug.zugId] = 0.0f;
        }

        std::vector<Vector2>& weg = zugWege[zug.zugId];
        int& wegIdx = zugWegIndex[zug.zugId];

        if (weg.empty() || wegIdx >= (int)weg.size()) {
            zugFahrplanIndex[zug.zugId]++;
            zugWege[zug.zugId] = {};
            zugWegIndex[zug.zugId] = 0;
            zugMoveProgress[zug.zugId] = 0.0f;
            continue;
        }

        if (wegIdx < (int)weg.size() - 1) {
            Vector2 current = weg[wegIdx];
            Vector2 next = weg[wegIdx + 1];

            if (IstRoteAmpelAnPosition((int)next.x, (int)next.y)) {
                // Zug wartet an der roten Ampel
                continue;
            }

            float dist = std::sqrt(
                (next.x - current.x) * (next.x - current.x) +
                (next.y - current.y) * (next.y - current.y)
            );

            float speed = (float)zug.geschwindichkeit * 0.5f;
            float movePerFrame = (speed * DeltaTime) / dist;
            zugMoveProgress[zug.zugId] += movePerFrame;

            if (zugMoveProgress[zug.zugId] >= 1.0f) {
                zug.posX = next.x;
                zug.posY = next.y;
                zugMoveProgress[zug.zugId] -= 1.0f;
                wegIdx++;
            }
            else {
                zug.posX = current.x + (next.x - current.x) * zugMoveProgress[zug.zugId];
                zug.posY = current.y + (next.y - current.y) * zugMoveProgress[zug.zugId];
            }

            float diffX = next.x - current.x;
            float diffY = next.y - current.y;
            zug.rotation = (int)(std::atan2(diffY, diffX) * 180.0f / 3.14159265f);
        }
        else {
            zug.posX = targetStation->GridX;
            zug.posY = targetStation->GridY;

            zugWaitTimer[zug.zugId] += DeltaTime;

            if (zugWaitTimer[zug.zugId] >= 3.0f) { // 3 Sekunden warten
                zugFahrplanIndex[zug.zugId]++;
                zugWege[zug.zugId] = {};
                zugWegIndex[zug.zugId] = 0;
                zugMoveProgress[zug.zugId] = 0.0f;
                zugWaitTimer[zug.zugId] = 0.0f;
            }
        }
    }
}