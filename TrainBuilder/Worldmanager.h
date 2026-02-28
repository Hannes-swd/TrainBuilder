#pragma once
#include <string>
#include <vector>
#include "Weltfilesystem.h"  // WeltInfo, LadeAlleWelten, ErstelleNeueWelt, LadeWelt, LöscheWelt, GetAktuelleWeltPfad

// Zeichnet das Weltauswahl-Menü und verarbeitet Eingaben.
// Gibt true zurück wenn eine Welt ausgewählt wurde.
bool WeltauswahlUpdate();

// Setzt den internen Zustand der Weltauswahl zurück (Weltliste, Dialoge, Scroll).
// Muss aufgerufen werden bevor man vom Spiel zurück ins Menü wechselt.
void ResetWeltauswahl();