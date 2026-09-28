#include <iostream>
#include <functional>
#include <array>

using namespace std;

int add(int a, int b){
  auto c = a + b;
  return c;
}

int sub(int a, int b){
  auto c = a - b;
  return c;
}

int main(void){


  using MathFunc = function<int(int, int)>;

  array<MathFunc, 2> mathFuncs = {add, sub};
  
  cout << "The value is : " << mathFuncs[1](10,5) << endl;

  cout << "The value is : " << mathFuncs[0](10,5) << endl;

  return 0;
}
