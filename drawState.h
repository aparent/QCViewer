#ifndef DRAWSTATE__INCLUDED
#define DRAWSTATE__INCLUDED

#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>

std::string draw_state (Cairo::RefPtr<Cairo::Context>,State*,float width,float height, float, float);
#endif
