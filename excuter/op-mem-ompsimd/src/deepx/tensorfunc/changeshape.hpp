#ifndef DEEPX_TENSORFUNC_CHANGESHAPE_HPP
#define DEEPX_TENSORFUNC_CHANGESHAPE_HPP

#include <stdexcept>
#include <vector>

#include "deepx/tensor.hpp"
#include "deepx/tensorfunc/new.hpp"
#include "deepx/shape_broadcast.hpp"

namespace deepx::tensorfunc
{
    template <typename T>
    void reshape(Tensor<T> &tensor, Tensor<T> &output, const std::vector<int> &shape)
    { // 参数改为单个tensor引用

        int new_prod = 1;
        for (int dim : shape)
        {
            new_prod *= dim;
        }

        if (tensor.shape.size != new_prod)
        {
            throw std::invalid_argument("Shape size mismatch");
        }
        if (tensor.data != output.data)
        {
            tensorfunc::copytensor(tensor, output);
        }
        output.shape = Shape(shape); // 直接修改原tensor的shape
    }

    template <typename T>
    void transpose(const Tensor<T> &tensor, Tensor<T> &result, const std::vector<int> &dimOrder)
    {
        if (dimOrder.size() != tensor.shape.dim)
        {
            throw std::invalid_argument("dimOrder size does not match the number of dimensions in the TensorCPU.");
        }
        if (result.shape.size != tensor.shape.size)
        {
            throw std::runtime_error("transpose error!shape");
        }
        result.shape.rangeParallel(dimOrder.size(), [&tensor, &result, &dimOrder](int idx_linear, const std::vector<int> &indices, std::vector<int> &newIndices)
                                   {
                           
                            for (size_t i = 0; i < dimOrder.size(); ++i) {
                                newIndices[dimOrder[i]] = indices[i];
                            }
                            result.data[idx_linear]= tensor.data[tensor.shape.linearat(newIndices)]; }, tensor.shape.dim);
    }

    template <typename T>
    void concat(const std::vector<Tensor<T> *> &tensors, const int axis, Tensor<T> &result)
    {
        // Shape shape=concatShape(tensors,axis);
        // result=New<T>(shape.shape);
        int dimC = axis + 1;
        result.shape.rangeParallel(dimC, [&](const int idx, const std::vector<int> &indices)
                                   {
                        int concatIdxCurrentTensor=indices[axis];;
                        int tensorIdx=0;
                        while (tensorIdx < tensors.size()  ) {
                            if (concatIdxCurrentTensor<tensors[tensorIdx]->shape[axis]) {
                                break;
                            }else{
                                concatIdxCurrentTensor-=tensors[tensorIdx]->shape[axis];
                                tensorIdx++;
                            }
                        }
                        
                        std::vector<int> currentTensorIndices=indices;
                        currentTensorIndices[axis]=concatIdxCurrentTensor;

                        int idxCurrentTensor=tensors[tensorIdx]->shape.linearat(currentTensorIndices);
                        int copylen=tensors[tensorIdx]->shape.strides[axis];
                        std::copy(tensors[tensorIdx]->data+idxCurrentTensor,tensors[tensorIdx]->data+idxCurrentTensor+copylen,result.data+idx); });
    }

    template <typename T>
    void split(const Tensor<T> &tensor, const int axis, std::vector<Tensor<T> *> &results)
    {
        tensor.shape.rangeParallel(axis, [&](const int idx, const std::vector<int> &indices)
                                   {
                        int splitIdxCurrentTensor=indices[axis];
                        int tensorIdx=0;
                        while (tensorIdx < results.size()  ) {
                            if (splitIdxCurrentTensor<results[tensorIdx]->shape[axis]) {
                                break;
                            }else{
                                splitIdxCurrentTensor-=results[tensorIdx]->shape[axis];
                                tensorIdx++;
                            }
                        }
                        std::vector<int> currentTensorIndices=indices;
                        currentTensorIndices[axis]=splitIdxCurrentTensor;
                        results[tensorIdx]->shape.linearat(currentTensorIndices);
                        int idxCurrentTensor=results[tensorIdx]->shape.linearat(currentTensorIndices);
                        int copylen=results[tensorIdx]->shape.strides[axis];
                        std::copy(tensor.data+idxCurrentTensor,tensor.data+idxCurrentTensor+copylen,results[tensorIdx]->data+idx); });
    }

    // 扩展张量维度 - 将形状中为1的维度扩展到目标维度
    template <typename T>
    void expand(const Tensor<T> &input, Tensor<T> &output)
    {
        // 检查输入和目标形状的兼容性
        if (input.shape.dim != output.shape.dim)
        {
            throw std::invalid_argument("expand维度不匹配: 输入维度 " +
                                        std::to_string(input.shape.dim) +
                                        ", 目标维度 " +
                                        std::to_string(output.shape.dim)+
                                        "请先前dim补1的方式reshape");
        }

        for (size_t i = 0; i < input.shape.dim; ++i)
        {
            if (input.shape[i] != output.shape[i] && input.shape[i] != 1)
            {
                throw std::invalid_argument("维度 " + std::to_string(i) +
                                            " 不能被扩展: " +
                                            std::to_string(input.shape[i]) +
                                            " 到 " +
                                            std::to_string(output.shape[i]));
            }
        }

        // 创建扩展映射
        std::vector<BroadcastMap> bm = broadcastMap(input.shape.shape, output.shape.shape);

        // 找到最后一个需要扩展的维度
        int last_expand_dim = -1;
        for (int i = input.shape.dim - 1; i >= 0; --i)
        {
            if (input.shape[i] != output.shape.shape[i])
            {
                last_expand_dim = i;
                break;
            }
        }

        // 如果最后几个维度不需要扩展，可以连续复制
        if (last_expand_dim < output.shape.dim - 1)
        {
            int copy_len = output.shape.strides[last_expand_dim + 1];
            output.shape.rangeParallel(last_expand_dim + 1, [&bm, &output, &input, copy_len](int idx_linear, const std::vector<int> &indices, std::vector<int> &oldIndices)
                                       {
                    fromBroadcastIndices(bm, indices, oldIndices);
                    int idx_old = input.shape.linearat(oldIndices);
                    std::copy(input.data + idx_old,
                             input.data + idx_old + copy_len,
                             output.data + idx_linear); }, input.shape.dim);
        }
        else
        {
            output.shape.rangeParallel(output.shape.dim, [&bm, &output, &input](int idx_linear, const std::vector<int> &indices, std::vector<int> &oldIndices)
                                       {
                    fromBroadcastIndices(bm, indices, oldIndices);
                    int idx_old = input.shape.linearat(oldIndices);
                    output.data[idx_linear] = input.data[idx_old]; }, input.shape.dim);
        }
    }


}
#endif