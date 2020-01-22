#include <iostream>
#include <vector>
#include "hashMap.hpp"

int main()
{
    try
    {
        std::vector<std::string> svec = std::vector<std::string>();
        std::vector<int> ivec = std::vector<int>();
        svec.push_back("hi");
        svec.push_back("you");
        svec.push_back("dick");
        svec.push_back("hi");
        ivec.push_back(15);
        ivec.push_back(-5);
        ivec.push_back(202);
        ivec.push_back(-8);

        HashMap<std::string, int> a;
        a["asd"] = 4;
        std::cout << a.at("asd") << std::endl;
        std::cout << a.at("as") << std::endl;
//        std::cout << a.size() << std::endl;
//        a["nope"] = 555;
//        std::cout << a["hi"] << std::endl;
//        a.erase("nope");
//        std::cout << a["nope"] << std::endl;
//        std::cout << a.size() << std::endl;
//        HashMap<std::string, int> b(a);
//        b["hi"] = 6;
//        std::cout << (b != a) << std::endl;
//        a.printer();
//        b.printer();
//        b = a;
//        b.printer();

    }
    catch (const std::exception &ex)
    {
        std::cout << "wow, you did it!" << std::endl;
        return 1;
    }
    std::cout << "you failed you failure" << std::endl;
    return 0;
}
