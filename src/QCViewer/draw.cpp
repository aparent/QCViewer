/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

See also 'ADDITIONAL TERMS' at the end of the included LICENSE file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QCViewer is a trademark of the of the The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

Authors: Alex Parent, Jacob Parker
---------------------------------------------------------------------*/


#include <cairo-ft.h>
#include <cmath>
#include "QCLib/draw_constants.h"
#include "draw.h"

#ifndef M_PI
#define M_PI 3.141592
#endif

using namespace std;

//Font stuff
FT_Library library;
FT_Face ft_face;
cairo_font_face_t * ft_default;

void init_fonts()
{
    FT_Init_FreeType( &library );
    FT_New_Face( library, "data/fonts/cmbx12.ttf", 0, &ft_face );
    ft_default = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
}



int pickWire (double y)
{
    if (y < yoffset+wireDist/2) return -1;
    if (y < yoffset+wireDist) return 0;
    int wire = floor((y-yoffset)/wireDist - 1);
    if ((double)(y - wireToY (wire)) > wireDist/2) return wire + 1;
    return wire;
}


int pickRect (const vector<gateRect> &rects, double x, double y, vector<int> &selections)
{
    for (int i = 0; i < (int)rects.size (); i++) {
        if (rects[i].x0 <= x && rects[i].x0+rects[i].width >= x && rects[i].y0 <= y && rects[i].y0 + rects[i].height >= y) selections.push_back(i);
        if (!rects[i].subRects.empty()) {
            pickRect (rects[i].subRects,x,y,selections);
        }
    }
    if (selections.size() > 0) {
        return selections.at(0);
    }
    return -1;
}

vector<Selection> pickRects (const vector<gateRect> &rects, const gateRect &s)
{
    vector<Selection> ans;
    for (uint32_t i = 0; i < (uint32_t)rects.size (); i++) {
        if (rects[i].x0 <= s.x0 && rects[i].x0+rects[i].width <= s.x0) continue;
        if (s.x0 <= rects[i].x0 && s.x0+s.width <= rects[i].x0) continue;
        if (rects[i].y0 <= s.y0 && rects[i].y0+rects[i].height <= s.y0) continue;
        if (s.y0 <= rects[i].y0 && s.y0+s.height <= rects[i].y0) continue;
        if (!rects[i].subRects.empty()) {
            vector<Selection> sub;
            sub = pickRects (rects[i].subRects, s);
            ans.push_back (Selection(i,sub));
        } else {
            ans.push_back (Selection(i));
        }
    }
    return ans;
}
