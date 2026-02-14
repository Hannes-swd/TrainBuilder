
#include <iostream>
#include <ctime>
#include <cmath>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "zug.h"
#include "Ui.h"
#include "ZugPlan.h"


void ZeichneZugplan(int id, float& scrollOffset, bool& isScrolling, float containerY) {
    for (auto& zug : aktiveZuege) {
        if (zug.zugId == id) {
            float containerX = GenaueBreite - 240.0f;
            float containerWidth = 220.0f;
            float containerHeight = 120.0f;

            DrawRectangle(containerX, containerY, containerWidth, zugplanContainerHeight, Color{ 255, 255, 255, 230 });
            DrawRectangleLines(containerX, containerY, containerWidth, zugplanContainerHeight, BLACK);

            DrawText("Fahrplan:", containerX + 10, containerY + 5, 14, BLACK);

            float contentHeight = 25.0f + (zug.Fahrplan.size() * 20.0f);
            float maxScroll = contentHeight - (containerHeight - 25.0f);
            if (maxScroll < 0) maxScroll = 0;

            BeginScissorMode(containerX + 5, containerY + 25, containerWidth - 20, zugplanContainerHeight - 30);

            float contentY = containerY + 25 - scrollOffset;

            if (zug.Fahrplan.empty()) {
                DrawText("Keine Fahrplaene", containerX + 15, containerY + 30, 12, GRAY);
				zugplanContainerHeight = 60;
            }
            else {
                //macht den kasten gröser
				zugplanContainerHeight = 25.0f + (zug.Fahrplan.size() * 20.0f) + 30.0f;
                for (size_t i = 0; i < zug.Fahrplan.size(); i++) {
                    DrawRectangle(containerX + 10, contentY + (i * 20), containerWidth - 30, 18, Color{ 200, 200, 255, 255 });
                    DrawRectangleLines(containerX + 10, contentY + (i * 20), containerWidth - 30, 18, DARKBLUE);
					DrawText(zug.Fahrplan[i].c_str(), containerX + 15, contentY + (i * 20) + 2, 12, BLACK);
                }
            }
			//neue haltestelle hinzufügen button
            NeueHaltestelleHinzufuegen(zug.zugId, containerY);
            
            EndScissorMode();

            if (maxScroll > 0) {
                float scrollbarX = containerX + containerWidth - SCROLLBAR_BREITE;
                float scrollbarY = containerY + 25;
                float scrollbarHeight = containerHeight - 30;

                scrollOffset = ProccessScrollInput(scrollOffset, maxScroll,
                    scrollbarX, scrollbarY, scrollbarHeight, isScrolling,
                    containerX, containerWidth - SCROLLBAR_BREITE);

                ZeichneScrollbar(scrollOffset, maxScroll, isScrolling, scrollbarX, scrollbarY, scrollbarHeight);
            }
            else {
                scrollOffset = 0.0f;
            }
            break;
        }
    }
}
void NeueHaltestelleHinzufuegen(int zugId, float containerY) {
    for (auto& zug : aktiveZuege) {
        if (zug.zugId == zugId) {
   //         float buttonX = GenaueBreite - 240.0f + 10.0f;
   //         float buttonY = containerY + zugplanContainerHeight - 30.0f;
   //         float buttonWidth = 200.0f;
   //         float buttonHeight = 25.0f;
   //         DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, Color{ 100, 200, 100, 255 });
   //         DrawRectangleLines(buttonX, buttonY, buttonWidth, buttonHeight, DARKGREEN);
   //         DrawText("Neue Haltestelle hinzufügen", buttonX + 10, buttonY + 5, 12, BLACK);
   //         // Überprüfen auf Klick
   //         if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
   //             Vector2 mousePos = GetMousePosition();
   //             if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth &&
   //                 mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
   //                 zug.Fahrplan.push_back("Neue Haltestelle");
   //                 zugplanContainerHeight += 20.0f; 
   //             }
   //         }
			//break;

        }
    }
}