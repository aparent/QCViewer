#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/filesystem.hpp>
#ifdef WIN32
#include <windows.h>
#endif
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
    std::shared_ptr<PopplerContainer> pdf;
};

class BatchLatexTextObject : public TextObject
{
public:
    BatchLatexTextObject(std::string text);
    ~BatchLatexTextObject();
    virtual void draw(cairo_t* cr, double x, double y);
private:
    std::shared_ptr<PopplerContainer> pdf;
    void assignContainer(std::shared_ptr<PopplerContainer>);
    friend class TextEngine;
};

/* PangoTextObject */

PangoTextObject::PangoTextObject(std::string text)
    : TextObject()
{
    contents = text;
    batch = false;

    desc = pango_font_description_from_string("LM Roman 12, Roman, Serif bold 18");

    cairo_surface_t *surf= cairo_recording_surface_create (CAIRO_CONTENT_COLOR, NULL);
    cairo_t *cr = cairo_create(surf);
    PangoLayout* layout = pango_cairo_create_layout(cr);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_markup(layout, contents.c_str(), contents.length());
    int w,h;
    pango_layout_get_size(layout, &w, &h);
    width = (double)w/(double)PANGO_SCALE;
    height = (double)h/(double)PANGO_SCALE;
    cairo_destroy (cr);
    cairo_surface_destroy (surf);

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
}

PangoTextObject::~PangoTextObject()
{
    pango_font_description_free(desc);
}

/* Latex Text Object */

#ifdef WIN32
int systemb(const char * cmd, const char * args)
{
    SHELLEXECUTEINFO se;
    memset(&se, 0, sizeof(se));
    se.cbSize = sizeof(se);
    se.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI|SEE_MASK_NO_CONSOLE;
    se.lpFile = cmd;
    se.lpParameters = args;

    int res = ::ShellExecuteEx(&se);

    if (res) {
        WaitForSingleObject(se.hProcess, INFINITE);
        CloseHandle(se.hProcess);
    } else {
        char * err;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR) &err, 0, NULL);
        std::string errs = "CreateProcess: ";
        errs += err;
        LocalFree(err);
        throw errs;
    }

    return !res;
}
#else
int systemb(const char * cmd, const char * args)
{
    char * bfr = (char*)alloca(strlen(cmd)+strlen(args)+2);
    strcpy(bfr, cmd);
    strcat(bfr, " ");
    strcat(bfr, args);
    return system(bfr);
}
#endif

LatexTextObject::LatexTextObject(std::string text)
    : TextObject()
{
    contents = text;
    batch = false;

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

    if(systemb("pdflatex", "--version")) {
        throw "Cannot find LaTeX installation.";
    }

    /* Get a temporary working directory. */
    boost::filesystem::path newwd;
    try {
        newwd = boost::filesystem::temp_directory_path();
        newwd /= boost::filesystem::unique_path();
        create_directory(newwd);
    } catch(boost::filesystem::filesystem_error & e) {
        throw std::string("Couldn't allocate temporary directory.");
    }

    /* Save the current working directory. */
    boost::filesystem::path oldwd;
    try {
        oldwd = boost::filesystem::current_path();
    } catch(boost::filesystem::filesystem_error & e) {
        throw std::string("Couldn't get working directory.");
    }

    /* Change to temporary directory. */
    try {
        boost::filesystem::current_path(newwd);
    } catch(boost::filesystem::filesystem_error & e) {
        throw std::string("Couldn't cd into temporary directory.");
    }

    FILE* texf = fopen("QCV.tex", "w");
    if(texf == NULL) {
        boost::filesystem::current_path(oldwd);
        throw std::string("Couldn't open TeX file.");
    }
    fprintf(texf, tmpl, text.c_str());
    fclose(texf);

    if(systemb("pdflatex", "-interaction=nonstopmode QCV.tex")) {
        boost::filesystem::current_path(oldwd);
        std::string msg = "Failed to render \"" + text + "\"";
        throw msg;
    }
    PDFReader rd("QCV.pdf");
    if(rd.getNumPages() != 1) {
        boost::filesystem::current_path(oldwd);
        throw std::string("Latex output had the wrong number of pages!");
    }
    pdf = rd.getPage(0);
    pdf->getPageDimensions(width, height);
    boost::filesystem::current_path(oldwd);
}

void LatexTextObject::draw(cairo_t* cr, double x, double y)
{
    assert(pdf);
    assert(cr);
    cairo_translate(cr, x, y);
    pdf->draw(cr);
    cairo_translate(cr, -x, -y);
}

