#ifndef DEEPX_MEM_MEM_HPP
#define DEEPX_MEM_MEM_HPP

#include <any>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <memory>
#include "deepx/tensor.hpp"
#include "deepx/tensorfunc/new.hpp"

namespace deepx::mem
{
    using namespace std;
    class Mem
    {
    private:
        unordered_map<string, std::any> args;

        std::unordered_map<std::string, std::shared_ptr<TensorBase>> mem;
        int tempidx = 0;

        // template <typename T>
        // static std::shared_ptr<void> type_erase(const std::shared_ptr<Tensor<T>> &ptr)
        // {
        //     return std::static_pointer_cast<void>(ptr);
        // }

        // template <typename T>
        // static std::shared_ptr<Tensor<T>> type_restore(const std::shared_ptr<void> &ptr)
        // {
        //     return std::static_pointer_cast<Tensor<T>>(ptr);
        // }

    public:
        Mem() = default;
        ~Mem() = default;
        Mem(const Mem &other)
        {
            args = other.args;
            mem = other.mem;
        }
        Mem(Mem &&other) noexcept
        {
            args = std::move(other.args);
            mem = std::move(other.mem);
        }
        Mem &operator=(const Mem &other)
        {
            args = other.args;
            mem = other.mem;
            return *this;
        }
        Mem &operator=(Mem &&other) noexcept
        {
            args = std::move(other.args);
            mem = std::move(other.mem);
            return *this;
        }
        template <typename T>
        void addarg(const string &name, const T value)
        {
            if (args.find(name) != args.end())
            {
                cerr << "Argument already exists: " << name << endl;
            }
            args[name] = value;
        }

        template <typename T>
        T getarg(const string &name) const
        {
            if (args.find(name) == args.end())
            {
                cerr << "Argument not found: " << name << endl;
                return T();
            }
            return any_cast<T>(args.at(name));
        }

        template <typename T>
        void addvector(const string &name, const vector<T> &value)
        {
            if (args.find(name) != args.end())
            {
                cerr << "Vector already exists: " << name << endl;
                return;
            }
            args[name] = value;
        }

        template <typename T>
        vector<T> getvector(const string &name) const
        {
            if (args.find(name) == args.end())
            {
                cerr << "Vector not found: " << name << endl;
                return vector<T>();
            }
            auto v = any_cast<vector<T>>(args.at(name));
            return v;
        }

        // tensor

        template <typename T>
        void addtensor(const string &name, Tensor<T> &&tensor)
        {
            if (mem.find(name) != mem.end())
            {
                cerr << "Tensor already exists: " << name << endl;
                return;
            }
            auto ptr = std::make_shared<Tensor<T>>(std::move(tensor));
            mem[name] = ptr;
        }

        template <typename T>
        void addtensor(const string &name, const Tensor<T> &tensor)
        {
            if (mem.find(name) != mem.end())
            {
                cerr << "Tensor already exists: " << name << endl;
                return;
            }
            auto ptr = std::make_shared<Tensor<T>>(tensor);
            mem[name] = ptr;
        }

        template <typename T>
        shared_ptr<Tensor<T>> temptensor(vector<int> shape)
        {
            // 直接构造到shared_ptr避免移动
            auto temp = tensorfunc::New<T>(shape); // 临时对象
            auto cloned = make_shared<Tensor<T>>(std::move(temp));
            mem["temp" + to_string(tempidx)] = cloned;
            tempidx++;
            return cloned;
        }

        bool existstensor(const string &name) const
        {
            return mem.find(name) != mem.end();
        }

        template <typename T>
        shared_ptr<Tensor<T>> gettensor(const string &name) const
        {
            auto ptr = mem.at(name);
            return std::static_pointer_cast<Tensor<T>>(ptr);
        }

        // 获取多个张量
        template <typename T>
        vector<Tensor<T> *> gettensors(const vector<string> &names) const
        {
            std::vector<Tensor<T> *> tensors;

            for (const auto &name : names)
            {
                if (mem.find(name) == mem.end())
                {
                    cerr << "Tensor not found: " << name << endl;
                    continue;
                }
                auto ptr = mem.at(name);
                tensors.push_back(std::static_pointer_cast<Tensor<T>>(ptr).get());
            }

            return tensors;
        }

        template <typename T>
        void delete_tensor(const string &name)
        {
            mem.erase(name);
        }

        void delete_arg(const string &name)
        {
            args.erase(name);
        }
        void clear()
        {
            args.clear();
            mem.clear();
        };
    };
}
#endif // DEEPX_MEM_MEM_HPP