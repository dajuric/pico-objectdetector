#include "../Shared/Config.hpp"
#include "../Shared/Cascade.hpp"
#include "../Test/Test.hpp"
#include "Dataset/Dataset.hpp"
#include "Dataset/SamplePositives.hpp"
#include "Util.hpp"

namespace ViolaJones
{
    using SplitErrorArgs = Tuple<List<Node>&, List<cv::Mat>&, List<float>&, List<float>&, Array<float>&>;

    /// @brief Creates a collection of random features used in node training.
    /// @return A list of random features.
    static List<Node> CreateRandomFeatures()
    {
        var features = List<Node>();
        var rand = Random();

        for (var i = 0; i < RANDOM_FEATURE_COUNT; i++)
        {
            var f = Node();
            f.RowA = rand.Next(-127, +127);
            f.ColA = rand.Next(-127, +127);

            f.RowB = rand.Next(-127, +127);
            f.ColB = rand.Next(-127, +127);

            features.Add(f);
        }

        return features;
    }

    /// @brief Calculates sum square of errors.
    /// @param labels Target labels.
    /// @param weights Sample weights.
    /// @return Sum square of errors.
    static float SSE(List<float>& labels, List<float>& weights)
    {
        var weightedAvg = WeightedAverage(labels, weights);
        var SSE = 0.0;

        for (var i = 0; i < labels.Count(); i++)
        {
            var delta = labels[i] - weightedAvg;
            SSE += (delta * delta * weights[i]);
        }

        return SSE;
    }

    /// @brief Calculates split error for a node. The less the error, the better the node.
    /// @param node Node to evaluate.
    /// @param patches Image patches.
    /// @param labels Patch labels.
    /// @param sampleWeights Sample weights.
    /// @return Split error for the node.
    static float CalculateSplitError(Node& node, List<cv::Mat>& patches, List<float>& labels, List<float>& sampleWeights)
    {
        var maskRight = List<bool>();
       
        for (var i = 0; i < labels.Count(); i++)
        {
            var isTrue = EvalFeature(node, patches[i]);
            maskRight.Add(isTrue);
        }

        var maskLeft = LogicalNot(maskRight);

        var labelsLeft = labels.Get(maskLeft);
        var weightsLeft = sampleWeights.Get(maskLeft);
        var errLeft  = SSE(labelsLeft, weightsLeft);

        var labelsRight = labels.Get(maskRight);
        var weightsRight = sampleWeights.Get(maskRight);
        var errRight = SSE(labelsRight, weightsRight);

        var err = (errLeft + errRight) / Sum(sampleWeights);
        return err;
    }

    /// @brief Calculates split errors for the provided features in parallel (if enabled).
    /// @param features Feature (node) collection.
    /// @param patches Image patches.
    /// @param labels Patch labels.
    /// @param weights Sample weights.
    /// @return A collection of split errors; one for each node.
    static Array<float> CalculateSplitErrors(List<Node>& features, List<cv::Mat>& patches, List<float>& labels, List<float>& weights)
    {      
        var errors = Array<float>(features.Count());

#ifndef PARALLEL   
        for (var i = 0; i < features.Count(); i++)
        {
            var error = CalculateSplitError(features[i], patches, labels, weights);
            errors[i] = error;
        }
#else
        var splitErrorsArgs = SplitErrorArgs(features, patches, labels, weights, errors);
        Parallel<SplitErrorArgs>::For(0, features.Count(), [](SplitErrorArgs args, long i, bool& shouldCancel) 
        {
            var& [features, patches, labels, weights, out] = args;

            var error = CalculateSplitError(features[i], patches, labels, weights);
            out[i] = error;
        }, 
        splitErrorsArgs);
#endif

        return errors;
    }

    /// @brief Evaluates selected feature against all samples to split the samples into two collections.
    /// @param node Node (feature) to evaluate.
    /// @param patches Image patches.
    /// @return Left and right indices for samples, weights and labels.
    static Tuple<List<int>, List<int>> SplitSamples(Node& node, List<cv::Mat>& patches)
    {
        List<int> indicesLeft, indicesRight;

        for (var i = 0; i < patches.Count(); i++)
        {
            var isTrue = EvalFeature(node, patches[i]);
            if (isTrue)
                indicesRight.Add(i);
            else
                indicesLeft.Add(i);
        }

        return Tuple<List<int>, List<int>>(indicesLeft, indicesRight);
    }

