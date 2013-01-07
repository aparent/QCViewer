#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/filesystem.hpp>
#include "text.h"
#include "pdf.h"

class PangoTextObject : public TextObject
{
public:
    PangoTextObject(std::string text);
    ~PangoTextObject();
    virtual void draw(cairo_t* cr, double x, double y);
private:
    PangoFontDescription* desc;
};

class LatexTextObject : public TextObject
{
public:
    LatexTextObject(std::string text);
    ~LatexTextObject();
    virtual void draw(cairo_t* cr, double x, double y);
private:
    PopplerContainer* pdf;
};

/* PangoTextObject */

PangoTextObject::PangoTextObject(std::string text)
{
    contents = text;
    desc = pango_font_description_from_string("LM Roman 12, Roman, Serif bold 18");

    // XXX TODO
    width = 0;
    height = 0;
    x = 0;
    y = 0;

    assert(desc != NULL);
}

void PangoTextObject::draw(cairo_t* cr, double x, double y)
{
    assert(cr != NULL);
    PangoLayout* layout = pango_cairo_create_layout(cr);
    assert(layout != NULL);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_markup(layout, contents.c_str(), contents.length());
    cairo_move_to(cr, x, y);
    pango_cairo_show_layout(cr, layout);

    // After we've drawn it once we will have more accurate measurements (these might be the same? XXX)
    int w, h;
    pango_layout_get_size(layout, &w, &h);
    width = (double)w/(double)PANGO_SCALE;
    height = (double)h/(double)PANGO_SCALE;
}

PangoTextObject::~PangoTextObject()
{
    pango_font_description_free(desc);
}

/* Latex Text Object */

LatexTextObject::LatexTextObject(std::string text)
{
    contents = text;
    pdf = NULL;

    const char* tmpl =
        "\\documentclass{article}\n"
        "\\usepackage[active, tightpage]{preview}\n"
        "\\usepackage{amsmath}\n"
        "\\usepackage{amssymb}\n"
        "\\usepackage[mathscr]{eucal}\n"
        "\\pagestyle{empty}\n"
        "\\begin{document}\n"
        "  \\begin{preview}\n"
        "    $%s$\n"
        "  \\end{preview}\n"
        "\\end{document}";

    if(system("pdflatex --version")) {
        throw "Cannot find LaTeX installation.";
    }

    /* Get a temporary working directory. */
    char wdbfr[10], *newwd;
    {
      const char * wdtmpl = "tmpXXXXXX";
      strcpy(wdbfr, wdtmpl);
      #ifdef WIN32
      if(newwd = wdbfr, _mktemp_s(wdbfr, wdtmpl))
      #else
      if((newwd = mkdtemp(wdbfr)) == NULL)
      #endif
      {
        throw "Couldn't create temporary directory for TeX.";
      }
    }

    /* Save the current working directory. */
    char oldwd[PATH_MAX];
    if(getcwd(oldwd, PATH_MAX) == NULL) {
      throw "Couldn't get current working directory.";
    }

    if(chdir(newwd)) {
        throw "Couldn't cd into temporary directory.";
    }
    FILE* texf = fopen("QCV.tex", "w");
    if(texf == NULL) {
        chdir(oldwd);
        throw "Couldn't open TeX file.";
    }
    fprintf(texf, tmpl, text.c_str());
    fclose(texf);
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "pdflatex -interaction=nonstopmode QCV.tex");
    if(system(cmd)) {
        chdir(oldwd);
        std::string msg = "Failed to render \"" + text + "\"";
        throw msg;
    }
    pdf = new PopplerContainer("QCV.pdf", width, height);
    x = 0; // XXX
    y = 0; // XXX
    chdir(oldwd);
    boost::filesystem::remove_all(newwd);
}

void LatexTextObject::draw(cairo_t* cr, double x, double y)
{
    assert(cr != NULL);
    assert(pdf != NULL);
    cairo_translate(cr, x, y);
    pdf->draw(cr);
    cairo_translate(cr, -x, -y);
}

LatexTextObject::~LatexTextObject()
{
    delete pdf;
}

/* TextEngine */

TextEngine::TextEngine()
{
    setMode(TEXT_PANGO);
}

TextEngine::~TextEngine()
{
    flushCache();
}

void TextEngine::flushCache()
{
    for(auto it = cache.begin(); it != cache.end(); it++) {
        delete *it;
    }
    cache.clear();
}

TextObject* TextEngine::renderText(std::string text)
{
    TextObject* obj = NULL;
    for(auto it = cache.begin(); it != cache.end(); it++) {
        if((*it)->contents == text) {
            obj = *it;
        }
    }
    if(obj == NULL) {
        switch(mode) {
        case TEXT_PANGO:
whoops_pango:
            cache.push_back(new PangoTextObject(text));
            break;
        case TEXT_LATEX:
            try {
                obj = new LatexTextObject(text);
            } catch(std::string msg) {
                latexFailure(msg);
                goto whoops_pango;
            } catch(...) {
                latexFailure("???");
                goto whoops_pango;
            }
            cache.push_back(obj);
            break;
        default:
            std::cerr << "Unknown text mode in TextEngine.\n";
            exit(1);
            break;
        }
        obj = cache[cache.size() - 1];
    }
    return obj;
}

void TextEngine::setMode(TextMode m)
{
    mode = m;
    flushCache();
}

void TextEngine::latexFailure(std::string msg)
{
    std::cerr << "in TextEngine::latexFailure. Reason: " << msg << "\n";
    setMode(TEXT_PANGO);
}
