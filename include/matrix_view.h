#ifndef CT_MATRIX_VIEW_H
#define CT_MATRIX_VIEW_H

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <ranges>
#include <span>

namespace ct { 
    
    enum class MatrixOrder {
        RowMajor,
        ColMajor
    };

    template<typename T>
    class MatrixView
    {
    private:
        T* data_ = nullptr; 
        ptrdiff_t rows_ = 0;
        ptrdiff_t cols_ = 0;
        MatrixOrder order_ = MatrixOrder::RowMajor;
    public:
        MatrixView(T* data, ptrdiff_t rows, ptrdiff_t cols, MatrixOrder = MatrixOrder::RowMajor) 
            : data_(data), rows_(rows), cols_(cols) {}
        ~MatrixView() = default;

        constexpr bool is_row_major() const noexcept { return order_ == MatrixOrder::RowMajor; }
        constexpr bool is_col_major() const noexcept { return order_ == MatrixOrder::ColMajor; }
    
        constexpr ptrdiff_t rows() const noexcept { return rows_; }
        constexpr ptrdiff_t cols() const noexcept { return cols_; }
        
        constexpr MatrixOrder order() const noexcept { return order_; }

        constexpr ptrdiff_t stride() const noexcept { return is_row_major() ? rows_ : cols_; }

        constexpr T* data() const noexcept { return data_; }

        constexpr size_t size() const noexcept { return static_cast<size_t>(rows_ * cols_); }

        void reset(T* new_ptr = nullptr) noexcept { 
            data_ = new_ptr; 
            if (new_ptr == nullptr) {
                rows_ = 0;
                cols_ = 0;
                order_ = MatrixOrder::RowMajor;
            }
        }

        std::span<T> to_span() noexcept {
            return { data(), size()};
        }

        std::span<const T> to_span() const noexcept {
            return { data(), size()};
        }

        void swap(MatrixView &other) noexcept {
            std::swap(data_, other.data_);
            std::swap(rows_, other.rows_);
            std::swap(cols_, other.cols_);
            std::swap(order_, other.order_);
        }

        T &at(std::ptrdiff_t i, std::ptrdiff_t j) noexcept {
            return is_row_major() ? data_[i * stride() + j] : data_[i + j * stride()];
        }

        const T &at(std::ptrdiff_t i, std::ptrdiff_t j) const  noexcept {
            return is_row_major() ? data_[i * stride() + j] : data_[i + j * stride()];
        }
    };

    template<typename T> 
    bool operator==(const MatrixView<T> lhs, const MatrixView<T> rhs) noexcept {
        if (lhs.size() != rhs.size() || 
            lhs.rows() != rhs.rows() || 
            lhs.cols() != rhs.cols()
        ) 
        { return false; }
    
        if (lhs.is_row_major() == rhs.is_row_major()) {
            return std::ranges::equal(lhs.to_span(), rhs.to_span());
        }

        for (std::ptrdiff_t i = 0; i < lhs.rows(); i++) { 
            for (std::ptrdiff_t j = 0; j < lhs.cols(); j++) { 
                if (lhs.at(i, j) != rhs.at(i,j)) { return false; }
            }
        }

        return true;
    }

    template<typename T> 
    bool operator!=(const MatrixView<T> lhs, const MatrixView<T>rhs) noexcept {
        return !(lhs == rhs); 
    }

    namespace detail {
        using namespace std; 

        struct AbsDiffernceHelper {
            template <typename T>
            auto operator()(const T& a, const T& b) {
                return abs(a, b);
            }
        };

        template<typename T>
        T matrix_max_abs_difference(const ct::MatrixView<T>&lhs, const ct::MatrixView<T>& rhs) noexcept {
            if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols()) {
                return std::numeric_limits<T>::infinity();
            }
            
            AbsDiffernceHelper abs_dif;

            auto max_diff = -std::numeric_limits<T>::infinity();
            for (std::ptrdiff_t i = 0; i < lhs.rows(); i++) {
                for (std::ptrdiff_t j = 0; j < lhs.cols(); j++) {
                    auto diff = abs_dif(lhs.at(i,j), rhs.at(i,j));
                    if (diff > max_diff) { max_diff = diff; }
                }
            }
            return max_diff;
        }
    }

}

#endif