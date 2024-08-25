#pragma once
#include "ThreadPool.h"

using namespace System;
using namespace System::Collections::Generic;

namespace System::Threading
{
    TC class Parallel
    {
        public:
            using LoopAction = void(*)(T arg, long index, bool& shouldCancel);

            static void For(long from, long toExclusive, LoopAction action, T arg)
            {
                if (threadPool.ThreadCount() == 0)
                    threadPool.Start();

                var slices = SliceRange(from, toExclusive);
                var shouldCancel = false;

                for (var slice: slices)
                    threadPool.QueueJob(RunSlice, SliceJobData(slice, action, arg, shouldCancel));

                threadPool.WaitAll();
            }

            static void InfiniteFor(long from, LoopAction action, T arg, int sliceSize)
            {
                if (threadPool.ThreadCount() == 0)
                    threadPool.Start();

                var shouldCancel = false;
                var i = from;

                while (!shouldCancel)
                {
                    var slice = Range<long>(i, i + sliceSize - 1);
                    threadPool.QueueJob(RunSlice, SliceJobData(slice, action, arg, shouldCancel), true);
                    i += sliceSize;
                }

                threadPool.WaitAll();
            }

        private:
            using SliceJobData = Tuple<Range<long>, LoopAction, T, bool&>; 

            inline static ThreadPool<SliceJobData> threadPool;

            static List<Range<long>> SliceRange(long from, long toExclusive)
            {
                var threadCount = threadPool.ThreadCount();
                var sliceSize = (int)Math::Ceil((float)(toExclusive - from) / threadCount);

                var slices = List<Range<long>>();
                var i = -1l;

                for (i = from; i < toExclusive; i+=sliceSize)
                {
                    var slice = Range<long>(i, Math::Min(i + sliceSize - 1, toExclusive - 1));
                    slices.Add(slice);
                }

                return slices;
            }

            static void RunSlice(SliceJobData data)
            {
                var& [slice, action, arg, shouldCancel] = data;

                for (var i = slice.Start; i <= slice.Stop; i++)
                {
                    action(arg, i, shouldCancel);

                    if (shouldCancel)
                        break;
                }
            }
    };

}