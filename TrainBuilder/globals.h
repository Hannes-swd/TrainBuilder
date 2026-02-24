#pragma once
#include "raylib.h"
#include <string>
#include <vector>

/*-------------------------------------------------
	GRUNDLAGEN
-------------------------------------------------*/
extern int GenaueHoehe;
extern int GenaueBreite;
extern const float GRID_SIZE;

extern bool kannBewegen;
extern std::string aktuellesUntermenue;
extern int ausgewählterUntermenuePunkt;

/*-------------------------------------------------
	KARAKTER
-------------------------------------------------*/
extern Vector2 Spielerposition;
extern float DeltaTime;
extern Camera2D Playercam;
extern float playerSpeed;


extern Vector2 mousePosition;
extern Vector2 gridPosition;

extern int ausgewahlterBanhof;
extern int ausgewahlterZugArt;
extern int ausgewahlterZug;
extern int ausgewahlterAmpel;
extern int ausgewahlterKnoten;

extern int aktuellesTool;

extern bool untermenueOffen;
/*-------------------------------------------------
	Maus
-------------------------------------------------*/
extern std::string AktuelleMaus;
extern std::string EndereMausZu;
/*-------------------------------------------------
	Scrolen
-------------------------------------------------*/
extern const float SCROLLBAR_BREITE;
/*-------------------------------------------------
	MOUSEKLICK/SCHIENEN PLAZIEREN
-------------------------------------------------*/

extern Vector2 ErsteKlickPosition;
extern Vector2 ZweiteKlickPosition;

extern bool haterstenKlick;


/*-------------------------------------------------
	GLEISE
-------------------------------------------------*/

struct GleisObjeckt {
	int ObjecktId;
	int GridX;
	int GridY;
	int Rotation;
};

extern std::vector<GleisObjeckt> gleisListe;

/*-------------------------------------------------
	Banhöfe
-------------------------------------------------*/
struct BanhofObjeckt {
	std::string Name;
	int BanhofId;
	int GridX;
	int GridY;
	int Rotation;
	std::string ID;
	BanhofObjeckt() : Name(""), BanhofId(0), GridX(0), GridY(0), Rotation(0) {}
};

extern std::vector<BanhofObjeckt> banhofListe;

/*-------------------------------------------------
	Züge und Zugarten
-------------------------------------------------*/
//zugarten
struct ZugArt {
	int id;
	std::string name;
	int geschwindichkeit;
	int passagiere;
	int gueter;
	std::string biildpfad;
	std::string farbe;
	std::string zugtyp;
};
extern std::vector<ZugArt> zugArtenListe;
/*-------------------------------------------------
	Züge
-------------------------------------------------*/
struct Zug {
	
	int zugId;
	//int GridX;
	//int GridY;
	float posX;
	float posY;
	int rotation;

	//zug infos
	std::string name;
	int maxPassagiere;
	int maxGueter;
	int passagiere;
	int gueter;
	std::vector<std::string> Fahrplan;

	//andere
	std::string zugtyp;
	std::string farbe;
	std::string biildpfad;
	int geschwindichkeit;
	//kolisionsbox
	Rectangle GetBoundingBox() const {
		return Rectangle{
			posX * GRID_SIZE,
			posY * GRID_SIZE,
			GRID_SIZE,
			GRID_SIZE
		};
	}
};
extern std::vector<Zug> aktiveZuege;
extern int zugplanContainerHeight;
extern bool haltestelleMenuOffen;

/*-------------------------------------------------
	Ampeln
-------------------------------------------------*/

struct ampel {
	int AmpelId;
	int GridX;
	int GridY;
	bool isGreen;
	std::string Name;
};
extern std::vector<ampel> ampelListe;

/*-------------------------------------------------
	***SCHALTUNGEN***
-------------------------------------------------*/
/*-------------------------------------------------
	Knoten
-------------------------------------------------*/
struct knoten {
	int GridX;
	int GridY;
	std::string Name;
	int eindeutigeId;
	bool Status;
	bool modus;
};
extern std::vector<knoten> knotenliste;
/*-------------------------------------------------
	Signale
-------------------------------------------------*/
enum Signaltyp{
	Ampel,
	Test
};
struct SignalTeil {
	int eindeutigeId;
	std::string nutzerId;
	bool wert = true;
	Signaltyp typ;
};
extern std::vector<SignalTeil> SignalTeilListe;

/*-------------------------------------------------
	Leiter
-------------------------------------------------*/
struct Leiterobjeckt {
	int GridX;
	int GridY;
	int Rotation;
	int eindeutigeId;
	bool Status;
};
extern std::vector<Leiterobjeckt> LeiterListe;

/*-------------------------------------------------
	Inverter
-------------------------------------------------*/
struct InverterObjeckt {
	int GridX;
	int GridY;
	int Rotation;
	int eindeutigeId;
	bool Status;
};
extern std::vector<InverterObjeckt> InverterListe;

/*-------------------------------------------------
	Marker
-------------------------------------------------*/
enum Markerfarben {
	Rot,
	Grün,
	Gelb,
	Orange,
	Braun,
	Violett
};
struct MarkerObjeckt {
	int GridX;
	int GridY;
	int eindeutigeId;
	Markerfarben farbe;
};
extern std::vector<MarkerObjeckt> MarkerListe;