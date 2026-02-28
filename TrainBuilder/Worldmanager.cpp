#include "WorldManager.h"
#include "raylib.h"
#include <string>
#include <vector>

// -----------------------------------------------------------------------
// Einfaches Texteingabe-Feld
// -----------------------------------------------------------------------
struct TextInput {
    char buffer[128];
    int  len;
    bool aktiv;

    TextInput() : len(0), aktiv(false) {
        buffer[0] = '\0';
    }

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
    std::string Get() const { return std::string(buffer); }
};

// -----------------------------------------------------------------------
// Interne Zeichen-Helfer (umgehen DrawRectangleRoundedLines-Signatur)
// -----------------------------------------------------------------------
static void Rahmen(Rectangle r, Color c) {
    DrawRectangleRoundedLines(r, 0.15f, 8, c);
}

static void RahmenRund(Rectangle r, Color c) {
    DrawRectangleRoundedLines(r, 0.3f, 8, c);
}

// -----------------------------------------------------------------------
// Reset-Flag: wird von ResetWeltauswahl() gesetzt, damit beim nächsten
// Aufruf von WeltauswahlUpdate() die Weltliste neu geladen wird und
// alle Dialog-Zustände sauber zurückgesetzt werden.
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
    static bool  weltenGeladen = false;
    static TextInput nameInput;
    static bool  zeigeDialog = false;
    static std::string fehlerText = "";
    static float fehlerTimer = 0.0f;
    static int   loescheIndex = -1;
    static float scrollOffset = 0.0f;

    // Reset aller statischen Zustände wenn wir frisch ins Menü zurückkehren
    if (s_resetPending) {
        weltenGeladen = false;
        zeigeDialog = false;
        fehlerText = "";
        fehlerTimer = 0.0f;
        loescheIndex = -1;
        scrollOffset = 0.0f;
        nameInput.Clear();
        nameInput.aktiv = false;
        s_resetPending = false;
    }

    if (!weltenGeladen) {
        welten = LadeAlleWelten();
        weltenGeladen = true;
    }

    int W = GetScreenWidth();
    int H = GetScreenHeight();
    Vector2 maus = GetMousePosition();

    if (fehlerTimer > 0.0f) fehlerTimer -= GetFrameTime();

    ClearBackground(Color{ 30, 30, 40, 255 });

    // ---- Titel ----
    const char* titel = "TrainBuilder";
    DrawText(titel, W / 2 - MeasureText(titel, 48) / 2, 40, 48, Color{ 220, 180, 60, 255 });
    const char* sub = "Weltauswahl";
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
    if (scrollOffset < 0.0f)        scrollOffset = 0.0f;
    if (scrollOffset > maxScroll)   scrollOffset = maxScroll;

    BeginScissorMode((int)lX - 5, (int)lY, (int)lW + 10, (int)lH);

    for (int i = 0; i < (int)welten.size(); i++) {
        float iy = lY + (float)i * (iH + iA) - scrollOffset;
        if (iy + iH < lY || iy > lY + lH) continue;

        float iW2 = lW - 80.0f;
        bool hover = (maus.x >= lX && maus.x <= lX + iW2 &&
            maus.y >= iy && maus.y <= iy + iH);

        // Welt-Kachel
        Color bg = hover ? Color{ 60, 80, 110, 255 } : Color{ 45, 55, 75, 255 };
        DrawRectangleRounded(Rectangle{ lX, iy, iW2, iH }, 0.15f, 8, bg);
        Rahmen(Rectangle{ lX, iy, iW2, iH }, Color{ 80, 100, 140, 255 });

        // Icon
        DrawRectangle((int)lX + 10, (int)iy + 10, 40, 40, Color{ 70, 130, 180, 255 });
        DrawText("W", (int)lX + 22, (int)iy + 18, 22, WHITE);

        DrawText(welten[i].name.c_str(), (int)lX + 60, (int)iy + 10, 20, WHITE);
        std::string dat = "Erstellt: " + welten[i].erstelltAm;
        DrawText(dat.c_str(), (int)lX + 60, (int)iy + 35, 13, Color{ 150, 160, 180, 255 });

        // Loeschen-Button
        float dX = lX + lW - 75.0f;
        float dY = iy + 10;
        float dW2 = 60.0f, dH2 = iH - 20.0f;
        bool dHov = (maus.x >= dX && maus.x <= dX + dW2 &&
            maus.y >= dY && maus.y <= dY + dH2);
        DrawRectangleRounded(Rectangle{ dX, dY, dW2, dH2 }, 0.3f, 8,
            dHov ? Color{ 180, 40, 40, 255 } : Color{ 130, 30, 30, 255 });
        DrawText("X", (int)(dX + dW2 / 2 - MeasureText("X", 16) / 2),
            (int)(dY + dH2 / 2 - 8), 16, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && loescheIndex == -1 && !zeigeDialog) {
            if (dHov) {
                loescheIndex = i;
            }
            else if (hover) {
                LadeWelt(welten[i].pfad);
                weltenGeladen = false;
                EndScissorMode();  // WICHTIG: immer aufrufen bevor return
                return true;
            }
        }
    }

    EndScissorMode();

    if (welten.empty()) {
        const char* t = "Keine Welten vorhanden - erstelle eine neue Welt!";
        DrawText(t, W / 2 - MeasureText(t, 15) / 2, H / 2, 15, Color{ 140, 140, 160, 255 });
    }

    // ---- Neue-Welt-Button ----
    float bW = 220.0f, bH = 45.0f;
    float bX = (float)W / 2.0f - bW / 2.0f;
    float bY = (float)H - 80.0f;
    bool bHov = (maus.x >= bX && maus.x <= bX + bW &&
        maus.y >= bY && maus.y <= bY + bH);

    DrawRectangleRounded(Rectangle{ bX, bY, bW, bH }, 0.3f, 8,
        bHov ? Color{ 60, 160, 60, 255 } : Color{ 40, 120, 40, 255 });
    RahmenRund(Rectangle{ bX, bY, bW, bH }, Color{ 80, 200, 80, 255 });
    const char* bT = "+ Neue Welt";
    DrawText(bT, (int)(bX + bW / 2 - MeasureText(bT, 20) / 2),
        (int)(bY + bH / 2 - 10), 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bHov && !zeigeDialog && loescheIndex == -1) {
        zeigeDialog = true;
        nameInput.Clear();
        nameInput.aktiv = true;
        fehlerText = "";
    }

    if (fehlerTimer > 0.0f && !fehlerText.empty())
        DrawText(fehlerText.c_str(),
            W / 2 - MeasureText(fehlerText.c_str(), 16) / 2,
            (int)bY - 30, 16, RED);

    // ---- Dialog: Neue Welt ----
    if (zeigeDialog) {
        DrawRectangle(0, 0, W, H, Color{ 0, 0, 0, 160 });

        float dW3 = 420.0f, dH3 = 200.0f;
        float dX3 = (float)W / 2.0f - dW3 / 2.0f;
        float dY3 = (float)H / 2.0f - dH3 / 2.0f;

        DrawRectangleRounded(Rectangle{ dX3, dY3, dW3, dH3 }, 0.1f, 8, Color{ 40, 45, 60, 255 });
        Rahmen(Rectangle{ dX3, dY3, dW3, dH3 }, Color{ 100, 120, 160, 255 });

        DrawText("Neue Welt erstellen", (int)(dX3 + 20), (int)(dY3 + 18), 22, WHITE);
        DrawText("Weltname:", (int)(dX3 + 20), (int)(dY3 + 60), 16, Color{ 180, 180, 200, 255 });

        // Eingabefeld
        float inX = dX3 + 20, inY = dY3 + 82, inW2 = dW3 - 40, inH2 = 36;
        DrawRectangleRounded(Rectangle{ inX, inY, inW2, inH2 }, 0.2f, 8, Color{ 25, 28, 40, 255 });
        DrawRectangleRoundedLines(Rectangle{ inX, inY, inW2, inH2 }, 0.2f, 8,
            nameInput.aktiv ? Color{ 100, 160, 240, 255 } : Color{ 70, 80, 110, 255 });
        DrawText(nameInput.Get().c_str(), (int)(inX + 8), (int)(inY + 8), 18, WHITE);

        // Cursor blinken
        if (nameInput.aktiv && (int)(GetTime() * 2) % 2 == 0) {
            int cX = (int)(inX + 8 + MeasureText(nameInput.Get().c_str(), 18));
            DrawLine(cX, (int)(inY + 5), cX, (int)(inY + inH2 - 5), WHITE);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            nameInput.aktiv = (maus.x >= inX && maus.x <= inX + inW2 &&
                maus.y >= inY && maus.y <= inY + inH2);
        nameInput.Update();

        // Lambda: Erstell-Versuch
        auto versuch = [&]() {
            std::string n = nameInput.Get();
            if (n.empty()) {
                fehlerText = "Bitte einen Namen eingeben!";
                fehlerTimer = 3.0f;
                return;
            }
            if (ErstelleNeueWelt(n)) {
                welten = LadeAlleWelten();
                zeigeDialog = false;
            }
            else {
                fehlerText = "Diese Welt existiert bereits!";
                fehlerTimer = 3.0f;
            }
            };

        if (IsKeyPressed(KEY_ENTER))  versuch();
        if (IsKeyPressed(KEY_ESCAPE)) zeigeDialog = false;

        // Erstellen
        float okX = dX3 + dW3 - 230, okY = dY3 + dH3 - 55, okW = 100, okH2 = 36;
        bool okHov = (maus.x >= okX && maus.x <= okX + okW &&
            maus.y >= okY && maus.y <= okY + okH2);
        DrawRectangleRounded(Rectangle{ okX, okY, okW, okH2 }, 0.3f, 8,
            okHov ? Color{ 60,160,60,255 } : Color{ 40,120,40,255 });
        DrawText("Erstellen",
            (int)(okX + okW / 2 - MeasureText("Erstellen", 16) / 2),
            (int)(okY + 9), 16, WHITE);

        // Abbrechen
        float abX = dX3 + dW3 - 120, abY = okY, abW = 100.0f;
        bool abHov = (maus.x >= abX && maus.x <= abX + abW &&
            maus.y >= abY && maus.y <= abY + okH2);
        DrawRectangleRounded(Rectangle{ abX, abY, abW, okH2 }, 0.3f, 8,
            abHov ? Color{ 160,50,50,255 } : Color{ 120,35,35,255 });
        DrawText("Abbrechen",
            (int)(abX + abW / 2 - MeasureText("Abbrechen", 14) / 2),
            (int)(abY + 10), 14, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (okHov) versuch();
            if (abHov) zeigeDialog = false;
        }
    }

    // ---- Dialog: Loeschen ----
    if (loescheIndex >= 0 && loescheIndex < (int)welten.size()) {
        DrawRectangle(0, 0, W, H, Color{ 0, 0, 0, 160 });

        float dW4 = 380.0f, dH4 = 160.0f;
        float dX4 = (float)W / 2.0f - dW4 / 2.0f;
        float dY4 = (float)H / 2.0f - dH4 / 2.0f;

        DrawRectangleRounded(Rectangle{ dX4, dY4, dW4, dH4 }, 0.1f, 8, Color{ 40, 28, 28, 255 });
        Rahmen(Rectangle{ dX4, dY4, dW4, dH4 }, Color{ 180, 60, 60, 255 });

        DrawText("Welt loeschen?", (int)(dX4 + 20), (int)(dY4 + 18), 22, RED);
        std::string frage = "\"" + welten[loescheIndex].name + "\" wirklich loeschen?";
        DrawText(frage.c_str(), (int)(dX4 + 20), (int)(dY4 + 55), 15, Color{ 220, 200, 200, 255 });
        DrawText("Nicht rueckgaengig machbar!", (int)(dX4 + 20), (int)(dY4 + 78), 12, Color{ 180, 120, 120, 255 });

        float jaX = dX4 + dW4 - 220, jaY = dY4 + dH4 - 50, jaW = 90.0f, jaH = 36.0f;
        bool jaHov = (maus.x >= jaX && maus.x <= jaX + jaW &&
            maus.y >= jaY && maus.y <= jaY + jaH);
        DrawRectangleRounded(Rectangle{ jaX, jaY, jaW, jaH }, 0.3f, 8,
            jaHov ? Color{ 180,40,40,255 } : Color{ 130,30,30,255 });
        DrawText("Ja!", (int)(jaX + jaW / 2 - MeasureText("Ja!", 16) / 2),
            (int)(jaY + 10), 16, WHITE);

        float nX = dX4 + dW4 - 115, nY = jaY, nW = 100.0f;
        bool nHov = (maus.x >= nX && maus.x <= nX + nW &&
            maus.y >= nY && maus.y <= nY + jaH);
        DrawRectangleRounded(Rectangle{ nX, nY, nW, jaH }, 0.3f, 8,
            nHov ? Color{ 60,90,60,255 } : Color{ 40,70,40,255 });
        DrawText("Abbrechen",
            (int)(nX + nW / 2 - MeasureText("Abbrechen", 14) / 2),
            (int)(nY + 10), 14, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (jaHov) {
                LöscheWelt(welten[loescheIndex].pfad);
                welten = LadeAlleWelten();
                loescheIndex = -1;
            }
            else if (nHov) {
                loescheIndex = -1;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) loescheIndex = -1;
    }

    return false;
}