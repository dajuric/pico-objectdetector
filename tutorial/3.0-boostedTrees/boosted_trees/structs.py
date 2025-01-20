from typing import *

class Node():
    """
    Internal tree node (also referenced as a feature).
    """
    
    def __init__(self):
        self.dim: int = 0 #data dimension index
        self.threshold: int = 0 #threshold

class Tree():
    """
    Tree, in a form of two arrays (internal and leaf nodes). 
    We always build full and complete tree.
    """
    
    def __init__(self):
        self.nodes: List[Node] = [] #internal nodes
        self.leafs: List[float] = [] #leaf nodes
        
class BCT():
    """
    Boosted classification trees.
    """
    
    def __init__(self) -> None:
        self.trees: List[Tree] = []
        self.threshold = -1000.0