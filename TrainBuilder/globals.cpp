#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include "raylib.h"
#include "globals.h"
#include "Ui.h"



/*-------------------------------------------------
	GRUNDLAGEN
-------------------------------------------------*/
int GenaueHoehe = 0;
int GenaueBreite = 0;
const float GRID_SIZE = 50.0f;

bool kannBewegen = true;
std::string aktuellesUntermenue = "gleistool";
//1 = züge
//2 = schienen
int ausgewählterUntermenuePunkt = 1;


/*-------------------------------------------------
	KARAKTER
-------------------------------------------------*/
Vector2 Spielerposition = { 0, 0 };
float DeltaTime = 0.0f;
Camera2D Playercam = { 0 };

float playerSpeed = 700.0f;

//ausgewähltes objeckt
int ausgewahlterBanhof = 0; //0 = keins
int ausgewahlterZugArt = 0; //0 = keins
int ausgewahlterZug = 0; //0 = keins
int ausgewahlterAmpel = 0; //0 = keins
int ausgewahlterKnoten = 0; //0 = keins


int aktuellesTool = 1; 
//1 = zeichnen
//2 = löschen
//3 = auswahl
//4 = banhof
//5 = zug
//6 = technik

//untermenü
bool untermenueOffen = true;
/*-------------------------------------------------
	Maus
-------------------------------------------------*/
std::string AktuelleMaus = "Standard";
std::string EndereMausZu = "";
/*-------------------------------------------------
	Scrolen
-------------------------------------------------*/
const float SCROLLBAR_BREITE = 12.0f;

/*-------------------------------------------------
	Mausposition
-------------------------------------------------*/
Vector2 mousePosition = { 0, 0 };
Vector2 gridPosition = { 0, 0 };

/*-------------------------------------------------
	MOUSEKLICK/SCHIENEN PLAZIEREN
-------------------------------------------------*/

Vector2 ErsteKlickPosition = { 0, 0 };
Vector2 ZweiteKlickPosition = { 0, 0 };
bool haterstenKlick = false;


/*-------------------------------------------------
	GLEISE
-------------------------------------------------*/
std::vector<GleisObjeckt> gleisListe;

/*-------------------------------------------------
	Banhöfe
-------------------------------------------------*/
std::vector<BanhofObjeckt> banhofListe;

/*-------------------------------------------------
	Züge und Zugarten
-------------------------------------------------*/
//zugarten

std::vector<ZugArt> zugArtenListe;
/*-------------------------------------------------
	Züge
-------------------------------------------------*/
std::vector<Zug> aktiveZuege;
int zugplanContainerHeight = 60;

bool haltestelleMenuOffen = false;

/*-------------------------------------------------
	Ampeln
-------------------------------------------------*/
std::vector<ampel> ampelListe;

/*-------------------------------------------------
	***SCHALTUNGEN***
-------------------------------------------------*/
/*-------------------------------------------------
	Knoten
-------------------------------------------------*/
std::vector<knoten> knotenliste;
/*-------------------------------------------------
	Signale
-------------------------------------------------*/
std::vector<SignalTeil> SignalTeilListe;

/*-------------------------------------------------
	Leiter
-------------------------------------------------*/
std::vector<Leiterobjeckt> LeiterListe;

/*-------------------------------------------------
	Inverter
-------------------------------------------------*/
std::vector<InverterObjeckt> InverterListe;

/*-------------------------------------------------
	Marker
-------------------------------------------------*/
std::vector<MarkerObjeckt> MarkerListe;