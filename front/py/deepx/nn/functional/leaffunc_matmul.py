from typing import Union

from deepx import Tensor,Shape
from .leaffunc_life import newtensor
from .authormap import defaultauthor

def matmul(a:Tensor,b:Tensor,out:Union[Tensor,str]='',bench:int=None)->Tensor:
    outtensor=out
    if isinstance(out,str) or out is None:
        outshape=Shape.matmul(a.shape,b.shape)
        outtensor=newtensor(outshape,dtype=a.dtype,name=out)
    from .rtf_matmul import rtf_matmul
    rtf_matmul(a,b,outtensor,defaultauthor['matmul'],bench)
    return outtensor
