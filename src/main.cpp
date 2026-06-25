// main.cpp

// 程序入口,创建导演并运行主循环
#include <SFML/Main.hpp>

#include "Director/Director.h"

int main()
{
    Director director;
    director.run();
    return 0;
}
