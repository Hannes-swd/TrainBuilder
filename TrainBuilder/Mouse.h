#pragma once
#include "raylib.h"
void ProcesMaus(Vector2 mausposition);
void LinksGeklickt(Vector2 mausposition);
void ZeichnePriviou(Vector2 mausposition);
void PlatziereSchienenZwischenPunkten(Vector2 start, Vector2 end);
void PlatziereEinzelneSchiene(int gridX, int gridY);
bool IstGleisBereitsVorhanden(int gridX, int gridY);
