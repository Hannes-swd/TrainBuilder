#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <raylib.h>
#include <string>

class TextBox {
private:
    Rectangle bounds;
    std::string text;
    int maxLength;
    bool active;
    bool hover;

public:
    TextBox(float x, float y, float width, float height, int maxLen = 256);

    void Update();

    void Draw() const;

    void SetPosition(float x, float y);
    void SetSize(float width, float height);
    void SetText(const std::string& newText);
    const std::string& GetText() const;
    bool IsActive() const;
    void Clear();
};

#endif