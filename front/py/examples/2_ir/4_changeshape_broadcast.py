########====DEEPX====########
from deepx import Tensor,ones,broadcast_to

a=ones( 4,2,3 ,name="a")
b=ones(  2,1 ,name='b')
bb=b.broadcast_to( a.shape,out="b.broadcasted")
print(bb)
c=a+bb

print(c)
import os
script_name = os.path.splitext(os.path.basename( os.path.abspath(__file__)))[0]  # 获取不带后缀的脚本名
str=b.graph.to_dot()
str.render(script_name+".dot", format='svg')

########====pytorch====########
import torch
a=torch.ones(4,2,3)
b=torch.ones(2,1)
bb=b.expand(4,2,3)
c=a+bb
print(c)

