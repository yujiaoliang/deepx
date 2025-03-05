from typing import Optional, Union
from deepx import Tensor
from deepx.autograd import Graph,DataNode,OpNode
from deepx.nn import DeepxIR
from deepx.scheduler import send

def _A_B_elementwiseop_C(
        a:Tensor,
        b: Tensor, 
        op:str=None,
        out:Union[Tensor,str]=""):
    g=a.graph
    if g is None:
       g=b.graph

    opnode = g.add_op(op)
    opnode.add_input(a.node)
    opnode.add_input(b.node)
    outtensor=None
    if isinstance(out,str):
        outtensor=Tensor(shape=a.shape, dtype=a.dtype, device=a.device)
        outtensor.addtograph(out)
    else:
        outtensor=out   
    outtensor.node.add_input(opnode)
    if g.eager:
        ir=DeepxIR(op, a.dtype, [a.node.name, b.node.name], [outtensor.node.name])
        send(ir)
def _A_b_elementwiseop_C(
        a:Optional[Union[ Tensor, float, int]] = None, 
        b: Optional[Union[ Tensor, float, int]] = None, 
        op:str=None,
        out:Union[Tensor,str]=""):
    if isinstance(a,Tensor):
        g=a.graph
    else:
        g=b.graph

    opnode = g.add_op(op)
    if isinstance(a,Tensor):
        opnode.add_input(a.node)
    else:
        varnode=g.add_var("",a)
        opnode.add_input(varnode)

    if isinstance(b,Tensor):
        opnode.add_input(b.node)
    else:
        varnode=g.add_var("",b)
        opnode.add_input(varnode)

    outtensor=None
    if isinstance(out,str):
        outtensor=Tensor(shape=a.shape, dtype=a.dtype, device=a.device)
        outtensor.addtograph(out)
    else:
        outtensor=out
    outtensor.node.add_input(opnode)
    if g.eager:
        ir=None
        if isinstance(a,Tensor):
            ir=DeepxIR(op, a.dtype, [a.node.name,b], [outtensor.node.name])
        else:
            ir=DeepxIR(op, b.dtype, [a,b.node.name], [outtensor.node.name])
        send(ir)
#add
OpNode.register("add")
OpNode.register("add_scalar")

def add(
        a:Tensor,
        b: Optional[Union[Tensor, float, int]] = None, 
        out:Union[Tensor,str]=''):
    if isinstance(b,Tensor):
        _A_B_elementwiseop_C(a,b,"add",out)
    else:
        _A_b_elementwiseop_C(a,b,"add_scalar",out)


#sub
OpNode.register("sub")
OpNode.register("sub_scalar")

def sub(
        a:Tensor,
        b: Optional[Union[Tensor, float, int]] = None, 
        out:Union[Tensor,str]=''):   
    if isinstance(b,Tensor):
        _A_B_elementwiseop_C(a,b,"sub",out)
    else:
        _A_b_elementwiseop_C(a,b,"sub_scalar",out)

#mul
OpNode.register("mul")
OpNode.register("mul_scalar")

def mul(
        a:Tensor,
        b: Optional[Union[Tensor, float, int]] = None, 
        out:Union[Tensor,str]=''):
    if isinstance(b,Tensor):
        _A_B_elementwiseop_C(a,b,"mul",out)
    else:
        _A_b_elementwiseop_C(a,b,"mul_scalar",out)
 

#div
OpNode.register("div")
OpNode.register("div_scalar")
OpNode.register("rdiv_scalar")
def div(
        a: Optional[Union[Tensor, float, int]] = None,
        b: Optional[Union[Tensor, float, int]] = None, 
        out:Union[Tensor,str]=''):
    if isinstance(b,Tensor) and isinstance(a,Tensor):
        _A_B_elementwiseop_C(a,b,"div",out)
    else:
        if isinstance(a,Tensor):
            #C=A/b
            _A_b_elementwiseop_C(a,b,"div_scalar",out)
        else:
            #C=a/B
            _A_b_elementwiseop_C(a,b,"rdiv_scalar",out)
#clamp
OpNode.register("clamp")
def clamp(
        a:Tensor,
        min: Optional[Union[ float, int]] = None, 
        max: Optional[Union[ float, int]] = None, 
        out:Union[Tensor,str]=''):   
    opnode = a.graph.add_op("clamp")
    opnode.add_input(a.node)
    outtensor=None
    if isinstance(out,str):
        outtensor=Tensor(shape=a.shape, dtype=a.dtype, device=a.device)
        outtensor.addtograph(out)
    else:
        outtensor=out
    if min is not None:
        min_node = a.graph.add_var("", min)
        opnode.add_input(min_node)
    if max is not None:
        max_node = a.graph.add_var("", max)
        opnode.add_input(max_node)
    outtensor.node.add_input(opnode)
    if a.graph.eager:
        varir=DeepxIR("clamp", a.dtype, [a.node.name,min,max], [outtensor.node.name])
        send(str(varir))

#exp
OpNode.register("exp")
def exp(
        a:Tensor,
        out:Union[Tensor,str]=''):
    opnode = a.graph.add_op("exp")
    opnode.add_input(a.node)
    outtensor=None
    if isinstance(out,str):
        outtensor=Tensor(shape=a.shape, dtype=a.dtype, device=a.device)
        outtensor.addtograph(out)
    else:
        outtensor=out
    outtensor.node.add_input(opnode)
    if a.graph.eager:
        ir=DeepxIR("exp", a.dtype, [a.node.name], [outtensor.node.name])
        send(ir)

#sqrt
OpNode.register("sqrt")
def sqrt(
        input:Tensor,
        out:Union[Tensor,str]='')->Tensor:
    outtensor=None
    if isinstance(out,str):
        outtensor=Tensor(shape=input.shape, dtype=input.dtype, device=input.device)
        outtensor.addtograph(out)
    else:
        outtensor=out
    g=input.graph
    opnode = g.add_op("sqrt")
    opnode.add_input(input.node)
    outtensor.node.add_input(opnode)
    if g.eager:
        ir=DeepxIR("sqrt", input.dtype, [input.node.name], [outtensor.node.name])
        send(ir)
    return out

def rsqrt(
        input:Tensor,
        out:Union[Tensor,str]='')->Tensor:
    outtensor=None
    if isinstance(out,str):
        outtensor=Tensor(shape=input.shape, dtype=input.dtype, device=input.device)
        outtensor.addtograph(out)
    else:
        outtensor=out
    outtensor=1/sqrt(input,outtensor)
    return outtensor
 
# OpNode.register("Placeholder", 102)
# OpNode.register("Neg", 103)
# NodeType.register("Less", 104)
# NodeType.register("Equal", 105)
 
# NodeType.register("Tanh", 107)
 
 
# def placeholder(name=None, shape=None):
#     node = OpNode("Placeholder", name)
#     if shape:
#         node.set_attr("shape", shape)
#     return node

# def neg(x):
#     node = OpNode("Neg")
#     node.add_input("x", x)
#     return node
 
# def less(a, b):
#     node = OpNode("Less")
#     node.add_input("a", a)
#     node.add_input("b", b)
#     return node

# def equal(a, b):
#     node = OpNode("Equal")
#     node.add_input("a", a)
#     node.add_input("b", b)
#     return node
 
# def tanh(x):
#     node = OpNode("Tanh")
#     node.add_input("x", x)
#     return node
 