LatexTextObject::~LatexTextObject()
{ }

BatchLatexTextObject::BatchLatexTextObject(std::string text)
    : TextObject()
{
    contents = text;
    batch = true;
}

void BatchLatexTextObject::assignContainer(std::shared_ptr<PopplerContainer> pg)
{
    pdf = pg;
    pdf->getPageDimensions(width, height);
}

void BatchLatexTextObject::draw(cairo_t* cr, double x, double y)
{
    assert(pdf);
    assert(cr);
    cairo_translate(cr, x, y);
    pdf->draw(cr);
    cairo_translate(cr, -x, -y);
}

BatchLatexTextObject::~BatchLatexTextObject()
{ }

/* TextEngine */

TextEngine::TextEngine() : mode(TEXT_PANGO), batch(false) { }

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
                if(batch) {
                    obj = new BatchLatexTextObject(text);
                    batchlist.push_back(obj);
                } else {
                    obj = new LatexTextObject(text);
                }
            } catch(std::string &msg) {
                latexFailure(msg);
                goto whoops_pango;
            } catch(std::exception &e) {
                latexFailure(e.what());
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

void TextEngine::beginBatch()
{
    assert(!batch);
    batch = true;
}

void TextEngine::endBatch()
{
    assert(batch);
    if(batchlist.size() > 0) {
        try {
            std::stringstream latexs;
            latexs <<
                   "\\documentclass{article}\n"
                   "\\usepackage[active, tightpage]{preview}\n"
                   "\\usepackage{amsmath}\n"
                   "\\usepackage{amssymb}\n"
                   "\\usepackage[mathscr]{eucal}\n"
                   "\\pagestyle{empty}\n"
                   "\\begin{document}\n";

            for(auto it = batchlist.begin(); it != batchlist.end(); it++) {
                assert((*it)->isBatch());
                latexs << "  \\begin{preview}\n"
                       << "    $" << (*it)->contents << "$\n"
                       << "  \\end{preview}\n"
                       << "  \\newpage\n";
            }
            latexs << "\\end{document}";

            if(systemb("pdflatex", "--version")) {
                throw "Cannot find LaTeX installation.";
            }

            /* Get a temporary working directory. */
            boost::filesystem::path newwd;
            try {
                newwd = boost::filesystem::temp_directory_path();
                newwd /= boost::filesystem::unique_path();
                create_directory(newwd);
            } catch(boost::filesystem::filesystem_error & e) {
                throw std::string("Couldn't allocate temporary directory.");
            }

            /* Save the current working directory. */
            boost::filesystem::path oldwd;
            try {
                oldwd = boost::filesystem::current_path();
            } catch(boost::filesystem::filesystem_error & e) {
                throw std::string("Couldn't get working directory.");
            }

            /* Change to temporary directory. */
            try {
                boost::filesystem::current_path(newwd);
            } catch(boost::filesystem::filesystem_error & e) {
                throw std::string("Couldn't cd into temporary directory.");
            }

            FILE* texf = fopen("QCV.tex", "w");
            if(texf == NULL) {
                boost::filesystem::current_path(oldwd);
                throw std::string("Couldn't open TeX file.");
            }
            fputs(latexs.str().c_str(), texf);
            fclose(texf);

            if(systemb("pdflatex", "-interaction=nonstopmode QCV.tex")) {
                boost::filesystem::current_path(oldwd);
                std::string msg = "Failed to render.";
                throw msg;
            }
            PDFReader rd("QCV.pdf");
            if(rd.getNumPages() != batchlist.size()) {
                boost::filesystem::current_path(oldwd);
                throw std::string("Latex output had the wrong number of pages!");
            }

            boost::filesystem::current_path(oldwd);

            for(uint32_t i = 0; i < batchlist.size(); i++) {
                BatchLatexTextObject * obj = dynamic_cast<BatchLatexTextObject*>(batchlist.at(i));
                std::shared_ptr<PopplerContainer> pdf = rd.getPage(i);
                obj->assignContainer(pdf);
            }
        } catch(std::string &msg) {
            latexFailure(msg);
            setMode(TEXT_PANGO);
        } catch(std::exception &e) {
            latexFailure(e.what());
            setMode(TEXT_PANGO);
        }

        batchlist.clear();
    }
    batch = false;
}

void TextEngine::latexFailure(std::string msg)
{
    std::cerr << "in TextEngine::latexFailure. Reason: " << msg << "\n";
    abort();
#ifdef WIN32
    MessageBox(NULL, msg.c_str(), "LaTeX Error", 0);
#endif
    setMode(TEXT_PANGO);
}
