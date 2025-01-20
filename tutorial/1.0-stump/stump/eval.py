from .structs import *

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

def predict(tree: Tree, sample):
    """Predicts whether a sample is positive or negative.

    Args:
        tree (Tree): Tree
        sample (array): Multi dimensional array (data).

    Returns:
        Boolean: True if a sample is positive, false otherwise.
    """
    
    is_true = eval_feature(tree.node, sample)
    if is_true:
        nodeIdx = 2
    else:
        nodeIdx = 1

    leaf_idx = nodeIdx - 1
    confidence = tree.leafs[leaf_idx]
    return confidence > 0.5
