#ifndef _INIT_SUDOKU_H_
#define _INIT_SUDOKU_H_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cassert>
#include <vector>
#include <termios.h>

const int Begin_Number = 1;     //从 1 开始的数字, 即数独中的数字为从 1 到 9
const int Number_Of_Digit = 9;  //一行数独的数字个数

class Init
{
private:
    struct Grid
    {
        int Digit;
        bool Fixed; //指示该数字是否为初始设定值, 若是则不允许变动
        Grid() {}
        Grid(int Digit, bool Fixed)
        {
            this->Digit = Digit;
            this->Fixed = Fixed;
        }
    };

    Grid ** Num_Matrix = nullptr;              //用于存放数独的矩阵
    int * Column_Swap_Helper = nullptr;       //为了避免交换列时出现大量的数据移动, 这里使用 Column_Swap_Helper 数组来交换 索引, 而不是交换数据

    //两行进行交换
    void Swap_Row(int Row_1, int Row_2)
    {
        Grid *Temp = Num_Matrix[Row_1];
        Num_Matrix[Row_1] = Num_Matrix[Row_2];
        Num_Matrix[Row_2] = Temp;
    }

    //两列进行交换, 仅改变 Column_Swap_Helper 中的索引
    void Swap_Column(int Column_1, int Column_2)
    {
        int Temp = Column_Swap_Helper[Column_1];
        Column_Swap_Helper[Column_1] = Column_Swap_Helper[Column_2];
        Column_Swap_Helper[Column_2] = Temp;
    }

    //得到最初的数独
    void get_Original_Sudoku()
    {
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            for (int j = 0; j < Number_Of_Digit; j++)
            {
                Num_Matrix[i][j] = {j + 1, true};
                while (!check_Row(i, j) || !check_Column(i, j))
                {
                    Num_Matrix[i][j].Digit = Num_Matrix[i][j].Digit % Number_Of_Digit + 1;  //如 9 的下一位为 1, 即 1 = 9 % 9 + 1; 8 的下一位为 9, 即 9 = 8 % 9 + 1
                }
            }
        }
    }

    //打乱之后的数独
    void get_Shuffled_Sudoku()
    {
        get_Original_Sudoku();
        const int Shuffle_Times = 30;   //默认行列各打乱 30 次
        srand(time(NULL));              //生成不同的数独, 每次生成一个新的数独只需要重置一次随机数种子即可
        for (int i = 0; i < Shuffle_Times; i++)
        {
            int Row_1 = random(0, Number_Of_Digit - 1);
            int Row_2 = random(0, Number_Of_Digit - 1);
            Swap_Row(Row_1, Row_2);

            int Column_1 = random(0, Number_Of_Digit - 1);
            int Column_2 = random(0, Number_Of_Digit - 1);
            Swap_Column(Column_1, Column_2);
        }

        //随机 dropout 一些数字
        system("clear");
        std::cout << "请选择游戏难度: 1.简单  2.适中  3.困难" << std::endl;
        std::string c;
        int difficult = 0;
        std::cin >> c;
        
        while (c[0] != '1' && c[0] != '2' && c[0] != '3')
        {
            std::cout << "输入错误！ 请输入数字 1 ~ 3: " << std::endl;
            std::cin >> c;
        }

        difficult = c[0] - '0';
        std::cout << "difficult: " << difficult << std::endl;
        
        int Drop_out_Number = 0;   //后续可根据难度进行改变

        switch (difficult)
        {
        case 1:
            Drop_out_Number = 35;
            break;
        
        case 2:
            Drop_out_Number = 45;
            break;
        
        case 3:
            Drop_out_Number = 55;
            break;
        
        default:
            Drop_out_Number = 35;
            break;
        }

        while (Drop_out_Number != 0)
        {
            int drop_Row = random(0, Number_Of_Digit - 1);
            int drop_Column = random(0, Number_Of_Digit - 1);
            std::cout << "drop_pos: (" << drop_Row << ", " << drop_Column << ")" << std::endl;
            if (get_Digit(drop_Row, drop_Column) != -1)
            {
                set_Fixed(drop_Row, drop_Column, false);
                set_Digit(drop_Row, drop_Column, -1);
                Drop_out_Number--;
            }
        }
    }

    //得到 [begin, end] 之间的随机数
    inline unsigned int random(int begin, int end)
    {
        assert(end >= begin && begin >= 0);
        return unsigned(rand() % (end - begin + 1) + begin);
    }

    //更改 Fixed 域
    inline bool set_Fixed(int Row, int Column, bool key)
    {
        if (Row >= Number_Of_Digit || Row < 0 || Column >= Number_Of_Digit || Column < 0)
        {
            return false;
        }
        else
        {
            Num_Matrix[Row][Column_Swap_Helper[Column]].Fixed = key;
            return true;
        }
    }
public:
    Init ()
    {
        Num_Matrix = new Grid * [Number_Of_Digit];
        Column_Swap_Helper = new int [Number_Of_Digit];
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            Column_Swap_Helper[i] = i;
            Num_Matrix[i] = new Grid [Number_Of_Digit];
        }
        get_Shuffled_Sudoku();
    }
    ~Init()
    {
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            delete [] Num_Matrix[i];
        }
        delete [] Num_Matrix;
        delete [] Column_Swap_Helper;
    }

    //得到坐标为 (Row, Column) 的数字
    inline int get_Digit(int Row, int Column)
    {
        return Num_Matrix[Row][Column_Swap_Helper[Column]].Digit;
    }

    //更改 Digit 域
    inline bool set_Digit(int Row, int Column, int key)
    {
        if (Row >= Number_Of_Digit || Row < 0 || Column >= Number_Of_Digit || Column < 0)
        {
            return false;
        }
        else if (Num_Matrix[Row][Column_Swap_Helper[Column]].Fixed)
        {//固定数字, 不允许更改
            return false;
        }
        else
        {
            Num_Matrix[Row][Column_Swap_Helper[Column]].Digit = key;
            return true;
        }
    }

    inline bool isFixed(int Row, int Column)
    {
        return Num_Matrix[Row][Column_Swap_Helper[Column]].Fixed;
    }

    //检查 (Row, Column) 在行方向上是否存在冲突
    bool check_Row(int Row, int Column)
    {
        if (Num_Matrix[Row][Column_Swap_Helper[Column]].Digit < 0 || Num_Matrix[Row][Column_Swap_Helper[Column]].Digit > 9)
        {
            return false;
        }
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            if (i == Column)
            {
                continue;
            }
            else if (Num_Matrix[Row][Column_Swap_Helper[Column]].Digit == Num_Matrix[Row][Column_Swap_Helper[i]].Digit)
            {
                return false;
            }
        }
        return true;
    }

    //检查 (Row, Column) 在列方向上是否存在冲突
    bool check_Column(int Row, int Column)
    {
        if (Num_Matrix[Row][Column_Swap_Helper[Column]].Digit < 0 || Num_Matrix[Row][Column_Swap_Helper[Column]].Digit > 9)
        {
            return false;
        }
        for (int i = 0; i < Number_Of_Digit; i++)
        {
            if (i == Row)
            {
                continue;
            }
            else if (Num_Matrix[Row][Column_Swap_Helper[Column]].Digit == Num_Matrix[i][Column_Swap_Helper[Column]].Digit)
            {
                return false;
            }
        }
        return true;
    }
};

#endif