    /// @brief Grows a tree using depth first node expansion (recursively). The function ensures that the tree is full.
    /// @param tree Tree to build.
    /// @param nodeIndex Current node index.
    /// @param patches Image patches.
    /// @param labels Patch labels.
    /// @param weights Sample weights.
    /// @param depth Current tree depth.
    /// @param maxDepth Max tree depth / target depth.
    static void TrainNode(Tree& tree, int nodeIndex, List<cv::Mat>& patches, List<float>& labels, List<float>& weights, int depth, int maxDepth)
    {
        //if we reached the end of a tree set the average value into its leafs
        if (depth == maxDepth)
        {
            var leafIdx = nodeIndex - (Math::Pow(2, maxDepth) - 1);

            if (labels.Count() == 0)
                tree.Leafs[leafIdx] = 0;
            else
                tree.Leafs[leafIdx] = WeightedAverage(labels, weights);

            return;
        }

        //if we run out of data create default node (does not matter) and keep building the tree 
        if (labels.Count() <= 1)
        {
            tree.Nodes[nodeIndex] = Node();
            TrainNode(tree, 2 * nodeIndex + 1, patches, labels, weights, depth + 1, maxDepth);
            TrainNode(tree, 2 * nodeIndex + 2, patches, labels, weights, depth + 1, maxDepth);
            return;
        }

        //create feature candidates
        var features = CreateRandomFeatures();
        var featureCount = features.Count();

        //calculate error for all features
        var errors = CalculateSplitErrors(features, patches, labels, weights);

        //select the best feature (that has the min split error)
        var bestFeatureIdx = Argmin(errors);
        var bestFeature = features[bestFeatureIdx];
        tree.Nodes[nodeIndex] = bestFeature;

        //split samples according to the selected feature and build next nodes recursively
        var [indicesLeft, indicesRight] = SplitSamples(bestFeature, patches);

        var leftPatches = patches.GetAt(indicesLeft); 
        var leftLabels = labels.GetAt(indicesLeft);
        var leftWeights = weights.GetAt(indicesLeft);
        TrainNode(tree, 2 * nodeIndex + 1, leftPatches,  leftLabels, leftWeights,  depth + 1, maxDepth);

        var rightPatches = patches.GetAt(indicesRight); 
        var rightLabels = labels.GetAt(indicesRight);
        var rightWeights = weights.GetAt(indicesRight);
        TrainNode(tree, 2 * nodeIndex + 2, rightPatches, rightLabels, rightWeights, depth + 1, maxDepth);
    }

    /// @brief Trains a tree. The trained tree is full meaning that all the leafs reside on the same level (max depth).
    /// @param patches Image patches.
    /// @param labels Patch labels.
    /// @param weights Sample weights.
    /// @param maxDepth Max tree depth / target depth.
    /// @return Trained tree.
    static Tree TrainTree(List<cv::Mat>& patches, List<float>& labels, List<float>& weights, int maxDepth)
    {
        var tree = Tree();
        tree.Threshold = -1000.0f;

        for (var i = 0; i < (Math::Pow(2, maxDepth) - 1); i++)
            tree.Nodes.Add(Node());

        for (var i = 0; i < Math::Pow(2, maxDepth); i++)
            tree.Leafs.Add(0.0f);

        TrainNode(tree, 0, patches, labels, weights, 0, maxDepth);
        return tree;
    }

    /// @brief Calculates weights for the provided labels given classifier confidences using GentleBoost algorithm.
    /// @param labels Target labels.
    /// @param confidences Classifier outputs.
    /// @return New weights for samples.
    static List<float> WeightSamples(List<float>& labels, List<float>& confidences)
    {
        List<float> weights;

        var pLabelsCnt = 0;
        for (var i = 0; i < labels.Count(); i++)
            if (labels[i] > 0) pLabelsCnt++;

        var nLabelsCnt = 0;
        for (var i = 0; i < labels.Count(); i++)
            if (labels[i] <= 0) nLabelsCnt++;

        for (var i = 0; i < labels.Count(); i++)
        {
            var weight = 0.0;
            if (labels[i] > 0)
                weight = Math::Exp(-1.0 * confidences[i]) / pLabelsCnt;
            else
                weight = Math::Exp(+1.0 * confidences[i]) / nLabelsCnt;

            weights.Add(weight);
        }

        var weightSum = Sum(weights);
        for (var i = 0; i < weights.Count(); i++)
            weights[i] /= weightSum;

        return weights;
    }
 
