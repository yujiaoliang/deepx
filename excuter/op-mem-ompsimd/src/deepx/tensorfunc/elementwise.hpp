#ifndef DEEPX_TENSORFUNC_ELEMENTWISE_HPP
#define DEEPX_TENSORFUNC_ELEMENTWISE_HPP

#include <cblas.h>
#include <cmath>
#include <hwy/highway.h>
#include "deepx/tensor.hpp"

namespace deepx::tensorfunc
{
    using namespace hwy::HWY_NAMESPACE;
 

    template <typename T>
    void add(const Tensor<T> &A, const Tensor<T> &B, Tensor<T> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            C.shape.rangeParallel(C.shape.dim - 1, [&A, &B, &C](int i)
                                  {
                                      int shape_last = C.shape[-1];
                                      const ScalableTag<T> tag;
                                      const size_t lanes = Lanes(tag);
                                      size_t j = 0;

                                      // 1. 处理前置未对齐部分
                                      while (j < shape_last && !IsAligned(tag, A.data + i + j))
                                      {
                                          C.data[i + j] = A.data[i + j] + B.data[i + j];
                                          ++j;
                                      }

                                      // 2. 处理中间对齐部分
                                      size_t aligned_end = shape_last - (shape_last % lanes);
                                      for (; j + lanes <= aligned_end; j += lanes)
                                      {
                                          auto vec1 = Load(tag, A.data + i + j);
                                          auto vec2 = Load(tag, B.data + i + j);
                                          auto vec_result = Add(vec1, vec2);
                                          Store(vec_result, tag, C.data + i + j);
                                      }

                                      // 3. 处理尾部剩余元素
                                      for (; j < shape_last; j++)
                                      {
                                          C.data[i + j] = A.data[i + j] + B.data[i + j];
                                      } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    // float特化
    template <>
    void add<float>(const Tensor<float> &A, const Tensor<float> &B, Tensor<float> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {

            // 先复制A到C，再累加B (C = 1*A + 1*B)
            if (std::addressof(A) != std::addressof(C))
            {
                cblas_scopy(A.shape.size, A.data, 1, C.data, 1);
            }
            cblas_saxpy(B.shape.size, 1.0f, B.data, 1, C.data, 1);
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    // double特化
    template <>
    void add<double>(const Tensor<double> &A, const Tensor<double> &B, Tensor<double> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            if (std::addressof(A) != std::addressof(C))
            {
                cblas_dcopy(A.shape.size, A.data, 1, C.data, 1);
            }
            cblas_daxpy(B.shape.size, 1.0, B.data, 1, C.data, 1);
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <typename T>
    void add(const Tensor<T> &input, const T value, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output, &value](int i)
                                       {
                                           int shape_last = output.shape[-1];
                                           const ScalableTag<T> tag;
                                           const size_t lanes = Lanes(tag);
                                           size_t j = 0;

                                           // 1. 处理前置未对齐部分
                                           while (j < shape_last && !IsAligned(tag, input.data + i + j))
                                           {
                                               output.data[i + j] = input.data[i + j] + value;
                                               ++j;
                                           }

                                           // 2. 处理中间对齐部分
                                           size_t aligned_end = shape_last - (shape_last % lanes);
                                           for (; j + lanes <= aligned_end; j += lanes)
                                           {
                                               auto vec = Load(tag, input.data + i + j);
                                               auto scalar = Set(tag, value);
                                               auto vec_result = Add(vec, scalar);
                                               Store(vec_result, tag, output.data + i + j);
                                           }

                                           // 3. 处理尾部剩余元素
                                           for (; j < shape_last; j++)
                                           {
                                               output.data[i + j] = input.data[i + j] + value;
                                           } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <typename T>
    void sub(const Tensor<T> &A, const Tensor<T> &B, Tensor<T> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            C.shape.rangeParallel(C.shape.dim - 1, [&A, &B, &C](int i)
                                  {
                int shape_last=C.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,A.data + i + j)) {
                    C.data[i+j] = A.data[i+j] - B.data[i+j];
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec1 = Load(tag, A.data + i + j);
                    auto vec2 = Load(tag, B.data + i + j);
                    auto vec_result = Sub(vec1, vec2);
                    Store(vec_result, tag, C.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    C.data[i+j] = A.data[i+j] - B.data[i+j];
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }
    template <>
    void sub<float>(const Tensor<float> &A, const Tensor<float> &B, Tensor<float> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            if (std::addressof(A) != std::addressof(C))
            {
                cblas_scopy(A.shape.size, A.data, 1, C.data, 1);
            }
            cblas_saxpy(B.shape.size, 1.0, B.data, 1, C.data, 1);
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <>
    void sub<double>(const Tensor<double> &A, const Tensor<double> &B, Tensor<double> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            if (std::addressof(A) != std::addressof(C))
            {
                cblas_dcopy(A.shape.size, A.data, 1, C.data, 1);
            }
            cblas_daxpy(B.shape.size, 1.0, B.data, 1, C.data, 1);
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <typename T>
    void sub(const Tensor<T> &input, const T value, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output, &value](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = input.data[i+j] - value;
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto scalar = Set(tag, value);
                    auto vec_result = Sub(vec, scalar);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = input.data[i+j] - value;
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <>
    void sub<float>(const Tensor<float> &A, const float value, Tensor<float> &C)
    {
        if (A.shape == C.shape)
        {
            if (std::addressof(A) != std::addressof(C))
            {
                cblas_scopy(A.shape.size, A.data, 1, C.data, 1);
            }
            cblas_saxpy(A.shape.size, 1.0, A.data, 1, C.data, 1);
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <>
    void sub<double>(const Tensor<double> &A, const double value, Tensor<double> &C)
    {
        if (A.shape == C.shape)
        {
            if (std::addressof(A) != std::addressof(C))
            {
                cblas_dcopy(A.shape.size, A.data, 1, C.data, 1);
            }
            cblas_daxpy(A.shape.size, 1.0, A.data, 1, C.data, 1);
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

     

    template <typename T>
    void mul(const Tensor<T> &A, const Tensor<T> &B, Tensor<T> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            C.shape.rangeParallel(C.shape.dim - 1, [&A, &B, &C](int i)
                                  {
                int shape_last=C.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,A.data + i + j)) {
                    C.data[i+j] = A.data[i+j] * B.data[i+j];
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec1 = Load(tag, A.data + i + j);
                    auto vec2 = Load(tag, B.data + i + j);
                    auto vec_result = Mul(vec1, vec2);
                    Store(vec_result, tag, C.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    C.data[i+j] = A.data[i+j] * B.data[i+j];
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

 
    
    template <typename T>
    void mul(const Tensor<T> &input, const T value, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output, &value](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = input.data[i+j] * value;
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto scalar = Set(tag, value);
                    auto vec_result = Mul(vec, scalar);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = input.data[i+j] * value;
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }
 
    template <typename T>
    void div(const Tensor<T> &A, const Tensor<T> &B, Tensor<T> &C)
    {
        if (A.shape == B.shape && A.shape == C.shape)
        {
            C.shape.rangeParallel(C.shape.dim - 1, [&A, &B, &C](int i)
                                  {
                int shape_last=C.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,A.data + i + j)) {
                    C.data[i+j] = A.data[i+j] / B.data[i+j];
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec1 = Load(tag, A.data + i + j);
                    auto vec2 = Load(tag, B.data + i + j);
                    auto vec_result = Div(vec1, vec2);
                    Store(vec_result, tag, C.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    C.data[i+j] = A.data[i+j] / B.data[i+j];
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <typename T>
    void div(const Tensor<T> &input, const T value, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output, &value](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = input.data[i+j] / value;
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto scalar = Set(tag, value);
                    auto vec_result = Div(vec, scalar);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = input.data[i+j] / value;
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

   
    template <typename T>
    void pow(const Tensor<T> &input, const T value, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output, &value](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = std::pow(input.data[i+j], value);
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto scalar = Set(tag, value);
                    auto vec_result = Pow(vec, scalar);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = std::pow(input.data[i+j], value);
                } });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <typename T>
    void log(const Tensor<T> &input, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = std::log(input.data[i+j]);
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto vec_result = Log(vec);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = std::log(input.data[i+j]);
                } 
            });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }

    template <typename T>
    void exp(const Tensor<T> &input, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output](int i)   
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0; 

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = std::exp(input.data[i+j]);
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto vec_result = Exp(vec); 
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = std::exp(input.data[i+j]);
                } 
            });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");  
        }
    } 

    template <typename T>
    void sin(const Tensor<T> &input, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = std::sin(input.data[i+j]);
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto vec_result = Sin(vec);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = std::sin(input.data[i+j]);
                } 
            });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }
    template <typename T>
    void cos(const Tensor<T> &input, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = std::cos(input.data[i+j]);
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto vec_result = Cos(vec);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = std::cos(input.data[i+j]);
                } 
            });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }
    
    template <typename T>
    void tan(const Tensor<T> &input, Tensor<T> &output)
    {
        if (input.shape == output.shape)
        {
            output.shape.rangeParallel(output.shape.dim - 1, [&input, &output](int i)
                                       {
                int shape_last=output.shape[-1];
                const ScalableTag<T> tag;
                const size_t lanes = Lanes(tag);
                size_t j=0;

                // 1. 处理前置未对齐部分
                while (j < shape_last && !IsAligned(tag,input.data + i + j)) {
                    output.data[i+j] = std::tan(input.data[i+j]);
                    ++j;
                }

                // 2. 处理中间对齐部分
                size_t aligned_end=shape_last-(shape_last%lanes);
                for (; j+lanes<=aligned_end; j +=  lanes  )
                {
                    auto vec = Load(tag, input.data + i + j);
                    auto vec_result = Tan(vec);
                    Store(vec_result, tag, output.data + i + j);
                }

                // 3. 处理尾部剩余元素
                for (;j<shape_last;j++)
                {
                    output.data[i+j] = std::tan(input.data[i+j]);
                } 
            });
        }
        else
        {
            throw std::invalid_argument("shape mismatch");
        }
    }   
  
}
#endif // DEEPX_OP_CPU_ELEMENTWISE_HPP