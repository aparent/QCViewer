#include <cairo.h>
#include <pango/pangocairo.h>
#include <string>
#include <vector>


#ifndef TEXT_H
#define TEXT_H

enum TextMode {
    TEXT_PANGO,
    TEXT_LATEX
};

class TextObject
{
public:
    TextObject() : width(0), height(0), x(0), y(0) { };
    virtual ~TextObject() {};
    virtual void draw(cairo_t* cr, double x, double y) = 0;
    virtual bool isBatch()
    {
        return batch;
    }
    double getWidth()
    {
        return width;
    }
    double getHeight()
    {
        return height;
    }
    double getX()
    {
        return x;
    }
    double getY()
    {
        return y;
    }
    std::string contents;
protected:
    bool batch;
    double width, height, x, y;
};

class TextEngine
{
public:
    TextEngine();
    ~TextEngine();

    TextObject* renderText(std::string text);
    void setMode(TextMode m);
    TextMode getMode()
    {
        return mode;
    }
    void beginBatch();
    void endBatch();

private:
    void latexFailure(std::string);

    TextMode mode;
    void flushCache();
    std::vector<TextObject*> cache;
    bool batch;
    std::vector<TextObject*> batchlist;
};

#endif
