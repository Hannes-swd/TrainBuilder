#include "raylib.h"
#include <string>

#include <cmath>
#include <fstream>
#include "json.hpp"
#include "globals.h"
#include "moofment.h"
#include "Json.h"

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
        //Höhe/Breite
        GenaueHöehe = GetScreenHeight();
        GenaueBreite = GetScreenWidth();

        if (GenaueHöehe != screenHeight)
            screenHeight = GenaueHöehe;
        if (GenaueBreite != screenWidth)
            screenWidth = GenaueBreite;

        //Spielerbewegen
        Spielermoovment();

        
        
        /*-------------------------------------------------
            ZEICHNEN
        -------------------------------------------------*/
        BeginDrawing();
        ClearBackground(RAYWHITE);

        
        BeginMode2D(Playercam);

        
        DrawRectangle(100, 100, 200, 200, BLUE);
        DrawRectangle(-300, -200, 100, 100, GREEN);

        
        EndMode2D();

        

        EndDrawing();
    }

    /*-------------------------------------------------
            BEENDEN
    -------------------------------------------------*/
    CloseWindow();

    return 0;
}