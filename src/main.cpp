//驱动文件
#include <iostream>
#include <cstdlib>
#include "./scene.hpp"
#include "./init.hpp"

extern const int Number_Of_Digit;

int main()
{
    MyScene Scene = MyScene();
    Init initialize = Init();
    Scene.run_game(initialize);

    return 0;
}