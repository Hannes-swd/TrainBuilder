#pragma once

enum class PlatzierTyp {
    Gleis,
    Banhof,
    Ampel, 
    Knoten,
    Leiter,
    Inverter,
    Marker,
    Gate,
    Navi
};

bool KannPlatzieren(int gridX, int gridY, PlatzierTyp typ);