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

int draggedZugId = -1;
int draggedHaltestelleIndex = -1;
int dropLinePosition = -1;
float dragOffsetY = 0.0f;

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

            DrawText("timetable:", containerX + 10, containerY + 5, 14, BLACK);

            float contentHeight = 25.0f + (zug.Fahrplan.size() * 20.0f);
            float maxScroll = contentHeight - (calculatedHeight - 25.0f);
            if (maxScroll < 0) maxScroll = 0;

            BeginScissorMode(containerX + 5, containerY + 25, containerWidth - 20, calculatedHeight - 30);

            float contentY = containerY + 25 - scrollOffset;

            if (zug.Fahrplan.empty()) {
                DrawText("No timetables", containerX + 15, containerY + 30, 12, GRAY);
            }
            else {
                Vector2 mousePos = GetMousePosition();

                for (size_t i = 0; i < zug.Fahrplan.size(); i++) {
                    float itemY = contentY + (i * 20);
                    float itemX = containerX + 10;
                    float itemWidth = containerWidth - 40;
                    float itemHeight = 18.0f;

                    bool isHovered = (mousePos.x >= itemX && mousePos.x <= itemX + itemWidth &&
                        mousePos.y >= itemY && mousePos.y <= itemY + itemHeight);

                    Color itemColor;
                    if (draggedZugId == zug.zugId && draggedHaltestelleIndex == (int)i) {
                        itemColor = Color{ 200, 200, 255, 80 };
                    }
                    else if (isHovered && draggedZugId == -1) {
                        itemColor = Color{ 220, 220, 255, 255 };
                    }
                    else {
                        itemColor = Color{ 200, 200, 255, 255 };
                    }

                    DrawRectangle(itemX, itemY, itemWidth, itemHeight, itemColor);
                    DrawRectangleLines(itemX, itemY, itemWidth, itemHeight, DARKBLUE);
                    DrawText(zug.Fahrplan[i].c_str(), itemX + 5, itemY + 2, 12, BLACK);

                    // Löschen Button
                    float deleteButtonX = containerX + containerWidth - 30;
                    float deleteButtonY = itemY;
                    float deleteButtonSize = 18.0f;

                    DrawRectangle(deleteButtonX, deleteButtonY, deleteButtonSize, deleteButtonSize, RED);
                    DrawLine(deleteButtonX, deleteButtonY, deleteButtonX + deleteButtonSize, deleteButtonY + deleteButtonSize, WHITE);
                    DrawLine(deleteButtonX + deleteButtonSize, deleteButtonY, deleteButtonX, deleteButtonY + deleteButtonSize, WHITE);

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHovered) {
                        draggedZugId = zug.zugId;
                        draggedHaltestelleIndex = i;
                        dragOffsetY = mousePos.y - itemY;
                    }

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (mousePos.x >= deleteButtonX && mousePos.x <= deleteButtonX + deleteButtonSize &&
                            mousePos.y >= deleteButtonY && mousePos.y <= deleteButtonY + deleteButtonSize) {
                            zug.Fahrplan.erase(zug.Fahrplan.begin() + i);
                            AktiveZuegeSpeichern();
                            break;
                        }
                    }
                }

                // Drag-Handling
                if (draggedZugId == zug.zugId && draggedHaltestelleIndex >= 0) {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        Vector2 mousePos = GetMousePosition();

                        dropLinePosition = -1;
                        for (size_t i = 0; i < zug.Fahrplan.size(); i++) {
                            float targetItemY = contentY + (i * 20);
                            if (mousePos.y >= targetItemY && mousePos.y < targetItemY + 20) {
                                if (mousePos.y < targetItemY + 10) {
                                    dropLinePosition = i; 
                                }
                                else {
                                    dropLinePosition = i + 1;
                                }
                                break;
                            }
                        }

                        if (dropLinePosition == -1 && mousePos.y > contentY + (zug.Fahrplan.size() * 20)) {
                            dropLinePosition = zug.Fahrplan.size();
                        }

                        if (dropLinePosition >= 0) {
                            float containerX = GenaueBreite - 240.0f;
                            float containerWidth = 220.0f;
                            float itemX = containerX + 10;
                            float itemWidth = containerWidth - 40;

                            if (dropLinePosition < (int)zug.Fahrplan.size()) {
                                // Linie
                                float lineY = contentY + (dropLinePosition * 20);
                                DrawLine(itemX - 5, lineY - 2, itemX + itemWidth + 5, lineY - 2, GREEN);
                                DrawLine(itemX - 5, lineY - 1, itemX + itemWidth + 5, lineY - 1, GREEN);
                            }
                            else {
                                float lineY = contentY + (zug.Fahrplan.size() * 20);
                                DrawLine(itemX - 5, lineY + 2, itemX + itemWidth + 5, lineY + 2, GREEN);
                                DrawLine(itemX - 5, lineY + 1, itemX + itemWidth + 5, lineY + 1, GREEN);
                            }
                        }
                    }
                    else {
                        if (draggedHaltestelleIndex >= 0 && dropLinePosition >= 0) {
                            if (dropLinePosition != draggedHaltestelleIndex &&
                                dropLinePosition != draggedHaltestelleIndex + 1) {

                                std::string draggedElement = zug.Fahrplan[draggedHaltestelleIndex];

                                zug.Fahrplan.erase(zug.Fahrplan.begin() + draggedHaltestelleIndex);

                                int newInsertPos = dropLinePosition;
                                if (dropLinePosition > draggedHaltestelleIndex) {
                                    newInsertPos--;
                                }

                                zug.Fahrplan.insert(zug.Fahrplan.begin() + newInsertPos, draggedElement);

                                AktiveZuegeSpeichern();
                            }
                        }

                        draggedZugId = -1;
                        draggedHaltestelleIndex = -1;
                        dropLinePosition = -1;
                    }
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

void ZeichneDraggingItem(int zugId, float containerY) {
    if (draggedZugId != zugId || draggedHaltestelleIndex < 0) return;

    for (auto& zug : aktiveZuege) {
        if (zug.zugId == zugId && draggedHaltestelleIndex < (int)zug.Fahrplan.size()) {
            float containerX = GenaueBreite - 240.0f;
            float containerWidth = 220.0f;

            Vector2 mousePos = GetMousePosition();
            float dragItemX = containerX + 10;
            float dragItemWidth = containerWidth - 40;
            float dragItemHeight = 18.0f;
            float dragItemY = mousePos.y - 9;

            DrawRectangle(dragItemX - 2, dragItemY - 2, dragItemWidth + 4, dragItemHeight + 4,
                Color{ 0, 0, 0, 80 });

            // Dragging
            DrawRectangle(dragItemX, dragItemY, dragItemWidth, dragItemHeight,
                Color{ 100, 150, 255, 255 });
            DrawRectangleLines(dragItemX, dragItemY, dragItemWidth, dragItemHeight, DARKBLUE);
            DrawText(zug.Fahrplan[draggedHaltestelleIndex].c_str(), dragItemX + 5, dragItemY + 2, 12, BLACK);

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
            DrawText("Add stop", buttonX + 10, buttonY + 5, 12, BLACK);

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

                DrawText("stations:", menuX + 8, adjustedMenuY + topPadding, 12, BLACK);

                if (banhofListe.empty()) {
                    DrawText("No train stations", menuX + 10, adjustedMenuY + topPadding + 20, 11, GRAY);
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