    /// @brief Appends a stage to a cascade.
    /// @param cascade A cascade to add a single stage to.
    /// @param patches Image patches.
    /// @param labels Target patch labels (+1, -1).
    /// @param outputs Classifier outputs - also modified when additional trees are added.
    /// @param minTPR Minimum TPR to retain.
    /// @param maxFPR Max FPR to tolerate for a stage.
    /// @param maxTreeCount Max tree count per stage.
    static void AppendStage(Cascade& cascade, List<cv::Mat>& patches, 
                            List<float>& labels, List<float>& outputs, 
                            float minTPR, float maxFPR, int maxTreeCount)
    {
        var treeIdx = 0; var FPR = 1.0f;
        var threshold = -1000.0f;

        Console::WriteLine((string)"Training:");
        while (treeIdx < maxTreeCount && FPR > maxFPR)
        {
            //calculate weights for samples (samples which are more misclassified, have larger weights)
            var weights = WeightSamples(labels, outputs);

            //train a single tree
            Console::Write((string)"\tTree (" + String(treeIdx + 1).PadLeft(2, '0') + "/" + maxTreeCount + "): ");
            var tree = TrainTree(patches, labels, weights, cascade.TreeDepth);

            //update overall classifier confidence for each sample
            for (var i = 0; i < patches.Count(); i++)
                outputs[i] += EvalTree(tree, patches[i]);

            cascade.Trees.Add(tree);

            //search ROC for cascade threshold to retain min TPR
            var _ = 0.0f;
            Tie<float, float, float>(_, FPR, threshold) = SearchROC(labels, outputs, minTPR);

            Console::WriteLine((string)"FPR: " + String(FPR, 3));
            treeIdx += 1;
        }

        //only the last tree in a stage has a threshold set to a non default value
        cascade.Trees[cascade.Trees.Count() - 1].Threshold = threshold;
    }

    /// @brief Appends a stage onto an existing cascade if target FPR is not achieved.
    /// @param cascade A cascade to add a single stage to.
    /// @param positives Positive dataset.
    /// @param negatives Negative dataset (an infinite amount of patches).
    /// @param minTPR Minimum TPR to retain.
    /// @param maxFPR Max FPR to tolerate for a stage.
    /// @param targetFPR Target FPR to achieve.
    /// @param maxTreeCount Max tree count per stage.
    /// @return True if the stage is added, false otherwise.
    bool TryAppendStage(Cascade& cascade, 
                        LabeledDataset& positives, LabeledDataset& negatives, 
                        float minTPR, float maxFPR = 0.5f, float targetFPR = 1e-3, int maxTreeCount = 64)
    {
        //sample positives and negatives
        Console::WriteLine((string)"Positives:");
        var [tpSamples, tpConfs, tprHitRatio] = SamplePositives(cascade, positives, positives.Count());
   
        var nFPsTpPick = 2 * positives.Count() - tpSamples.Count();
        Console::WriteLine((string)"Negatives:");
        var [fpSamples, fpConfs, fprHitRatio] = SamplePositives(cascade, negatives, nFPsTpPick, targetFPR);
        
        Console::ForegroundColor = ConsoleColor::Green;
        Console::WriteLine((string)"\nStats:");
        Console::WriteLine((string)"\tTPR: " + String(tprHitRatio, 3) + ". FPR: " + String(fprHitRatio, 3) + "\n");
        Console::ForegroundColor = ConsoleColor::Default;

        //if we reached the target FPR, we are done
        if (fprHitRatio <= targetFPR)
            return false;

        //concatenate positive and negative data (samples + labels)
        var labels = List<float>();
        labels.Add(+1.0f, tpSamples.Count());
        labels.Add(-1.0f, fpSamples.Count());

        var samples = List<cv::Mat>();
        samples.AddRange(tpSamples);
        samples.AddRange(fpSamples);

        var confs = List<float>();
        confs.AddRange(tpConfs);
        confs.AddRange(fpConfs);

        //add single stage
        AppendStage(cascade, samples, labels, confs, minTPR, maxFPR, maxTreeCount);
        return true;
    }

};