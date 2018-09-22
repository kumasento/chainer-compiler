# coding: utf-8

import chainer
import chainer.functions as F


class A(chainer.Chain):

    def __init__(self):
        super(A, self).__init__()

    def forward(self, x, t):
        loss = F.softmax_cross_entropy(x, t)
        return loss


import numpy as np
import chainer2onnx
if __name__ == '__main__':

    out_n = 2
    batch_size = 1
    model = A()

    v = np.random.rand(batch_size, out_n).astype(np.float32)
    w = np.random.randint(out_n, size=batch_size)
    chainer2onnx.generate_testcase(model, [v, w])
