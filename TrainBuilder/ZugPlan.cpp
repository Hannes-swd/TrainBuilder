
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
            }
            else {
                for (size_t i = 0; i < zug.Fahrplan.size(); i++) {
                    DrawText(TextFormat("Eintrag %d", (int)i + 1), containerX + 15, contentY, 12, DARKGRAY);
                    contentY += 20;
                }
            }

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