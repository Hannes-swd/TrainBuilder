#include "globals.h"
#include <fstream>
#include <iostream>

void mauszeiger(std::string mouszeiger) {
	if (mouszeiger == "Standard")
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	if (mouszeiger == "Pfeil")
		SetMouseCursor(MOUSE_CURSOR_ARROW);
	if (mouszeiger == "Fadenkreuz")
		SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
}