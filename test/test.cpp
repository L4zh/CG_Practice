#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v = { 894, 34, 434, 43 };
    for (auto i : v)
        std::cout << i << " ";
    std::cout << std::end;
}
