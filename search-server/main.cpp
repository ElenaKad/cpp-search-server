#include <iostream>
int main()
{
    int x = 0;
    for (int i = 1; i <= 1000; i++)
        if (i > 99 && i < 1000)
        {
            int a = i / 100;       // Сотни
            int b = (i / 10) % 10; // Десятки
            int c = i % 10;        // Единицы
            if (a == b && b == c)
                x++;
        }
    std::cout << "Result = " << x;
    return 0;
}
