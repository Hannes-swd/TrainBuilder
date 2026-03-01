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
#include "Weltfilesystem.h"

using json = nlohmann::json;



std::string aktuellerWeltPfad = "";

bool VerzeichnisExistiert(const std::string& pfad) {
    struct stat info;
    if (stat(pfad.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR) != 0;
}

bool ErstelleVerzeichnis(const std::string& pfad) {  
    return _mkdir(pfad.c_str()) == 0;
}

std::string AktuellesDatum() { 
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

                if (weltName == "vorlage" || weltName == "template" ||
                    weltName == "Vorlage" || weltName == "Template")
                    continue;

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

bool ErstelleNeueWelt(const std::string& weltName) {
    if (weltName.empty()) return false;

    std::string weltPfad = "welten/" + weltName;
    std::string jsonPfad = weltPfad + "/json";

    if (VerzeichnisExistiert(weltPfad)) return false;

    ErstelleVerzeichnis("welten");

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
                else if (objektName == "Ampeln")      objektName = "Ampeln";
                else if (objektName == "Banhof")      objektName = "BanhofObjeckte";
                else if (objektName == "Gate")        objektName = "Gate";
                else if (objektName == "Gleise")      objektName = "Objeckte";
                else if (objektName == "Inverter")    objektName = "Inverter";
                else if (objektName == "Knoten")      objektName = "Knoten";
                else if (objektName == "Leiter")      objektName = "Leiter";
                else if (objektName == "marker")      objektName = "marker";
                else if (objektName == "Signale")     objektName = "Signale";
                file << "{\n    \"" << objektName << "\": []\n}";
            }
            file.close();
        }
    }

    // Profilbild aus Vorlage kopieren
    std::string zielBild = weltPfad + "/Profilbild.png";
    const char* quellen[] = {
        "welten/vorlage/Profielbild.png",
        "welten/vorlage/Profilbild.png",
        "welten/template/Profilbild.png"
    };
    for (const auto& q : quellen) {
        std::ifstream src(q, std::ios::binary);
        if (src.is_open()) {
            std::ofstream dst(zielBild, std::ios::binary);
            dst << src.rdbuf();
            break;
        }
    }

    std::cout << "Welt '" << weltName << "' erfolgreich erstellt!" << std::endl;
    return true;
}

bool LadeWelt(const std::string& weltPfad) {
    if (!VerzeichnisExistiert(weltPfad)) return false;
    aktuellerWeltPfad = weltPfad;
    return true;
}

bool LoescheVerzeichnisRekursiv(const std::string& pfad) {
    struct _finddata_t fi;
    intptr_t h = _findfirst((pfad + "/*").c_str(), &fi);
    if (h != -1) {
        do {
            if (strcmp(fi.name, ".") == 0 || strcmp(fi.name, "..") == 0) continue;
            std::string voll = pfad + "/" + fi.name;
            if (fi.attrib & _A_SUBDIR)
                LoescheVerzeichnisRekursiv(voll);
            else
                remove(voll.c_str());
        } while (_findnext(h, &fi) == 0);
        _findclose(h);
    }
    return _rmdir(pfad.c_str()) == 0;
}

bool LoescheWelt(const std::string& weltPfad) {
    if (!VerzeichnisExistiert(weltPfad)) return false;
    bool ok = LoescheVerzeichnisRekursiv(weltPfad);
    if (ok) std::cout << "Welt geloescht: " << weltPfad << std::endl;
    else    std::cerr << "Fehler beim Loeschen: " << weltPfad << std::endl;
    return ok;
}

bool BenenneWeltUm(const std::string& alterPfad, const std::string& neuerName) {
    if (neuerName.empty()) return false;
    if (!VerzeichnisExistiert(alterPfad)) return false;

    std::string neuerPfad = "welten/" + neuerName;

    if (VerzeichnisExistiert(neuerPfad)) {
        std::cerr << "Fehler: Welt '" << neuerName << "' existiert bereits!" << std::endl;
        return false;
    }

    if (rename(alterPfad.c_str(), neuerPfad.c_str()) != 0) {
        std::cerr << "Fehler: Konnte Ordner nicht umbenennen!" << std::endl;
        return false;
    }

    std::cout << "Welt umbenannt: " << alterPfad << " -> " << neuerPfad << std::endl;
    return true;
}

std::string GetAktuelleWeltPfad() {
    return aktuellerWeltPfad;
}

void SetzeWeltPfad(const std::string& pfad) {
    aktuellerWeltPfad = pfad;
}