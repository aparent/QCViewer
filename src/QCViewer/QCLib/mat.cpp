#include "mat.h"
#include <iostream>

void test_mat()
{
    Matrix<float> u(4,5,0.f);
    u.set(2,3,11.f);
    u.set(0,0,12.f);
    u.set(1,0,13.f);
    u.set(0,1,14.f);

    Matrix<float> v(4,5,0.f);
    v.set(2,2,15.f);
    v.set(2,3,-12.f);
    v.set(0,0,-11.f);

    Matrix<float> w = u.transpose();
    Matrix<float> uu = 2.f * u * 2.f;
    uu *= 1.f/2.f;

    std::cout << u.toString() << "\n" << v.toString() << "\n" << (u += v).toString()
              << "\n\n" << (w * v).toString() << "\n" << (v * w).toString() << "\n";

    std::cout << Matrix<float>(uu).vappend(v).toString() << "\n" << Matrix<float>(uu).happend(v).toString();

    std::cout << u.toString() << "\n" << v.toString() << "\n" << (u % v).toString() << "\n" << (v % u).toString();
}
