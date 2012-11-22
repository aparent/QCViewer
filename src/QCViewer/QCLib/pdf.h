#ifndef _PDF_INCLUDED_
#define _PDF_INCLUDED_
#include <cairo.h>
#include <poppler.h>
#include <string>

class PopplerContainer
{
public:
    PopplerContainer(const char*, double&, double&);
    ~PopplerContainer();
    void draw(cairo_t* cr);

private:
    PopplerDocument* document;
    PopplerPage* page;
};

#endif
