from typing import *

class Node():
    """
    Internal tree node (also referenced as a feature).
    """
    
    def __init__(self):
        self.rowA:int = 0
        self.colA:int = 0
        self.rowB:int = 0
        self.colB:int = 0

class Tree():
    """
    Tree, in a form of two arrays (internal and leaf nodes). 
    We always build full and complete tree.
    """
    
    def __init__(self):
        self.nodes: List[Node] = [] #internal nodes
        self.leafs: List[float] = [] #leaf nodes
        self.threshold: float = -1000.0
        
class Cascade():
    """
    A list of stages.
    """
    
    def __init__(self, wh_ratio, tree_depth):
        self.trees: List[Tree] = []
        self.wh_ratio = wh_ratio
        self.tree_depth = tree_depth
        
    def stage_count(self):
        cnt = 0
        
        for t in self.trees:
            if t.threshold > -999.0:
                cnt += 1
                
        return cnt