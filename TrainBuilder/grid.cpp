#include "grid.h"
#include <cmath>

void DrawInfiniteGrid() {
    Color gridColor = Fade(LIGHTGRAY, 0.3f);
    
    const int MAX_LINES = 200;

    Vector2 camPos = Playercam.target;
    float zoom = Playercam.zoom;

    float screenWidth = (float)GenaueBreite / zoom;
    float screenHeight = (float)GenaueHoehe / zoom;

    float left = camPos.x - screenWidth / 2;
    float right = camPos.x + screenWidth / 2;
    float top = camPos.y - screenHeight / 2;
    float bottom = camPos.y + screenHeight / 2;

    float gridStartX = floor(left / GRID_SIZE) * GRID_SIZE;
    float gridStartY = floor(top / GRID_SIZE) * GRID_SIZE;

    int lineCount = 0;

    for (float x = gridStartX; x <= right && lineCount < MAX_LINES; x += GRID_SIZE) {
        Vector2 start = { x, top };
        Vector2 end = { x, bottom };
        DrawLineV(start, end, gridColor);
        lineCount++;
    }

    lineCount = 0;
    for (float y = gridStartY; y <= bottom && lineCount < MAX_LINES; y += GRID_SIZE) {
        Vector2 start = { left, y };
        Vector2 end = { right, y };
        DrawLineV(start, end, gridColor);
        lineCount++;
    }
}


