#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "globals.h"

struct BanhofObjeckt;

void plaziereBanhof(Vector2 position);
bool IstBanhofBereitsVorhanden(int gridX, int gridY);
void BanhofSpeichern();
bool schautObSchiene(int gridX, int gridY);
void ZeichneBanhof();
bool IstZugImBanhof(const std::string& banhofId);
