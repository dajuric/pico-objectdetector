#pragma once

#include <System.h>
#include <System.Collections.h>

using namespace System;
using namespace System::Collections::Generic;

namespace ViolaJones
{
    /// @brief Flips boolean values in the collection.
    /// @param list Collection of booleans.
    /// @return A collection with flipped boolean values.
    List<bool> LogicalNot(List<bool>& list)
    {
        List<bool> nList;
        for (var i = 0; i < list.Count(); i++)
            nList.Add(!list[i]);

        return nList;
    }

    /// @brief Calculates a sum of the provided values.
    /// @param values Values of floats.
    /// @return Sum.
    float Sum(List<float>& values)
    {
        var sum = 0.0f;

        for (var& v: values)
            sum += v;

        return sum;
    }

    /// @brief Calculates an average of the provided values with the provided values.
    /// @param values Values of float values.
    /// @param weights Weights for the provided values.
    /// @return Weighted average.
    float WeightedAverage(List<float>& values, List<float>& weights)
    {
        var s = 0.0f;
        var weightsSum = 0.0f;

        for (var i = 0; i < values.Count(); i++)
        {
            s += (values[i] * weights[i]);
            weightsSum += weights[i];
        }

        var mean = s / (weightsSum + 1e-5);
        return mean;
    }

    /// @brief Finds an index of a minimum element in the collection.
    /// @param list A collection of elements.
    /// @return An index of a minimum element in the collection.
    int Argmin(Collection<float>& list)
    {
        if (list.Count() == 0)
            throw Exception((string)"Unable to calculate arg-min. The list does not contain any elements.");

        var min = list[0];
        var minIdx = 0;

        for (var i = 0; i < list.Count(); i++)
        {
            if (list[i] < min)
            {
                min = list[i];
                minIdx = i;
            }
        }

        return minIdx;
    }


    /// @brief Calculates a point on a ROC curve with a provided threshold.
    /// @param targets Target values, where >0 are positive samples.
    /// @param outputs Classifier outputs.
    /// @param threshold Threshold for a ROC point.
    /// @return TPR and FPR.
    static Tuple<float, float> CalculateRocPoint(List<float>& targets, List<float>& outputs, float threshold)
    {
        var TP = 0.0f, FP = 0.0f, TN = 0.0f, FN = 0.0f;

        for (var idx = 0; idx < targets.Count(); idx++)
        {
            var trueClass = targets[idx] > 0;
            var predictedClass = outputs[idx] >= threshold;

            if (trueClass)
            {
                if (predictedClass)
                    TP += 1;
                else
                    FN += 1;
            }
            else
            {
                if (!predictedClass)
                    TN += 1;
                else
                    FP += 1;
            }
        }

        var TPR = TP / (TP + FN);
        var FPR = FP / (FP + TN);
        return Tuple<float, float>(TPR, FPR);
    }

    /// @brief Searches a threshold on a ROC curve where minimum TPR is achieved.
    /// @param labels Target values.
    /// @param outputs Classifier outputs.
    /// @param minTPR Minimum TPR to target.
    /// @return TPR, FPR and threshold.
    Tuple<float, float, float> SearchROC(List<float>& labels, List<float>& outputs, float minTPR)
    {
        var TPR = 0.0f, FPR = -1.0f;

        var threshold = 0.0f;
        for (var i = 0; i < outputs.Count(); i++)
            threshold = Math::Max(threshold, outputs[i]);

        while (TPR < minTPR)
        {
            Tie<float, float>(TPR, FPR) = CalculateRocPoint(labels, outputs, threshold);
            threshold -= 0.001;
        }

        return Tuple<float, float, float>(TPR, FPR, threshold);
    }
}