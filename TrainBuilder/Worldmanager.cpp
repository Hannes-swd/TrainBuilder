#include "WorldManager.h"
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <string>

#include "raylib.h"
#include <iostream>
#include <vector>
#include "LoadTexture.h"
#include "Weltfilesystem.h"


std::string offeneOptionen = "";

// -----------------------------------------------------------------------
// Einfaches Texteingabe-Feld (intern)
// -----------------------------------------------------------------------
struct TextInput {
    char buffer[128];
    int  len;
    bool aktiv;

    TextInput() : len(0), aktiv(false) { buffer[0] = '\0'; }

    void Update() {
        if (!aktiv) return;
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && len < 80) {
                buffer[len++] = (char)key;
                buffer[len] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0)
            buffer[--len] = '\0';
    }

    void Clear() { len = 0; buffer[0] = '\0'; }

    void Set(const std::string& s) {
        len = (int)s.size();
        if (len > 80) len = 80;
        s.copy(buffer, len);
        buffer[len] = '\0';
    }

    std::string Get() const { return std::string(buffer); }
};

// -----------------------------------------------------------------------
// Interne Zeichen-Helfer
// -----------------------------------------------------------------------
static void Rahmen(Rectangle r, Color c) {
    DrawRectangleRoundedLines(r, 0.15f, 8, c);
}
static void RahmenRund(Rectangle r, Color c) {
    DrawRectangleRoundedLines(r, 0.3f, 8, c);
}

// -----------------------------------------------------------------------
// Reset-Flag
// -----------------------------------------------------------------------
static bool s_resetPending = false;

void ResetWeltauswahl() {
    s_resetPending = true;
}

