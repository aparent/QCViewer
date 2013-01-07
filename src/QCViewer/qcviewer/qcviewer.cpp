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


#include "window.h"
#include "options.h"
#include "QCLib/gates/UGateLookup.h"
#include <iostream>
#include <cstdlib>
#include <boost/program_options.hpp>

QCViewer* window;

QCVOptions handleOptions(int,char*[]);

int main (int ac, char *av[])
{
    srand((unsigned)time(NULL));
    g_type_init();
    Gtk::Main kit(ac, av);
    QCVOptions ops = handleOptions(ac, av);
    init_fonts();
    UGateSetup();
    window = new QCViewer(ops);
    window->set_default_size (800,600);
    std::cerr << "Running window\n";

    Gtk::Main::run(*window);

    return EXIT_SUCCESS;
}


QCVOptions handleOptions(int ac,char *av[])
{
    namespace po = boost::program_options;

    QCVOptions QCVOp;

    //Options parsed from the command line
    po::options_description cmd("CmdLine Options");
    cmd.add_options()
    ("help", "produce help message")
    ("version", "prints out the version")
    ;

    //Options parsed from the config file
    po::options_description config("Config");
    config.add_options()
    ("draw.dotradius", po::value<double>(&QCVOp.draw.dotradius)->default_value(10.0), "The Radius of the control dot.")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, cmd), vm);
    po::store(po::parse_config_file<char>("QCV.cfg", config), vm);
    po::notify(vm);

    return QCVOp;
}
