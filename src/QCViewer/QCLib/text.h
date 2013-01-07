#include <cairo.h>
#include <pango/pangocairo.h>
#include <string>
#include <vector>

enum TextMode {
    TEXT_PANGO,
    TEXT_LATEX
};

class TextObject
{
public:
    virtual ~TextObject() {};
    virtual void draw(cairo_t* cr, double x, double y) = 0;
    double getWidth() {
        return width;
    }
    double getHeight() {
        return height;
    }
    double getX() {
        return x;
    }
    double getY() {
        return y;
    }
    std::string contents;
protected:
    double width, height, x, y;
};

class TextEngine
{
public:
    TextEngine();
    ~TextEngine();

    TextObject* renderText(std::string text);
    void setMode(TextMode m);
    TextMode getMode() {
        return mode;
    }

private:
    void latexFailure(std::string);

    TextMode mode;
    void flushCache();
    std::vector<TextObject*> cache;
};

