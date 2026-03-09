#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cmath>

// Closest positive signal 
//  -----------------
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

// find root by bisect
//  -----------------
template <typename T>
concept RealNumber = std::is_floating_point_v<T>; 

// given a function
template <typename FN, typename Real>
concept ContinuousFN = RealNumber<Real> &&
    requires(FN f, Real x) {{ f(x) } -> std::convertible_to<Real>; };

template <RealNumber Real>
Real midpoint(Real upper, Real lower) {
    return lower + (upper - lower) / static_cast<Real>(2.0);
}

template <RealNumber Real>
bool compare_reals_equal(Real upper, Real lower) {
    return std::abs(upper - lower) < static_cast<Real>(0.001);
}

template <typename Function, RealNumber Real>
    requires ContinuousFN<Function, Real>
Real find_root_bisect(Function&& function, Real pos, Real neg, Real tol) {
    // check precondition
    if (std::signbit(pos) == std::signbit(neg)) {
        throw std::invalid_argument("pos and  neg parameters must have opposite signs.");
    }

    auto fpos = function(pos);

    // Driving loop
    while (!compare_reals_equal(pos, neg)) {
        auto m = midpoint(pos, neg);
        auto fm = function(m); 

        // quit early if function almost 0
        if (std::abs(fm) < tol) { return m; }

        // decision logic to find next point to check
        if (std::signbit(fm) == std::signbit(fpos)) {
            pos = m; 
            fpos = fm;
        } else {
            neg = m;
        }
    }
    return fpos;
}

template <RealNumber Real>
Real cubic_fn(Real t) { return (t*t*t)+1; }

template <RealNumber Real>
Real sin_fn(Real t) { return static_cast<Real>(std::sin(t + static_cast<Real>(std::numbers::pi))); }

template <RealNumber Real>
Real cos_fn(Real t) { return static_cast<Real>(std::cos(t + static_cast<Real>(std::numbers::pi))); }

// classes to provide behavior for raw data
// -----------------
class AddressBookRecord {
public:
    size_t id;
    int house_number;
    std::string street_address;
    std::string city_and_state;
    int zip_code;

    AddressBookRecord() = default;
    ~AddressBookRecord() = default;

    inline bool operator==(const AddressBookRecord& rhs) noexcept{
        return id == rhs.id;
    }
};

class RecordView  {
    const AddressBookRecord*  p_data;
public:
    size_t id() const noexcept { return p_data->id; }

    inline bool operator==(const RecordView& rhs) noexcept {
        return id() == rhs.id();
    }
};

// Classes that reprersent physical objects
// ------------
class TempSensor
{
private:
public:
    TempSensor() = default;
    virtual ~TempSensor() = default;

    virtual std::string_view id() const noexcept = 0;
    virtual float temperaturer_kelvin() const noexcept = 0;
};

void check_sensors(std::span<const TempSensor*> sensors, float threshold) {
    for (const auto& sensor : sensors) {
        const auto temp = sensor->temperaturer_kelvin();
        if (temp > threshold) {
            throw std::runtime_error(std::format("Sensor {} reports temperature {}", sensor->id(), temp));
        }
    }
}

// templates 
// -------------
template <typename T>
concept GridPosition = requires (T t) {
    std::is_same_v<decltype(t.x), int>;
    std::is_same_v<decltype(t.y), int>;
};

// -------------
template <std::integral From, std::integral To>
struct ExactConversionTraits {
    using from_ref = const From&;
    using to_ref = To&;

    static void convert(to_ref to, from_ref from) {
        if (from <= std::numeric_limits<To>::max && from >= std::numeric_limits<To>::min) {
            throw std::runtime_error("invalid exact conversion");
        }
        to = static_cast<To>(from);
    }
};

int main(int argc, char** argv) {
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

    std::cout << std::endl;

    auto rootCubic = find_root_bisect(cubic_fn<double>, 10.0, -10.0, 0.001);
    std::cout << "find_root_bisect (cubic): " <<  rootCubic << std::endl;
    
    auto rootSin = find_root_bisect(sin_fn<double>, 3.14, -3.14, 0.001);
    std::cout << "find_root_bisect (sin): " <<  rootSin << std::endl;

    auto rootCos = find_root_bisect(cos_fn<double>, 3.14, -3.14, 0.001);
    std::cout << "find_root_bisect (cos): " <<  rootCos << std::endl;

    return 0;
}