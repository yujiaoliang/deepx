#include "deepx/tensorfunc/init.hpp"
#include "deepx/tensor.hpp"
#include "deepx/tensorfunc/new.hpp"
#include "deepx/tensorfunc/print_miaobyte.hpp"
#include "deepx/tensorfunc/init_miaobyte.hpp"
#include "deepx/tensorfunc/authors.hpp"

using namespace deepx::tensorfunc;
using namespace deepx;
void test_new()
{
    Tensor<float> a=New<float>({10, 10});
    arange<miaobyte,float>(a, 1.0f, 0.1f);
    print<miaobyte>(a,"%.2f");
}

int main()
{
    test_new();
}