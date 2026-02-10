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

/*-------------------------------------------------
	SCROLLBAR VARIABLEN
-------------------------------------------------*/
static float bahnhofScrollOffset = 0.0f;
static float zugScrollOffset = 0.0f;
static bool bahnhofScrolling = false;
static bool zugScrolling = false;

const float SEITENMENÜ_BREITE = 250.0f;
const float SEITENMENÜ_Y_START = 80.0f;
const float SCROLLBAR_BREITE = 12.0f;

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

void ZeichneScrollbar(float scrollOffset, float maxScroll, bool isActive) {
	float seitenmenüX = GenaueBreite - SEITENMENÜ_BREITE;
	float scrollbarX = seitenmenüX + SEITENMENÜ_BREITE - SCROLLBAR_BREITE;
	float scrollbarY = SEITENMENÜ_Y_START;
	float scrollbarHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;

	if (maxScroll <= 0) return;

	DrawRectangle(scrollbarX, scrollbarY, SCROLLBAR_BREITE, scrollbarHoehe, Color{ 200, 200, 200, 255 });

	float thumbHoehe = scrollbarHoehe * (scrollbarHoehe / (scrollbarHoehe + maxScroll));
	float thumbY = scrollbarY + (scrollOffset / maxScroll) * (scrollbarHoehe - thumbHoehe);

	Color thumbColor = isActive ? DARKGRAY : GRAY;
	DrawRectangle(scrollbarX, thumbY, SCROLLBAR_BREITE, thumbHoehe, thumbColor);
	DrawRectangleLines(scrollbarX, thumbY, SCROLLBAR_BREITE, thumbHoehe, BLACK);
}

