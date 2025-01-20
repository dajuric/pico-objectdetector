#pragma once

#include "Config.hpp"
#include <System.h>
#include <System.Collections.h>
#include <System.IO.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;


namespace ViolaJones
{
    /// @brief An internal node of a tree.
    struct Node
    {
        sbyte RowA;
        sbyte ColA;

        sbyte RowB;
        sbyte ColB;
    };

    /// @brief Single tree represented as an array. All leafs share the same level.
    struct Tree
    {
        List<Node> Nodes;
        List<float> Leafs;
        float Threshold = -1000.0f;
    };

    /// @brief Cascade structure containing tree collection.
    struct Cascade
    {
        int TreeDepth = 0;
        float WidthHeightRatio = 0;
        List<Tree> Trees;

        /// @brief Loads a cascade if exists (and modified config), or creates a new one using provided config.
        /// @param file Cascade file path.
        /// @param config Config containing cascade parameters.
        /// @return Cascade
        static Cascade LoadOrCreate(const string& file, TrainConfig& config)
        {
            if (File::Exists(file))
            {
                var cascade = FromFile(file);

                config.MaxTreeDepth = cascade.TreeDepth;
                config.WidthHeightRatio = cascade.WidthHeightRatio;

                return cascade;
            }

            var cascade = Cascade();
            cascade.TreeDepth = config.MaxTreeDepth;
            cascade.WidthHeightRatio = config.WidthHeightRatio;

            return cascade;
        }

        /// @brief Writes the cascade to a file.
        /// @param file Target file name.
        void ToFile(const string& file)
        {
            var fs = FileStream(file, FileMode::WriteOnly);

            fs.WriteValue((float)1);
            fs.WriteValue(this->WidthHeightRatio);
            fs.WriteValue(this->TreeDepth);
            fs.WriteValue((int)this->Trees.Count());

            for (var& tree: this->Trees)
            {
                for (var& node: tree.Nodes)
                {
                    fs.WriteValue(node.RowA);
                    fs.WriteValue(node.ColA);

                    fs.WriteValue(node.RowB);
                    fs.WriteValue(node.ColB);
                }

                for (var leaf: tree.Leafs)
                    fs.WriteValue(leaf);

                fs.WriteValue(tree.Threshold);
            }

            fs.Close();
        }

        /// @brief  Gets a stage count from the cascade.
        /// @return Stage count.
        int StageCount()
        {
            var nStages = 0;

            for (var& tree: this->Trees)
            {
                if (tree.Threshold < -999.0f)
                    continue;

                nStages++;
            }

            return nStages;
        }

        /// @brief  Loads a cascade from a file.
        /// @param file Source cascade file path.
        /// @return Cascade.
        static Cascade FromFile(const string& file)
        {
            Cascade cascade;
            var fs = FileStream(file, FileMode::ReadOnly);

            var rowScale = fs.ReadValue<float>();
            var colScale = fs.ReadValue<float>();
            var treeDepth = fs.ReadValue<int>();
            var treeCount = fs.ReadValue<int>();

            cascade.TreeDepth = treeDepth;
            cascade.WidthHeightRatio = colScale;

            for (var treeIdx = 0; treeIdx < treeCount; treeIdx++)
            {
                var tree = Tree();

                var nodeCount = (int)Math::Pow(2, treeDepth) - 1;
                for (var nodeIdx = 0; nodeIdx < nodeCount; nodeIdx++)
                {
                    var node = Node();
                    node.RowA = fs.ReadValue<sbyte>();
                    node.ColA = fs.ReadValue<sbyte>();

                    node.RowB = fs.ReadValue<sbyte>();
                    node.ColB = fs.ReadValue<sbyte>();

                    tree.Nodes.Add(node);
                }

                var leafCount = (int)Math::Pow(2, treeDepth);
                for (var leafIdx = 0; leafIdx < leafCount; leafIdx++)
                {
                    var leaf = fs.ReadValue<float>();
                    tree.Leafs.Add(leaf);
                }

                tree.Threshold = fs.ReadValue<float>();
                cascade.Trees.Add(tree);
            }

            fs.Close();
            return cascade;
        }
    };
};