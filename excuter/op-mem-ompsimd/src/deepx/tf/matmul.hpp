#ifndef DEEPX_TF_MATMUL_HPP
#define DEEPX_TF_MATMUL_HPP

#include "deepx/tf/tf.hpp"
#include "deepx/dtype.hpp"
#include "deepx/dtype_ompsimd.hpp"
#include "deepx/tensorfunc/matmul.hpp"
#include "deepx/tensorfunc/matmul_cblas.hpp"
#include "deepx/tensorfunc/matmul_miaobyte.hpp"
namespace deepx::tf
{
    template <typename Author>
    class MatMul : public TF
    {
    public:
        MatMul(const vector<Param> &args, const vector<Param> &returns)
        {
            this->name = "matmul";
            this->metadata.author = Author::name();
            this->tftype = "matmul";
            this->args = args;
            this->returns = returns;
        }

        string math_formula() const override
        {
            return "T3=T1 @ T2";
        }
        shared_ptr<TF> clone() const override
        {
            return make_shared<MatMul<Author>>(*this);
        }

        int run(shared_ptr<MemBase> mem, string &error) override
        {
            Precision a_type = mem->gettensor(this->args[0].textvalue).get()->shape.dtype;
            Precision b_type = mem->gettensor(this->args[1].textvalue).get()->shape.dtype;
            Precision c_type = mem->gettensor(this->returns[0].textvalue).get()->shape.dtype;
            if (a_type != b_type || a_type != c_type)
            {
                error = "Type mismatch: " + precision_str(a_type) + " != " + precision_str(b_type) + " != " + precision_str(c_type);
                return 1;
            }
            switch (a_type)
            {
            case Precision::Float64:
                tensorfunc::matmul<Author, double>(*mem->gettensor<double>(this->args[0].textvalue), *mem->gettensor<double>(this->args[1].textvalue), *mem->gettensor<double>(this->returns[0].textvalue));
                break;
            case Precision::Float32:
                tensorfunc::matmul<Author, float>(*mem->gettensor<float>(this->args[0].textvalue), *mem->gettensor<float>(this->args[1].textvalue), *mem->gettensor<float>(this->returns[0].textvalue));
                break;
            case Precision::Int64:
                tensorfunc::matmul<Author, int64_t>(*mem->gettensor<int64_t>(this->args[0].textvalue), *mem->gettensor<int64_t>(this->args[1].textvalue), *mem->gettensor<int64_t>(this->returns[0].textvalue));
                break;
            case Precision::Int32:
                tensorfunc::matmul<Author, int32_t>(*mem->gettensor<int32_t>(this->args[0].textvalue), *mem->gettensor<int32_t>(this->args[1].textvalue), *mem->gettensor<int32_t>(this->returns[0].textvalue));
                break;
            case Precision::Int16:
                tensorfunc::matmul<Author, int16_t>(*mem->gettensor<int16_t>(this->args[0].textvalue), *mem->gettensor<int16_t>(this->args[1].textvalue), *mem->gettensor<int16_t>(this->returns[0].textvalue));
                break;
            case Precision::Int8:
                tensorfunc::matmul<Author, int8_t>(*mem->gettensor<int8_t>(this->args[0].textvalue), *mem->gettensor<int8_t>(this->args[1].textvalue), *mem->gettensor<int8_t>(this->returns[0].textvalue));
                break;
            default:
                error = "Unsupported dtype: " + precision_str(a_type);
                return 1;
            }
            return 0;
        }
    };
}

#endif
