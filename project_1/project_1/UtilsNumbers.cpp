#include "UtilsNumbers.h"
#include <string>
#include <sstream>

std::string UtilsNumbers::formatThousands(int number)
{
    std::string s = std::to_string(number);
    int n = s.length() - 3;
    int end = (n >= 0) ? 0 : 1;
    while (n > end) {
        s.insert(n, ",");
        n -= 3;
    }

    return s;
}
