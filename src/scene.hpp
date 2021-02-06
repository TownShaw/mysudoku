#ifndef _SCENE_H_
#define _SCENE_H_

#include <iostream>
#include "./init.hpp"

extern const int Number_Of_Digit;

class MyScene
{
private:
    //当前指向的 Grid 的指针
    const char * GREEN = "32";   //绿色的颜色代码为 32
    const char * WHITE = "37;97";   //白色的颜色代码为 37
    int ptr_row = 0;
    int ptr_column = 0;

    //不回显读取输入, 使用终端原始模式实现
    inline char getch(void)
    {
        struct termios tmtemp, tm;
        char c;
        int fd = 0;
        if (tcgetattr(fd, &tm) != 0)
        { /*获取当前的终端属性设置，并保存到tm结构体中*/
            return -1;
        }
        tmtemp = tm;
        cfmakeraw(&tmtemp); /* 将tetemp初始化为终端原始模式的属性设置, 原始模式下默认不会回显 */
        if (tcsetattr(fd, TCSANOW, &tmtemp) != 0)
        { /*将终端设置为原始模式的设置*/
            return -1;
        }
        c = getchar();
        if (tcsetattr(fd, TCSANOW, &tm) != 0)
        { /*接收字符完毕后将终端设置回原来的属性*/
            return 0;
        }
        return c;
    }

public:
    MyScene() {}
    ~MyScene() {}

    const int get_Num_of_Digit()
    {
        return Number_Of_Digit;
    }

    //打印无数字行, 纯打印框架
    void print_line_without_Number(int Row)
    {
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            std::cout << "\u254B" << "\u2501" << (((Row - 1) == ptr_row && i == ptr_column) ? "^" : "\u2501") << "\u2501";
        }
        std::cout << "\u254B" << std::endl;
    }

    //根据 Num_Array 打印有数字行
    void print_line_with_Number(Init &initialize, int Row)
    {
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            std::cout << "\u2503";
            if (initialize.get_Digit(Row, i) != -1)
            {
                std::cout << " \033[" << (initialize.isFixed(Row, i) ? WHITE : GREEN) << "m" << initialize.get_Digit(Row, i) << "\033[0m ";
            }
            else
            {//说明此处没有数字 (被挖空) , 仅打印空格
                std::cout << "   ";
            }
        }
        std::cout << "\u2503" << std::endl;
    }

    //打印整个框架
    void print_scene(Init &initialize) //传递 Init 类, 用于访问数独矩阵中的数据
    {
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            print_line_without_Number(i);
            print_line_with_Number(initialize, i);
        }
        print_line_without_Number(Number_Of_Digit);
    }

    //游戏启动
    void run_game(Init &initialize)
    {
        system("clear");    //清屏
        print_scene(initialize);
        while (1)
        {
        Start:
            char c = getch();
            char cer;   //used for certificate
            system("clear");    //清屏
            switch (c)
            {
                case 'w':
                    ptr_row = (ptr_row > 0) ? (ptr_row - 1) : ptr_row;
                    break;
            
                case 's':
                    ptr_row = (ptr_row < Number_Of_Digit - 1) ? (ptr_row + 1) : ptr_row;
                    break;

                case 'a':
                    ptr_column = (ptr_column > 0) ? (ptr_column - 1) : ptr_column;
                    break;

                case 'd':
                    ptr_column = (ptr_column < Number_Of_Digit - 1) ? (ptr_column + 1) : ptr_column;
                    break;

                case 'q':
                    print_scene(initialize);
                    std::cout << "Quit Game ? (Y/N): ";
                    cer = getchar();
                    if (cer == 'y' || cer == 'Y')
                    {
                        return ;
                    }
                    continue;
                
                case '\r':  //验证答案
                    for (int i = 0; i < Number_Of_Digit; i++)
                    {
                        for (int j = 0; j < Number_Of_Digit; j++)
                        {
                            if (!initialize.check_Row(i, j) || !initialize.check_Column(i, j))
                            {
                                print_scene(initialize);
                                std::cout << "Wrong Answer!" << std::endl;
                                goto Start;
                            }
                        }
                    }
                    print_scene(initialize);
                    std::cout << "Congratulations!" << std::endl;
                    return ;

                default:
                    if (c >= '1' && c <= '9')
                    {
                        if (!initialize.isFixed(ptr_row, ptr_column))
                        {
                            initialize.set_Digit(ptr_row, ptr_column, c - '0');
                        }
                        else
                        {
                            print_scene(initialize);
                            std::cout << "This Number can't be changed!" << std::endl;
                            continue;
                        }
                    }
                    break;
            }
            print_scene(initialize);
        }
    }
};


#endif