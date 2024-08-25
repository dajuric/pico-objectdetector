from .structs import *
from .eval import eval_feature, eval_tree
from .util import search_ROC
import numpy as np

# --------- candidate feature creation
import random
def create_feature_candidates():
    features = []
    
    for i in range(0, 1024):
        f = Node()
        f.rowA = random.randint(-127, +127)
        f.colA = random.randint(-127, +127)
        f.rowB = random.randint(-127, +127)
        f.colB = random.randint(-127, +127)

        features.append(f)

    return features

# --------- util functions for feature selection
def calc_split_error(node: Node, samples, labels, weights):
    """Calculates a value of a split error (SSE) for the provided samples and labels.

    Args:
        node (Node): Node
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.
        weights (array): Array of sample weights.
    """
    
    def SSE(mLabels, mWeights):
        weightedAvg = np.sum(mLabels * mWeights) / (np.sum(mWeights) + 1E-5)

        delta = mLabels - weightedAvg
        SSE = np.sum(delta * delta * mWeights)
        return SSE


    maskRight = np.zeros((len(labels, )), bool)
    for i in range(len(labels)):
        isTrue = eval_feature(node, samples[i])
        maskRight[i] = isTrue

    left_lbls = labels[~maskRight]
    left_ws = weights[~maskRight]
    left_err = SSE(left_lbls, left_ws)

    right_lbls = labels[maskRight]
    right_ws = weights[maskRight]
    right_err = SSE(right_lbls, right_ws)

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
def train_feature_node(tree: Tree, node_idx: int, samples, labels, weights):
    """Trains internal node by selecting the best feature of a feature pool.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index.
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.
        weights (array): Array of sample weights.

    Returns:
        Node: Trained node.
    """
        
    features = create_feature_candidates(samples)
    feat_count = len(features)

    errors = np.zeros((feat_count, ))
    for i in range(feat_count):
        error = calc_split_error(features[i], samples, labels, weights)
        errors[i] = error

    best_feat_idx = np.argmin(errors)
    best_feat = features[best_feat_idx]
    tree.nodes[node_idx] = best_feat
    
    return best_feat

def train_leaf_node(tree: Tree, node_idx: int, max_depth: int, labels, weights):
    """Trains leaf node by returning weighted label.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index.
        max_depth (int): Max tree depth.
        labels (array): Array of labels: zeros and ones.
        weights (array): Array of sample weights.
    """
    
    leaf_idx = node_idx - (2 ** max_depth - 1)

    if len(labels) == 0:
        tree.leafs[leaf_idx] = 0
    else:
        tree.leafs[leaf_idx] = np.sum(weights * labels) / np.sum(weights)

# --------- tree training
def train_node(tree: Tree, node_idx: int, samples, labels, weights, depth: int, max_depth: int):
    """Builds a tree in a recursive fashion. The resulting tree is always full and complete.

    Args:
        tree (Tree): Tree.
        node_idx (int): Node index to train.
        samples (matrix): Rows of arrays (samples).
        labels (array): Array of labels: zeros and ones.
        weights (array): Array of sample weights.
        depth (int): Current tree depth.
        max_depth (int): Target tree depth.
    """
    
    processedNodeCnt = sum(1 for x in tree.nodes if x != None)
    print(f"\r   Training {round(processedNodeCnt * 100 / len(tree.nodes))} % (node {node_idx})...", end="")

    if depth == max_depth:
        train_leaf_node(tree, node_idx, max_depth, labels, weights)
        return

    if len(labels) <= 1:
        tree.nodes[node_idx] = Node()
        train_node(tree, 2 * node_idx + 1, samples, labels, weights, depth + 1, max_depth)
        train_node(tree, 2 * node_idx + 2, samples, labels, weights, depth + 1, max_depth)
        return

    best_feature = train_feature_node(tree, node_idx, samples, labels, weights)

    left_ids, right_ids = split_samples(best_feature, samples)
    train_node(tree, 2 * node_idx + 1, samples[left_ids],  labels[left_ids],  weights[left_ids],  depth + 1, max_depth)
    train_node(tree, 2 * node_idx + 2, samples[right_ids], labels[right_ids], weights[right_ids], depth + 1, max_depth)

def train_tree(samples, labels, weights, max_depth):
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

    train_node(tree, 0, samples, labels, weights, 0, max_depth)
    return tree

# --------- BCT (stage) training
def train_bct(cascade: Cascade, confs, 
              samples, labels, max_tree_depth: int, max_tree_count: int, minTPR: float, maxFPR: float):
    """Trains a boosted classification tree ensamble (here stage) also specifying a threshold at the end.

    Args:
        cascade (Cascade): Cascade.
        confs (array): An array of confidences from previous stages.
        samples (List[image]): A list of positive and negative image patches.
        labels (array): Array of labels [-1, +1].
        max_tree_depth (int): Target tree depth.
        max_tree_count (int): Target tree count. 
        minTPR (float): Minimal TPR to satisfy.
        maxFPR (float): Maximum FPR.
    """
    
    tree_cnt = 0; FPR = 1.0
    threshold = -1000.0
    
    while tree_cnt < max_tree_count and FPR > maxFPR:
        weights = np.zeros((len(labels), ))

        pMask = labels > 0
        weights[pMask]  = np.exp(-1.0 * confs[pMask])  / np.sum(pMask)
        weights[~pMask] = np.exp(+1.0 * confs[~pMask]) / np.sum(~pMask)
        weights /= np.sum(weights)

        print(f"\nTraining {tree_cnt + 1} tree...")
        tree = train_tree(samples, labels, weights, max_tree_depth)

        for i in range(0, len(samples)):
            confs[i] += eval_tree(tree, samples[i])

        cascade.trees.append(tree)
        _, FPR, threshold = search_ROC(labels, confs, minTPR)
        tree_cnt += 1
        
        print(F"\n   Current FPR: {FPR}")

    cascade.trees[-1].threshold = threshold
    print("-------------------------------------------")