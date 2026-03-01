#include "raylib.h"
#include <string>
#include "zug.h"
#include <cmath>
#include <fstream>
#include "json.hpp"
#include "globals.h"
#include "moofment.h"
#include "Json.h"
#include "grid.h"
#include "Mouse.h"
#include "LoadTexture.h"
#include "gleise.h"
#include "UI.h"
#include "Banhof.h"
#include "untermenü.h"
#include "ampel.h"
#include "knoten.h"
#include "Signal.h"
#include "Leiter.h"
#include "Inverter.h"
#include "mauszeiger.h"
#include "Marker.h"
#include "Gate.h"
#include "WorldManager.h" 

/*
* IDEEN
*
* - Erklerung für technik
* - ZUG-FAHRPLAN ender BLOCK
* - schalter
* - blueprint
* - displays
* - tutorial
* - welten speichern endern exportieren
*/

// -----------------------------------------------------------------------
// Spielzustand
// -----------------------------------------------------------------------
enum class Spielzustand {
    Weltauswahl,
    Spiel
};

// -----------------------------------------------------------------------
// Alle weltspezifischen Daten speichern
// -----------------------------------------------------------------------
static void WeltDatenSpeichern() {
    SpeicherJson();
    AktiveZuegeSpeichern();
    GleiseSpeichern();
    BanhofSpeichern();
    KnotenSpeichern();
    LeiterSpeichern();
    InvertorSpeichern();
    GateSpeichern();
    SignalTeilSpeichern();
    MarkerSpeichern();
}

