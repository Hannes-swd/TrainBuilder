#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include "LoadTexture.h"
#include "raylib.h"
#include "globals.h"
#include "Json.h"
#include "textbox.h"
#include "Banhof.h"
#include "untermenü.h"
#include "json.hpp"
#include "zug.h"

using json = nlohmann::json;

void KnotenPlazieren(int gridX, int gridY) {
	//kan nur auf blöcken ohne plazeren
	if (schautObSchiene(gridX, gridY)) {
		return;
	}
	for (const auto& knoten : knotenliste) {
		if (knoten.GridX == gridX && knoten.GridY == gridY)
			return;
	}
	knoten newKnoten;
	newKnoten.GridX = gridX;
	newKnoten.GridY = gridY;
	newKnoten.Status = false;
	//eigene id erstellen
	int neieId = 1;
	for (const auto& knoten : knotenliste) {
		if (knoten.eindeutigeId >= neieId) {
			neieId = knoten.eindeutigeId + 1;
		}
	}
	newKnoten.eindeutigeId = neieId;
	knotenliste.push_back(newKnoten);
	std::cout << "test" ;
}