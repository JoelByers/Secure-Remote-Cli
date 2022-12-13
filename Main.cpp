#include "Cli.h"
#include <iostream>
using namespace std;

int main(){

   Cli c;

   c.call("ps -a > a.txt");
   c.call("cat a.txt");
   c.call("ls");

   return 0;
}