#include <iostream>
#include <vector>
#include <string>

/// @brief Templated function that takes a container and return the max value.
/// @tparam Container 
/// @param container 
/// @return 
template <typename Container> 
typename Container::value_type max_element(const Container& container) {
    auto begin = container.begin();
    auto end = container.end();

    if (begin == end) {
        throw std::invalid_argument("container must be non-empty");
    }

    // 1. take first element and stare as current max
    auto max = *begin;
    ++begin;

    // 2. for each of the remaining elements, compare to current max and replace if larger
    for (; begin != end; begin++) {
        const auto& current = *begin;
        if (max < current) {
            max = current;
        }
    }

    // 3. Return current max, which should now contain global max
    return max;
}

size_t end_of_first_word(std::string_view s) noexcept {
    
    if (!s.starts_with('[')) {
        return 0;
    }

    size_t position = 0; 
    
    // s[0] is a '['
    position += 1;

    // find the first word after '['
    std::string_view a = s.substr(position);
    size_t i = end_of_first_word(a);
    position += i;

    // s[position + i] is ','
    position += 2;

    // get then end of the first word after ','
    std::string_view b = s.substr(position);
    size_t j = end_of_first_word(b);
    position += j;

    // s[position + 1] is a ']' matching s[0]
    position += 1;

    return position;
}

int main() {
    // test max_element algorithm
    std::vector<int>input({1,2,3,4,5});
    std::cout << "max element in input: " << max_element(input) << std::endl;
    
    // test end_of_first_word algorith
    std::string word1 = "a";                    // -> 0
    std::string word2 = "[a,a]";                // -> 4
    std::string word3 = "[a,[a,b]]";            // -> 8
    std::string word4 = "[[a,a],[a,[a,b]]]";    // -> 16

    std::cout << "end of first word (" << word1 << "): " << end_of_first_word(word1) << std::endl;
    std::cout << "end of first word (" << word2 << "): " << end_of_first_word(word2) << std::endl;
    std::cout << "end of first word (" << word3 << "): " << end_of_first_word(word3) << std::endl;
    std::cout << "end of first word (" << word4 << "): " << end_of_first_word(word4) << std::endl;
    
    return 0;
}
