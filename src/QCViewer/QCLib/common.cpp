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

#include "common.h"


using namespace std;


Selection::Selection()
{
    gate = 0;
}

Selection::Selection(uint32_t n)
{
    gate = n;
}

Selection::Selection(uint32_t n, vector<Selection> s)
{
    gate = n;
    sub = s;
}

gateRect::gateRect() : x0(0),y0(0),width(0),height(0) {}

gateRect combine_gateRect (const gateRect &a, const gateRect &b)
{
    gateRect c;
    c.x0 = min(a.x0, b.x0);
    c.y0 = min(a.y0, b.y0);
    c.width =  max (a.x0-c.x0 + a.width,  b.x0 - c.x0 + b.width);
    c.height = max (a.y0-c.y0 + a.height, b.y0 - c.y0 + b.height);
    return c;
}

int pickRect (const vector<gateRect> &rects, double x, double y, vector<int> &selections)
{
    for (int i = 0; i < (int)rects.size (); i++) {
        if (rects[i].x0 <= x && rects[i].x0+rects[i].width >= x && rects[i].y0 <= y && rects[i].y0 + rects[i].height >= y) selections.push_back(i);
        if (!rects[i].subRects.empty()) {
            pickRect (rects[i].subRects,x,y,selections);
        }
    }
    if (!selections.empty()) {
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
