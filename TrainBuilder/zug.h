#pragma once
#include "raylib.h"
#include <vector>

void zugPlazieren(int gridX, int gridY, int zugArtId);
bool IstZugBereitsVorhanden(int gridX, int gridY);
void AktiveZuegeSpeichern();
void ZeichneZuege();
void BewegeZuege();
std::vector<Vector2> FindPathBFS(Vector2 start, Vector2 end);
bool IstRoteAmpelAnPosition(int gridX, int gridY);