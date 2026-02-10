#pragma once
#include "raylib.h"

void zugPlazieren(int gridX, int gridY, int zugArtId);
bool IstZugBereitsVorhanden(int gridX, int gridY);
void AktiveZuegeSpeichern();
void ZeichneZuege();
void BewegeZuege();