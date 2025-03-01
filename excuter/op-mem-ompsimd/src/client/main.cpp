#include <mutex>

#include <deepx/tensorfunc/init.hpp>
#include "deepx/op/op.hpp"
#include "deepx/op/opfactory.hpp"
#include "deepx/mem/mem.hpp"
#include "client/udpserver.hpp"
 
using namespace deepx::tensorfunc;
using namespace deepx::mem;

int main()
{
    Mem  mem;
    std::mutex memmutex;

    client::udpserver server(8080);
    deepx::op::OpFactory opfactory;
    register_all(opfactory);

    server.func = [&mem, &opfactory, &memmutex](const char *buffer)->std::string
    {

        
        deepx::op::Op op;
        op.recv_at = chrono::system_clock::now();
        op.load(buffer);
        std::string resp=to_string(op.id);
        resp+=" recv_at:";
        resp+=to_string(op.recv_at.time_since_epoch().count());
        if (opfactory.ops.find(op.name)==opfactory.ops.end()){
            cout<<"<op> "<<op.name<<" not found"<<endl;
            resp+="error op not found";
        }
        auto &type_map = opfactory.ops.find(op.name)->second;
        if (type_map.find(op.dtype)==type_map.end()){
            cout<<"<op>"<<op.name<<" "<<op.dtype<<" not found"<<endl;
            resp+="error dtype not found";
        }
        auto src = type_map.find(op.dtype)->second;

        (*src).init(op.name, op.dtype, op.args, op.returns, op.grad, op.args_grad, op.returns_grad);
        memmutex.lock();
        if (op.grad) {
            (*src).backward(mem);
        }else {
           (*src).forward(mem);
        }
        memmutex.unlock();
        resp+=" success";
        return resp;
    };
    server.start();
    return 0;
}
