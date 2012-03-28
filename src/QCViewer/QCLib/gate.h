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


#ifndef GATE
#define GATE

#include <string>
#include <vector>
#include "state.h"
#include "common.h"
#include "draw_common.h"

//used to specify a control number and polarity
class Control
{
public:
    Control (int,bool);
    unsigned int wire;
    bool polarity;
};

/*
GATE
---------------
This is the general gate class from which all gates are derived.
Name is kept private since in the case of gates like the arbitray
rotation gate we may want the name to be dependent on the rot amount
*/

class Gate
{
public:
    enum gateType {RGATE, UGATE, SUBCIRC};
    enum dType {NOT, FRED, D_SUBCIRC, DEFAULT};
    virtual ~Gate();
    Gate();
    virtual Gate* clone() const=0;
    virtual void draw(cairo_t *cr,double &xcurr,double &maxX, std::vector <gateRect> &rects) =0;
    virtual std::string getName() const=0;
    virtual State applyToBasis(index_t) const=0;

    unsigned int getLoopCount() const;
    void setLoopCount(unsigned int count);
    gateType type; //used with enum gateType
    dType drawType;
    std::vector <Control> controls;
    std::vector <unsigned int> targets;
		bool breakpoint;

protected:
    unsigned int loop_count;
    gateRect drawControls (cairo_t *cr,uint32_t xc) const;

};

//A gate matrix struct for UGate
struct gateMatrix {
    std::string drawName;
    unsigned int dim;
    std::complex<float_type> * data;
};

//A general unitary gate
class UGate : public Gate
{
public:
    UGate(std::string);
    Gate* clone() const;
    std::string getName() const;
    State applyToBasis(index_t) const;
    void setName(std::string);
    void draw(cairo_t *cr,double &xcurr,double &maxX, std::vector <gateRect> &rects);
    std::string getDrawName();
private:
    gateRect drawFred (cairo_t *cr, uint32_t xc) const;
    gateRect drawCNOT (cairo_t *cr, uint32_t xc) const;
    gateRect drawCU (cairo_t *cr, uint32_t xc) ;
    gateRect drawX (cairo_t *cr, double xc, double yc, double radius) const;
    gateRect drawNOT(cairo_t *cr, double xc, double yc, double radius, bool opaque=true) const;
    unsigned int ExtractInput (index_t) const;
    index_t BuildBitString (index_t, unsigned int) const;
    State ApplyU(index_t) const;
    std::string name;

    std::string dname;
    bool dname_checked;
};

//An arbitrary rotation gate
class RGate : public Gate
{
public:
    enum Axis { X, Y, Z };
    RGate(float_type, Axis);
    Gate* clone() const;
    std::string getName() const;
    State applyToBasis(index_t) const;
    float_type get_rotVal () const; // XXX: remove float_type, consildate this stuff!!
    void set_rotVal (float_type);
    Axis get_axis () const;
    void set_axis (Axis);
    void draw(cairo_t *cr,double &xcurr,double &maxX, std::vector <gateRect> &rects);
private:
    float_type rot;
    index_t BuildBitString (index_t, unsigned int) const;
    State ApplyU(index_t) const;
    Axis axis;
};


void minmaxWire (const std::vector<Control> &ctrl,const std::vector<unsigned int> &targ, unsigned int &dstmin, unsigned int &dstmax);
#endif
