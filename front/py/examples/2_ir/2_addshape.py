from deepx.tensor import Tensor,mul,add
print()

t1 = Tensor(shape=[2,3,4],dtype="float32")
t2 = Tensor(shape=[2,3,4],dtype="float32")
t3 = t1.add_(t2)
