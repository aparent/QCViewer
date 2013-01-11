#include <assert.h>
#include <unistd.h>
#include <iostream>
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
    PopplerContainer* pdf;
};

/* PangoTextObject */

PangoTextObject::PangoTextObject(std::string text)
{
    contents = text;
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
    char bfr[strlen(cmd)+strlen(args)+2];
    strcpy(bfr, cmd);
    strcat(bfr, " ");
    strcat(bfr, args);
    system(bfr);
}
#endif

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

    if(systemb("pdflatex", "--version")) {
        throw "Cannot find LaTeX installation.";
    }

    /* Get a temporary working directory. */
    char *newwd;
    {
        char wdbfr[10];
        const char * wdtmpl = "tmpXXXXXX";
        strcpy(wdbfr, wdtmpl);
#ifdef WIN32
        if((newwd = mktemp(wdbfr)) == NULL)
#else
        if((newwd = mkdtemp(wdbfr)) == NULL)
#endif
        {
            throw "Couldn't create temporary directory for TeX (mk[d]temp).";
        }
#ifdef WIN32
        if(mkdir(wdbfr)) {
            throw "Couldn't create temporary directory for TeX (mkdir).";
        }
#endif
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

    if(systemb("pdflatex", "-interaction=nonstopmode QCV.tex")) {
        chdir(oldwd);
        std::string msg = "Failed to render \"" + text + "\"";
        throw msg;
    }
    pdf = new PopplerContainer("QCV.pdf", width, height);
    x = 0; // XXX
    y = 0; // XXX
    chdir(oldwd);
#ifdef WIN32
    const char * swit = "/C del /F/S/Q ";
    char * bfr = (char*)alloca(strlen(swit)+strlen(newwd)+1);
    strcpy(bfr, swit);
    strcat(bfr, newwd);
    systemb("cmd.exe", bfr);
#else
    boost::filesystem::remove_all(newwd);
#endif
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
            } catch(std::string &msg) {
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
#ifdef WIN32
    MessageBox(NULL, msg.c_str(), "LaTeX Error", 0);
#endif
    setMode(TEXT_PANGO);
}
