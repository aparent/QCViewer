#ifndef _PDF_INCLUDED_
#define _PDF_INCLUDED_
#include <cairo.h>
#include <poppler.h>
#include <string>
#include <vector>
#include <memory>

class PDFReader;

class PopplerContainer
{
public:
    ~PopplerContainer();
    void getPageDimensions(double&, double&);
    void draw(cairo_t* cr);

private:
    PopplerContainer(PopplerPage * pg);
    PopplerPage* page;
    friend class PDFReader;
};

class PDFReader
{
public:
    PDFReader(const char*);
    ~PDFReader();
    uint32_t getNumPages();
    std::shared_ptr<PopplerContainer> getPage(int);

private:
    std::vector<std::shared_ptr<PopplerContainer> > pages;
    PopplerDocument* document;
};

#endif
