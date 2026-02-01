#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"
#include "Json.h"
void Spielermoovment() {
    Playercam.offset = Vector2{ (float)GenaueBreite / 2, (float)GenaueHöehe / 2 };

    Vector2 movement = { 0, 0 };
    if (IsKeyDown(KEY_W)) {
        movement.y -= 1;
    }
        
    if (IsKeyDown(KEY_S)) {
        movement.y += 1;
    }
        
    if (IsKeyDown(KEY_A)) {
        movement.x -= 1;
    }
        
    if (IsKeyDown(KEY_D)) {
        movement.x += 1;
    }
        

    float length = sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length > 0 && (movement.x != 0 || movement.y != 0))
    {
        movement.x /= length;
        movement.y /= length;
    }

    Spielerposition.x += movement.x * playerSpeed * DeltaTime;
    Spielerposition.y += movement.y * playerSpeed * DeltaTime;

    Playercam.target = Spielerposition;
}