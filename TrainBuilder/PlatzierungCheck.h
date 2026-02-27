#pragma once

enum class PlatzierTyp {
    Gleis,       // Nur auf leeren Feldern
    Banhof,      // Nur auf Gleisen
    Ampel,       // Nur auf Gleisen
    Knoten,      // Nur auf leeren Feldern (kein Gleis, kein anderes Objekt)
    Leiter,      // Nur auf leeren Feldern
    Inverter,    // Nur auf leeren Feldern
    Marker       // Nur auf leeren Feldern
};

bool KannPlatzieren(int gridX, int gridY, PlatzierTyp typ);