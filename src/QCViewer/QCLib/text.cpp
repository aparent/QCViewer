#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text.h"
#include "pdf.h"

class PangoTextObject : public TextObject {
  public:
    PangoTextObject(std::string text);
    ~PangoTextObject();
    virtual void draw(cairo_t* cr, double x, double y);
  private:
    PangoFontDescription* desc;
};

class LatexTextObject : public TextObject {
  public:
    LatexTextObject(std::string text);
    ~LatexTextObject();
    virtual void draw(cairo_t* cr, double x, double y);
  private:
    PopplerContainer* pdf;
};

/* PangoTextObject */

PangoTextObject::PangoTextObject(std::string text) {
  contents = text;
  desc = pango_font_description_from_string("LM Roman 12, Roman, Serif bold 18");

  // XXX TODO
  width = 0;
  height = 0;
  x = 0;
  y = 0;

  assert(desc != NULL);
}

void PangoTextObject::draw(cairo_t* cr, double x, double y) {
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

PangoTextObject::~PangoTextObject() {
  pango_font_description_free(desc);
}

/* Latex Text Object */

LatexTextObject::LatexTextObject(std::string text) {
  contents = text;
  pdf = NULL;

  /*
      XXX XXX XXX

      This is _not_ good for concurrent qcviewer processes. We should really
      put things into /tmp correctly. The annoyingness is preview.sty.
  */
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
  if(chdir("tex/")) {
    std::string msg = "Couldn't cd into tex/";
    throw msg;
  }
  char filename[256];
  int id = rand();
  snprintf(filename, sizeof(filename), "QCV%d.tex", id);
  FILE* texf = fopen(filename, "w");
  if(texf == NULL) {
    throw "Could not open TeX file.";
  }
  fprintf(texf, tmpl, text.c_str());
  fclose(texf);
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "pdflatex -interaction=nonstopmode %s", filename);
  if(system(cmd)) {
    chdir("..");
    std::string msg = "Failed to render \"" + text + "\"";
    throw msg;
  }
  snprintf(filename, sizeof(filename), "QCV%d.pdf", id);
  pdf = new PopplerContainer(filename, width, height);
  x = 0; // XXX
  y = 0; // XXX
  system("rm QCV*.*"); // XXX
  chdir("..");
}

void LatexTextObject::draw(cairo_t* cr, double x, double y) {
  assert(cr != NULL);
  assert(pdf != NULL);
  cairo_translate(cr, x, y);
  pdf->draw(cr);
  cairo_translate(cr, -x, -y);
}

LatexTextObject::~LatexTextObject() {
  delete pdf;
}

/* TextEngine */

TextEngine::TextEngine() {
  setMode(TEXT_LATEX);
}

TextEngine::~TextEngine() {
  flushCache();
}

void TextEngine::flushCache() {
  for(auto it = cache.begin(); it != cache.end(); it++) {
    delete *it;
  }
  cache.clear();
}

TextObject* TextEngine::renderText(std::string text) {
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

void TextEngine::setMode(TextMode m) {
  mode = m;
  flushCache();
}

void TextEngine::latexFailure(std::string msg) {
  std::cerr << "in TextEngine::latexFailure. Reason: " << msg << "\n";
  setMode(TEXT_PANGO);
}
