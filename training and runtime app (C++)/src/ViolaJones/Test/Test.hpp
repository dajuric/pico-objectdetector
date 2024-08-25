#pragma once
#include <opencv2/core.hpp>

#include <System.Threading.h>
#include "../Shared/Cascade.hpp"
#include "../Shared/Config.hpp"

using namespace System::Threading;

namespace ViolaJones
{
    /// @brief Object detection info containing center coordinates, scale and a detection confidence.  
    struct Detection
    {
        int Row;
        int Col;
        float Scale;
        float Confidence;
    };

    /// @brief Evaluates a single internal node on an image patch using a pixel comparison.
    /// @param binTest Binary test containing two normalized coordinates.
    /// @param patch Image grayscale patch.
    /// @return True if a value of a pixel on the first coordinate is larger than on a second coordinate.
    bool EvalFeature(Node& binTest, cv::Mat& patch)
    {
        var pW = patch.cols;
        var pH = patch.rows;

        var rA = ((pH / 2) * 256 + binTest.RowA * pH) / 256;
        var cA = ((pW / 2) * 256 + binTest.ColA * pW) / 256;

        var rB = ((pH / 2) * 256 + binTest.RowB * pH) / 256;
        var cB = ((pW / 2) * 256 + binTest.ColB * pW) / 256;

        var result = patch.at<byte>(rA, cA) <= patch.at<byte>(rB, cB);
        return result; 
    }

    /// @brief Evaluates a tree on a single image patch.
    /// @param tree Tree to evaluate.
    /// @param patch Image grayscale patch.
    /// @return Leaf value. 
    float EvalTree(Tree& tree, cv::Mat& patch)
    {
        var treeDepth = int(Math::Log2(tree.Nodes.Count() + 1));

        var nodeIdx = 0;
        for (var depth = 0; depth < treeDepth; depth++)
        {
            var isTrue = EvalFeature(tree.Nodes[nodeIdx], patch);
            if (isTrue)
                nodeIdx = nodeIdx * 2 + 2; //go right
            else
                nodeIdx = nodeIdx * 2 + 1; //go left
        }

        var leafIdx = nodeIdx - (Math::Pow(2, treeDepth) - 1);
        var confidence = tree.Leafs[leafIdx];
        return confidence;
    }

    /// @brief Classifies a single patch (positive vs negative).
    /// @param cascade Cascade to evaluate.
    /// @param patch Image grayscale patch.
    /// @param confidence Is set to a confidence of a patch being positive.
    /// @return True if a patch containg an object (is positive), false otherwise.
    bool ClassifyPatch(Cascade& cascade, cv::Mat& patch, float& confidence)
    {
        confidence = 0.0f;

        for (var& tree: cascade.Trees)
        {
            var treeConf = EvalTree(tree, patch);
            confidence += treeConf;

            if (confidence < tree.Threshold)
                return false;
        }

        return true;
    }



    using DetectionArgs = Tuple<Cascade&, cv::Mat&, Range<int>, List<Detection>&, Mutex&>; 
    inline static ThreadPool<DetectionArgs> threadPool;
   
    /// @brief Detects objects on an image vertical slice. Used in parallel object detection.
    /// @param args Function arguments passed in a thread. 
    static void DetectObjectsSlice(DetectionArgs args)
    {
        var& [cascade, image, rowRange, detections, lockObj] = args;
        var w = image.cols;
        var h = image.rows;

        var s = MIN_SCALE_FACTOR * Math::Min(w, h);
        while (s < Math::Min(w, h))
        {
            var step = (int)Math::Max((float)Math::Floor(STEP_SCALE * s), 1.0f);
            var ww = Math::Floor(s * cascade.WidthHeightRatio);

            for (var r = rowRange.Start; r < Math::Min(rowRange.Stop + 1, (int)(h - s)); r += step)
            {
                for (var c = 0; c < (w - ww); c += step)
                {
                    var rect = cv::Rect(c, r, ww, s);
                    var patch = cv::Mat(image, rect);

                    var conf = 0.0f;
                    var result = ClassifyPatch(cascade, patch, conf);
                    if (result)
                    {
                        Detection d = { .Row = r, .Col = c, .Scale = s, .Confidence = conf };

                        lockObj.Lock();
                        detections.Add(d);
                        lockObj.Unlock();
                    }
                }
            }

            s = Math::Floor(s * SCALE_INCREASE);
        }
    }

    /// @brief Detects objects on an image in parallel (using a thread pool).
    /// @param cascade Cascade to evaluate.
    /// @param image Image to scan.
    /// @return Collection of found objects.
    static List<Detection> DetectObjectsParallel(Cascade& cascade, cv::Mat& image)
    {
        //start the thread pool, if not started already.
        if (threadPool.ThreadCount() == 0)
            threadPool.Start();

        List<Detection> detections;
        Mutex lockObj;

        var w = image.cols;
        var h = image.rows;
        var maxSliceHeight = (int)Math::Max(1, h / threadPool.ThreadCount());

        //break the image into slices which are then queued by the thread pool.
        var row = 0;
        while (row < h)
        {
            var sliceBottom = (int)Math::Min(row + maxSliceHeight - 1, h - 1);
            var rowSlice = Range<int>(row, sliceBottom);

            var args = DetectionArgs(cascade, image, rowSlice, detections, lockObj);
            threadPool.QueueJob(DetectObjectsSlice, args);

            row += maxSliceHeight / 2;
        }

        //wait all thread to finish execution (they are reused afterwards).
        threadPool.WaitAll();
        return detections;
    }

    /// @brief Detects objects on an image on a single thread.
    /// @param cascade Cascade to evaluate.
    /// @param image Image to scan.
    /// @return Collection of found objects.
    static List<Detection> DetectObjectsSequential(Cascade& cascade, cv::Mat& image)
    {
        List<Detection> detections;
        var w = image.cols;
        var h = image.rows;
        var s = MIN_SCALE_FACTOR * Math::Min(w, h);

        while (s < Math::Min(w, h))
        {
            var step = (int)Math::Max((float)Math::Floor(STEP_SCALE * s), 1.0f);
            var ww = Math::Floor(s * cascade.WidthHeightRatio);

            for (var r = 0; r < h - s; r += step)
            {
                for (var c = 0; c < (w - ww); c += step)
                {
                    var rect = cv::Rect(c, r, ww, s);
                    var patch = cv::Mat(image, rect);

                    var conf = 0.0f;
                    var result = ClassifyPatch(cascade, patch, conf);
                    if (result)
                    {
                        Detection d = { .Row = r, .Col = c, .Scale = s, .Confidence = conf };
                        detections.Add(d);
                    }
                }
            }

            s = Math::Floor(s * SCALE_INCREASE);
        }
        
        return detections;
    }

    /// @brief Detects objects on an image.
    /// @param cascade Cascade to evaluate.
    /// @param image Image to scan.
    /// @return Collection of found objects.
    List<Detection> DetectObjects(Cascade& cascade, cv::Mat& image)
    {
#ifndef PARALLEL
        return DetectObjectsSequential(cascade, image);
#else
         return DetectObjectsParallel(cascade, image);
#endif
    }
};