// -----------------------------------------------------------------------
// WeltauswahlUpdate
// -----------------------------------------------------------------------
bool WeltauswahlUpdate() {
    static std::vector<WeltInfo> welten;
    static bool       weltenGeladen = false;
    static TextInput  nameInput;
    static bool       zeigeDialog = false;
    static std::string fehlerText = "";
    static float      fehlerTimer = 0.0f;
    static float      scrollOffset = 0.0f;

    static std::unordered_map<std::string, Texture2D> textureCache;
    static std::unordered_map<std::string, bool>      textureExistiert;

    // ---- Reset ----
    if (s_resetPending) {
        for (auto& pair : textureCache)
            if (pair.second.id != 0) UnloadTexture(pair.second);
        textureCache.clear();
        textureExistiert.clear();

        weltenGeladen = false;
        zeigeDialog = false;
        fehlerText = "";
        fehlerTimer = 0.0f;
        scrollOffset = 0.0f;
        nameInput.Clear();
        nameInput.aktiv = false;
        s_resetPending = false;
    }

    if (!weltenGeladen) {
        welten = LadeAlleWelten();
        weltenGeladen = true;
    }

    int     W = GetScreenWidth();
    int     H = GetScreenHeight();
    Vector2 maus = GetMousePosition();

    if (fehlerTimer > 0.0f) fehlerTimer -= GetFrameTime();

    ClearBackground(Color{ 30, 30, 40, 255 });

    // ---- Titel ----
    const char* titel = "TrainBuilder";
    DrawText(titel, W / 2 - MeasureText(titel, 48) / 2, 40, 48, Color{ 220, 180, 60, 255 });
    const char* sub = "world selection";
    DrawText(sub, W / 2 - MeasureText(sub, 22) / 2, 100, 22, Color{ 180, 180, 200, 255 });

    // ---- Weltenliste ----
    float lX = (float)(W / 2 - 250);
    float lY = 145.0f;
    float lW = 500.0f;
    float iH = 60.0f;
    float iA = 8.0f;
    float lH = (float)H - lY - 120.0f;

    float maxScroll = (float)welten.size() * (iH + iA) - lH;
    if (maxScroll < 0.0f) maxScroll = 0.0f;
    scrollOffset -= GetMouseWheelMove() * 30.0f;
    if (scrollOffset < 0.0f)      scrollOffset = 0.0f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    BeginScissorMode((int)lX - 5, (int)lY, (int)lW + 10, (int)lH);

    for (int i = 0; i < (int)welten.size(); i++) {
        float iy = lY + (float)i * (iH + iA) - scrollOffset;
        if (iy + iH < lY || iy > lY + lH) continue;

        float iW2 = lW;
        bool  hover = (maus.x >= lX && maus.x <= lX + iW2 &&
            maus.y >= iy && maus.y <= iy + iH);

        Color bg = hover ? Color{ 60, 80, 110, 255 } : Color{ 45, 55, 75, 255 };
        DrawRectangleRounded(Rectangle{ lX, iy, iW2, iH }, 0.15f, 8, bg);
        Rahmen(Rectangle{ lX, iy, iW2, iH }, Color{ 80, 100, 140, 255 });

        // Profilbild
        std::string bildPfad = welten[i].pfad + "/Profilbild.png";
        if (textureExistiert.find(bildPfad) == textureExistiert.end()) {
            std::ifstream bildTest(bildPfad.c_str());
            textureExistiert[bildPfad] = bildTest.good();
        }
        if (textureExistiert[bildPfad]) {
            if (textureCache.find(bildPfad) == textureCache.end()) {
                Image img = LoadImage(bildPfad.c_str());
                if (img.data != nullptr) {
                    ImageResize(&img, 40, 40);
                    Texture2D tex = LoadTextureFromImage(img);
                    UnloadImage(img);
                    if (tex.id != 0) textureCache[bildPfad] = tex;
                }
            }
            if (textureCache.find(bildPfad) != textureCache.end() && textureCache[bildPfad].id != 0)
                DrawTexture(textureCache[bildPfad], (int)lX + 10, (int)iy + 10, WHITE);
            else {
                DrawRectangle((int)lX + 10, (int)iy + 10, 40, 40, Color{ 70, 130, 180, 255 });
                DrawText("W", (int)lX + 22, (int)iy + 18, 22, WHITE);
            }
        }
        else {
            DrawRectangle((int)lX + 10, (int)iy + 10, 40, 40, Color{ 70, 130, 180, 255 });
            DrawText("W", (int)lX + 22, (int)iy + 18, 22, WHITE);
        }

        DrawText(welten[i].name.c_str(), (int)lX + 60, (int)iy + 10, 20, WHITE);
        std::string dat = "Created: " + welten[i].erstelltAm;
        DrawText(dat.c_str(), (int)lX + 60, (int)iy + 35, 13, Color{ 150, 160, 180, 255 });

        float optSize = 24.0f;
        float optX = lX + lW - optSize - 5.0f;
        float optY = iy + 5.0f;

        bool optHover = (maus.x >= optX && maus.x <= optX + optSize &&
            maus.y >= optY && maus.y <= optY + optSize);

        if (optHover) {
            DrawRectangle(optX, optY, optSize, optSize, Color{ 200, 200, 200, 255 });
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (offeneOptionen == "")
                    offeneOptionen = welten[i].name;
                else
                    offeneOptionen = "";
            }
        }
        else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !zeigeDialog && offeneOptionen == "") {
            if (hover) {
                LadeWelt(welten[i].pfad);
                weltenGeladen = false;
                EndScissorMode();
                return true;
            }
        }

        DrawTexture("options", optX, optY, optSize, optSize,
            optHover ? WHITE : Color{ 200, 200, 200, 255 });
    }

    EndScissorMode();

    if (welten.empty()) {
        const char* t = "No worlds available - create a new world!";
        DrawText(t, W / 2 - MeasureText(t, 15) / 2, H / 2, 15, Color{ 140, 140, 160, 255 });
    }

    // ---- Neue-Welt-Button ----
    float bW = 220.0f, bH = 45.0f;
    float bX = (float)W / 2.0f - bW / 2.0f;
    float bY = (float)H - 80.0f;
    bool  bHov = (maus.x >= bX && maus.x <= bX + bW &&
        maus.y >= bY && maus.y <= bY + bH);

    DrawRectangleRounded(Rectangle{ bX, bY, bW, bH }, 0.3f, 8,
        bHov ? DARKBLUE : VIOLET);
    RahmenRund(Rectangle{ bX, bY, bW, bH }, DARKBLUE);
    const char* bT = "New World";
    DrawText(bT, (int)(bX + bW / 2 - MeasureText(bT, 20) / 2),
        (int)(bY + bH / 2 - 10), 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bHov && !zeigeDialog) {
        zeigeDialog = true;
        nameInput.Clear();
        nameInput.aktiv = true;
        fehlerText = "";
    }

    if (fehlerTimer > 0.0f && !fehlerText.empty())
        DrawText(fehlerText.c_str(),
            W / 2 - MeasureText(fehlerText.c_str(), 16) / 2,
            (int)bY - 30, 16, RED);

    // ================================================================
    // Neue Welt erstellen
    // ================================================================
    if (zeigeDialog) {
        DrawRectangle(0, 0, W, H, Color{ 0, 0, 0, 160 });

        float dW3 = 420.0f, dH3 = 200.0f;
        float dX3 = (float)W / 2.0f - dW3 / 2.0f;
        float dY3 = (float)H / 2.0f - dH3 / 2.0f;

        DrawRectangleRounded(Rectangle{ dX3, dY3, dW3, dH3 }, 0.1f, 8, Color{ 40, 45, 60, 255 });
        Rahmen(Rectangle{ dX3, dY3, dW3, dH3 }, Color{ 100, 120, 160, 255 });

        DrawText("Create new world", (int)(dX3 + 20), (int)(dY3 + 18), 22, WHITE);
        DrawText("world name:", (int)(dX3 + 20), (int)(dY3 + 60), 16, Color{ 180, 180, 200, 255 });

        float inX = dX3 + 20, inY = dY3 + 82, inW2 = dW3 - 40, inH2 = 36;
        DrawRectangleRounded(Rectangle{ inX, inY, inW2, inH2 }, 0.2f, 8, Color{ 25, 28, 40, 255 });
        DrawRectangleRoundedLines(Rectangle{ inX, inY, inW2, inH2 }, 0.2f, 8,
            nameInput.aktiv ? Color{ 100, 160, 240, 255 } : Color{ 70, 80, 110, 255 });
        DrawText(nameInput.Get().c_str(), (int)(inX + 8), (int)(inY + 8), 18, WHITE);

        if (nameInput.aktiv && (int)(GetTime() * 2) % 2 == 0) {
            int cX = (int)(inX + 8 + MeasureText(nameInput.Get().c_str(), 18));
            DrawLine(cX, (int)(inY + 5), cX, (int)(inY + inH2 - 5), WHITE);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            nameInput.aktiv = (maus.x >= inX && maus.x <= inX + inW2 &&
                maus.y >= inY && maus.y <= inY + inH2);
        nameInput.Update();

        auto versuch = [&]() {
            std::string n = nameInput.Get();
            if (n.empty()) {
                fehlerText = "Please enter a name!";
                fehlerTimer = 3.0f;
                return;
            }
            if (ErstelleNeueWelt(n)) {
                welten = LadeAlleWelten();
                zeigeDialog = false;
            }
            else {
                fehlerText = "This world already exists!";
                fehlerTimer = 3.0f;
            }
            };

        if (IsKeyPressed(KEY_ENTER))  versuch();
        if (IsKeyPressed(KEY_ESCAPE)) zeigeDialog = false;

        float okX = dX3 + dW3 - 230, okY = dY3 + dH3 - 55, okW = 100, okH2 = 36;
        bool okHov = (maus.x >= okX && maus.x <= okX + okW &&
            maus.y >= okY && maus.y <= okY + okH2);
        DrawRectangleRounded(Rectangle{ okX, okY, okW, okH2 }, 0.3f, 8,
            okHov ? Color{ 60,160,60,255 } : Color{ 40,120,40,255 });
        DrawText("Create",
            (int)(okX + okW / 2 - MeasureText("Create", 16) / 2),
            (int)(okY + 9), 16, WHITE);

        float abX = dX3 + dW3 - 120, abY = okY, abW = 100.0f;
        bool abHov = (maus.x >= abX && maus.x <= abX + abW &&
            maus.y >= abY && maus.y <= abY + okH2);
        DrawRectangleRounded(Rectangle{ abX, abY, abW, okH2 }, 0.3f, 8,
            abHov ? Color{ 160,50,50,255 } : Color{ 120,35,35,255 });
        DrawText("Cancel",
            (int)(abX + abW / 2 - MeasureText("Cancel", 14) / 2),
            (int)(abY + 10), 14, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (okHov) versuch();
            if (abHov) zeigeDialog = false;
        }
    }

    return false;
}

