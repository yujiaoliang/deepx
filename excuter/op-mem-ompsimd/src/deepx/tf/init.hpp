#ifndef DEEPX_TF_INIT_HPP
#define DEEPX_TF_INIT_HPP

#include "deepx/tf/tf.hpp"
#include "deepx/tensorfunc/init.hpp"
#include "deepx/tensorfunc/init_miaobyte.hpp"
#include "stdutil/num.hpp"
namespace deepx::tf
{   
    //填充
    // constant
    template <typename Author>
    class Constant : public TF
    {
    public:
        Constant(const vector<Param> &args, const vector<Param> &returns)
        {
            this->name = "constant";
            this->metadata.author = Author::name();
            this->tftype = "init";
            this->args = args;
            this->returns = returns;
        }
        string math_formula() const override
        {
            return "constant(value)->T1";
        }
        shared_ptr<TF> clone() const override
        {
            return make_shared<Constant<Author>>(*this);
        }
        int run(shared_ptr<MemBase> mem, string &error) override
        {
            string name = this->returns[0].textvalue;
            auto tensor = mem->gettensor(name).get();
            if (tensor == nullptr)
            {
                error = "tensor not found: " + name;
                return 1;
            }
            auto type = tensor->shape.dtype;
            switch (type)
            {
            case Precision::Float64:
            {
                auto output = mem->gettensor<double>(name).get();
                tensorfunc::constant<Author, double>(*output, this->getvar<double>(0, mem));
                break;
            }
            case Precision::Float32:
            {
                auto output = mem->gettensor<float>(name).get();
                tensorfunc::constant<Author, float>(*output, this->getvar<float>(0, mem));
                break;
            }

            case Precision::Int64:
            {
                auto output = mem->gettensor<int64_t>(name).get();
                tensorfunc::constant<Author, int64_t>(*output, this->getvar<int64_t>(0, mem));
                break;
            }
            case Precision::Int32:
            {
                auto output = mem->gettensor<int32_t>(name).get();
                tensorfunc::constant<Author, int32_t>(*output, this->getvar<int32_t>(0, mem));
                break;
            }
            case Precision::Int16:
            {
                auto output = mem->gettensor<int16_t>(name).get();
                tensorfunc::constant<Author, int16_t>(*output, this->getvar<int16_t>(0, mem));
                break;
            }
            case Precision::Int8:
            {
                auto output = mem->gettensor<int8_t>(name).get();
                tensorfunc::constant<Author, int8_t>(*output, this->getvar<int8_t>(0, mem));
                break;
            }
            default:
            {
                error = "unsupported dtype: " + precision_str(type);
                return 1;
            }
            }
            return 0;
        };
    };

   //dropout
    template <typename Author>
    class Dropout : public TF
    {
    public:
        Dropout(const vector<Param> &args, const vector<Param> &returns)
        {
            this->name = "dropout";
            this->metadata.author = Author::name();
            this->tftype = "init";
            this->args = args;
            this->returns = returns;
        }
        string math_formula() const override
        {
            return "dropout(p,seed)->A";
        }
        shared_ptr<TF> clone() const override
        {
            return make_shared<Dropout<Author>>(*this);
        }
        int run(shared_ptr<MemBase> mem, string &error) override
        {
            Precision a_type = mem->gettensor(this->returns[0].textvalue).get()->shape.dtype;
    
            switch (a_type)
            {
            case Precision::Float64:
                tensorfunc::dropout<Author, double>(*mem->gettensor<double>(this->returns[0].textvalue), this->getvar<float>(0,mem,true), this->getvar<unsigned int>(1,mem,true));
                break;  
            case Precision::Float32:
                tensorfunc::dropout<Author, float>(*mem->gettensor<float>(this->returns[0].textvalue), this->getvar<float>(0,mem,true), this->getvar<unsigned int>(1,mem,true));
                break;
            case Precision::Int64:
                tensorfunc::dropout<Author, int64_t>(*mem->gettensor<int64_t>(this->returns[0].textvalue), this->getvar<float>(0,mem,true), this->getvar<unsigned int>(1,mem,true));
                break;  
            case Precision::Int32:
                tensorfunc::dropout<Author, int32_t>(*mem->gettensor<int32_t>(this->returns[0].textvalue), this->getvar<float>(0,mem,true), this->getvar<unsigned int>(1,mem,true));
                break;
            case Precision::Int16:
                tensorfunc::dropout<Author, int16_t>(*mem->gettensor<int16_t>(this->returns[0].textvalue), this->getvar<float>(0,mem,true), this->getvar<unsigned int>(1,mem,true));
                break;  
            case Precision::Int8:
                tensorfunc::dropout<Author, int8_t>(*mem->gettensor<int8_t>(this->returns[0].textvalue), this->getvar<float>(0,mem,true), this->getvar<unsigned int>(1,mem,true));
                break;
            default:
                error = "Unsupported dtype: " + precision_str(a_type);
                return 1;   
            }
            return 0;
        }
    };


