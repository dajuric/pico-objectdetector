from .base import *
import struct
from typing import *

def read_cascade(file_name):
    file = open(file_name, "rb")

    row_scale   = struct.unpack('f', file.read(4))[0]
    col_scale   = struct.unpack('f', file.read(4))[0]
    tree_depth  = struct.unpack('i', file.read(4))[0]
    tree_count  = struct.unpack('i', file.read(4))[0]

    cascade = Cascade(col_scale, tree_depth)

    for i in range(0, tree_count):
        tree = Tree()

        for nodeIdx in range(0, 2 ** tree_depth-1):
            node = Node()
            node.rowA = struct.unpack('b', file.read(1))[0]
            node.colA = struct.unpack('b', file.read(1))[0]
            node.rowB = struct.unpack('b', file.read(1))[0]
            node.colB = struct.unpack('b', file.read(1))[0]

            tree.nodes.append(node)

        for leafIdx in range(0, 2 ** tree_depth):
            leaf = struct.unpack('f', file.read(4))[0]
            tree.leafs.append(leaf)

        tree.threshold = struct.unpack('f', file.read(4))[0]
        cascade.trees.append(tree)

    file.close()
    return cascade

def write_cascade(cascade, file_name):
    file = open(file_name, "wb")

    file.write(struct.pack('f', 1))
    file.write(struct.pack('f', cascade.wh_ratio))
    file.write(struct.pack('i', cascade.tree_depth))
    file.write(struct.pack('i', len(cascade.trees)))

    for tree in cascade.trees:
        for node in tree.nodes:
            file.write(struct.pack('b', node.rowA))
            file.write(struct.pack('b', node.colA))
            file.write(struct.pack('b', node.rowB))
            file.write(struct.pack('b', node.colB))

        for leaf in tree.leafs:
            file.write(struct.pack('f', leaf))

        file.write(struct.pack('f', tree.threshold))

    file.close()