#include "raylib.h"
#include "LoadTexture.h"
#include <unordered_map>
#include <iostream>
#include <string>

std::unordered_map<std::string, Texture2D> textures;
std::unordered_map<std::string, Texture2D> dynamicTextures;

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
    textures["zugicon"] = LoadTexture("resurses/Texturen/Ui/zugicon.png");
    textures["zahnrad"] = LoadTexture("resurses/Texturen/Ui/zahnrad.png");
    textures["KnotenSymbol"] = LoadTexture("resurses/Texturen/Ui/KnotenSymbol.png");
    textures["Drehen"] = LoadTexture("resurses/Texturen/Ui/Drehen.png");
    textures["marker"] = LoadTexture("resurses/Texturen/Ui/marker.png");
    
    
    /*-------------------------------------------------
        BANHOF
    -------------------------------------------------*/

    /*-------------------------------------------------
        Ampel
    -------------------------------------------------*/
    textures["Ampel_Gruen"] = LoadTexture("resurses/Texturen/schienen/Ampel/Ampel_Gruen.png");
    textures["Ampel_Rot"] = LoadTexture("resurses/Texturen/schienen/Ampel/Ampel_Rot.png");

    /*-------------------------------------------------
        TECHNIK
    -------------------------------------------------*/
    //knoten
    textures["knoten"] = LoadTexture("resurses/Texturen/Technik/knoten.png");
    textures["knoten_An"] = LoadTexture("resurses/Texturen/Technik/knoten_An.png");
    //leiter
    textures["Leiter_Aus"] = LoadTexture("resurses/Texturen/Technik/Leiter_Aus.png");
    textures["Leiter_An"] = LoadTexture("resurses/Texturen/Technik/Leiter_An.png");
    //inverter
    textures["Inverter_Aus"] = LoadTexture("resurses/Texturen/Technik/Inverter_Aus.png");
    textures["Inverter_AN"] = LoadTexture("resurses/Texturen/Technik/Inverter_AN.png");
    
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

//bilder durch pfad laden
Texture2D LoadTextureFromPath(const std::string& path) {
    if (dynamicTextures.find(path) != dynamicTextures.end()) {
        return dynamicTextures[path];
    }

    Texture2D texture = LoadTexture(path.c_str());

    if (texture.id == 0) {
        std::cerr << "Fehler beim Laden der Textur: " << path << std::endl;
        
        Texture2D emptyTexture = { 0 };
        return emptyTexture;
    }

    dynamicTextures[path] = texture;

    return texture;
}