    //初始化
    // arange
    template <typename Author>
    class Arange : public TF
    {
    public:
        Arange(const vector<Param> &args, const vector<Param> &returns)
        {
            this->name = "arange";
            this->metadata.author = Author::name();
            this->tftype = "init";
            this->args = args;
            this->returns = returns;
        }
         string math_formula() const override
        {
            return "arange(start,step)->T1";
        }
        shared_ptr<TF> clone() const override
        {
            return make_shared<Arange<Author>>(*this);
        }
        int run(shared_ptr<MemBase> mem, string &error) override
        {
            string name = this->returns[0].textvalue;
            auto tensor = mem->gettensor(name).get();
            auto type = tensor->shape.dtype;
            switch (type)
            {
            case Precision::Float64:
            {
                auto output = mem->gettensor<double>(name).get();
                tensorfunc::arange<Author, double>(*output, this->getvar<double>(0, mem), this->getvar<double>(1, mem));
                break;
            }
            case Precision::Float32:
            {
                auto output = mem->gettensor<float>(name).get();
                tensorfunc::arange<Author, float>(*output, this->getvar<float>(0, mem), this->getvar<float>(1, mem));
                break;
            }

            case Precision::Int64:
            {
                auto output = mem->gettensor<int64_t>(name).get();
                tensorfunc::arange<Author, int64_t>(*output, this->getvar<int64_t>(0, mem), this->getvar<int64_t>(1, mem));
                break;
            }
            case Precision::Int32:
            {
                auto output = mem->gettensor<int32_t>(name).get();
                tensorfunc::arange<Author, int32_t>(*output, this->getvar<int32_t>(0, mem), this->getvar<int32_t>(1, mem));
                break;
            }
            case Precision::Int16:
            {
                auto output = mem->gettensor<int16_t>(name).get();
                tensorfunc::arange<Author, int16_t>(*output, this->getvar<int16_t>(0, mem), this->getvar<int16_t>(1, mem));
                break;
            }
            case Precision::Int8:
            {
                auto output = mem->gettensor<int8_t>(name).get();
                tensorfunc::arange<Author, int8_t>(*output, this->getvar<int8_t>(0, mem), this->getvar<int8_t>(1, mem));
                break;
            }
            default:
            {
                error = "unsupported dtype: " + precision_str(type);
                return 1;
            }
            }
            return 0;
        }
       
    };

