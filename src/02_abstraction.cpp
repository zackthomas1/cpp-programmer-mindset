#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>

const double detection_intensity = 2.0; 

struct Pos {
    int x;
    int y;
};

Pos closest_positive_signal_views(const std::vector<std::vector<double>>& grid, const Pos& start) {
    const int dim_x = grid[0].size(), dim_y = grid.size();

    auto candidates = std::views::cartesian_product(
        std::views::iota(0, dim_y),
        std::views::iota(0, dim_x))
    | std::views::transform([](auto pair) { 
        return Pos(std::get<0>(pair), std::get<1>(pair)); 
    })
    | std::views::filter([&grid](const Pos& p) {
        return grid[p.x][p.y] > detection_intensity;
    });

    auto dist_to_start_fn = [&start](const Pos& pos) {
        return std::max(std::abs(start.x-pos.x), std::abs(start.y-pos.y));
    };

    auto ordering = [&dist_to_start_fn](const Pos& a, const Pos& b) {
        return dist_to_start_fn(a) < dist_to_start_fn(b);
    };

    return *std::ranges::min_element(candidates, ordering);
}

Pos closest_positive_signal_ranges(const std::vector<std::vector<double>>& grid, const Pos& start) {
    const int dim_x = grid[0].size(), dim_y = grid.size();

    auto compute_signal_intensity = [&grid](int x, int y) {
        return grid[x][y];
    };

    std::vector<Pos> signals;
    signals.reserve(dim_x * dim_y);

    for (int y = 0; y < dim_y; ++y) {
        for (int x = 0; x < dim_x; ++x) {
            if (compute_signal_intensity(x,y) > detection_intensity) {
                signals.emplace_back(x,y);
            }
        }
    }

    auto dist_to_start_fn = [&start](const Pos& pos) {
        return std::max(std::abs(start.x-pos.x), std::abs(start.y-pos.y));
    };

    auto ordering = [&dist_to_start_fn](const Pos& a, const Pos& b) {
        return dist_to_start_fn(a) < dist_to_start_fn(b);
    };

    return *std::ranges::min_element(signals, ordering);
}

Pos closest_positive_signal_naive(const std::vector<std::vector<double>>& grid, const Pos& start) {
    // validate input preconditions
    const int m = grid.size(), n = grid[0].size();
    if ((start.x < 0 || start.y < 0) || (start.x >= m || start.y >= n)) {
        throw std::invalid_argument("start coordinates must be positive index in range from [0, grid.size()].");
    }

    std::vector<Pos> positiveSignals;

    // search entire grid row by row. Starting from top-left.
    for (size_t i = 0; i < m; i++) {  // row
        for (size_t j = 0; j < n; j++) {   // column
            // ignore value at start index
            if (i == start.x && j == start.y) { continue; }

            // find all positive signals
            if (grid[i][j] > detection_intensity) {
                positiveSignals.emplace_back(i,j);
            }
        }
    }

    // find the closest to the starting position
    double closestDistance = std::numeric_limits<double>::max();
    Pos closest = {-1, -1};
    for (const auto& p : positiveSignals) {
        int distance = std::abs(p.x - start.x) + std::abs(p.y - start.y);
        if (distance < closestDistance) {
            closestDistance = distance; 
            closest = p;
        }
    }

    return closest;
}

Pos closest_positive_signal_expand(const std::vector<std::vector<double>>& grid, const Pos& start) {

    // validate input preconditions
    const size_t m = grid.size(), n = grid[0].size();
    if ((start.x < 0 || start.y < 0) || (start.x >= m || start.y >= n)) {
        throw std::invalid_argument("start coordinates must be positive index in range from [0, grid.size()].");
    }

    std::vector<Pos> signals;

    size_t width = 1, height = 1; 
    while ( start.y >= height || start.y + height < m ||
        start.x >= width || start.x + width < n) 
    {

        for (size_t i = std::max(static_cast<size_t>(0), start.y - height); i <= std::min(m, start.y + height); i++) {
            for (size_t j = std::max(static_cast<size_t>(0), start.x-width); j <= std::min(n, start.x + width); j++) {
                if (i == start.y && j == start.x) { continue; }
            
                if (grid[i][j] > detection_intensity) {
                    signals.emplace_back(i,j);
                }
            }
        }

        if (!signals.empty()) { break; }

        width++;
        height++;
    }

    return signals.front();
}

int main() {
    std::vector<std::vector<double>> grid = {
        { 5.0,-1.0, 3.0,-1.0,-1.0},
        {-1.0,-1.0,-1.0,-1.0,-1.0},
        {-1.0, 6.0, 0.0,-1.0,-1.0},
        {-1.0,-1.0,-1.0,-1.0,-1.0},
        {-1.0,-1.0,-1.0,-1.0,-1.0},
    };
    Pos start = {2,2};

    Pos resViews = closest_positive_signal_views(grid, start);
    std::cout << "Result (views): (" << resViews.x << "," << resViews.y << ")" << std::endl;

    Pos resRanges = closest_positive_signal_ranges(grid, start);
    std::cout << "Result (Ranges): (" << resRanges.x << "," << resRanges.y << ")" << std::endl;

    Pos resNaive = closest_positive_signal_naive(grid, start);
    std::cout << "Result (Naive): (" << resNaive.x << "," << resNaive.y << ")" << std::endl;

    Pos resExpand = closest_positive_signal_expand(grid, start);
    std::cout << "Result (Expand): (" << resExpand.x << "," << resExpand.y << ")" << std::endl;

    return 0;
}