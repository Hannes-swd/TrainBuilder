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

            float calculatedHeight;
            if (zug.Fahrplan.empty()) {
                calculatedHeight = 60.0f;
            }
            else {
                calculatedHeight = 25.0f + (zug.Fahrplan.size() * 20.0f) + 30.0f;
            }

            DrawRectangle(containerX, containerY, containerWidth, calculatedHeight, Color{ 255, 255, 255, 230 });
            DrawRectangleLines(containerX, containerY, containerWidth, calculatedHeight, BLACK);

            DrawText("Fahrplan:", containerX + 10, containerY + 5, 14, BLACK);

            float contentHeight = 25.0f + (zug.Fahrplan.size() * 20.0f);
            float maxScroll = contentHeight - (calculatedHeight - 25.0f);
            if (maxScroll < 0) maxScroll = 0;

            BeginScissorMode(containerX + 5, containerY + 25, containerWidth - 20, calculatedHeight - 30);

            float contentY = containerY + 25 - scrollOffset;

            if (zug.Fahrplan.empty()) {
                DrawText("Keine Fahrplaene", containerX + 15, containerY + 30, 12, GRAY);
            }
            else {
                for (size_t i = 0; i < zug.Fahrplan.size(); i++) {
                    DrawRectangle(containerX + 10, contentY + (i * 20), containerWidth - 30, 18, Color{ 200, 200, 255, 255 });
                    DrawRectangleLines(containerX + 10, contentY + (i * 20), containerWidth - 30, 18, DARKBLUE);
                    DrawText(zug.Fahrplan[i].c_str(), containerX + 15, contentY + (i * 20) + 2, 12, BLACK);
                }
            }

            EndScissorMode();

            NeueHaltestelleHinzufuegen(zug.zugId, containerY, calculatedHeight);

            if (maxScroll > 0) {
                float scrollbarX = containerX + containerWidth - SCROLLBAR_BREITE;
                float scrollbarY = containerY + 25;
                float scrollbarHeight = calculatedHeight - 30;

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

void NeueHaltestelleHinzufuegen(int zugId, float containerY, float containerHeight) {
    static int selectedZugId = 0;
    static float menuX = 0;
    static float menuY = 0;
    static bool clickProcessed = false;

    for (auto& zug : aktiveZuege) {
        if (zug.zugId == zugId) {
            float containerX = GenaueBreite - 240.0f;
            float containerWidth = 220.0f;

            float buttonX = containerX + 10.0f;
            float buttonY = containerY + containerHeight + 5.0f;
            float buttonWidth = 200.0f;
            float buttonHeight = 25.0f;

            Vector2 mousePos = GetMousePosition();
            bool isHovered = (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth &&
                mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight);

            DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight,
                isHovered ? Color{ 150, 200, 150, 255 } : Color{ 100, 200, 100, 255 });
            DrawRectangleLines(buttonX, buttonY, buttonWidth, buttonHeight, DARKGREEN);
            DrawText("Haltestelle hinzufuegen", buttonX + 10, buttonY + 5, 12, BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHovered && !clickProcessed) {
                haltestelleMenuOffen = !haltestelleMenuOffen;
                selectedZugId = zugId;
                menuX = buttonX;
                menuY = buttonY + buttonHeight + 3;
                clickProcessed = true;
            }

            if (haltestelleMenuOffen && selectedZugId == zugId) {
                float menuWidth = containerWidth - 20.0f;
                float itemHeight = 18.0f;
                float itemPadding = 1.0f;
                float headerHeight = 18.0f;
                float topPadding = 5.0f;
                float bottomPadding = 5.0f;

                float menuHeight = topPadding + headerHeight + bottomPadding;

                if (!banhofListe.empty()) {
                    menuHeight += (banhofListe.size() * (itemHeight + itemPadding));
                }
                else {
                    menuHeight += 20.0f;
                }

                float adjustedMenuY = menuY;
                if (adjustedMenuY + menuHeight > GenaueHoehe - 80.0f) {
                    adjustedMenuY = buttonY - menuHeight - 5;
                }

                DrawRectangle(menuX, adjustedMenuY, menuWidth, menuHeight, LIGHTGRAY);
                DrawRectangleLines(menuX, adjustedMenuY, menuWidth, menuHeight, DARKGRAY);

                DrawText("Bahnhoefe:", menuX + 8, adjustedMenuY + topPadding, 12, BLACK);

                if (banhofListe.empty()) {
                    DrawText("Keine Bahnhoefe", menuX + 10, adjustedMenuY + topPadding + 20, 11, GRAY);
                }
                else {
                    for (size_t i = 0; i < banhofListe.size(); i++) {
                        float itemY = adjustedMenuY + topPadding + headerHeight + bottomPadding + (i * (itemHeight + itemPadding));
                        float itemX = menuX + 5.0f;
                        float itemWidth = menuWidth - 10.0f;

                        bool itemHovered = (mousePos.x >= itemX && mousePos.x <= itemX + itemWidth &&
                            mousePos.y >= itemY && mousePos.y <= itemY + itemHeight);

                        if (itemHovered) {
                            DrawRectangle(itemX, itemY, itemWidth, itemHeight, Color{ 200, 220, 255, 255 });
                        }

                        DrawRectangleLines(itemX, itemY, itemWidth, itemHeight, DARKGRAY);
                        DrawText(banhofListe[i].Name.c_str(), itemX + 5, itemY + 2, 11, BLACK);

                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && itemHovered && !clickProcessed) {
                            zug.Fahrplan.push_back(banhofListe[i].Name);
                            haltestelleMenuOffen = false;
                            AktiveZuegeSpeichern();
                            clickProcessed = true;
                        }
                    }
                }

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !clickProcessed) {
                    if (!(mousePos.x >= menuX && mousePos.x <= menuX + menuWidth &&
                        mousePos.y >= adjustedMenuY && mousePos.y <= adjustedMenuY + menuHeight)) {
                        haltestelleMenuOffen = false;
                        clickProcessed = true;
                    }
                }
            }

            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                clickProcessed = false;
            }

            break;
        }
    }
}