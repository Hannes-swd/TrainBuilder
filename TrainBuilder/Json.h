#pragma once
#include <string>

// Gibt den aktuellen JSON-Basispfad zurück (z.B. "welten/MeineWelt/json")
std::string GetJsonPfad();

void LadeJson();
void SpeicherJson();

std::string GetFullPath(const std::string& filename);