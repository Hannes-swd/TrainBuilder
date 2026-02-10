#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"

void UntermenueZeichnen() {
    if (untermenueOffen) {
        DrawRectangle(0, 80.0f, (float)GenaueBreite, 100, LIGHTGRAY);
        DrawRectangleLines(0, 80, GenaueBreite, 100, DARKGRAY);
    }
}

void UntermenueKlick(Vector2 mausposition) {
    
    float boxWidth = 80.0f;
    float boxHeight = 80.0f;
    float padding = 10.0f;
    float startX = padding;
    float startY = 90.0f;

    for (size_t i = 0; i < zugArtenListe.size(); i++) {
        float x = startX + i * (boxWidth + padding);
        float y = startY;

        if (mausposition.x >= x && mausposition.x <= x + boxWidth &&
            mausposition.y >= y && mausposition.y <= y + boxHeight) {

            ausgewählterUntermenuePunkt = (int)i + 1;

			ausgewahlterZug = zugArtenListe[i].id;

            return;
        }
    }

}

void ZeichneUnterpunkte() {
    if (!untermenueOffen || zugArtenListe.empty()) return;

    float boxWidth = 80.0f;
    float boxHeight = 80.0f;
    float padding = 10.0f;
    float startX = padding;
    float startY = 90.0f;

    float untermenueY = 80.0f;
    float untermenueHoehe = 100.0f;
    float untermenueEndeY = untermenueY + untermenueHoehe;

    Vector2 mousePos = GetMousePosition();

    for (size_t i = 0; i < zugArtenListe.size(); i++) {
        const ZugArt& zug = zugArtenListe[i];

        float x = startX + i * (boxWidth + padding);
        float y = startY;

        bool isHovered = (mousePos.x >= x && mousePos.x <= x + boxWidth &&
            mousePos.y >= y && mousePos.y <= y + boxHeight);

        Color bgColor = isHovered ? GRAY : LIGHTGRAY;

        DrawRectangle(x, y, boxWidth, boxHeight, bgColor);
        DrawRectangleLines(x, y, boxWidth, boxHeight, DARKGRAY);


        if (ausgewählterUntermenuePunkt == (int)i + 1) {
            DrawRectangle(x, y, boxWidth, boxHeight, GRAY);
        }


        if (!zug.biildpfad.empty()) {
            Texture2D zugTexture = LoadTextureFromPath(zug.biildpfad);

            if (zugTexture.id != 0) {
                float imageHeight = boxHeight * 0.6f;
                float imageWidth = boxWidth * 0.8f;
                float imageX = x + (boxWidth - imageWidth) / 2;
                float imageY = y + 5;

                DrawTexturePro(
                    zugTexture,
                    Rectangle{ 0, 0, (float)zugTexture.width, (float)zugTexture.height },
                    Rectangle{ imageX, imageY, imageWidth, imageHeight },
                    Vector2{ 0, 0 },
                    0.0f,
                    WHITE
                );
            }
            else {
                const char* keinBildText = "Kein Bild";
                int textWidth = MeasureText(keinBildText, 10);
                DrawText(keinBildText,
                    (int)(x + boxWidth / 2 - textWidth / 2),
                    (int)(y + boxHeight / 2 - 5),
                    10, GRAY);
            }
        }
        else {
            const char* keinBildText = "Kein Bild";
            int textWidth = MeasureText(keinBildText, 10);
            DrawText(keinBildText,
                (int)(x + boxWidth / 2 - textWidth / 2),
                (int)(y + boxHeight / 2 - 5),
                10, GRAY);
        }

        if (!zug.name.empty()) {
            std::string displayName = zug.name;
            if (displayName.length() > 10) {
                displayName = displayName.substr(0, 8) + "..";
            }

            int textWidth = MeasureText(displayName.c_str(), 12);
            DrawText(displayName.c_str(),
                (int)(x + boxWidth / 2 - textWidth / 2),
                (int)(y + boxHeight - 20),
                12, BLACK);
        }
        
        if (isHovered) {
            float tooltipX = 10.0f;
            float tooltipY = untermenueEndeY + 10.0f;
            float tooltipWidth = 250.0f;
            float tooltipHeight = 130.0f;

            if (tooltipX + tooltipWidth > GenaueBreite) {
                tooltipWidth = GenaueBreite - tooltipX - 10;
            }

            DrawRectangle(tooltipX, tooltipY, tooltipWidth, tooltipHeight, Color{ 240, 240, 240, 230 });
            DrawRectangleLines(tooltipX, tooltipY, tooltipWidth, tooltipHeight, DARKGRAY);

            float textY = tooltipY + 10;
            float textX = tooltipX + 10;

            int nameWidth = MeasureText(zug.name.c_str(), 16);
            DrawText(zug.name.c_str(),
                textX + (tooltipWidth - 20 - nameWidth) / 2,
                textY, 16, BLACK);
            textY += 25;

            DrawLine(textX, textY - 5, textX + tooltipWidth - 20, textY - 5, DARKGRAY);

            float col1X = textX;
            float col2X = textX + 120;

            std::string speedText = "Geschwindigkeit:";
            DrawText(speedText.c_str(), col1X, textY, 12, DARKGRAY);

            std::string speedValue = std::to_string(zug.geschwindichkeit) + " km/h";
            DrawText(speedValue.c_str(), col2X, textY, 12, BLUE);
            textY += 18;

            
            std::string passText = "Max Passagiere:";
            DrawText(passText.c_str(), col1X, textY, 12, DARKGRAY);

            std::string passValue = std::to_string(zug.passagiere);
            DrawText(passValue.c_str(), col2X, textY, 12, BLUE);
            textY += 18;

            std::string gueterText = "Max Gueter:";
            DrawText(gueterText.c_str(), col1X, textY, 12, DARKGRAY);

            std::string gueterValue = std::to_string(zug.gueter);
            DrawText(gueterValue.c_str(), col2X, textY, 12, BLUE);
            textY += 18;

            if (!zug.zugtyp.empty()) {
                std::string typText = "Zugtyp:";
                DrawText(typText.c_str(), col1X, textY, 12, DARKGRAY);
                DrawText(zug.zugtyp.c_str(), col2X, textY, 12, DARKBLUE);
            }
        }  
    }
}