// -----------------------------------------------------------------------
// Spielzustand zurücksetzen (für Rückkehr ins Menü)
// -----------------------------------------------------------------------
static void SpielzustandZuruecksetzen() {
    gleisListe.clear();
    banhofListe.clear();
    aktiveZuege.clear();
    ampelListe.clear();
    knotenliste.clear();
    SignalTeilListe.clear();
    LeiterListe.clear();
    InverterListe.clear();
    MarkerListe.clear();
    GateListe.clear();
    Spielerposition = { 0, 0 };

    // UI-Zustand zurücksetzen – verhindert, dass Untermenü/Tools
    // aus der alten Sitzung in der neuen Welt noch sichtbar sind
    untermenueOffen = false;
    aktuellesUntermenue = "";
    ausgewählterUntermenuePunkt = 0;
    aktuellesTool = 0;
    kannBewegen = true;

    // Mauszeiger zurücksetzen
    AktuelleMaus = "";
    EndereMausZu = "";
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

// -----------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------
int main(void)
{
    int screenWidth = 800;
    int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "TrainBuilder");
    SetTargetFPS(60);

    loadTextures();

    Spielzustand zustand = Spielzustand::Weltauswahl;

    // Kamera
    Playercam.offset = Vector2{ (float)screenWidth / 2, (float)screenHeight / 2 };
    Playercam.rotation = 0.0f;
    Playercam.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        SetExitKey(KEY_NULL);

        GenaueHoehe = GetScreenHeight();
        GenaueBreite = GetScreenWidth();
        if (GenaueHoehe != screenHeight) screenHeight = GenaueHoehe;
        if (GenaueBreite != screenWidth)  screenWidth = GenaueBreite;

        // ================================================================
        //  WELTAUSWAHL-MODUS
        // ================================================================
        if (zustand == Spielzustand::Weltauswahl)
        {
            BeginDrawing();

            bool weltGewaehlt = WeltauswahlUpdate();

            zeichneWeldOptionen();
            EndDrawing();

            if (weltGewaehlt) {
                // Bildschirmdimensionen aktualisieren
                GenaueHoehe = GetScreenHeight();
                GenaueBreite = GetScreenWidth();

                // Weltpfad setzen
                SetzeWeltPfad(GetAktuelleWeltPfad());

                // Spielzustand zurücksetzen und Daten laden
                SpielzustandZuruecksetzen();
                LadeJson();
                SignalTeileLaden();

                // Kamera vollständig zurücksetzen
                Spielerposition = { 0, 0 };
                Playercam.target = Spielerposition;
                Playercam.offset = Vector2{ (float)GenaueBreite / 2, (float)GenaueHoehe / 2 };
                Playercam.rotation = 0.0f;
                Playercam.zoom = 1.0f;

                // Zustand ändern
                zustand = Spielzustand::Spiel;

                std::cout << "Welt geladen: " << GetAktuelleWeltPfad() << std::endl;
                
            }
            

            continue;
        }

        // ================================================================
        //  SPIEL-MODUS
        // ================================================================

        if (IsKeyPressed(KEY_ESCAPE))
        {
            WeltDatenSpeichern();
            SpielzustandZuruecksetzen();
            ResetWeltauswahl();
            zustand = Spielzustand::Weltauswahl;
            continue;
        }

        DeltaTime = GetFrameTime();
        mousePosition = GetScreenToWorld2D(GetMousePosition(), Playercam);

        if (kannBewegen)
            Spielermoovment();

        gridPosition.x = (int)floor(mousePosition.x / GRID_SIZE);
        gridPosition.y = (int)floor(mousePosition.y / GRID_SIZE);

        BewegeZuege();
        UpdateSignale();

        if (AktuelleMaus != EndereMausZu) {
            mauszeiger(EndereMausZu);
            AktuelleMaus = EndereMausZu;
        }

        // ---- Zeichnen ----
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(Playercam);
        DrawInfiniteGrid();

        verbindeSchienen();

        for (auto& l : LeiterListe)   l.Status = false;
        for (auto& k : knotenliste)   if (!k.modus) k.Status = false;

        bool changed = true;
        int maxDurchlaeufe = (int)(LeiterListe.size() + InverterListe.size() + knotenliste.size() + 1);
        int durchlauf = 0;

        while (changed && durchlauf < maxDurchlaeufe) {
            changed = false;
            durchlauf++;

            std::vector<bool> leiterVorher, knotenVorher, inverterVorher;
            for (const auto& l : LeiterListe)   leiterVorher.push_back(l.Status);
            for (const auto& k : knotenliste)   knotenVorher.push_back(k.Status);
            for (const auto& inv : InverterListe) inverterVorher.push_back(inv.Status);

            GateOutput();
            CheckInput();
            CheckOutput();
            InvertorCheckInput();
            InvertorCheckOutput();

            for (int i = 0; i < (int)LeiterListe.size(); i++)
                if (LeiterListe[i].Status != leiterVorher[i]) { changed = true; break; }
            if (!changed)
                for (int i = 0; i < (int)knotenliste.size(); i++)
                    if (knotenliste[i].Status != knotenVorher[i]) { changed = true; break; }
            if (!changed)
                for (int i = 0; i < (int)InverterListe.size(); i++)
                    if (InverterListe[i].Status != inverterVorher[i]) { changed = true; break; }
        }

        UpdateSignale();

        ZeichneGleise();
        AmpelZeichnen();
        ZeichneZuege();
        ZeichneBanhof();
        KnotenZeichnen();
        LeiterZeichnen();
        InvertorZeichnen();
        GateZeichnen();
        MarkerZeichnen();

        ProcesMaus(mousePosition);

        

        EndMode2D();

        // HUD
        DrawText(TextFormat("Grid: [%d, %d]", (int)gridPosition.x, (int)gridPosition.y),
            10, 10, 20, BLACK);

        // Welt-Name anzeigen
        std::string weltName = GetAktuelleWeltPfad();
        size_t letzterSlash = weltName.find_last_of("/\\");
        if (letzterSlash != std::string::npos)
            weltName = weltName.substr(letzterSlash + 1);
        DrawText(("Welt: " + weltName).c_str(), GenaueBreite - 200, 10, 16, DARKGRAY);

        


        zeichneUI();
        NichtImFeldZeichnen();

        if (untermenueOffen) {
            UntermenueZeichnen();
            ZeichneUnterpunkte();
        }

        EndDrawing();
    }

    // ---- Aufräumen ----
    if (zustand == Spielzustand::Spiel) {
        WeltDatenSpeichern();
        std::cout << "Spieldaten gespeichert." << std::endl;
    }

    unloadTextures();
    CloseWindow();
    return 0;
}