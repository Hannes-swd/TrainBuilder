#include "raylib.h"
#include "LoadTexture.h"
#include <unordered_map>
#include <string>

std::unordered_map<std::string, Texture2D> textures;

void loadTextures() {
    /*-------------------------------------------------
        GLEISE
    -------------------------------------------------*/
    textures["GleisVertikal"] = LoadTexture("resurses/Texturen/schienen/GleisVertikal.png");
    textures["GleisHorizontal"] = LoadTexture("resurses/Texturen/schienen/GleisHorizontal.png");
    //Kurven
    textures["GleisKurve1"] = LoadTexture("resurses/Texturen/schienen/GleisKurve1.png");
    textures["GleisKurve2"] = LoadTexture("resurses/Texturen/schienen/GleisKurve2.png");
    textures["GleisKurve3"] = LoadTexture("resurses/Texturen/schienen/GleisKurve3.png");
    textures["GleisKurve4"] = LoadTexture("resurses/Texturen/schienen/GleisKurve4.png");
    //Kreuzung
    textures["GleisKreuzung1"] = LoadTexture("resurses/Texturen/schienen/GleisKreuzung1.png");
    textures["GleisKreuzung3"] = LoadTexture("resurses/Texturen/schienen/GleisKreuzung3.png");
    textures["GleisKreuzung2"] = LoadTexture("resurses/Texturen/schienen/GleisKreuzung2.png");
    textures["GleisKreuzung4"] = LoadTexture("resurses/Texturen/schienen/GleisKreuzung4.png");
    //Kreuz
    textures["GleisKreuz"] = LoadTexture("resurses/Texturen/schienen/GleisKreuz.png");
    /*-------------------------------------------------
        UI
    -------------------------------------------------*/
    textures["zeichnen"] = LoadTexture("resurses/Texturen/Ui/zeichnen.png");
    textures["Löschen"] = LoadTexture("resurses/Texturen/Ui/Löschen.png");
    textures["Auswahl"] = LoadTexture("resurses/Texturen/Ui/Auswahl.png");
    textures["Banhof"] = LoadTexture("resurses/Texturen/Ui/Banhof.png");
    
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