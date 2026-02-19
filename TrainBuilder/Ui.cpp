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
#include "Ampel.h"
#include "knoten.h"

static TextBox nahmeEingabe(0, 0, 200.0f, 30.0f, 32);
static TextBox zugnameEingabe(0, 0, 200.0f, 30.0f, 32);
static TextBox knotenEingabe(0, 0, 200.0f, 30.0f, 32);
static TextBox AmpelEingabe(0, 0, 200.0f, 30.0f, 32);
static int letzterAusgewahlterBanhof = 0;
static int letzterAusgewahlterZug = 0;
static int letzterAusgewahlterKnoten = 0;
static int letzterAusgewahlteAmpel = 0;


/*-------------------------------------------------
    SCROLLBAR VARIABLEN
-------------------------------------------------*/
static float bahnhofScrollOffset = 0.0f;
static float zugScrollOffset = 0.0f;
static float knotenOffset = 0.0f;
static float ampelOffset = 0.0f;

static bool bahnhofScrolling = false;
static bool zugScrolling = false;

static float zugplanScrollOffset = 0.0f;
static bool zugplanScrolling = false;

const float SEITENMENÜ_BREITE = 250.0f;
const float SEITENMENÜ_Y_START = 80.0f;

float zugplanContainerY = 0.0f;

float scrollbarX = 0.0f;
float scrollbarY = 0.0f;
float scrollbarHoehe = 0.0f;

/*-------------------------------------------------
    HILFSFUNKTIONEN SCROLLBAR
-------------------------------------------------*/
float MeineClamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float BerechneMaxScrollHoehe(float contentHoehe) {
    float viewportHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;
    if (contentHoehe <= viewportHoehe) return 0.0f;
    return contentHoehe - viewportHoehe;
}

void ZeichneScrollbar(float scrollOffset, float maxScroll, bool isActive, float x, float y, float hoehe) {
    if (maxScroll <= 0) return;

    DrawRectangle(x, y, SCROLLBAR_BREITE, hoehe, Color{ 200, 200, 200, 255 });

    float thumbHoehe = hoehe * (hoehe / (hoehe + maxScroll));
    float thumbY = y + (scrollOffset / maxScroll) * (hoehe - thumbHoehe);

    Color thumbColor = isActive ? DARKGRAY : GRAY;
    DrawRectangle(x, thumbY, SCROLLBAR_BREITE, thumbHoehe, thumbColor);
    DrawRectangleLines(x, thumbY, SCROLLBAR_BREITE, thumbHoehe, BLACK);
}

float ProccessScrollInput(float currentOffset, float maxScroll, float scrollbarX, float scrollbarY,
    float scrollbarHoehe, bool& isScrolling, float contentAreaX, float contentAreaWidth) {
    Vector2 mousePos = GetMousePosition();

    /*-------------------------------------------------
        MAUSRAD SCROLLING
    -------------------------------------------------*/
    if (CheckCollisionPointRec(mousePos, Rectangle{ contentAreaX,
        scrollbarY, contentAreaWidth, scrollbarHoehe })) {
        float wheelInput = GetMouseWheelMove();
        if (wheelInput != 0) {
            float newOffset = currentOffset - wheelInput * 30.0f;
            return MeineClamp(newOffset, 0.0f, maxScroll);
        }
    }

    /*-------------------------------------------------
        SCROLLBAR DRAG
    -------------------------------------------------*/
    if (maxScroll > 0) {
        float thumbHoehe = scrollbarHoehe * (scrollbarHoehe / (scrollbarHoehe + maxScroll));
        float thumbY = scrollbarY + (currentOffset / maxScroll) * (scrollbarHoehe - thumbHoehe);
        Rectangle thumbRect = { scrollbarX, thumbY, SCROLLBAR_BREITE, thumbHoehe };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, thumbRect)) {
            isScrolling = true;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isScrolling = false;
        }

        if (isScrolling) {
            float mouseDelta = GetMouseDelta().y;
            float scrollRatio = maxScroll / (scrollbarHoehe - thumbHoehe);
            float newOffset = currentOffset + mouseDelta * scrollRatio;
            return MeineClamp(newOffset, 0.0f, maxScroll);
        }
    }

    return currentOffset;
}

