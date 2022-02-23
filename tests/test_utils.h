#include <vector>
#include <random>
#include <limits>

using namespace std;

template <typename T>
vector<T> generateArbitraryIntData(
    uint32_t seed, 
    uint8_t count, 
    T lower = numeric_limits<T>::min(), 
    T upper = numeric_limits<T>::max()
)
{
    vector<T> data;
    default_random_engine gen(seed);
    uniform_int_distribution<T> dist(lower, upper);
    for (int i = 0; i < count; i++) { data.push_back(dist(gen)); }
    return data;
}

template <typename T>
vector<T> generateArbitraryRealData(
    uint32_t seed, 
    uint8_t count, 
    T lower = numeric_limits<T>::min(), 
    T upper = numeric_limits<T>::max()
)
{
    vector<T> data;
    default_random_engine gen(seed);
    uniform_real_distribution<T> dist(lower, upper);
    for (int i = 0; i < count; i++) { data.push_back(dist(gen)); }
    return data;
}

template <typename T>
bool isBounded(T x, T lower, T upper) { return((x >= lower) && (x <= upper)); }