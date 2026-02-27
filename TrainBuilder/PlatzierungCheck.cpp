#include "PlatzierungCheck.h"
#include "globals.h"
#include "Banhof.h"
#include "knoten.h"
#include "Leiter.h"
#include "Inverter.h"
#include "Marker.h"
#include "Ampel.h"


static bool HatGleis(int x, int y) {
    return schautObSchiene(x, y);
}

static bool HatAmpel(int x, int y) {
    for (const auto& a : ampelListe) {
        if (a.GridX == x && a.GridY == y) return true;
    }
    return false;
}

static bool HatTechnikObjekt(int x, int y) {
    return istKnotenVorhanden(x, y)
        || IstLeiterVorhanden(x, y)
        || IstInverterVorhanden(x, y)
        || IstMarkerVorhanden(x, y);
}

static bool IstFeldLeer(int x, int y) {
    return !HatGleis(x, y)
        && !IstBanhofBereitsVorhanden(x, y)
        && !HatAmpel(x, y)
        && !HatTechnikObjekt(x, y);
}

// -----------------------------------------------------------------------
// Prüffunktion
// -----------------------------------------------------------------------

bool KannPlatzieren(int gridX, int gridY, PlatzierTyp typ) {
    switch (typ) {

        // --- Gleise: nur auf komplett leere Felder ---
    case PlatzierTyp::Gleis:
        return IstFeldLeer(gridX, gridY);

        // --- Bahnhof: nur auf vorhandenes Gleis, kein Bahnhof doppelt ---
    case PlatzierTyp::Banhof:
        if (!HatGleis(gridX, gridY))              return false;
        if (IstBanhofBereitsVorhanden(gridX, gridY)) return false;
        return true;

        // --- Ampel: nur auf vorhandenes Gleis, keine Ampel doppelt ---
    case PlatzierTyp::Ampel:
        if (!HatGleis(gridX, gridY)) return false;
        if (HatAmpel(gridX, gridY))  return false;
        return true;

        // --- Knoten, Leiter, Inverter, Marker: nur auf leere Felder (kein Gleis, kein anderes Technik-Objekt) ---
    case PlatzierTyp::Knoten:
    case PlatzierTyp::Leiter:
    case PlatzierTyp::Inverter:
    case PlatzierTyp::Marker:
        return IstFeldLeer(gridX, gridY);

    default:
        return false;
    }
}