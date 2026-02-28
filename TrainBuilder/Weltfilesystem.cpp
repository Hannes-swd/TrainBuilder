
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <direct.h>
#include <io.h>   
#include <sys/stat.h>
#include "json.hpp"
#include "globals.h"

using json = nlohmann::json;

struct WeltInfo {
    std::string name;
    std::string pfad;
    std::string erstelltAm;
};

static std::string aktuellerWeltPfad = "";

static bool VerzeichnisExistiert(const std::string& pfad) {
    struct stat info;
    if (stat(pfad.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

static bool ErstelleVerzeichnis(const std::string& pfad) {
    return _mkdir(pfad.c_str()) == 0;
}

static std::string AktuellesDatum() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
#ifdef _WIN32
    localtime_s(&tstruct, &now);
#else
    tstruct = *localtime(&now);
#endif
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tstruct);
    return buf;
}

std::vector<WeltInfo> LadeAlleWelten() {
    std::vector<WeltInfo> welten;

    if (!VerzeichnisExistiert("welten")) {
        ErstelleVerzeichnis("welten");
        return welten;
    }

    struct _finddata_t fileInfo;
    intptr_t handle = _findfirst("welten/*", &fileInfo);

    if (handle != -1) {
        do {
            if ((fileInfo.attrib & _A_SUBDIR) &&
                strcmp(fileInfo.name, ".") != 0 &&
                strcmp(fileInfo.name, "..") != 0) {

                std::string weltName = fileInfo.name;
                std::string weltPfad = "welten/" + weltName;
                std::string infoDatei = weltPfad + "/welt.info";

                WeltInfo info;
                info.name = weltName;
                info.pfad = weltPfad;
                info.erstelltAm = "Unbekannt";

                std::ifstream file(infoDatei.c_str());
                if (file.is_open()) {
                    std::getline(file, info.erstelltAm);
                    file.close();
                }

                welten.push_back(info);
            }
        } while (_findnext(handle, &fileInfo) == 0);

        _findclose(handle);
    }

    return welten;
}

// Neue Welt erstellen
bool ErstelleNeueWelt(const std::string& weltName) {
    if (weltName.empty()) return false;

    std::string weltPfad = "welten/" + weltName;
    std::string jsonPfad = weltPfad + "/json";

    if (VerzeichnisExistiert(weltPfad)) {
        return false;
    }

    // Verzeichnisse erstellen
    if (!ErstelleVerzeichnis("welten")) {
        // Welten-Verzeichnis existiert bereits oder wurde erstellt
    }

    if (!ErstelleVerzeichnis(weltPfad)) {
        std::cerr << "Fehler: Konnte Weltverzeichnis nicht erstellen: " << weltPfad << std::endl;
        return false;
    }

    if (!ErstelleVerzeichnis(jsonPfad)) {
        std::cerr << "Fehler: Konnte JSON-Verzeichnis nicht erstellen: " << jsonPfad << std::endl;
        return false;
    }

    std::ofstream info((weltPfad + "/welt.info").c_str());
    if (info.is_open()) {
        info << AktuellesDatum();
        info.close();
    }

    // Leere JSON-Dateien erstellen
    std::vector<std::string> jsonDateien = {
        "AktiveZuege.json", "Ampeln.json", "Banhof.json",
        "Gate.json", "Gleise.json", "Inverter.json",
        "Knoten.json", "Leiter.json", "marker.json",
        "Signale.json", "User.json"
    };

    for (const auto& datei : jsonDateien) {
        std::string vollerPfad = jsonPfad + "/" + datei;
        std::ofstream file(vollerPfad.c_str());
        if (file.is_open()) {
            if (datei == "User.json") {
                file << "{\n    \"SpielerpositionX\": 0,\n    \"SpielerpositionY\": 0\n}";
            }
            else {
                std::string objektName = datei.substr(0, datei.find('.'));
                if (objektName == "AktiveZuege") objektName = "AktiveZuege";
                else if (objektName == "Ampeln") objektName = "Ampeln";
                else if (objektName == "Banhof") objektName = "BanhofObjeckte";
                else if (objektName == "Gate") objektName = "Gate";
                else if (objektName == "Gleise") objektName = "Objeckte";
                else if (objektName == "Inverter") objektName = "Inverter";
                else if (objektName == "Knoten") objektName = "Knoten";
                else if (objektName == "Leiter") objektName = "Leiter";
                else if (objektName == "marker") objektName = "marker";
                else if (objektName == "Signale") objektName = "Signale";

                file << "{\n    \"" << objektName << "\": []\n}";
            }
            file.close();
        }
        else {
            std::cerr << "Warnung: Konnte Datei nicht erstellen: " << vollerPfad << std::endl;
        }
    }

    std::cout << "Welt '" << weltName << "' erfolgreich erstellt!" << std::endl;
    return true;
}

// Welt laden
bool LadeWelt(const std::string& weltPfad) {
    if (!VerzeichnisExistiert(weltPfad)) {
        return false;
    }

    aktuellerWeltPfad = weltPfad;
    return true;
}

bool LöscheWelt(const std::string& weltPfad) {
    std::cout << "ACHTUNG: Zum vollständigen Löschen einer Welt bitte manuell den Ordner löschen: " << weltPfad << std::endl;
    std::cout << "Diese Funktion löscht nur die JSON-Dateien, nicht den Ordner selbst." << std::endl;

    // Hier könnten Sie eine Warnung ausgeben oder eine einfache Lösung implementieren
    // Für eine vollständige Implementierung wäre ein rekursives Löschen nötig

    // Alternative: Nur eine Markierung setzen
    return false;
}

std::string GetAktuelleWeltPfad() {
    return aktuellerWeltPfad;
}