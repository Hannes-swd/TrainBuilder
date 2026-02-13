#pragma once
#include "raylib.h"

void zeichneUI();

float MeineClamp(float value, float min, float max);
float BerechneMaxScrollHoehe(float contentHoehe);
void ZeichneScrollbar(float scrollOffset, float maxScroll, bool isActive, float x, float y, float hoehe);
float ProccessScrollInput(float currentOffset, float maxScroll, float scrollbarX, float scrollbarY,
    float scrollbarHoehe, bool& isScrolling, float contentAreaX, float contentAreaWidth);