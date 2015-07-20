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
#include "info.h"
#include <iostream>
#include <cstdlib>
#include <boost/program_options.hpp>

QCViewer* window;

struct Options {
    QCVOptions qcvOptions;
    bool exit;
    std::string circuitFile;

    Options() : exit(false) {}
};

Options handleOptions(int,char*[]);

int main (int ac, char *av[])
{
    srand((unsigned)time(NULL));
    Gtk::Main kit(ac, av);
    Options ops = handleOptions(ac, av);
    if (ops.exit)
        return EXIT_SUCCESS;
    UGateSetup();
    window = new QCViewer(ops.qcvOptions);
    window->set_default_size (800,600);
    if (!ops.circuitFile.empty())
        window->open_circuit(ops.circuitFile);
    std::cerr << "Running window\n";

    Gtk::Main::run(*window);

    return EXIT_SUCCESS;
}


Options handleOptions(int ac,char *av[])
{
    namespace po = boost::program_options;

    Options Op;

    //Options parsed from the command line
    po::options_description cmd("CmdLine Options");
    cmd.add_options()
    ("help", "produce help message")
    ("version", "prints out the version")
    ;

    //Options parsed from the command line and not shown in help message
    po::options_description cmdHidden("Hidden CmdLine Options");
    cmdHidden.add_options()
    ("input-file", po::value<std::string>(), "circuit file to open")
    ;
    po::positional_options_description p;
    p.add("input-file", 1);

    //Options parsed from the config file
    po::options_description config("Config");
    config.add_options()
    ("draw.dotradius", po::value<double>(&Op.qcvOptions.draw.dotradius)->default_value(10.0), "The Radius of the control dot.")
    ;

    po::options_description cmdOptions;
    cmdOptions.add(cmd).add(cmdHidden);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(cmdOptions).positional(p).run(), vm);
    po::store(po::parse_config_file<char>("QCV.cfg", config), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << cmd << std::endl;
        Op.exit = true;
    }
    if (vm.count("version")) {
        std::cout << QCV_NAME << " " << QCV_VERSION << std::endl;
        Op.exit = true;
    }
    if (vm.count("input-file")) {
        Op.circuitFile = vm["input-file"].as<std::string>();
    }

    return Op;
}
