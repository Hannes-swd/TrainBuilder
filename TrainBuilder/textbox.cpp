#include "raylib.h"
#include "globals.h"
#include <string>
#include "textbox.h"
#include <cstdio>

TextBox::TextBox(float x, float y, float width, float height, int maxLen)
    : bounds{ x, y, width, height }, maxLength(maxLen), active(false), hover(false) {
}

void TextBox::Update() {
    Vector2 mousePos = GetMousePosition();

    hover = CheckCollisionPointRec(mousePos, bounds);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = hover;
    }

    if (active) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && text.length() < maxLength) {
                text += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
            active = false;
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }
}

void TextBox::Draw() const {
    Color bgColor = active ? LIGHTGRAY : (hover ? GRAY : RAYWHITE);
    Color borderColor = active ? BLUE : DARKGRAY;

    DrawRectangleRec(bounds, bgColor);
    DrawRectangleLinesEx(bounds, 2, borderColor);

    const char* displayText = text.empty() ? " " : text.c_str();
    DrawText(displayText, bounds.x + 5, bounds.y + 10, 20, BLACK);

    if (active && (static_cast<int>(GetTime() * 2) % 2 == 0)) {
        int textWidth = MeasureText(text.c_str(), 20);
        DrawText("|", bounds.x + 8 + textWidth, bounds.y + 10, 20, BLACK);
    }
}

void TextBox::SetPosition(float x, float y) {
    bounds.x = x;
    bounds.y = y;
}

void TextBox::SetSize(float width, float height) {
    bounds.width = width;
    bounds.height = height;
}

void TextBox::SetText(const std::string& newText) {
    text = newText;
    if (text.length() > maxLength) {
        text.resize(maxLength);
    }
}

const std::string& TextBox::GetText() const {
    return text;
}

bool TextBox::IsActive() const {
    return active;
}

void TextBox::Clear() {
    text.clear();
}