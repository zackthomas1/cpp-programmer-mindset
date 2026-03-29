#include <iostream>
#include <assert.h>
#include <random>
#include <chrono>
#include "matrix.h"

namespace ct {
    template<typename T>
    void check_dimensions(MatrixView<const T> A, MatrixView<const T> B, MatrixView<T> C) { 
        assert(A.rows() == B.rows() && B.rows() == C.rows());
        assert(A.cols() == B.cols() && B.cols() == C.cols());
    }

    void dgemm_basic(MatrixView<double> C,
        MatrixView<const double> A, MatrixView<const double> B,
        double alpha, double beta
    ) {

        check_dimensions(A, B, C);

        assert(A.is_row_major());
        assert(B.is_row_major());
        assert(C.is_row_major());

        for (ptrdiff_t i = 0; i < A.rows(); i++) {
            for (ptrdiff_t j = 0; j < A.cols(); j++) {

                double temp = 0; 

                // innermost loop accessing data n elements apart in memory.
                // CPUs like accessing contiguous data
                for (ptrdiff_t k = 0; k < B.cols(); k++) {
                    auto &a_val = A.at(i, k);
                    auto &b_val = B.at(k, j);
                    temp += a_val * b_val;
                }
                auto &c_elt = C.at(i,j); 
                c_elt = beta * c_elt + alpha * temp;
            }
        }
    }

    void dgemm_block(MatrixView<double> C,
        MatrixView<const double> A, MatrixView<const double> B,
        double alpha, double beta, ptrdiff_t block_size
    ) {
        check_dimensions(A, B, C);

        // For simplicity only consider block sizes that are power of 2
        assert((block_size & (block_size-1)) == 0);

        // assert that A<B<C matrices have same data layout
        assert(A.is_row_major());
        assert(B.is_row_major());
        assert(C.is_row_major());
    
        // Create a tempory matrix for store a block of A * B products
        // NOTE: with a small enough block size (32 ~ * kib) the entire tile can sit in a cache
        // this means while accesses within tile are spread, the entire operation is done in cache
        Matrix<double> tile(block_size, block_size);

        for (ptrdiff_t i_block = 0; i_block < C.rows(); i_block += block_size) {
            auto i_bound = std::min(C.rows() - i_block, block_size);

            for (ptrdiff_t j_block = 0; j_block < C.cols(); j_block += block_size) {
                auto j_bound = std::min(C.cols() - j_block, block_size);
                std::fill_n(tile.data(), tile.size(), 0.0); 

                for (ptrdiff_t k_block = 0; k_block < B.cols(); k_block += block_size) {
                    auto k_bound = std::min(B.cols() - k_block, block_size);
                
                    for (ptrdiff_t i = 0; i < i_bound; i++) {
                        auto i_index = i_block + i;
                        for (ptrdiff_t j = 0; j < j_bound; j++) {
                            auto j_index = j_block + j;
                            for (ptrdiff_t k = 0; k < k_bound; k++) {
                                auto k_index = k_block + k;
                            
                                auto& a_val = A.at(i_index, k_index);
                                auto& b_val = B.at(i_index, j_index);
                            
                                auto& t_val = tile.at(i, j);
                                t_val += a_val * b_val;
                            }
                        }
                    }
                }

                // copy values from tile matrix into output matrix C
                // at correct offset position in C
                for (ptrdiff_t i = 0; i < i_bound; i++) {
                    auto i_index = i_block + i; 
                    for (ptrdiff_t j = 0; j < j_bound; j++) {
                        auto j_index = j_block + j;
                        auto& c_elt = C.at(i_index, j_index);
                        auto& t_val = tile.at(i,j);
                        c_elt = beta * c_elt + alpha * t_val;
                    }
                }
            }
        }
    }

    template<typename T>
    void saxpy (float a, const std::span<T>& x, std::span<T>& y) {
        assert(x.size() == y.size());

        for (auto&& [xv,yv] : std::views::zip(x,y)) {
            yv += a*xv;
        }
    }

    template<typename T>
    void saxpy_vectorize (float a, const std::span<T>& x, std::span<T>& y) {
        assert(x.size() == y.size());

        for (size_t i = 0; i < x.size(); i++) {
            y[i] += a*x[i];
        }
    }
}

int main() {

    // initialize random number generator 
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distrib(1,100);

    // declare matrices
    size_t m = 16, n = 16;
    ct::Matrix<double> res(m,n);
    ct::Matrix<double> A(m,n);
    ct::Matrix<double> B(m,n);

    // initialize matrix values
    for (ptrdiff_t i = 0; i < A.rows(); i++) {
        for (ptrdiff_t j = 0; j < A.cols(); j++) {
            A.at(i,j) = distrib(gen);
            B.at(i,j) = distrib(gen);
        }
    }

    dgemm_block(res, A, B, 1.0, 1.0, 4);

    std::vector<double> xv = {1.0, 2.0, 3.0};
    std::vector<double> yv = {5.0, 6.0, 7.0};
    std::span<double> x = {xv}; 
    std::span<double> y = {yv};

    // ct::saxpy(2.0f, x, y);
    ct::saxpy_vectorize(2.0f, x, y);
    for(auto v : y) { 
        std::cout << v << ", ";
    } 
    std::cout << std::endl;

    return 0;
}
