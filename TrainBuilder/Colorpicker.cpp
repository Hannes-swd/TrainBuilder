#include "ColorPicker.h"
#include <cmath>
#include <algorithm>

// ---- interner Zustand (statisch, bleibt zwischen Frames erhalten) ----
static float s_hue = 0.0f;
static float s_saturation = 1.0f;
static float s_value = 1.0f;
static float s_cursorX = -1.0f;   // -1 = noch nicht initialisiert
static float s_cursorY = -1.0f;
static bool  s_draggingSv = false;
static bool  s_draggingHue = false;

// ---- Hilfsfunktionen ----
static float Clamp01(float v) { return v < 0.f ? 0.f : v > 1.f ? 1.f : v; }

static Color HsvToColor(float h, float s, float v) {
    if (s <= 0.f) {
        unsigned char c = (unsigned char)(v * 255);
        return { c, c, c, 255 };
    }
    float hh = (h >= 360.f ? 0.f : h) / 60.f;
    int   i = (int)hh;
    float ff = hh - i;
    float p = v * (1.f - s);
    float q = v * (1.f - s * ff);
    float t = v * (1.f - s * (1.f - ff));
    float r, g, b;
    switch (i) {
    case 0:  r = v; g = t; b = p; break;
    case 1:  r = q; g = v; b = p; break;
    case 2:  r = p; g = v; b = t; break;
    case 3:  r = p; g = q; b = v; break;
    case 4:  r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
    }
    return { (unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255), 255 };
}

// ---- Hauptfunktion ----
Color ColorPicker(float x, float y, float width, float height) {
    const float padding = 4.f;
    const float barHeight = height * 0.10f;
    const float sqHeight = height - barHeight - padding;

    Rectangle svRect = { x, y,                          width, sqHeight };
    Rectangle hueRect = { x, y + sqHeight + padding,     width, barHeight };

    // Cursor beim ersten Aufruf initialisieren (ganz rechts oben = volle Sättigung + Helligkeit)
    if (s_cursorX < 0.f) {
        s_cursorX = x + width;
        s_cursorY = y;
    }

    // ---- INPUT ----
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        // Hue-Streifen
        if (CheckCollisionPointRec(mouse, hueRect) || s_draggingHue) {
            s_draggingHue = true;
            float t = (mouse.x - hueRect.x) / hueRect.width;
            s_hue = Clamp01(t) * 360.f;
        }
        // SV-Quadrat
        if (CheckCollisionPointRec(mouse, svRect) || s_draggingSv) {
            s_draggingSv = true;
            s_saturation = Clamp01((mouse.x - svRect.x) / svRect.width);
            s_value = Clamp01(1.f - (mouse.y - svRect.y) / svRect.height);
            s_cursorX = x + s_saturation * width;
            s_cursorY = y + (1.f - s_value) * sqHeight;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        s_draggingHue = false;
        s_draggingSv = false;
    }

    // ---- DRAW ----

    // SV-Quadrat (spaltenweise, 1px-Streifen für glatten Verlauf)
    for (int i = 0; i < (int)width; i++) {
        float s = (float)i / (width - 1.f);
        DrawRectangleGradientV(
            (int)(x + i), (int)y, 1, (int)sqHeight,
            HsvToColor(s_hue, s, 1.f),   // oben: hell
            HsvToColor(s_hue, s, 0.f)    // unten: schwarz
        );
    }
    DrawRectangleLinesEx(svRect, 1.5f, ColorAlpha(WHITE, 0.25f));

    // Cursor-Kreis
    DrawCircleLines((int)s_cursorX, (int)s_cursorY, 6.f, WHITE);
    DrawCircleLines((int)s_cursorX, (int)s_cursorY, 5.f, BLACK);

    // Hue-Streifen
    for (int i = 0; i < (int)width; i++) {
        float hue = ((float)i / (width - 1.f)) * 360.f;
        DrawRectangle((int)(x + i), (int)hueRect.y, 1, (int)barHeight,
            HsvToColor(hue, 1.f, 1.f));
    }
    // Hue-Cursor (vertikale Linie)
    float hueX = x + (s_hue / 360.f) * width;
    DrawRectangle((int)hueX - 1, (int)hueRect.y, 3, (int)barHeight, WHITE);
    DrawRectangleLinesEx(hueRect, 1.5f, ColorAlpha(WHITE, 0.25f));

    return HsvToColor(s_hue, s_saturation, s_value);
}