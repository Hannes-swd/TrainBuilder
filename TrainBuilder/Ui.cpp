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
#include "zug.h"

static TextBox nahmeEingabe(0, 0, 200.0f, 30.0f, 32);
static TextBox zugnameEingabe(0, 0, 200.0f, 30.0f, 32);
static int letzterAusgewahlterBanhof = 0;
static int letzterAusgewahlterZug = 0;

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

    DrawTexture("zeichnen", 10.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Löschen", 80.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Auswahl", 150.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("Banhof", 220.0f, 10.0f, 60.0f, 60.0f);
    DrawTexture("zugicon", 290.0f, 10.0f, 60.0f, 60.0f);

    // Bahnhof-Menü
    if (ausgewahlterBanhof != 0) {
        DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
        DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);

        nahmeEingabe.SetPosition((float)GenaueBreite - 240.0f, 100.0f);

        if (letzterAusgewahlterBanhof != ausgewahlterBanhof) {
            for (const auto& banhof : banhofListe) {
                if (banhof.BanhofId == ausgewahlterBanhof) {
                    nahmeEingabe.SetText(banhof.Name);
                    break;
                }
            }
            letzterAusgewahlterBanhof = ausgewahlterBanhof;
        }

        nahmeEingabe.Update();
        nahmeEingabe.Draw();

        if (nahmeEingabe.IsActive()) {
            kannBewegen = false;
        }

        for (const auto& banhof : banhofListe) {
            if (banhof.BanhofId == ausgewahlterBanhof) {
                DrawText(TextFormat("Name: %s", banhof.Name.c_str()), GenaueBreite - 240, 150, 20, BLACK);
                DrawText(TextFormat("ID: %d", banhof.BanhofId), GenaueBreite - 240, 180, 20, BLACK);
                DrawText(TextFormat("Position: [%d, %d]", banhof.GridX, banhof.GridY), GenaueBreite - 240, 210, 20, BLACK);
                DrawText(TextFormat("Rotation: %d", banhof.Rotation), GenaueBreite - 240, 240, 20, BLACK);
                break;
            }
        }

        if (ausgewahlterBanhof > 0 && ausgewahlterBanhof <= banhofListe.size()) {
            banhofListe[ausgewahlterBanhof - 1].Name = nahmeEingabe.GetText();
        }

        if (!nahmeEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            BanhofSpeichern();
        }
    }

    if (ausgewahlterZug != 0) {
        DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
        DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);

        zugnameEingabe.SetPosition((float)GenaueBreite - 240.0f, 100.0f);

        if (letzterAusgewahlterZug != ausgewahlterZug) {
            for (const auto& zug : aktiveZuege) {
                if (zug.zugId == ausgewahlterZug) {
                    zugnameEingabe.SetText(zug.name);
                    break;
                }
            }
            letzterAusgewahlterZug = ausgewahlterZug;
        }

        zugnameEingabe.Update();
        zugnameEingabe.Draw();

        if (zugnameEingabe.IsActive()) {
            kannBewegen = false;
        }

        int yPos = 150;
        const int lineHeight = 25;

        for (const auto& zug : aktiveZuege) {
            if (zug.zugId == ausgewahlterZug) {
                const_cast<Zug&>(zug).name = zugnameEingabe.GetText();

                DrawText(TextFormat("Name: %s", zug.name.c_str()), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("ID: %d", zug.zugId), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Position: [%d, %d]", zug.posX, zug.posY), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Rotation: %d°", zug.rotation), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Geschwindigkeit: %d km/h", zug.geschwindichkeit), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Passagiere: %d", zug.passagiere), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Güter: %d", zug.gueter), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                DrawText(TextFormat("Zugtyp: %s", zug.zugtyp.c_str()), GenaueBreite - 240, yPos, 20, BLACK);
                yPos += lineHeight;

                if (!zug.Fahrplan.empty()) {
                    yPos += 5;
                    DrawText("Fahrplan:", GenaueBreite - 240, yPos, 18, DARKBLUE);
                    yPos += lineHeight - 5;

                    for (const auto& halt : zug.Fahrplan) {
                        if (yPos < GenaueHoehe - 50) {
                            DrawText(TextFormat("- %s", halt.c_str()), GenaueBreite - 230, yPos, 16, DARKGRAY);
                            yPos += lineHeight - 5;
                        }
                    }
                }

                break;
            }
        }

        if (!zugnameEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AktiveZuegeSpeichern();
        }
    }
}