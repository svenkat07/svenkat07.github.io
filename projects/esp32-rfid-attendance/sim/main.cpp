#include "access.hpp"
#include <iostream>
int main(){
 access_demo::Controller c;
 struct Scan{uint32_t ms;const char* uid;};
 const Scan scans[]={{0,"A1B2C3D4"},{500,"A1B2C3D4"},{1000,"11223344"},
  {3000,"FFFFFFFF"},{4000,"11223344"},{4500,"BAD"},{7500,"A1B2C3D4"}};
 for(auto s:scans){char b[256];auto e=c.scan(s.ms,s.uid);int n=access_demo::json(e,true,b,sizeof(b));if(n<0||n>=int(sizeof(b)))return 1;std::cout<<b<<'\n';}
}
