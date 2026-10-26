#include "access.hpp"
#include <cassert>
int main(){
 using namespace access_demo;Controller c;
 assert(!c.tick(0));assert(c.scan(0,"invalid").decision==Decision::invalid);
 assert(c.scan(0,"FFFFFFFF").decision==Decision::denied);
 assert(c.scan(1,"A1B2C3D4").decision==Decision::granted);
 assert(c.scan(100,"A1B2C3D4").decision==Decision::duplicate);
 assert(c.scan(200,"11223344").decision==Decision::busy);
 assert(c.tick(3000));assert(!c.tick(3001));
 assert(c.scan(4000,"11223344").decision==Decision::granted);
 assert(!c.tick(7000));
 Controller w;assert(w.scan(0xffffff00u,"A1B2C3D4").decision==Decision::granted);
 assert(w.tick(100));assert(!w.tick(2744));
 assert(c.scan(8000,"a1b2c3d4").decision==Decision::invalid);
}
