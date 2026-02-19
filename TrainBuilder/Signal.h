#pragma once
#include <vector>
#include <string>
#include "raylib.h"
#include "globals.h"

extern std::vector<SignalTeil> SignalTeilListe;

void SignalTeilHinzufuegen(int eindeutigeId, std::string nutzerId, bool wert, Signaltyp typ);
void SignalTeilEntfernen(int eindeutigeId);
void SignalTeilSpeichern();
void SignalTeileLaden();
SignalTeil* SucheSignalTeil(int eindeutigeId);
std::vector<SignalTeil*> SucheAlleMitNutzerId(std::string nutzerId);
void UpdateSignale();
