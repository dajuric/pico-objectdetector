from .structs import *
import math

def eval_feature(feature: Node, sample):
    """Evaluates a feature on an image patch.

    Args:
        feature (Node): Binary comparsion feature.
        sample (2DArray): Gray image patch.

    Returns:
        Boolean: True to go next to the right node, false to the left.
    """
    
    w = sample.shape[1]
    h = sample.shape[0]

    rA = ((h // 2) * 256 + feature.rowA * h) // 256
    cA = ((w // 2) * 256 + feature.colA * w) // 256

    rB = ((h // 2) * 256 + feature.rowB * h) // 256
    cB = ((w // 2) * 256 + feature.colB * w) // 256

    result = sample[rA, cA] <= sample[rB, cB]
    return result

def eval_tree(tree: Tree, sample):
    """Evaluates a tree on a sample.

    Args:
        tree (Tree): Tree
        sample (array): Gray image patch.

    Returns:
        float: A confidence of a sample being positive.
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

def predict(cascade: Cascade, sample):
    """Predicts usign a cascade whether an image patch has an object or not.

    Args:
        cascade (Cascade): Cascade.
        sample (array): Gray image patch.

    Returns:
        (Boolean, float): True if an object is present, false otherwise; confidence of object presence.
    """
    
    confidence = 0.0
    
    for tree in cascade.trees:
        tree_conf = eval_tree(tree, sample)
        confidence += tree_conf
        
        is_ok = confidence >= tree.threshold
        if not is_ok:
            return False, confidence
        
    return True, confidence