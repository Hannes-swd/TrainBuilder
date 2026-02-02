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
            EINGABEN
        -------------------------------------------------*/
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            MausGecklicktL(mousePosition);
        }

        /*-------------------------------------------------
            VARIABLEN UPDATEN
        -------------------------------------------------*/
        DeltaTime = GetFrameTime();

        mousePosition = GetScreenToWorld2D(GetMousePosition(), Playercam);


        //Höhe/Breite
        GenaueHöehe = GetScreenHeight();
        GenaueBreite = GetScreenWidth();

        if (GenaueHöehe != screenHeight)
            screenHeight = GenaueHöehe;
        if (GenaueBreite != screenWidth)
            screenWidth = GenaueBreite;

        //Spielerbewegen
        Spielermoovment();

        //koordinaten im grid
        gridPosition.x = (int)(mousePosition.x / 50.0f);
        gridPosition.y = (int)(mousePosition.y / 50.0f);

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

        //zeichne block
        //DrawRectangle(4 * 50.0f, 4 * 50.0f, 50, 50, RED);

        ZeichneGleise();


        EndMode2D();
        /*-------------------------------------------------
            ALLES AUSERHALB VOM GRID MOOVMENT
        -------------------------------------------------*/

        DrawText(TextFormat("Grid: [%d, %d]", (int)gridPosition.x, (int)gridPosition.y),
            10, 10, 20, BLACK);

        EndDrawing();
    }

    /*-------------------------------------------------
            BEENDEN
    -------------------------------------------------*/
    SpeicherJson();
    unloadTextures();
    CloseWindow();
    

    return 0;
}