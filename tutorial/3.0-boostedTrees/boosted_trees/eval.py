from .structs import *
import math

def eval_feature(feature: Node, sample):
    """Evaluates a rule of a internal node (feature).

    Args:
        feature (Node): Node
        sample (array): Multi dimensional array (data).

    Returns:
        Boolean: True to visit the right node, left otherwise.
    """
        
    result = sample[feature.dim] <= feature.threshold
    return result

def eval_tree(tree: Tree, sample):
    """Evaluates a tree on a sample.

    Args:
        tree (Tree): Tree
        sample (array): Multi dimensional array (data).

    Returns:
        Float: A confidence of a sample being positive.
    """
    
    treeDepth = int(math.log2(len(tree.nodes) + 1))

    nodeIdx = 0
    for depth in range(0, treeDepth):
        is_true = eval_feature(tree.nodes[nodeIdx], sample)
        if is_true:
            nodeIdx = nodeIdx * 2 + 2
        else:
            nodeIdx = nodeIdx * 2 + 1

    leafIdx = nodeIdx - (2 ** treeDepth - 1)
    confidence = tree.leafs[leafIdx]
    return confidence

def predict(bct: BCT, sample):
    """Predicts whether a sample is positive or negative.

    Args:
        bct (BCT): Boosted classification trees.
        sample (array): Multi dimensional array (data).

    Returns:
        Boolean: True if the sample is postiive, false otherwise.
    """
    
    confidence = 0.0
    
    for tree in bct.trees:
        tree_conf = eval_tree(tree, sample)
        confidence += tree_conf
        
    return confidence > bct.threshold