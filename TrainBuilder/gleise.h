#include <string>
#pragma once
void ZeichneGleise();
void GleiseSpeichern();
void verbindeSchienen();
void ZeichnePriviou(Vector2 mausposition);
void PlatziereSchienenZwischenPunkten(Vector2 start, Vector2 end);
void PlatziereEinzelneSchiene(int gridX, int gridY);
bool IstGleisBereitsVorhanden(int gridX, int gridY);
