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

extern int aktuellesTool;

extern bool untermenueOffen;

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
	BanhofObjeckt() : Name(""), BanhofId(0), GridX(0), GridY(0), Rotation(0) {}
};

extern std::vector<BanhofObjeckt> banhofListe;

/*-------------------------------------------------
	Züge und Zugarten
-------------------------------------------------*/
//zugarten
struct ZugArt {
	std::string name;
	int geschwindichkeit;
	int passagiere;
	int güter;
	std::string biildpfad;
	std::string farbe;
	std::string zugtyp;
};
extern std::vector<ZugArt> zugArtenListe;