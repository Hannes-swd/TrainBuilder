#pragma once
#include "raylib.h"

// Einfach aufrufen – gibt die aktuell ausgewählte Farbe zurück.
// Intern merkt sich die Funktion den Zustand automatisch.
// 
// Beispiel:
//   Color meineFarbe = ColorPicker(50, 50, 300, 280);
//
Color ColorPicker(float x, float y, float width, float height);