void zeichneUI() {
    kannBewegen = true;

    DrawRectangle(0, 0, (float)GenaueBreite, 80.0f, LIGHTGRAY);

    if (aktuellesTool == 1) {
        DrawRectangle(10.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
    else if (aktuellesTool == 2) {
        DrawRectangle(80.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
    else if (aktuellesTool == 3) {
        DrawRectangle(150.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
    else if (aktuellesTool == 4) {
        DrawRectangle(220.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
    else if (aktuellesTool == 5) {
        DrawRectangle(290.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }
    else if (aktuellesTool == 6) {
        DrawRectangle(360.0f, 10.0f, 60.0f, 60.0f, DARKGRAY);
    }

    DrawTexture("zeichnen", 10.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Löschen", 80.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Auswahl", 150.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Banhof", 220.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("zugicon", 290.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("zahnrad", 360.0f, 10.0f, 60.0f, 60.0f);

    /*-------------------------------------------------
        BAHNHOF MENÜ
    -------------------------------------------------*/
    if (ausgewahlterBanhof != 0) {
        DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
        DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);

        BeginScissorMode(GenaueBreite - 250, 80, 250, GenaueHoehe - 80);

        nahmeEingabe.SetPosition((float)GenaueBreite - 240.0f, 100.0f - bahnhofScrollOffset);

        if (letzterAusgewahlterBanhof != ausgewahlterBanhof) {
            for (const auto& banhof : banhofListe) {
                if (banhof.BanhofId == ausgewahlterBanhof) {
                    nahmeEingabe.SetText(banhof.Name);
                    break;
                }
            }
            letzterAusgewahlterBanhof = ausgewahlterBanhof;
            bahnhofScrollOffset = 0.0f;
        }

        nahmeEingabe.Update();
        nahmeEingabe.Draw();

        if (nahmeEingabe.IsActive()) {
            kannBewegen = false;
        }

        /*-------------------------------------------------
            BAHNHOF CONTENT MIT SCROLLING
        -------------------------------------------------*/
        float contentX = GenaueBreite - 240.0f;
        float contentY = 150.0f - bahnhofScrollOffset;

        for (const auto& banhof : banhofListe) {
            if (banhof.BanhofId == ausgewahlterBanhof) {
                DrawText(TextFormat("Name: %s", banhof.Name.c_str()), contentX, contentY, 20, BLACK);
                contentY += 30;

                DrawText(TextFormat("ID: %d", banhof.BanhofId), contentX, contentY, 20, BLACK);
                contentY += 30;

                DrawText(TextFormat("Position: [%d, %d]", banhof.GridX, banhof.GridY), contentX, contentY, 20, BLACK);
                contentY += 30;

                DrawText(TextFormat("Rotation: %d", banhof.Rotation), contentX, contentY, 20, BLACK);
                contentY += 30;

                break;
            }
        }

        EndScissorMode();

        float bahnhofContentHoehe = 4 * 30 + 100;
        float maxBahnhofScroll = BerechneMaxScrollHoehe(bahnhofContentHoehe);

        float scrollbarX = GenaueBreite - SCROLLBAR_BREITE;
        float scrollbarY = SEITENMENÜ_Y_START;
        float scrollbarHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;

        bahnhofScrollOffset = ProccessScrollInput(bahnhofScrollOffset, maxBahnhofScroll,
            scrollbarX, scrollbarY, scrollbarHoehe, bahnhofScrolling,
            GenaueBreite - 250, 250.0f - SCROLLBAR_BREITE);

        if (maxBahnhofScroll > 0) {
            ZeichneScrollbar(bahnhofScrollOffset, maxBahnhofScroll, bahnhofScrolling,
                scrollbarX, scrollbarY, scrollbarHoehe);
        }

        if (ausgewahlterBanhof > 0 && ausgewahlterBanhof <= banhofListe.size()) {
            banhofListe[ausgewahlterBanhof - 1].Name = nahmeEingabe.GetText();
        }

        if (!nahmeEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            BanhofSpeichern();
        }
    }

    /*-------------------------------------------------
        ZUG MENÜ
    -------------------------------------------------*/
    if (ausgewahlterZug != 0) {
        DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
        DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);

        BeginScissorMode(GenaueBreite - 250, 80, 250, GenaueHoehe - 80);

        zugnameEingabe.SetPosition((float)GenaueBreite - 240.0f, 100.0f - zugScrollOffset);

        if (letzterAusgewahlterZug != ausgewahlterZug) {
            for (const auto& zug : aktiveZuege) {
                if (zug.zugId == ausgewahlterZug) {
                    zugnameEingabe.SetText(zug.name);
                    break;
                }
            }
            letzterAusgewahlterZug = ausgewahlterZug;
            zugScrollOffset = 0.0f;
        }

        zugnameEingabe.Update();
        zugnameEingabe.Draw();

        if (zugnameEingabe.IsActive()) {
            kannBewegen = false;
        }

        /*-------------------------------------------------
            ZUG CONTENT MIT SCROLLING
        -------------------------------------------------*/
        float contentX = GenaueBreite - 240.0f;
        float yPos = 150.0f - zugScrollOffset;
        const int lineHeight = 25;

        for (auto& zug : aktiveZuege) {
            if (zug.zugId == ausgewahlterZug) {
                zug.name = zugnameEingabe.GetText();

                DrawText(TextFormat("Name: %s", zug.name.c_str()), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("ID: %d", zug.zugId), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Position: [%.2f, %.2f]", zug.posX, zug.posY), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Rotation: %d", zug.rotation), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Geschwindigkeit: %d km/h", zug.geschwindichkeit), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Passagiere: %d", zug.passagiere), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Gueter: %d", zug.gueter), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Zugtyp: %s", zug.zugtyp.c_str()), contentX, yPos, 20, BLACK);
                yPos += lineHeight;

                break;
            }
        }

        EndScissorMode();

        zugplanContainerY = 150.0f + (8 * lineHeight) - zugScrollOffset + 20;

        float actualZugplanHeight = 0;

        for (auto& zug : aktiveZuege) {
            if (zug.zugId == ausgewahlterZug) {
                if (zug.Fahrplan.empty()) {
                    actualZugplanHeight = 60.0f;
                }
                else {
                    actualZugplanHeight = 25.0f + (zug.Fahrplan.size() * 20.0f) + 30.0f;
                }
                break;
            }
        }

        /*-------------------------------------------------
            LÖSCHEN BUTTON
        -------------------------------------------------*/
        float haltestelleButtonY = zugplanContainerY + actualZugplanHeight + 5;
        float haltestelleButtonHeight = 25.0f;
        float deleteButtonY = haltestelleButtonY + haltestelleButtonHeight + 10;
        float deleteButtonX = GenaueBreite - 240.0f;
        float deleteButtonWidth = 220.0f;
        float deleteButtonHeight = 35.0f;

        if (deleteButtonY > 80.0f && deleteButtonY + deleteButtonHeight < GenaueHoehe - 20.0f) {
            DrawRectangle(deleteButtonX, deleteButtonY, deleteButtonWidth, deleteButtonHeight, RED);
            DrawRectangleLines(deleteButtonX, deleteButtonY, deleteButtonWidth, deleteButtonHeight, WHITE);

            float textWidth = MeasureText("Zug loeschen", 20);
            DrawText("Zug loeschen",
                deleteButtonX + (deleteButtonWidth - textWidth) / 2,
                deleteButtonY + (deleteButtonHeight - 20) / 2,
                20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                !haltestelleMenuOffen &&
                GetMouseX() >= deleteButtonX && GetMouseX() <= deleteButtonX + deleteButtonWidth &&
                GetMouseY() >= deleteButtonY && GetMouseY() <= deleteButtonY + deleteButtonHeight) {
                for (auto it = aktiveZuege.begin(); it != aktiveZuege.end(); ++it) {
                    if (it->zugId == ausgewahlterZug) {
                        aktiveZuege.erase(it);
                        ausgewahlterZug = 0;
                        AktiveZuegeSpeichern();
                        break;
                    }
                }
            }
        }

        ZeichneZugplan(ausgewahlterZug, zugplanScrollOffset, zugplanScrolling, zugplanContainerY);

        /*-------------------------------------------------
            ZUG SCROLLBAR BERECHNUNG
        -------------------------------------------------*/
        float zugContentHoehe = 200.0f;

        for (const auto& zug : aktiveZuege) {
            if (zug.zugId == ausgewahlterZug) {
                zugContentHoehe += (8 * lineHeight);
                if (zug.Fahrplan.empty()) {
                    zugContentHoehe += 60.0f;
                }
                else {
                    zugContentHoehe += 25.0f + (zug.Fahrplan.size() * 20.0f) + 30.0f;
                }
                zugContentHoehe += 40.0f;
                zugContentHoehe += 45.0f;
                break;
            }
        }

        float maxZugScroll = BerechneMaxScrollHoehe(zugContentHoehe);

        scrollbarX = GenaueBreite - SCROLLBAR_BREITE;
        scrollbarY = SEITENMENÜ_Y_START;
        scrollbarHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;

        zugScrollOffset = ProccessScrollInput(zugScrollOffset, maxZugScroll,
            scrollbarX, scrollbarY, scrollbarHoehe, zugScrolling,
            GenaueBreite - 250, 250.0f - SCROLLBAR_BREITE);

        if (maxZugScroll > 0) {
            ZeichneScrollbar(zugScrollOffset, maxZugScroll, zugScrolling,
                scrollbarX, scrollbarY, scrollbarHoehe);
        }

        if (!zugnameEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AktiveZuegeSpeichern();
        }
    }
    /*-------------------------------------------------
        Ampel
    -------------------------------------------------*/
    if (ausgewahlterAmpel != 0) {
        bool ampelExistiert = false;
        for (const auto& ampel : ampelListe) {
            if (ampel.AmpelId == ausgewahlterAmpel) {
                ampelExistiert = true;
                break;
            }
        }

        if (!ampelExistiert) {
            ausgewahlterAmpel = 0;
        }
        else {
            DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
            DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);
            BeginScissorMode(GenaueBreite - 250, 80, 250, GenaueHoehe - 80);

            DrawText("Status: ", GenaueBreite - 240.0f, 100.0f, 20, BLACK);

            for (const auto& ampel : ampelListe) {
                if (ampel.AmpelId == ausgewahlterAmpel) {
                    if (ampel.isGreen) {
                        DrawText("Gruen", GenaueBreite - 150.0f, 100.0f, 20, GREEN);
                    }
                    else {
                        DrawText("Rot", GenaueBreite - 150.0f, 100.0f, 20, RED);
                    }
                    break;
                }
            }

            float buttonX = GenaueBreite - 240.0f;
            float buttonY = 150.0f;
            float buttonWidth = 220.0f;
            float buttonHeight = 35.0f;

            DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, BLUE);
            DrawRectangleLines(buttonX, buttonY, buttonWidth, buttonHeight, WHITE);
            DrawText("Ampel umschalten", buttonX + 10, buttonY + 10, 15, WHITE);

            float deleteButtonY = buttonY + buttonHeight + 10;
            DrawRectangle(buttonX, deleteButtonY, buttonWidth, buttonHeight, RED);
            DrawRectangleLines(buttonX, deleteButtonY, buttonWidth, buttonHeight, WHITE);
            DrawText("Ampel loeschen", buttonX + 10, deleteButtonY + 10, 15, WHITE);

            // ID 
            DrawText("ID:", (float)GenaueBreite - 240.0f, 250, 20, BLACK);
            AmpelEingabe.SetPosition((float)GenaueBreite - 240.0f, 270.0f);

            if (letzterAusgewahlteAmpel != ausgewahlterAmpel) {
                for (const auto& ampel : ampelListe) {
                    if (ampel.AmpelId == ausgewahlterAmpel) {
                        AmpelEingabe.SetText(ampel.Name);
                        break;
                    }
                }
                letzterAusgewahlteAmpel = ausgewahlterAmpel;
            }

            AmpelEingabe.Update();
            AmpelEingabe.Draw();

            if (AmpelEingabe.IsActive()) {
                kannBewegen = false;
            }

            for (auto& ampel : ampelListe) {
                if (ampel.AmpelId == ausgewahlterAmpel) {
                    ampel.Name = AmpelEingabe.GetText();
                    break;
                }
            }

            // Buttons
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();

                if (mousePos.x >= GenaueBreite - 250.0f && mousePos.x <= GenaueBreite &&
                    mousePos.y >= 80.0f && mousePos.y <= GenaueHoehe) {

                    if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth &&
                        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {

                        for (auto& ampel : ampelListe) {
                            if (ampel.AmpelId == ausgewahlterAmpel) {
                                ampel.isGreen = !ampel.isGreen;
                                AmpelSpeichern();
                                break;
                            }
                        }
                    }
                    // Löschen
                    else if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth &&
                        mousePos.y >= deleteButtonY && mousePos.y <= deleteButtonY + buttonHeight) {

                        for (auto it = ampelListe.begin(); it != ampelListe.end(); ++it) {
                            if (it->AmpelId == ausgewahlterAmpel) {
                                ampelListe.erase(it);
                                ausgewahlterAmpel = 0;
                                AmpelSpeichern();
                                break;
                            }
                        }
                    }
                }
            }

            EndScissorMode();

            if (!AmpelEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                AmpelSpeichern();
            }
        }
    }
    if (ausgewahlterKnoten != 0) {
        DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
        DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);
        BeginScissorMode(GenaueBreite - 250, 80, 250, GenaueHoehe - 80);

        // Seitenmenü
        // Informationen
        DrawText("ID:", (float)GenaueBreite - 240.0f, 100, 20, BLACK);
        knotenEingabe.SetPosition((float)GenaueBreite - 240.0f, 130.0f - knotenOffset);

        if (letzterAusgewahlterKnoten != ausgewahlterKnoten) {
            for (const auto& knoten : knotenliste) {
                if (knoten.eindeutigeId == ausgewahlterKnoten) {
                    knotenEingabe.SetText(knoten.Name);
                    break;
                }
            }
            letzterAusgewahlterKnoten = ausgewahlterKnoten;
            knotenOffset = 0.0f;
        }

        knotenEingabe.Update();
        knotenEingabe.Draw();

        if (knotenEingabe.IsActive()) {
            kannBewegen = false;
        }

        for (auto& knoten : knotenliste) {
            if (knoten.eindeutigeId == ausgewahlterKnoten) {
                knoten.Name = knotenEingabe.GetText();
                break;
            }
        }
        bool aktuellermodus = true;
        for (const auto& knoten : knotenliste) {
            if (knoten.eindeutigeId == ausgewahlterKnoten) {
                
                DrawText(TextFormat("Position: [%d, %d]", knoten.GridX, knoten.GridY),
                    (float)GenaueBreite - 240.0f, 200.0f - knotenOffset, 20, DARKGRAY);
                aktuellermodus = knoten.modus;
                break;
            }
        }

        //modus Anzeigen und umschalten

        DrawText("Aktueller Modus:", (float)GenaueBreite - 240.0f, 230, 20, BLACK);
        if (aktuellermodus) {
            DrawText("Lesen", (float)GenaueBreite - 240.0f, 250, 20, BLACK);
        }
        else
            DrawText("Schreiben", (float)GenaueBreite - 240.0f, 250, 20, BLACK);

        EndScissorMode();

        if (!knotenEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            KnotenSpeichern();
        }

        float knotenContentHoehe = 300.0f;
        float maxKnotenScroll = BerechneMaxScrollHoehe(knotenContentHoehe);

        float scrollbarX = GenaueBreite - SCROLLBAR_BREITE;
        float scrollbarY = SEITENMENÜ_Y_START;
        float scrollbarHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;

        static bool knotenScrolling = false;
        knotenOffset = ProccessScrollInput(knotenOffset, maxKnotenScroll,
            scrollbarX, scrollbarY, scrollbarHoehe, knotenScrolling,
            GenaueBreite - 250, 250.0f - SCROLLBAR_BREITE);

        if (maxKnotenScroll > 0) {
            ZeichneScrollbar(knotenOffset, maxKnotenScroll, knotenScrolling,
                scrollbarX, scrollbarY, scrollbarHoehe);
        }
    }

    if (ausgewahlterZug != 0) {
        ZeichneDraggingItem(ausgewahlterZug, zugplanContainerY);
    }
}