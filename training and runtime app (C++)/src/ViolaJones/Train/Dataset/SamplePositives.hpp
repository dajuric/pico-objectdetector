#pragma once

#include <System.h>
#include "../../Test/Test.hpp"
#include "Dataset.hpp"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace ViolaJones
{
    using SamplePositivesResult = Tuple<List<cv::Mat>&, List<float>&, int&, Mutex&>;
    using SamplePositivesTerm = Tuple<int, float>;
    using SamplePositivesArgs = Tuple<Cascade&, LabeledDataset&, SamplePositivesTerm, SamplePositivesResult>;

    /// @brief Takes samples from a provided dataset, classifies them and takes only positive ones.
    /// @param cascade Cascade to evaluate.
    /// @param patches Dataset.
    /// @param pickCount Max number of positive samples to pick.
    /// @param minHitRate Min hit rate to achieve while sampling.
    /// @return Collection of samples, classifier confidences and a hit rate (TPR or FPR depending on a dataset).
    static Tuple<List<cv::Mat>, List<float>, float> SamplePositivesSequential(Cascade& cascade, LabeledDataset& patches, int pickCount, float minHitRate)
    {
        List<cv::Mat> positiveSamples;
        List<float> confidences;
        float hitRate;

        var i = 0, nPicked = 0;
        while (nPicked < pickCount && i < patches.Count())
        {
            //classify patch and store a result in a list
            var patch = patches[i];
            var conf = 0.0f;
            var result = ClassifyPatch(cascade, patch, conf);

            if (result)
            {
                positiveSamples.Add(patch);
                confidences.Add(conf);
                nPicked += 1;
            }

            i += 1;

            //break loop if too hard
            if (i % pickCount == 0)
            {
                hitRate = (float)nPicked / i;
                if (hitRate < minHitRate)
                    break;
            }

            Console::Write((string)"\r\tSampling: " + nPicked + " / " + pickCount);
        }

        Console::WriteLine();
        hitRate = (float)nPicked / i;

        return Tuple<List<cv::Mat>, List<float>, float>(positiveSamples, confidences, hitRate);
    }


    /// @brief Takes only positive samples from a dataset. Used in parallel sample retrieval.
    /// @param args Action arguments.
    /// @param index Index for a dataset.
    /// @param shouldCancel Cancellation token.
    static void SamplePositiveParallelAction(SamplePositivesArgs args, long index, bool& shouldCancel)
    {
        var& [cascade, patches, termData, resultData] = args;
        var& [pickCount, minHitRate] = termData;
        var& [positiveSamples, confidences, i, lockObj] = resultData;

        //termination condition check
        lockObj.Lock();
        {
            if (i >= patches.Count())
                shouldCancel = true;

            if (positiveSamples.Count() >= pickCount)
                shouldCancel = true;

            //break if too hard
            if (i % pickCount == 0 && i > 0)
            {
                var hitRate = (float)positiveSamples.Count() / i;
                if (hitRate < minHitRate)
                    shouldCancel = true;
            }
        }
        lockObj.Unlock();
        if (shouldCancel || (index >= patches.Count())) 
            return;

        //evaluate patch
        var patch = patches[index];
        var conf = 0.0f;
        var result = ClassifyPatch(cascade, patch, conf);

        //update collections and counter
        lockObj.Lock();
        {
            if (result)
            {
                positiveSamples.Add(patch);
                confidences.Add(conf);
            }

            i += 1;
            Console::Write((string)"\r\tSampling: " + positiveSamples.Count());
        }
        lockObj.Unlock();
    }

    /// @brief Takes samples from a provided dataset, classifies them and takes only positive ones. Does this in parallel if parallelism enabled.
    /// @param cascade Cascade to evaluate.
    /// @param patches Dataset.
    /// @param pickCount Max number of positive samples to pick.
    /// @param minHitRate Min hit rate to achieve while sampling.
    /// @return Collection of samples, classifier confidences and a hit rate (TPR or FPR depending on a dataset).
    Tuple<List<cv::Mat>, List<float>, float> SamplePositives(Cascade& cascade, LabeledDataset& patches, int pickCount, float minHitRate = 0.0f)
    {
#ifndef PARALLEL
        return SamplePositivesSequential(cascade, patches, pickCount, minHitRate);
#else
        List<cv::Mat> positiveSamples;
        List<float> confidences;
        int nTrials = 0;
        Mutex lockObj;

        //thread arguments
        var result = SamplePositivesResult(positiveSamples, confidences, nTrials, lockObj);
        var term = SamplePositivesTerm(pickCount, minHitRate);
        var args = SamplePositivesArgs(cascade, patches, term, result);

        //run an infinite for loop, cancelled only by a provided token
        var sliceSize = (int)Math::Max(pickCount / Environment::ProcessorCount(), 1);
        Parallel<SamplePositivesArgs>::InfiniteFor(0, SamplePositiveParallelAction, args, sliceSize);
        Console::WriteLine(); 

        //return results
        var hitRate = (float)positiveSamples.Count() / nTrials;
        return Tuple<List<cv::Mat>, List<float>, float>(positiveSamples, confidences, hitRate);
#endif
    }
}