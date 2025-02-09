#include <chrono>

#include "deepx/tensor.hpp"
#include "deepx/tensorfunc/broadcast.hpp"
#include "deepx/tensorfunc/print.hpp"
#include "deepx/tensorfunc/new.hpp"
#include "deepx/tensorfunc/init.hpp"
#include "deepx/tensorfunc/elementwise.hpp"
using namespace deepx;
using namespace deepx::tensorfunc;

void test_broadcast()
{
    Tensor tensor = New<float>({4});
    Tensor result = broadcast(tensor, {2, 3, 4});
    print(result);
}
void bench_broadcast(int i)
{
    Tensor tensor = New<float>({i});
    std::cout << "broadcast "<<i<<"x"<<i<<std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Tensor result = broadcast(tensor, {4*i , i  , i});
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "time:" << duration.count() << " seconds" << std::endl;
}

void bench_broadcast_add(int i){
    Tensor tensor = New<float>({i,i});
    uniform(tensor,0.0f,1.0f);
    Tensor other = New<float>({i,i});
    uniform(other,0.0f,1.0f);
    std::cout <<  "broadcast add "<<tensor.shape.shape<<std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    add(tensor, other,tensor);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "time:" << duration.count() << " seconds" << std::endl;
}
void bench_broadcast_mul(int i){
    Tensor tensor = New<float>({i,i});
    uniform(tensor,0.0f,1.0f);
    Tensor other = New<float>({i,i});
    uniform(other,0.0f,1.0f);
    std::cout <<  "broadcast mul "<<tensor.shape.shape<<std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    mul(tensor, other,tensor);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "time:" << duration.count() << " seconds" << std::endl;
}
int main()
{
    test_broadcast();
    for (int i = 32; i <= 1024 * 1024; i *= 2)
    {
        //bench_broadcast(i);
        //bench_broadcast_add(i);
        bench_broadcast_mul(i);
    }
}