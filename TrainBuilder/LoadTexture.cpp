#include "raylib.h"
#include "LoadTexture.h"
#include <unordered_map>
#include <string>

std::unordered_map<std::string, Texture2D> textures;

void loadTextures() {

    textures["GleisGerade"] = LoadTexture("resurses/Texturen/schienen/gleis.png");
}

void unloadTextures() {
    for (auto& pair : textures) {
        UnloadTexture(pair.second);
    }
    textures.clear();
}

void DrawTexture(const std::string& name, float x, float y, float width, float height, Color tint) {
    if (textures.find(name) != textures.end()) {
        Texture2D tex = textures[name];
        Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
        Rectangle dest = { x, y, width, height };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(tex, source, dest, origin, 0.0f, tint);
    }
}