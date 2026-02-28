#pragma once
#include <string>

// Setzt den Basispfad für alle JSON-Operationen (z.B. "welten/MeineWelt")
// Muss VOR LadeJson()/SpeicherJson() aufgerufen werden.
void SetzeWeltPfad(const std::string& pfad);

// Gibt den aktuellen JSON-Basispfad zurück (z.B. "welten/MeineWelt/json")
std::string GetJsonPfad();

void LadeJson();
void SpeicherJson();

std::string GetJsonPfad();
std::string GetFullPath(const std::string& filename);