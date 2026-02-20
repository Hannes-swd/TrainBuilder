#pragma once
#include "Signal.h"

void AmpelPlazieren(int gridX, int gridY);
void AmpelZeichnen();
void AmpelSpeichern();

void AmpelUmschalten(int gridX, int gridY);
void AmpelUmschaltenNachId(int ampelId);
void AmpelAufGruen(int gridX, int gridY);
void AmpelAufRot(int gridX, int gridY);
void AmpelSignalSynchronisieren(const ampel& a);
