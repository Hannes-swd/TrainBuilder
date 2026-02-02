#include <cstdlib>
#include <iostream>
#include <ctime>
#include "raylib.h"
#include "globals.h"

#include <fstream>
#include "json.hpp"
#include "LoadTexture.h"

void ZeichneGleise() {
	/*DrawTexture("GleisGerade", 1 * 50.0f, 3 * 50.0f, 50.0f, 50.0f, WHITE);*/

	for (const auto& gleis : gleisListe) {
		float x = gleis.GridX * 50.0f;
		float y = gleis.GridY * 50.0f;

		DrawTexture("GleisGerade", x, y, 50.0f, 50.0f);
	}
}