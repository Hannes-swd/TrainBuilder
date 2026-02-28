// WeltManager.h
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
bool LöscheWelt(const std::string& weltPfad);
std::string GetAktuelleWeltPfad();