    // uniform
    template <typename Author>
    class Uniform : public TF
    {
    public:
        Uniform(const vector<Param> &args, const vector<Param> &returns)
        {
            this->name = "uniform";
            this->metadata.author = Author::name();
            this->tftype = "init";
            this->args = args;
            this->returns = returns;
        }
        string math_formula() const override
        {
            return "uniform(low,high,seed)->T1";
        }
        shared_ptr<TF> clone() const override
        {
            return make_shared<Uniform<Author>>(*this);
        }
        int run(shared_ptr<MemBase> mem, string &error) override
        {
            string name = this->returns[0].textvalue;
            auto tensor = mem->gettensor(name).get();
            auto type = tensor->shape.dtype;
            int low_pos=0;
            int high_pos=1;
            unsigned int seed = static_cast<unsigned int>(this->getvar<int>(2, mem));
            switch (type)
            {
            case Precision::Float64:
            {
                auto output = mem->gettensor<double>(name).get();
                tensorfunc::uniform<Author, double>(*output, this->getvar<double>(low_pos, mem), this->getvar<double>(high_pos, mem), seed);
                break;
            }
            case Precision::Float32:
            {
                auto output = mem->gettensor<float>(name).get();
                tensorfunc::uniform<Author, float>(*output, this->getvar<float>(low_pos, mem), this->getvar<float>(high_pos, mem), seed);
                break;
            }

            case Precision::Int64:
            {
                auto output = mem->gettensor<int64_t>(name).get();
                tensorfunc::uniform<Author, int64_t>(*output, this->getvar<int64_t>(low_pos, mem), this->getvar<int64_t>(high_pos, mem), seed);
                break;
            }
            case Precision::Int32:
            {
                auto output = mem->gettensor<int32_t>(name).get();
                tensorfunc::uniform<Author, int32_t>(*output, this->getvar<int32_t>(low_pos, mem), this->getvar<int32_t>(high_pos, mem), seed);
                break;
            }
            case Precision::Int16:
            {
                auto output = mem->gettensor<int16_t>(name).get();
                tensorfunc::uniform<Author, int16_t>(*output, this->getvar<int16_t>(low_pos, mem), this->getvar<int16_t>(high_pos, mem), seed);
                break;
            }
            case Precision::Int8:
            {
                auto output = mem->gettensor<int8_t>(name).get();
                tensorfunc::uniform<Author, int8_t>(*output, this->getvar<int8_t>(low_pos, mem), this->getvar<int8_t>(high_pos, mem), seed);
                break;
            }
            default:
            {
                error = "unsupported dtype: " + precision_str(type);
                return 1;
            }
            }
            return 0;
        }
        
    };

    // normal
    template <typename Author>
    class Normal : public TF
    {
    public:
        Normal(const vector<Param> &args, const vector<Param> &returns)
        {
            this->name = "normal";
            this->metadata.author = Author::name();
            this->tftype = "init";
            this->args = args;
            this->returns = returns;
        }

        string math_formula() const override
        {
            return "normal(mean,stddev,seed)->T1";
        }
        shared_ptr<TF> clone() const override
        {
            return make_shared<Normal<Author>>(*this);
        }
        int run(shared_ptr<MemBase> mem, string &error) override
        {
            string name = this->returns[0].textvalue;
            auto tensor = mem->gettensor(name).get();
            auto type = tensor->shape.dtype;
            int mean_pos=0;
            int stddev_pos=1;
            unsigned int seed = static_cast<unsigned int>(this->getvar<int>(2, mem));
            switch (type)
            {
            case Precision::Float64:
                tensorfunc::normal<Author, double>(*mem->gettensor<double>(name).get(), this->getvar<double>(mean_pos, mem), this->getvar<double>(stddev_pos, mem), seed);
                break;
            case Precision::Float32:
                tensorfunc::normal<Author, float>(*mem->gettensor<float>(name).get(), this->getvar<float>(mean_pos, mem), this->getvar<float>(stddev_pos, mem), seed);
                break;
            case Precision::Int64:
                tensorfunc::normal<Author, int64_t>(*mem->gettensor<int64_t>(name).get(), this->getvar<int64_t>(mean_pos, mem), this->getvar<int64_t>(stddev_pos, mem), seed);
                break;
            case Precision::Int32:
                tensorfunc::normal<Author, int32_t>(*mem->gettensor<int32_t>(name).get(), this->getvar<int32_t>(mean_pos, mem), this->getvar<int32_t>(stddev_pos, mem), seed);
                break;
            case Precision::Int16:
                tensorfunc::normal<Author, int16_t>(*mem->gettensor<int16_t>(name).get(), this->getvar<int16_t>(mean_pos, mem), this->getvar<int16_t>(stddev_pos, mem), seed);
                break;
            case Precision::Int8:
                tensorfunc::normal<Author, int8_t>(*mem->gettensor<int8_t>(name).get(), this->getvar<int8_t>(mean_pos, mem), this->getvar<int8_t>(stddev_pos, mem), seed);
                break;
            default:
            {
                error = "unsupported dtype: " + precision_str(type);
                return 1;
            }
            }
            return 0;
        }
    };
}

#endif
