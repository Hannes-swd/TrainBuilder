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
#include "untermenü.h"
#include "zug.h"

void zugPlazieren(int gridX, int gridY, int zugArt) {
	if (zugArt == 0) 
		return;
	if (IstZugBereitsVorhanden(gridX, gridY)) 
		return;

	

}
bool IstZugBereitsVorhanden(int gridX, int gridY) {

	for (const auto& zug : aktiveZuege) {
		if (zug.GridX == gridX && zug.GridY == gridY) {
			return true;
		}
	}
	return false;
}
