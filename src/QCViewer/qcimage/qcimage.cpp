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

Authors: Alex Parent
---------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <QCLib/circuitParser.h>
#include <cairo-ft.h>
#include <QCLib/gates/UGateLookup.h>

using namespace std;
int main (int argc, char *argv[])
{
    Circuit* c;
    UGateSetup();
    FT_Library library;
    FT_Face ft_face;
    FT_Init_FreeType( &library );
    FT_New_Face( library, "data/fonts/cmbx12.ttf", 0, &ft_face );
    cairo_font_face_t * ft_default = cairo_ft_font_face_create_for_ft_face (ft_face, 0);

    if (argc > 0) {
        vector<string> error_log;
        c = parseCircuit(argv[1],error_log);
        c->getGreedyParallel();
        c->expandAll();
        ofstream myfile;
        string filename = string(argv[1])+".metrics";
        cout << filename << endl;
        myfile.open (filename.c_str());
        myfile << "Gates: " << c->totalGates() << endl;
        myfile.close();
    } else {
        cout << "No circuit file provided" << endl;
        return 0;
    }
    if (argc>3) {
        if (string(argv[2]).compare("-svg") == 0) {
            c->savesvg(argv[3],ft_default);
        } else if (string(argv[2]).compare("-ps") == 0) {
            c->saveps(argv[3],ft_default);
        } else if (string(argv[2]).compare("-png") == 0) {
            c->savepng(argv[3],ft_default);
        }
    } else if (argc > 2) {
        c->savepng(argv[2],ft_default);
    } else {
    }
    return 0;
}
