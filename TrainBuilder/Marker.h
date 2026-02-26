#pragma once
void MarkerPlazieren(int gridX, int gridY);
void MarkerSpeichern();
void MarkerZeichnen();
bool MarkerInSichtfeld(int markerId);
void NichtImFeldZeichnen();
void MarkerLöschen(int MouseGridX, int MouseGridY);
bool IstMarkerVorhanden(int MouseGridX, int MouseGridY);