float ProccessScrollInput(float currentOffset, float maxScroll, float scrollbarX, float scrollbarY,
	float scrollbarHoehe, bool& isScrolling) {
	Vector2 mousePos = GetMousePosition();
	float scrollbarThumbHoehe = scrollbarHoehe * (scrollbarHoehe / (scrollbarHoehe + maxScroll));

	/*-------------------------------------------------
		MAUSRAD SCROLLING
	-------------------------------------------------*/
	if (CheckCollisionPointRec(mousePos, Rectangle{ scrollbarX - 250 + SCROLLBAR_BREITE,
		scrollbarY, 250.0f - SCROLLBAR_BREITE, scrollbarHoehe })) {
		float wheelInput = GetMouseWheelMove();
		if (wheelInput != 0) {
			float newOffset = currentOffset - wheelInput * 30.0f;
			return MeineClamp(newOffset, 0.0f, maxScroll);
		}
	}

	/*-------------------------------------------------
		SCROLLBAR DRAG
	-------------------------------------------------*/
	float thumbY = scrollbarY + (currentOffset / maxScroll) * (scrollbarHoehe - scrollbarThumbHoehe);
	Rectangle thumbRect = { scrollbarX, thumbY, SCROLLBAR_BREITE, scrollbarThumbHoehe };

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, thumbRect)) {
		isScrolling = true;
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		isScrolling = false;
	}

	if (isScrolling) {
		float mouseDelta = GetMouseDelta().y;
		float scrollRatio = maxScroll / (scrollbarHoehe - scrollbarThumbHoehe);
		float newOffset = currentOffset + mouseDelta * scrollRatio;
		return MeineClamp(newOffset, 0.0f, maxScroll);
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

	DrawTexture("zeichnen", 10.0f, 10.0f, 60.0f, 60.0f);
	DrawTexture("Löschen", 80.0f, 10.0f, 60.0f, 60.0f);
	DrawTexture("Auswahl", 150.0f, 10.0f, 60.0f, 60.0f);
	DrawTexture("Banhof", 220.0f, 10.0f, 60.0f, 60.0f);
	DrawTexture("zugicon", 290.0f, 10.0f, 60.0f, 60.0f);

	/*-------------------------------------------------
		BAHNHOF MENÜ
	-------------------------------------------------*/
	if (ausgewahlterBanhof != 0) {
		DrawRectangle((float)GenaueBreite - 250.0f, 80.0f, 250.0f, (float)GenaueHoehe - 80.0f, LIGHTGRAY);
		DrawRectangleLines((GenaueBreite - 250), 80, 250, GenaueHoehe - 80, DARKGRAY);

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

		float bahnhofContentHoehe = 4 * 30;
		float maxBahnhofScroll = BerechneMaxScrollHoehe(bahnhofContentHoehe);

		float scrollbarX = GenaueBreite - SCROLLBAR_BREITE;
		float scrollbarY = SEITENMENÜ_Y_START;
		float scrollbarHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;

		bahnhofScrollOffset = ProccessScrollInput(bahnhofScrollOffset, maxBahnhofScroll,
			scrollbarX, scrollbarY, scrollbarHoehe, bahnhofScrolling);

		if (maxBahnhofScroll > 0) {
			ZeichneScrollbar(bahnhofScrollOffset, maxBahnhofScroll, bahnhofScrolling);
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
		int yPos = 150 - (int)zugScrollOffset;
		const int lineHeight = 25;
		float contentX = GenaueBreite - 240.0f;

		for (const auto& zug : aktiveZuege) {
			if (zug.zugId == ausgewahlterZug) {
				const_cast<Zug&>(zug).name = zugnameEingabe.GetText();

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
				/*
					FAHRPLAN
				*/
				if (!zug.Fahrplan.empty()) {
					yPos += 5;
					DrawText("Fahrplan:", contentX, yPos, 18, DARKBLUE);
					yPos += lineHeight - 5;

					for (const auto& halt : zug.Fahrplan) {
						DrawText(TextFormat("- %s", halt.c_str()), contentX + 10, yPos, 16, DARKGRAY);
						yPos += lineHeight - 5;
					}
				}

				/*
					LÖSCHEN BUTTON
				*/
				DrawRectangle(GenaueBreite - 240, yPos + 10, 220, 30, RED);
				DrawRectangleLines(GenaueBreite - 240, yPos + 10, 220, 30, WHITE);
				DrawText("Zug loeschen", GenaueBreite - 230, yPos + 15, 20, WHITE);

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
					GetMouseX() >= GenaueBreite - 240 && GetMouseX() <= GenaueBreite - 20 &&
					GetMouseY() >= yPos + 10 && GetMouseY() <= yPos + 40) {
					// Zug löschen
					for (auto it = aktiveZuege.begin(); it != aktiveZuege.end(); ++it) {
						if (it->zugId == ausgewahlterZug) {
							aktiveZuege.erase(it);
							ausgewahlterZug = 0;
							AktiveZuegeSpeichern();
							break;
						}
					}
				}
				/*
					--LÖSCHEN BUTTON
				*/

				break;
			}
		}

		/*-------------------------------------------------
			ZUG SCROLLBAR BERECHNUNG
		-------------------------------------------------*/
		int zugContentHoehe = 30 + (8 * lineHeight);
		for (const auto& zug : aktiveZuege) {
			if (zug.zugId == ausgewahlterZug) {
				if (!zug.Fahrplan.empty()) {
					zugContentHoehe += (int)(5 + (zug.Fahrplan.size() * (lineHeight - 5)));
				}
				zugContentHoehe += 50;
				break;
			}
		}

		float maxZugScroll = BerechneMaxScrollHoehe(zugContentHoehe);

		float scrollbarX = GenaueBreite - SCROLLBAR_BREITE;
		float scrollbarY = SEITENMENÜ_Y_START;
		float scrollbarHoehe = GenaueHoehe - SEITENMENÜ_Y_START - 50.0f;

		zugScrollOffset = ProccessScrollInput(zugScrollOffset, maxZugScroll,
			scrollbarX, scrollbarY, scrollbarHoehe, zugScrolling);

		if (maxZugScroll > 0) {
			ZeichneScrollbar(zugScrollOffset, maxZugScroll, zugScrolling);
		}

		if (!zugnameEingabe.IsActive() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			AktiveZuegeSpeichern();
		}
	}
}