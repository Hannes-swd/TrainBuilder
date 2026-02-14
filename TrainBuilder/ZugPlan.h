#ifndef ZUGPLAN_H
#define ZUGPLAN_H

#include <vector>
#include <string>

class Zug;

extern int draggedZugId;
extern int draggedHaltestelleIndex;
extern int dropLinePosition;
extern float dragOffsetY;

// Funktionen
void ZeichneZugplan(int id, float& scrollOffset, bool& isScrolling, float containerY);
void ZeichneDraggingItem(int zugId, float containerY);
void NeueHaltestelleHinzufuegen(int zugId, float containerY, float containerHeight);

#endif