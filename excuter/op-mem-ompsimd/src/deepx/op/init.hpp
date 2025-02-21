#ifndef DEEPX_OP_INIT_HPP
#define DEEPX_OP_INIT_HPP

#include "deepx/op/op.hpp"
#include "deepx/tensorfunc/init.hpp"
namespace deepx::op{
    template<typename T>
    class Uniform : public OpT<T>{
        public:
        Uniform(){
            this->init("uniform",dtype<T>::name(), {}, {}, false, {}, {});
        }
        Uniform(vector<string> args, vector<string> returns, bool require_grad = false, vector<string> args_grad = {}, vector<string> returns_grad = {}){
            this->init("uniform",dtype<T>::name(), args, returns, require_grad, args_grad, returns_grad);
        }
        Uniform(initializer_list<string> args, initializer_list<string> returns, bool require_grad = false, initializer_list<string> args_grad = {}, initializer_list<string> returns_grad = {}){
            this->init("uniform",dtype<T>::name(), args, returns, require_grad, args_grad, returns_grad);
        }
        void forward(mem::Mem &mem) override{
            auto output = mem.gettensor<T>(this->returns[0]).get();
            T low = mem.getarg<T>(this->args[0]);
            T high = mem.getarg<T>(this->args[1]);
            tensorfunc::uniform(*output,low,high);
        } 
        void backward(mem::Mem &mem) override{
            throw std::runtime_error("Uniform op does not support backward");
        }
    };

    template<typename T>
    class Constant : public OpT<T>{
        public:
        Constant(){
            this->init("constant",dtype<T>::name(), {}, {}, false, {}, {});
        }
        Constant(vector<string> args, vector<string> returns, bool require_grad = false, vector<string> args_grad = {}, vector<string> returns_grad = {}){
            this->init("constant",dtype<T>::name(), args, returns, require_grad, args_grad, returns_grad);
        }
        Constant(initializer_list<string> args, initializer_list<string> returns, bool require_grad = false, initializer_list<string> args_grad = {}, initializer_list<string> returns_grad = {}){
            this->init("constant",dtype<T>::name(), args, returns, require_grad, args_grad, returns_grad);
        }
        void forward(mem::Mem &mem) override{
            auto output = mem.gettensor<T>(this->returns[0]).get();
            T value = mem.getarg<T>(this->args[0]);
            tensorfunc::constant(*output,value);
        }
        void backward(mem::Mem &mem) override{
            throw std::runtime_error("Constant op does not support backward");
        }
    };

    template<typename T>
    class Arange : public OpT<T>{
        public:
        Arange(){
            this->init("arange",dtype<T>::name(), {}, {}, false, {}, {});
        }
        Arange(vector<string> args, vector<string> returns, bool require_grad = false, vector<string> args_grad = {}, vector<string> returns_grad = {}){
            this->init("arange",dtype<T>::name(), args, returns, require_grad, args_grad, returns_grad);
        }
        Arange(initializer_list<string> args, initializer_list<string> returns, bool require_grad = false, initializer_list<string> args_grad = {}, initializer_list<string> returns_grad = {}){
            this->init("arange",dtype<T>::name(), args, returns, require_grad, args_grad, returns_grad);
        }
        void forward(mem::Mem &mem) override{
            auto output = mem.gettensor<T>(this->returns[0]).get();
            T start = mem.getarg<T>(this->args[0]);
            T step = mem.getarg<T>(this->args[1]);
            tensorfunc::arange(*output,start,step);
        }
        void backward(mem::Mem &mem) override{
            throw std::runtime_error("Arange op does not support backward");
        }
    };
}

#endif
