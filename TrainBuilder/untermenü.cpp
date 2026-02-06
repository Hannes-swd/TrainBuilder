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
		//float untermenueHoehe = 100.0f;  // Höhe des Untermenüs
		DrawRectangle(0, 80.0f, (float)GenaueBreite, 100, LIGHTGRAY);
		DrawRectangleLines(0, 80, GenaueBreite, 100, DARKGRAY);
	}
}
void UntermenueKlick(Vector2 mausposition) {

}
void ZeichneUnterpunkte() {
    if (aktuellesUntermenue == "zugtool") {
        float buttonWidth = 80.0f;
        float buttonHeight = 60.0f;
        float startX = 10.0f;
        float startY = 90.0f;  // 10 Pixel unterhalb der Untermenü-Oberkante
        float padding = 10.0f;

        // Zeichne jeden Zugtyp als Button im Untermenü
        for (size_t i = 0; i < zugArtenListe.size(); i++) {
            float x = startX + (i * (buttonWidth + padding));
            float y = startY;

            // Zeichne Button-Hintergrund
            DrawRectangle(x, y, buttonWidth, buttonHeight, GRAY);
            DrawRectangleLines(x, y, buttonWidth, buttonHeight, DARKGRAY);

            // Zeichne Zugnamen (gekürzt wenn zu lang)
            std::string displayName = zugArtenListe[i].name;
            if (displayName.length() > 8) {
                displayName = displayName.substr(0, 8) + "...";
            }

            // Zentriere den Text im Button
            float textWidth = MeasureText(displayName.c_str(), 12);
            float textX = x + (buttonWidth - textWidth) / 2;
            float textY = y + (buttonHeight - 12) / 2;

            DrawText(displayName.c_str(), textX, textY, 12, BLACK);

            // Zeige zusätzliche Infos bei Mouseover
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x >= x && mousePos.x <= x + buttonWidth &&
                mousePos.y >= y && mousePos.y <= y + buttonHeight) {

                // Tooltip mit vollständigen Informationen
                std::string tooltip = "Geschwindigkeit: " + std::to_string(zugArtenListe[i].geschwindichkeit) +
                    "\nPassagiere: " + std::to_string(zugArtenListe[i].passagiere) +
                    "\nGüter: " + std::to_string(zugArtenListe[i].güter);

                // Zeichne Tooltip-Hintergrund
                float tooltipWidth = 150.0f;
                float tooltipHeight = 60.0f;
                float tooltipX = mousePos.x + 10;
                float tooltipY = mousePos.y + 10;

                // Stelle sicher, dass der Tooltip nicht außerhalb des Bildschirms ist
                if (tooltipX + tooltipWidth > GenaueBreite) {
                    tooltipX = mousePos.x - tooltipWidth - 10;
                }
                if (tooltipY + tooltipHeight > GenaueHoehe) {
                    tooltipY = mousePos.y - tooltipHeight - 10;
                }

                DrawRectangle(tooltipX, tooltipY, tooltipWidth, tooltipHeight, LIGHTGRAY);
                DrawRectangleLines(tooltipX, tooltipY, tooltipWidth, tooltipHeight, DARKGRAY);

                // Zeichne Tooltip-Text
                DrawText(zugArtenListe[i].name.c_str(), tooltipX + 5, tooltipY + 5, 12, BLACK);
                DrawText(TextFormat("Geschw.: %d", zugArtenListe[i].geschwindichkeit),
                    tooltipX + 5, tooltipY + 20, 10, DARKGRAY);
                DrawText(TextFormat("Passagiere: %d", zugArtenListe[i].passagiere),
                    tooltipX + 5, tooltipY + 32, 10, DARKGRAY);
                DrawText(TextFormat("Güter: %d", zugArtenListe[i].güter),
                    tooltipX + 5, tooltipY + 44, 10, DARKGRAY);
            }
        }
    }
}