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


Selection::~Selection()
{
    if (sub != NULL) {
        //delete sub;
    }
}

Selection::Selection()
{
    gate = 0;
    sub = NULL;
}

Selection::Selection(uint32_t n)
{
    gate = n;
    sub = NULL;
}

Selection::Selection(uint32_t n, vector<Selection>* s)
{
    gate = n;
    sub = s;
}

void gateRect::remove()
{
    if (subRects != NULL) {
        for (int i = 0; i < subRects->size(); i++) {
            subRects->at(i).remove();
        }
    }
}

gateRect::gateRect() : x0(0),y0(0),width(0),height(0),subRects(NULL) {}
