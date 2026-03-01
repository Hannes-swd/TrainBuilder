#pragma once
#include <string>
#include <vector>


struct WeltInfo {
    std::string name;
    std::string pfad;
    std::string erstelltAm;
};

std::vector<WeltInfo> LadeAlleWelten();
bool ErstelleNeueWelt(const std::string& weltName);
bool LadeWelt(const std::string& weltPfad);
bool LoescheWelt(const std::string& weltPfad);
bool BenenneWeltUm(const std::string& alterPfad, const std::string& neuerName);
std::string GetAktuelleWeltPfad();
void SetzeWeltPfad(const std::string& pfad);