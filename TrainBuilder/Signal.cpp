#include "Signal.h"
#include "json.hpp"
#include "globals.h"
#include <fstream>
#include <iostream>
#include "Ampel.h"
#include "Banhof.h"
#include "Json.h"


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

    std::ofstream datei(GetFullPath("Signale.json").c_str());
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
        if (knoten.modus == true) { 
            bool istBanhofId = false;

            // Bahnhof ID gibt
            for (const auto& banhof : banhofListe) {
                if (banhof.ID == knoten.Name) {
                    istBanhofId = true;
                    break;
                }
            }

            if (istBanhofId) {
                // Bahnhof-Status  (ob ein Zug drin steht)
                bool zugImBanhof = IstZugImBanhof(knoten.Name);

                // Knoten-Status
                knoten.Status = zugImBanhof;

               
                for (auto& signal : SignalTeilListe) {
                    if (signal.nutzerId == knoten.Name) {
                        signal.wert = knoten.Status;
                    }
                }

                // Ampeln 
                for (auto& ampel : ampelListe) {
                    if (ampel.Name == knoten.Name) {
                        ampel.isGreen = knoten.Status;
                    }
                }
            }
            else {
                //  Leselogik Knoten
                for (auto& ampel : ampelListe) {
                    if (ampel.Name == knoten.Name) {
                        knoten.Status = ampel.isGreen;
                        break;
                    }
                }

                for (auto& signal : SignalTeilListe) {
                    if (signal.nutzerId == knoten.Name) {
                        signal.wert = knoten.Status;
                    }
                }
            }
        }
        else { // Schreibmodus
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
    }

    // UpdateSignale-Logik
    for (const auto& Inverter : InverterListe) {
        if (!Inverter.Status) continue;

        int targetX = Inverter.GridX;
        int targetY = Inverter.GridY;

        if (Inverter.Rotation == 0)      targetY = Inverter.GridY - 1;
        else if (Inverter.Rotation == 1) targetX = Inverter.GridX + 1;
        else if (Inverter.Rotation == 2) targetY = Inverter.GridY + 1;
        else if (Inverter.Rotation == 3) targetX = Inverter.GridX - 1;

        for (auto& knoten : knotenliste) {
            if (knoten.GridX == targetX && knoten.GridY == targetY && !knoten.modus) {
                for (auto& signal : SignalTeilListe) {
                    if (signal.nutzerId == knoten.Name) {
                        signal.wert = true;
                    }
                }
                for (auto& ampel : ampelListe) {
                    if (ampel.Name == knoten.Name) {
                        ampel.isGreen = true;
                    }
                }
                break;
            }
        }
    }
}