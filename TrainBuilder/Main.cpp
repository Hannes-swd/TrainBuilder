#include "raylib.h"
#include <string>

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

int main(void)
{
    //STANDART EINSTELLUNGEN
    int screenWidth = 800;
    int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "TrainBuilder");

    //KAMMERA
    Playercam.target = Spielerposition;
    Playercam.offset = Vector2{ (float)screenWidth / 2, (float)screenHeight / 2 };
    Playercam.rotation = 0.0f;
    Playercam.zoom = 1.0f;
    /*-------------------------------------------------
        ALLES LADEN
    -------------------------------------------------*/
    LadeJson();
    loadTextures();



    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        /*-------------------------------------------------
            GRUNDEINSTELLUNGEN
        -------------------------------------------------*/
        SetExitKey(KEY_NULL);

        
        
        /*-------------------------------------------------
            VARIABLEN UPDATEN
        -------------------------------------------------*/
        DeltaTime = GetFrameTime();

        mousePosition = GetScreenToWorld2D(GetMousePosition(), Playercam);


        //Höhe/Breite
        GenaueHoehe = GetScreenHeight();
        GenaueBreite = GetScreenWidth();

        if (GenaueHoehe != screenHeight)
            screenHeight = GenaueHoehe;
        if (GenaueBreite != screenWidth)
            screenWidth = GenaueBreite;

        bool eingabefeldAktiv = false;
        //Spielerbewegen
        if(!eingabefeldAktiv) {
            Spielermoovment();
        }

        //koordinaten im grid
        gridPosition.x = (int)floor(mousePosition.x / GRID_SIZE);
        gridPosition.y = (int)floor(mousePosition.y / GRID_SIZE);

        


        /*-------------------------------------------------
            ZEICHNEN
        -------------------------------------------------*/
        BeginDrawing();
        ClearBackground(RAYWHITE);

        /*-------------------------------------------------
            ALLES IM GRID MOOVMENT
        -------------------------------------------------*/
        BeginMode2D(Playercam);
        DrawInfiniteGrid();

        
        ZeichneGleise();
        ZeichneBanhof();

        //gleise verbinden
        verbindeSchienen();

        /*-------------------------------------------------
            EINGABEN IM GRID
        -------------------------------------------------*/
        ProcesMaus(mousePosition);


        EndMode2D();
        /*-------------------------------------------------
            ALLES AUSERHALB VOM GRID MOOVMENT
        -------------------------------------------------*/

        DrawText(TextFormat("Grid: [%d, %d]", (int)gridPosition.x, (int)gridPosition.y),
            10, 10, 20, BLACK);

        //ZEICHNET UI


        zeichneUI();

        EndDrawing();
    }

    /*-------------------------------------------------
            BEENDEN
    -------------------------------------------------*/
    SpeicherJson();
    GleiseSpeichern();
    BanhofSpeichern();
    unloadTextures();
    CloseWindow();
    

    return 0;
}