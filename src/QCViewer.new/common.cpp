#include "common.h"
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#define __QCV_GIT_DESCRIBE__ "foo"
#define __BUG_LOCATION__ "j3parker@uwaterloo.ca and aparent@uwaterloo.ca"

void panic_internal (std::string reason,
                     std::string file,
                     uint32_t line,
                     std::string func)
{
    std::cerr << "\n\nFATAL ERROR OCCURED.\n";
    std::cerr << "\n\tReason:   " << reason;
    std::cerr << "\n\tLocation: " << func << " in " << file << " line " << line;
    std::cerr << "\n\tVersion:  " << __QCV_GIT_DESCRIBE__;
    std::cerr << "\n\nPlease send this text to " << __BUG_LOCATION__ << ".\n";
    std::cerr << "\n\n";

    exit (1);
}

rectangle_t& rectangle_t::operator+= (const rectangle_t& rhs)
{
    if ( rhs.width == rhs.height && rhs.width == 0 ) return *this;
    if ( width == height && width == 0 ) {
        *this = rhs;
        return *this;
    }
    rectangle_t ans(0,0,0,0);
    ans.x0 = std::min (x0, rhs.x0);
    ans.y0 = std::min (y0, rhs.y0);

    if ( x0 + width < rhs.x0 + rhs.width ) ans.width = rhs.x0 + rhs.width - ans.x0;
    else                                   ans.width = x0 + width - ans.x0;

    if ( y0 + height < rhs.y0 + rhs.height ) ans.height = rhs.y0 + rhs.height - ans.y0;
    else                                     ans.height = y0 + height - ans.y0;

    *this = ans;
    return *this;
}

bool ranges_intersect(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1)
{
    assert (x0 <= x1 && y0 <= y1);
    if (x1 < y0 || y1 < x0) return false;
    return true;
}
