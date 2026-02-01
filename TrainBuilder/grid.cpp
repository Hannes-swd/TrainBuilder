#include "grid.h"
#include <cmath>

void DrawInfiniteGrid() {
    Color gridColor = Fade(LIGHTGRAY, 0.3f);
    const float GRID_SIZE = 50.0f;

    // Kamera-Position
    Vector2 camPos = Playercam.target;
    float zoom = Playercam.zoom;

    float screenWidth = (float)GenaueBreite / zoom;
    float screenHeight = (float)GenaueHöehe / zoom;

    float left = camPos.x - screenWidth / 2;
    float right = camPos.x + screenWidth / 2;
    float top = camPos.y - screenHeight / 2;
    float bottom = camPos.y + screenHeight / 2;

    // Finde den nächsten Grid-Punkt
    float gridStartX = floor(left / GRID_SIZE) * GRID_SIZE;
    float gridStartY = floor(top / GRID_SIZE) * GRID_SIZE;

    // Zeichne vertikale Linien
    for (float x = gridStartX; x <= right; x += GRID_SIZE) {
        Vector2 start = { x, top };
        Vector2 end = { x, bottom };

        DrawLineV(start, end, gridColor);
    }

    // Zeichne horizontale Linien
    for (float y = gridStartY; y <= bottom; y += GRID_SIZE) {
        Vector2 start = { left, y };
        Vector2 end = { right, y };

        DrawLineV(start, end, gridColor);
    }
}