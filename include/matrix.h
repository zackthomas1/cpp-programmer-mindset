#ifndef CT_MATRIX_H
#define CT_MATRIX_H

#include "matrix_view.h"

namespace ct {
    template<typename T>
    class Matrix : public MatrixView<T>
    {
    private:
        static constexpr std::align_val_t alignment{64};

        static T* allocate(size_t n_rows, size_t n_cols) {
            return static_cast<T*>(::operator new(n_rows * n_cols * sizeof(T), alignment));
        }

        static void deallocate(T* data, size_t n_rows, size_t n_cols) {
            // WARNING: ptr data must be exact ptr returned by operator new
            // WARNING: alignment value must be the same in new and delete operations
            ::operator delete(data, n_rows * n_cols * sizeof(T), alignment);    // delete(ptr, size, align)
        }
    
    public:
        // NOTE: explicit keyword prevvents compiler from using constructor for implicit conversions 
        // do not wwant matrrix allocation happening silentlly as side effect of implicit conversion
        explicit Matrix(ptrdiff_t rows, ptrdiff_t cols, MatrixOrder order = MatrixOrder::RowMajor)
            : MatrixView<T>(allocate(rows, cols), rows, cols, order) 
        {
            // NOTE: std::uninitialized_default_construct_n constucts n objects 
            // in-place at raw memory starting at ptr without zeroing memory first
            std::uninitialized_default_construct_n(this->data(), this->size());
        };

        ~Matrix();
    };
    

}


#endif