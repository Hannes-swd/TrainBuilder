#include "NaviPlan.h"
#include "raylib.h"
#include "globals.h"
#include "Banhof.h"
#include "Navi.h"

static float NaviClamp(float v, float lo, float hi) {
    return v < lo ? lo : v > hi ? hi : v;
}

void ZeichneNaviplan(int id, float& scrollOffset, bool& isScrolling, float containerY)
{
    // Navi-Objekt finden
    NaviObjeckt* navi = nullptr;          // NaviObjeckt statt Navi!
    for (auto& n : NaviListe) {
        if (n.eindeutigeId == id) { navi = &n; break; }
    }
    if (!navi) return;

    const float panelX = (float)GenaueBreite - 250.0f;
    const float contentX = panelX + 10.0f;
    const float contentW = 250.0f - 20.0f - SCROLLBAR_BREITE;
    const float rowH = 28.0f;
    const float btnH = 24.0f;

    float y = containerY - scrollOffset;

    // Überschrift
    DrawText("Bahnhof-Liste:", contentX, y, 18, DARKGRAY);
    y += 24.0f;

    // Einträge
    for (int i = 0; i < (int)navi->Haltestellen.size(); i++) {
        NaviHaltestelle& hs = navi->Haltestellen[i];
        float rowY = y;

        Color rowBg = (i % 2 == 0) ? Color{ 220,220,220,255 } : Color{ 200,200,200,255 };
        DrawRectangle(contentX, rowY, contentW, rowH - 2, rowBg);

        std::string bName = "Banhof " + std::to_string(hs.banhofId);
        for (const auto& b : banhofListe)
            if (b.BanhofId == hs.banhofId) { bName = b.Name; break; }
        DrawText(bName.c_str(), contentX + 4, rowY + 5, 14, BLACK);

        // Modus-Button
        float modBtnW = 68.0f;
        float modBtnX = contentX + contentW - modBtnW - btnH - 4.0f;
        float modBtnY = rowY + 2.0f;
        Color modCol = (hs.modus == NaviHaltestelle::HINZUFUEGEN)
            ? Color{ 50,180,50,255 } : Color{ 200,60,60,255 };
        const char* modLabel = (hs.modus == NaviHaltestelle::HINZUFUEGEN) ? "Add" : "Remove";

        DrawRectangle(modBtnX, modBtnY, modBtnW, btnH, modCol);
        DrawRectangleLines(modBtnX, modBtnY, modBtnW, btnH, WHITE);
        float tw = (float)MeasureText(modLabel, 13);
        DrawText(modLabel, modBtnX + (modBtnW - tw) / 2, modBtnY + (btnH - 13) / 2, 13, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (mp.x >= modBtnX && mp.x <= modBtnX + modBtnW &&
                mp.y >= modBtnY && mp.y <= modBtnY + btnH) {
                hs.modus = (hs.modus == NaviHaltestelle::HINZUFUEGEN)
                    ? NaviHaltestelle::ENTFERNEN : NaviHaltestelle::HINZUFUEGEN;
                NaviSpeichern();
            }
        }

        // Löschen-Button
        float delBtnX = contentX + contentW - btnH;
        float delBtnY = rowY + 2.0f;
        DrawRectangle(delBtnX, delBtnY, btnH, btnH, RED);
        DrawRectangleLines(delBtnX, delBtnY, btnH, btnH, WHITE);
        DrawText("X", delBtnX + (btnH - (float)MeasureText("X", 13)) / 2,
            delBtnY + (btnH - 13) / 2, 13, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (mp.x >= delBtnX && mp.x <= delBtnX + btnH &&
                mp.y >= delBtnY && mp.y <= delBtnY + btnH) {
                navi->Haltestellen.erase(navi->Haltestellen.begin() + i);
                NaviSpeichern();
                break;
            }
        }
        y += rowH;
    }

    // "Add stop" Button + Dropdown (1:1 wie ZugPlan)
    static bool  naviMenuOffen = false;
    static int   naviMenuFurId = 0;
    static bool  clickProcessed = false;

    y += 6.0f;
    float addBtnX = contentX;
    float addBtnY = y;
    float addBtnW = contentW;
    float addBtnH = 25.0f;

    Vector2 mousePos = GetMousePosition();
    bool addHov = (mousePos.x >= addBtnX && mousePos.x <= addBtnX + addBtnW &&
        mousePos.y >= addBtnY && mousePos.y <= addBtnY + addBtnH);

    DrawRectangle(addBtnX, addBtnY, addBtnW, addBtnH,
        addHov ? Color{ 150,200,150,255 } : Color{ 100,200,100,255 });
    DrawRectangleLines(addBtnX, addBtnY, addBtnW, addBtnH, DARKGREEN);
    DrawText("Add stop", addBtnX + 10, addBtnY + 5, 12, BLACK);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && addHov && !clickProcessed) {
        naviMenuOffen = !naviMenuOffen;
        naviMenuFurId = id;
        clickProcessed = true;
    }

    y += addBtnH + 4.0f;

    if (naviMenuOffen && naviMenuFurId == id) {
        // Verfügbare Bahnhöfe (noch nicht in der Liste)
        std::vector<const BanhofObjeckt*> verfuegbar;
        for (const auto& b : banhofListe) {
            bool drin = false;
            for (const auto& hs : navi->Haltestellen)
                if (hs.banhofId == b.BanhofId) { drin = true; break; }
            if (!drin) verfuegbar.push_back(&b);
        }

        float itemH = 18.0f;
        float itemPad = 1.0f;
        float headerH = 18.0f;
        float padTop = 5.0f;
        float padBot = 5.0f;
        float menuW = contentW;
        float menuH = padTop + headerH + padBot +
            (verfuegbar.empty() ? 20.0f : verfuegbar.size() * (itemH + itemPad));
        float menuX = addBtnX;
        float menuY = addBtnY + addBtnH + 3.0f;
        if (menuY + menuH > GenaueHoehe - 80.0f)
            menuY = addBtnY - menuH - 5.0f;

        DrawRectangle(menuX, menuY, menuW, menuH, LIGHTGRAY);
        DrawRectangleLines(menuX, menuY, menuW, menuH, DARKGRAY);
        DrawText("stations:", menuX + 8, menuY + padTop, 12, BLACK);

        if (verfuegbar.empty()) {
            DrawText("No train stations", menuX + 10, menuY + padTop + 20, 11, GRAY);
        }
        else {
            for (int i = 0; i < (int)verfuegbar.size(); i++) {
                float itemY = menuY + padTop + headerH + padBot + i * (itemH + itemPad);
                float itemX = menuX + 5.0f;
                float itemW = menuW - 10.0f;
                bool hov = (mousePos.x >= itemX && mousePos.x <= itemX + itemW &&
                    mousePos.y >= itemY && mousePos.y <= itemY + itemH);
                if (hov) DrawRectangle(itemX, itemY, itemW, itemH, Color{ 200,220,255,255 });
                DrawRectangleLines(itemX, itemY, itemW, itemH, DARKGRAY);
                DrawText(verfuegbar[i]->Name.c_str(), itemX + 5, itemY + 2, 11, BLACK);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hov && !clickProcessed) {
                    NaviHaltestelle neu;
                    neu.banhofId = verfuegbar[i]->BanhofId;
                    neu.modus = NaviHaltestelle::HINZUFUEGEN;
                    navi->Haltestellen.push_back(neu);
                    naviMenuOffen = false;
                    NaviSpeichern();
                    clickProcessed = true;
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !clickProcessed) {
            if (!(mousePos.x >= menuX && mousePos.x <= menuX + menuW &&
                mousePos.y >= menuY && mousePos.y <= menuY + menuH)) {
                naviMenuOffen = false;
                clickProcessed = true;
            }
        }
    }

    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        clickProcessed = false;

    // Scrollbar
    float totalContentH = (y + scrollOffset) - containerY;
    float viewportH = (float)GenaueHoehe - containerY - 50.0f;
    float maxScroll = totalContentH > viewportH ? totalContentH - viewportH : 0.0f;
    float sbX = (float)GenaueBreite - SCROLLBAR_BREITE;
    float sbY = containerY;
    float sbH = viewportH;

    if (maxScroll > 0.0f) {
        DrawRectangle(sbX, sbY, SCROLLBAR_BREITE, sbH, Color{ 200,200,200,255 });
        float thumbH = sbH * (sbH / (sbH + maxScroll));
        float thumbY = sbY + (scrollOffset / maxScroll) * (sbH - thumbH);
        DrawRectangle(sbX, thumbY, SCROLLBAR_BREITE, thumbH, isScrolling ? DARKGRAY : GRAY);
        DrawRectangleLines(sbX, thumbY, SCROLLBAR_BREITE, thumbH, BLACK);

        if (mousePos.x >= panelX && mousePos.x <= (float)GenaueBreite &&
            mousePos.y >= sbY && mousePos.y <= sbY + sbH) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
                scrollOffset = NaviClamp(scrollOffset - wheel * 30.0f, 0.0f, maxScroll);
        }

        Rectangle thumbRect = { sbX, thumbY, SCROLLBAR_BREITE, thumbH };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(GetMousePosition(), thumbRect))
            isScrolling = true;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            isScrolling = false;
        if (isScrolling) {
            float ratio = maxScroll / (sbH - thumbH);
            scrollOffset = NaviClamp(scrollOffset + GetMouseDelta().y * ratio, 0.0f, maxScroll);
        }
    }
    else {
        scrollOffset = 0.0f;
    }
}