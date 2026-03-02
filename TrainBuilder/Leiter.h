#pragma once
#include "globals.h"

void LeiterPlazieren(int gridX, int gridY);
void LeiterSpeichern();
void LeiterZeichnen();
void LeiterLöschen(int gridX, int gridY);
bool IstLeiterVorhanden(int gridX, int gridY);

void CheckInput();
void CheckOutput();