#include "Cli.h"
#include <iostream>
using namespace std;

int main(){

   Cli c;

   c.call("sl");
   c.call("ps -a > a.txt");
   c.call("cat a.txt");
   c.call("pwd");
   c.call("touch b.txt");
   c.call("cat a.txt > c.txt");
   c.call("ls -l");

   return 0;
}