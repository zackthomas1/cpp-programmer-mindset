#include <stdint.h>
#include <iostream>
#include <vector>

template <typename T>
T power_iterative(T x, uint32_t n) {
    T result = 1; 
    for (size_t i = 0; i < n; i++) {
        result *= x;
    }
    return result;
}

template <typename T> 
T power_recursive(T x, uint32_t n) {
    // base case
    if (n == 0) { return T(1); }
    if (n == 1) { return x; }

    // recursive step
    T pow2 = power_recursive(x, n / 2);
    T result = (n % 2) == 0 ? T(1) : x;
    
    return result * pow2 * pow2;
}

template <typename T>
std::ptrdiff_t partition(std::span<T> data) {
    auto& pivot_value = data.back(); 
    std::ptrdiff_t pivot_pos = 0; 
    for (auto j = 0; j < data.size() - 1; ++j) {
        if (data[j] <= pivot_value) {
            std::swap(data[pivot_pos], data[j]);
            ++pivot_pos;
        }
    }
    std::swap(data[pivot_pos], pivot_value);
    return pivot_pos;
}

template <typename T>
void quicksort(std::span<T> data) {
    if (data.empty()) { return; }

    std::ptrdiff_t pivot = partition(data);

    quicksort(data.subspan(0, pivot));
    quicksort(data.subspan(pivot+1));

}

struct Coord {
    float x, y;

    Coord operator+(const Coord& other) const { 
        return Coord {x+other.x, y+other.y};
    }
    Coord operator*(float scalar) const { 
        return Coord {x*scalar, y*scalar};
    }
    friend std::ostream& operator<<(std::ostream& os, const Coord& other) {
        os << "(" << other.x << "," << other.y << ")";
        return os;
    }
};

Coord gradient_descent(const Coord& init_coord) {
    const float LEARNINGRATE = 0.25;
    const float THRESHOLD = 0.000001;

    auto gradient = [] (Coord coord) {
        float dfdx = -2.0 * coord.x;
        float dfdy = -2.0 * coord.y;
        
        return Coord {dfdx, dfdy}; 
    };

    auto distance_sqr = [] (Coord a, Coord b) {
        return ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
    };

    Coord start_coord = init_coord;
    Coord update_coord = start_coord + (gradient(start_coord) * LEARNINGRATE);
    do { 
        std::cout << update_coord << std::endl;
        start_coord = update_coord;
        update_coord = start_coord + (gradient(start_coord) * LEARNINGRATE);
    } while (distance_sqr(start_coord, update_coord) > THRESHOLD);

    return update_coord;
}


int main (int argc, char** argv) {
    std::cout << "(iterative) 2^3 = "<< power_iterative(2.0,3) << std::endl;
    std::cout << "(iterative) 4^4 = "<< power_iterative(4.0,4) << std::endl;

    std::cout << "(recursive) 2^3 = "<< power_recursive(2.0,3) << std::endl;
    std::cout << "(recursive) 4^4 = "<< power_recursive(4.0,4) << std::endl;

    std::vector<int> input{6,3,5,1,4};
    for(int v : input) { std::cout << v << " "; } std::cout << std::endl;
    
    quicksort(std::span<int>{input});
    for(int v : input) { std::cout << v << " "; } std::cout << std::endl;

    Coord start {1,2};
    Coord output = gradient_descent(start);
    std::cout << output << std::endl;

    return 0;
}