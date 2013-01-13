#include <poppler.h>
#include <assert.h>
#include <iostream>
#include <cstdlib>
#include "pdf.h"

#define LATEX_SCALEF 2.5

PDFReader::PDFReader(const char * filename)
{
    GError* error = NULL;
    gchar* absolute;
    gchar* uri;
    if(g_path_is_absolute(filename)) {
        absolute = g_strdup(filename);
    } else {
        gchar* dir = g_get_current_dir();
        absolute = g_build_filename(dir, filename, (gchar*)0);
        free(dir);
    }

    uri = g_filename_to_uri(absolute, NULL, &error);
    free(absolute);
    if(uri == NULL) {
        throw std::string("PDFReader: Couldn't convert PDF filename to URI.");
    }

    document = poppler_document_new_from_file(uri, NULL, &error);

    if(document == NULL) {
        throw std::string("PDFReader: Couldn't open PDF document.");
    }

    int num_pages = poppler_document_get_n_pages(document);
    pages.reserve(num_pages);

    for(int i=0; i<num_pages; i++) {
        PopplerPage * page = poppler_document_get_page(document, i);
        if(page == NULL) {
            throw std::string("PDFReader: Error loading PDF: page not found.");
        }
        pages.push_back(std::shared_ptr<PopplerContainer>(new PopplerContainer(page)));
    }
}

PDFReader::~PDFReader()
{
    g_object_unref(document);
}

uint32_t PDFReader::getNumPages()
{
    return pages.size();
}

std::shared_ptr<PopplerContainer>
PDFReader::getPage(int n)
{
    std::shared_ptr<PopplerContainer> page = pages.at(n);
    if(!page) {
        throw std::string("PDFReader: Requested a nonexistent page.");
    }
    return page;
}

PopplerContainer::~PopplerContainer()
{
    g_object_unref(page);
}

void PopplerContainer::getPageDimensions(double &width, double &height)
{
    poppler_page_get_size(page, &width, &height);
    width *= LATEX_SCALEF;
    height *= LATEX_SCALEF;
}

void PopplerContainer::draw(cairo_t* cr)
{
    assert(page != NULL);
    assert(cr != NULL);
    cairo_scale(cr, LATEX_SCALEF, LATEX_SCALEF);
    poppler_page_render(page, cr);
    cairo_scale(cr, 1.0/LATEX_SCALEF, 1.0/LATEX_SCALEF);
}

PopplerContainer::PopplerContainer(PopplerPage * pg)
{
    page = pg;
}
