#include <poppler.h>
#include <assert.h>
#include <iostream>
#include "pdf.h"

PopplerContainer::~PopplerContainer()
{
    g_object_unref(page);
    g_object_unref(document);
}

#define LATEX_SCALEF 2.5

void PopplerContainer::draw(cairo_t* cr)
{
    assert(page != NULL);
    assert(cr != NULL);
    cairo_scale(cr, LATEX_SCALEF, LATEX_SCALEF);
    poppler_page_render(page, cr);
    cairo_scale(cr, 1.0/LATEX_SCALEF, 1.0/LATEX_SCALEF);
}

PopplerContainer::PopplerContainer(const char* filename, double& width, double& height)
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
        std::string msg = "Error loading PDF: foo";
        throw msg;
    }

    document = poppler_document_new_from_file(uri, NULL, &error);

    if(document == NULL) {
        std::string msg = "Error loading PDF: foo";
        throw msg;
    }

    int num_pages = poppler_document_get_n_pages(document);
    if(num_pages != 1) {
        std::string msg = "Error loading PDF: more than 1 page.";
        throw msg;
    }

    page = poppler_document_get_page(document, 0);
    if(page == NULL) {
        std::string msg = "Error loading PDF: page not found";
        throw msg;
    }

    poppler_page_get_size(page, &width, &height);
    width *= LATEX_SCALEF;
    height *= LATEX_SCALEF;
}
