#include "Signal.h"
#include "json.hpp"
#include "globals.h"
#include <fstream>
#include <iostream>
#include "Ampel.h"

using json = nlohmann::json;


void SignalTeileLaden() {
    std::ifstream datei("resurses/json/Signale.json");
    if (!datei.is_open()) {
        std::cout << "Keine Signale.json gefunden  starte mit leerer Liste" << std::endl;
        return;
    }

    json jsonDaten;
    datei >> jsonDaten;
    datei.close();

    SignalTeilListe.clear();

    if (jsonDaten.contains("Signale")) {
        for (const auto& obj : jsonDaten["Signale"]) {
            SignalTeil signal;
            signal.eindeutigeId = obj["eindeutigeId"];
            signal.nutzerId = obj["nutzerId"];
            signal.wert = obj["wert"];
            signal.typ = (Signaltyp)obj["typ"];

            SignalTeilListe.push_back(signal);
        }
    }

    std::cout << "Signale geladen: " << SignalTeilListe.size() << std::endl;
}

void SignalTeilSpeichern() {
    json jsonDaten;
    json signalArray = json::array();

    for (const auto& signal : SignalTeilListe) {
        json signalJson;
        signalJson["eindeutigeId"] = signal.eindeutigeId;
        signalJson["nutzerId"] = signal.nutzerId;
        signalJson["wert"] = signal.wert;
        signalJson["typ"] = (int)signal.typ;

        signalArray.push_back(signalJson);
    }

    jsonDaten["Signale"] = signalArray;

    std::ofstream datei("resurses/json/Signale.json");
    if (datei.is_open()) {
        datei << jsonDaten.dump(4);
        datei.close();
        std::cout << "Signale gespeichert: " << SignalTeilListe.size() << std::endl;
    }
}

void SignalTeilHinzufuegen(int eindeutigeId, std::string nutzerId, bool wert, Signaltyp typ) {
    SignalTeil neu;
    neu.eindeutigeId = eindeutigeId;
    neu.nutzerId = nutzerId;
    neu.wert = wert;
    neu.typ = typ;

    SignalTeilListe.push_back(neu);
    SignalTeilSpeichern();
}

void SignalTeilEntfernen(int eindeutigeId) {
    for (auto it = SignalTeilListe.begin(); it != SignalTeilListe.end(); ++it) {
        if (it->eindeutigeId == eindeutigeId) {
            SignalTeilListe.erase(it);
            SignalTeilSpeichern();
            break;
        }
    }
}

SignalTeil* SucheSignalTeil(int eindeutigeId) {
    for (auto& signal : SignalTeilListe) {
        if (signal.eindeutigeId == eindeutigeId) {
            return &signal;
        }
    }
    return nullptr;
}

std::vector<SignalTeil*> SucheAlleMitNutzerId(std::string nutzerId) {
    std::vector<SignalTeil*> ergebnis;
    for (auto& signal : SignalTeilListe) {
        if (signal.nutzerId == nutzerId) {
            ergebnis.push_back(&signal);
        }
    }
    return ergebnis;
}

void UpdateSignale() {
    for (auto& knoten : knotenliste) {
        if (knoten.modus == false) {
            for (auto& signal : SignalTeilListe) {
                if (signal.nutzerId == knoten.Name) {
                    signal.wert = knoten.Status;
                }
            }
            for (auto& ampel : ampelListe) {
                if (ampel.Name == knoten.Name) {
                    ampel.isGreen = knoten.Status;
                }
            }
        }
        else {
            for (auto& ampel : ampelListe) {
                if (ampel.Name == knoten.Name) {
                    knoten.Status = ampel.isGreen;
                }
            }
            for (auto& signal : SignalTeilListe) {
                if (signal.nutzerId == knoten.Name) {
                    signal.wert = knoten.Status;
                }
            }
        }
    }
}