from .structs import *
from .eval import eval_feature
import numpy as np

# --------- candidate feature creation
def create_feature_candidates(samples):
    """_summary_

    Args:
        samples (matrix): Rows of arrays (samples).

    Returns:
        List[Node]: A list of features (nodes).
    """
    
    data_dim = samples.shape[1]
    min_val = np.min(samples) #for min threshold (can be -1 if a function is normalized)
    max_val = np.max(samples) #for max threshold (can be +1 if a function is normalized)

    features = []
    for i in range(data_dim):
        for t in np.linspace(min_val, max_val, 100):
            f = Node()
            f.dim = i
            f.threshold = t
            features.append(f)

    return features

# --------- util functions for feature selection
def calc_split_error(node: Node, samples, labels):
    """Calculates a value of a split error (SSE) for the provided samples and labels.

    Args:
        node (Node): Node
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.
    """
    
    def SSE(mLabels):
        if len(mLabels) == 0: return float("Inf")
        avg = np.mean(mLabels)

        delta = mLabels - avg
        SSE = np.sum(delta * delta)
        return SSE


    maskRight = np.zeros((len(labels, )), bool)
    for i in range(len(labels)):
        isTrue = eval_feature(node, samples[i])
        maskRight[i] = isTrue

    left_lbls = labels[~maskRight]
    left_err = SSE(left_lbls)

    right_lbls = labels[maskRight]
    right_err = SSE(right_lbls)

    err = (left_err + right_err)
    return err

def split_samples(node: Node, samples):
    """Determines a data split of the provided node. Returns left and right indices for samples and labels.

    Args:
        node (Node): Node
        samples (matrix): Rows of arrays (samples).

    Returns:
        (List[int], List[int]): Left and right indices.
    """
    
    left_ids = []
    right_ids = []

    for i in range(len(samples)):
        isTrue = eval_feature(node, samples[i])
        if isTrue:
            right_ids.append(i)
        else:
            left_ids.append(i)

    return left_ids, right_ids

# --------- node training
def train_feature_node(tree: Tree, samples, labels):
    """Trains internal node by selecting the best feature of a feature pool.

    Args:
        tree (Tree): Tree.
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.

    Returns:
        Node: Trained node.
    """
        
    features = create_feature_candidates(samples)
    feat_count = len(features)

    errors = np.zeros((feat_count, ))
    for i in range(feat_count):
        error = calc_split_error(features[i], samples, labels)
        errors[i] = error

    best_feat_idx = np.argmin(errors)
    best_feat = features[best_feat_idx]
    tree.node = best_feat
    
    return best_feat

def train_leaf_node(tree: Tree, node_idx, labels):
    """Trains leaf node by returning mean label.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index.
        labels (array): Array of labels: zeros and ones.
    """
    
    leafIdx = node_idx - 1

    if len(labels) == 0:
        tree.leafs[leafIdx] = 0
    else:
        tree.leafs[leafIdx] = np.mean(labels)

# --------- tree training
def train(samples, labels):
    """Trains a stump tree.

    Args:
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.

    Returns:
        Tree: Trained tree.
    """
    
    tree = Tree()
    tree.node = Node()
    tree.leafs = [0] * 2

    bestFeature = train_feature_node(tree, samples, labels)
    left_ids, right_ids = split_samples(bestFeature, samples)
    
    train_leaf_node(tree, 1, labels[left_ids])
    train_leaf_node(tree, 2, labels[right_ids])
    return tree