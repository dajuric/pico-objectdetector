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
def train_feature_node(tree: Tree, node_idx: int, samples, labels):
    """Trains internal node by selecting the best feature of a feature pool.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index.
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
    tree.nodes[node_idx] = best_feat
    
    return best_feat

def train_leaf_node(tree: Tree, node_idx: int, max_depth: int, labels):
    """Trains leaf node by returning mean label.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index.
        max_depth (int): Max tree depth.
        labels (array): Array of labels: zeros and ones.
    """
    
    leaf_idx = node_idx - (2 ** max_depth - 1)

    if len(labels) == 0:
        tree.leafs[leaf_idx] = 0
    else:
        tree.leafs[leaf_idx] = np.mean(labels)

# --------- tree training
def train_node(tree: Tree, node_idx: int, samples, labels, depth: int, max_depth: int):
    """Builds a tree in a recursive fashion. The resulting tree is always full and complete.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index to train.
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.
        depth (int): Current tree depth.
        max_depth (int): Target tree depth.
    """
    
    processedNodeCnt = sum(1 for x in tree.nodes if x != None)
    print(f"\r   Training {round(processedNodeCnt * 100 / len(tree.nodes))} % (node {node_idx})...", end="")

    if depth == max_depth:
        train_leaf_node(tree, node_idx, max_depth, labels)
        return

    if len(labels) <= 1:
        tree.nodes[node_idx] = Node()
        train_node(tree, 2 * node_idx + 1, samples, labels, depth + 1, max_depth)
        train_node(tree, 2 * node_idx + 2, samples, labels, depth + 1, max_depth)
        return

    best_feature = train_feature_node(tree, node_idx, samples, labels)

    left_ids, right_ids = split_samples(best_feature, samples)
    train_node(tree, 2 * node_idx + 1, samples[left_ids],  labels[left_ids],  depth + 1, max_depth)
    train_node(tree, 2 * node_idx + 2, samples[right_ids], labels[right_ids], depth + 1, max_depth)

def train(samples, labels, max_depth = 5):
    """Trains a tree.

    Args:
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.
        max_depth (_type_): Target tree depth.

    Returns:
        Tree: Trained tree.
    """
    
    tree = Tree()
    tree.nodes = [None] * (2 ** max_depth - 1)
    tree.leafs = [0]    * (2 ** max_depth)

    train_node(tree, 0, samples, labels, 0, max_depth)
    return tree