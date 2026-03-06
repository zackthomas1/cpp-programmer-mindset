#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <string_view>
#include <ranges>
#include <cctype>

// checkst that T has begin and end that return iterators that read values in a forward iteration pattern
template <typename T>
concept OrderableContainer = std::ranges::input_range<const T> && 
    std::totally_ordered<std::ranges::range_value_t<const T>> && 
    std::copy_constructible<std::ranges::range_value_t<const T>>;

/// @brief Templated function that takes a container and return the max value.
/// @tparam Container 
/// @param container 
/// @return 
template <OrderableContainer Container> 
std::ranges::range_value_t<Container> max_element(const Container& container) {
    auto begin = std::ranges::begin(container);
    const auto end = std::ranges::end(container);

    if (begin == end) {
        throw std::invalid_argument("container must be non-empty");
    }

    // 1. take first element and stare as current max
    auto max = *begin;
    ++begin;

    // 2. for each of the remaining elements, compare to current max and replace if larger
    for (; begin != end; ++begin) {
        if (max < *begin) {
            max = *begin;
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
    assert(s[position] == '[');
    
    // s[0] is a '['
    position += 1;

    // find the first word after '['
    std::string_view a = s.substr(position);
    size_t i = end_of_first_word(a);
    position += i;

    // s[position + i] is ','
    position += 1;
    assert(s[position] == ',');
    position += 1;
    
    // get then end of the first word after ','
    std::string_view b = s.substr(position);
    size_t j = end_of_first_word(b);
    position += j;

    // s[position + 1] is a ']' matching s[0]
    position += 1;
    assert(s[position] == ']');

    return position;
}

size_t end_of_first_word_iterative(std::string_view s) noexcept {
    size_t position = 0; 
    int depth = 0; 
    for (const auto& c : s) {
        switch (c)
        {
        case '[':
            ++depth;
            break;
        case ']':
            --depth;
            if (depth == 0) {
                return position;
            }
            break;
        default:
            if (depth == 0) {
                return position;
            }
            break;
        }
        ++position;
    }
    return position;
}

size_t end_of_first_word_views(std::string_view s) noexcept {
    int depth = 0;
    for (const auto [position, ch] : std::views::enumerate(s)) {
        switch (ch)
        {
        case '[':
            ++depth;
            break;
        case ']':
            --depth;
            if (depth == 0) {
                return position;
            }
            break;
        default:
            if (depth == 0) {
                return position;
            }
            break;
        }
    }
    return -1;
}

int main() {
    // test max_element algorithm
    std::vector<int>input({1,2,3,4,5});
    std::cout << "max element in input: " << max_element(input) << std::endl;
    
    std::cout <<std::endl;

    // test end_of_first_word algorith
    std::string word1 = "a";                    // -> 0
    std::string word2 = "[a,a]";                // -> 4
    std::string word3 = "[a,[a,b]]";            // -> 8
    std::string word4 = "[[a,a],[a,[a,b]]]";    // -> 16

    // recursive
    std::cout << "end of first word (recursive) - " << word1 << ": " << end_of_first_word(word1) << std::endl;
    std::cout << "end of first word (recursive) - " << word2 << ": " << end_of_first_word(word2) << std::endl;
    std::cout << "end of first word (recursive) - " << word3 << ": " << end_of_first_word(word3) << std::endl;
    std::cout << "end of first word (recursive) - " << word4 << ": " << end_of_first_word(word4) << std::endl;

    std::cout <<std::endl;

    // iterative
    std::cout << "end of first word (iterative) - " << word1 << ": " << end_of_first_word_iterative(word1) << std::endl;
    std::cout << "end of first word (iterative) - " << word2 << ": " << end_of_first_word_iterative(word2) << std::endl;
    std::cout << "end of first word (iterative) - " << word3 << ": " << end_of_first_word_iterative(word3) << std::endl;
    std::cout << "end of first word (iterative) - " << word4 << ": " << end_of_first_word_iterative(word4) << std::endl;

    std::cout <<std::endl;

    // views
    std::cout << "end of first word (views) - " << word1 << ": " << end_of_first_word_views(word1) << std::endl;
    std::cout << "end of first word (views) - " << word2 << ": " << end_of_first_word_views(word2) << std::endl;
    std::cout << "end of first word (views) - " << word3 << ": " << end_of_first_word_views(word3) << std::endl;
    std::cout << "end of first word (views) - " << word4 << ": " << end_of_first_word_views(word4) << std::endl;
    
    return 0;
}
