#include "UtilsNumbers.h"
#include <string>
#include <sstream>
#include <glm/glm.hpp>

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
/// <summary>
/// Returns a random vector with a lenght between 0.002 and 1;
/// </summary>
/// <returns></returns>
glm::vec3 UtilsNumbers::getRandomOffset()
{
    glm::vec3 randomOffset = glm::vec3(
        ((rand()%1000)+1) / 500.0f - 1,
        ((rand()%1000)+1) / 500.0f - 1,
        ((rand()%1000)+1) / 500.0f - 1
    );
    return randomOffset;
}
