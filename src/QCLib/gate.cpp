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

Authors: Alex Parent, Jakub Parker
---------------------------------------------------------------------*/


#include "gate.h"
#include "utility.h"

Control::Control(int setWire, bool setPol) : wire(setWire), polarity (setPol) {}

Gate::~Gate() {}

void minmaxWire (vector<Control>* ctrl, vector<unsigned int>* targ, unsigned *dstmin, unsigned *dstmax)
{
    unsigned int minw = (*targ)[0];
    unsigned int maxw = (*targ)[0];
    for (unsigned int i = 1; i < targ->size (); i++) {
        minw = min (minw, (*targ)[i]);
        maxw = max (maxw, (*targ)[i]);
    }

    for (unsigned int i = 0; i < ctrl->size (); i++) {
        minw = min (minw, (*ctrl)[i].wire);
        maxw = max (maxw, (*ctrl)[i].wire);
    }
    *dstmin = minw;
    *dstmax = maxw;
}