// -----------------------------------------------------------------------
// zeichneWeldOptionen
// -----------------------------------------------------------------------
void zeichneWeldOptionen() {
    // --- Zustand für Umbenennen-Dialog ---
    static TextInput   umbenInput;
    static bool        zeigeUmbenennen = false;
    static std::string umbenenFehler = "";
    static float       umbenenFehlerTimer = 0.0f;
    static std::string umbenennePfad = "";

    // --- Zustand für Löschen-Bestätigung ---
    static bool        zeigeLoeschen = false;
    static std::string loescheWeltName = "";

    // --- instant-close Fix ---
    static std::string letzteOption = "";
    static bool        frischGeoeffnet = false;

    int     W = GetScreenWidth();
    int     H = GetScreenHeight();
    Vector2 mausPos = GetMousePosition();

    if (umbenenFehlerTimer > 0.0f) umbenenFehlerTimer -= GetFrameTime();

    // ================================================================
    // Umbenennen Dialog
    // ================================================================
    if (zeigeUmbenennen) {
        DrawRectangle(0, 0, W, H, Color{ 0, 0, 0, 160 });

        float dW = 420.0f, dH = 215.0f;
        float dX = (float)W / 2.0f - dW / 2.0f;
        float dY = (float)H / 2.0f - dH / 2.0f;

        DrawRectangleRounded(Rectangle{ dX, dY, dW, dH }, 0.1f, 8, Color{ 35, 40, 55, 255 });
        DrawRectangleRoundedLines(Rectangle{ dX, dY, dW, dH }, 0.1f, 8, Color{ 100, 150, 255, 255 });

        DrawText("Rename world", (int)(dX + 20), (int)(dY + 18), 22, WHITE);
        DrawText("New name:", (int)(dX + 20), (int)(dY + 62), 16, Color{ 180, 180, 200, 255 });

        // Eingabefeld
        float inX = dX + 20, inY = dY + 85, inW = dW - 40, inH = 36;
        DrawRectangleRounded(Rectangle{ inX, inY, inW, inH }, 0.2f, 8, Color{ 25, 28, 40, 255 });
        DrawRectangleRoundedLines(Rectangle{ inX, inY, inW, inH }, 0.2f, 8,
            umbenInput.aktiv ? Color{ 100, 160, 240, 255 } : Color{ 70, 80, 110, 255 });
        DrawText(umbenInput.Get().c_str(), (int)(inX + 8), (int)(inY + 8), 18, WHITE);

        if (umbenInput.aktiv && (int)(GetTime() * 2) % 2 == 0) {
            int cX = (int)(inX + 8 + MeasureText(umbenInput.Get().c_str(), 18));
            DrawLine(cX, (int)(inY + 5), cX, (int)(inY + inH - 5), WHITE);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            umbenInput.aktiv = (mausPos.x >= inX && mausPos.x <= inX + inW &&
                mausPos.y >= inY && mausPos.y <= inY + inH);
        umbenInput.Update();

        // Fehlermeldung
        if (umbenenFehlerTimer > 0.0f && !umbenenFehler.empty())
            DrawText(umbenenFehler.c_str(),
                (int)(dX + 20), (int)(dY + 134), 14, Color{ 255, 100, 100, 255 });

        // Buttons
        float okX = dX + dW - 230, okY = dY + dH - 52, okW = 110.0f, okH = 36.0f;
        bool  okHov = (mausPos.x >= okX && mausPos.x <= okX + okW &&
            mausPos.y >= okY && mausPos.y <= okY + okH);
        DrawRectangleRounded(Rectangle{ okX, okY, okW, okH }, 0.3f, 8,
            okHov ? Color{ 60,160,60,255 } : Color{ 40,120,40,255 });
        DrawText("rename",
            (int)(okX + okW / 2 - MeasureText("rename", 14) / 2),
            (int)(okY + 10), 14, WHITE);

        float abX = dX + dW - 110, abY = okY, abW = 100.0f;
        bool  abHov = (mausPos.x >= abX && mausPos.x <= abX + abW &&
            mausPos.y >= abY && mausPos.y <= abY + okH);
        DrawRectangleRounded(Rectangle{ abX, abY, abW, okH }, 0.3f, 8,
            abHov ? Color{ 160,50,50,255 } : Color{ 120,35,35,255 });
        DrawText("Cancel",
            (int)(abX + abW / 2 - MeasureText("Cancel", 14) / 2),
            (int)(abY + 10), 14, WHITE);

        auto versuchUmbenennen = [&]() {
            std::string neuerName = umbenInput.Get();
            if (neuerName.empty()) {
                umbenenFehler = "Please enter a name!";
                umbenenFehlerTimer = 3.0f;
                return;
            }
            if (BenenneWeltUm(umbenennePfad, neuerName)) {
                zeigeUmbenennen = false;
                offeneOptionen = "";
                umbenennePfad = "";
                s_resetPending = true;
            }
            else {
                umbenenFehler = "Name already exists!";
                umbenenFehlerTimer = 3.0f;
            }
            };

        if (IsKeyPressed(KEY_ENTER))  versuchUmbenennen();
        if (IsKeyPressed(KEY_ESCAPE)) zeigeUmbenennen = false;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (okHov) versuchUmbenennen();
            if (abHov) zeigeUmbenennen = false;
        }

        return;
    }

// ================================================================
// Löschen
// ================================================================
    if (zeigeLoeschen) {
        DrawRectangle(0, 0, W, H, Color{ 0, 0, 0, 160 });

        float dW4 = 400.0f, dH4 = 170.0f;
        float dX4 = (float)W / 2.0f - dW4 / 2.0f;
        float dY4 = (float)H / 2.0f - dH4 / 2.0f;

        DrawRectangleRounded(Rectangle{ dX4, dY4, dW4, dH4 }, 0.1f, 8, Color{ 40, 20, 20, 255 });
        DrawRectangleRoundedLines(Rectangle{ dX4, dY4, dW4, dH4 }, 0.1f, 8, Color{ 200, 60, 60, 255 });

        DrawText("Delete world?", (int)(dX4 + 20), (int)(dY4 + 18), 22, RED);
        std::string frage = "\"" + loescheWeltName + "\" Are you sure you want to delete this?";
        DrawText(frage.c_str(), (int)(dX4 + 20), (int)(dY4 + 55), 15, Color{ 220, 200, 200, 255 });
        DrawText("Non-refundable!", (int)(dX4 + 20), (int)(dY4 + 78), 12, Color{ 180, 120, 120, 255 });

        float jaX = dX4 + dW4 - 230, jaY = dY4 + dH4 - 50, jaW = 110.0f, jaH = 36.0f;
        bool  jaHov = (mausPos.x >= jaX && mausPos.x <= jaX + jaW &&
            mausPos.y >= jaY && mausPos.y <= jaY + jaH);
        DrawRectangleRounded(Rectangle{ jaX, jaY, jaW, jaH }, 0.3f, 8,
            jaHov ? Color{ 200,40,40,255 } : Color{ 140,25,25,255 });
        DrawText("Yes, delete",
            (int)(jaX + jaW / 2 - MeasureText("Yes, delete", 13) / 2),
            (int)(jaY + 11), 13, WHITE);

        float nX = dX4 + dW4 - 110, nY = jaY, nW = 100.0f;
        bool  nHov = (mausPos.x >= nX && mausPos.x <= nX + nW &&
            mausPos.y >= nY && mausPos.y <= nY + jaH);
        DrawRectangleRounded(Rectangle{ nX, nY, nW, jaH }, 0.3f, 8,
            nHov ? Color{ 60,90,60,255 } : Color{ 40,65,40,255 });
        DrawText("Cancel",
            (int)(nX + nW / 2 - MeasureText("Cancel", 14) / 2),
            (int)(nY + 10), 14, WHITE);

        if (jaHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            std::string pfad = "welten/" + loescheWeltName;
            LoescheWelt(pfad);
            zeigeLoeschen = false;
            offeneOptionen = "";
            s_resetPending = true;
        }

        if (IsKeyPressed(KEY_ESCAPE)) zeigeLoeschen = false;

        return;
    }

    // ================================================================
    // Optionen-Panel
    // ================================================================
    if (offeneOptionen != "") {
        if (letzteOption != offeneOptionen) frischGeoeffnet = true;
        letzteOption = offeneOptionen;

        int breite = 350;
        int hoehe = 280;
        int panelX = W / 2 - breite / 2;
        int panelY = H / 2 - hoehe / 2;

        // Abdunkeln
        DrawRectangle(0, 0, W, H, Color{ 0, 0, 0, 100 });

        // Panel-Hintergrund
        DrawRectangleRounded(
            Rectangle{ (float)panelX, (float)panelY, (float)breite, (float)hoehe },
            0.15f, 10, Color{ 35, 35, 45, 255 });
        DrawRectangleRoundedLines(
            Rectangle{ (float)panelX, (float)panelY, (float)breite, (float)hoehe },
            0.15f, 10, Color{ 100, 150, 255, 255 });

        // Weltname als Titel
        DrawText(offeneOptionen.c_str(), panelX + 20, panelY + 22, 22,
            Color{ 200, 220, 255, 255 });

        // Trennlinie
        DrawLine(panelX + 15, panelY + 58, panelX + breite - 15, panelY + 58,
            Color{ 80, 100, 140, 255 });

        int startY = panelY + 74;

        // Option 1: Löschen
        Rectangle opt1 = { (float)(panelX + 20), (float)startY, 310.0f, 48.0f };
        bool hov1 = CheckCollisionPointRec(mausPos, opt1);
        DrawRectangleRounded(opt1, 0.3f, 8,
            hov1 ? Color{ 180, 40, 40, 255 } : Color{ 110, 28, 28, 255 });
        DrawText("Delete world",
            panelX + 38, startY + 14, 18,
            hov1 ? WHITE : Color{ 255, 170, 170, 255 });

        // Option 2: Umbenennen
        Rectangle opt2 = { (float)(panelX + 20), (float)(startY + 68), 310.0f, 48.0f };
        bool hov2 = CheckCollisionPointRec(mausPos, opt2);
        DrawRectangleRounded(opt2, 0.3f, 8,
            hov2 ? Color{ 60, 110, 210, 255 } : Color{ 38, 72, 140, 255 });
        DrawText("Change name",
            panelX + 38, startY + 82, 18,
            hov2 ? WHITE : Color{ 170, 200, 255, 255 });

        // Option 3: Abbrechen
        Rectangle opt3 = { (float)(panelX + 20), (float)(startY + 136), 310.0f, 48.0f };
        bool hov3 = CheckCollisionPointRec(mausPos, opt3);
        DrawRectangleRounded(opt3, 0.3f, 8,
            hov3 ? Color{ 80, 80, 100, 255 } : Color{ 52, 52, 68, 255 });
        DrawText("Cancel",
            panelX + 38, startY + 150, 18,
            hov3 ? WHITE : Color{ 180, 180, 200, 255 });

        // Klick auswerten
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !frischGeoeffnet) {
            if (hov1) {
                loescheWeltName = offeneOptionen;
                zeigeLoeschen = true;
            }
            else if (hov2) {
                umbenennePfad = "welten/" + offeneOptionen;
                umbenInput.Set(offeneOptionen);
                umbenInput.aktiv = true;
                umbenenFehler = "";
                umbenenFehlerTimer = 0.0f;
                zeigeUmbenennen = true;
            }
            else if (hov3) {
                offeneOptionen = "";
            }
            else if (mausPos.x < panelX || mausPos.x > panelX + breite ||
                mausPos.y < panelY || mausPos.y > panelY + hoehe) {
                offeneOptionen = "";
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) offeneOptionen = "";

        frischGeoeffnet = false;
    }
    else {
        letzteOption = "";
        frischGeoeffnet = false;
    }
}