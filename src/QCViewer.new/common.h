#ifndef COMMON__INCLUDED
#define COMMON__INCLUDED

#include <string>
#define STR_EXPAND(tok) #tok
#define assert(exp) if ((exp)) ; \
                    else panic_internal("Assertion " STR_EXPAND(exp) " failed", __FILE__, __LINE__, __FUNCTION__)
#define panic(str) panic_internal(str, __FILE__, __LINE__, __FUNCTION__)

struct rectangle_t {
public:
    rectangle_t (float x, float y, float w, float h)
        : x0(x), y0(y), width(w), height(h) {}
    rectangle_t& operator+= (const rectangle_t&);
    float x0;
    float y0;
    float width;
    float height;
};

bool ranges_intersect (uint32_t, uint32_t, uint32_t, uint32_t);
void panic_internal (std::string, std::string, uint32_t, std::string);

#endif
