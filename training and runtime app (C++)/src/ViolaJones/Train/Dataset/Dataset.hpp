#pragma once

#include <System.h>
#include <System.Collections.h>
#include <System.IO.h>

#include "../../Shared/Config.hpp"
#include "Transforms.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;


namespace ViolaJones
{
    /// @brief Base class for loading YOLOv3 style data (collection of image + labels files).
    class LabeledDataset
    {
        using Objects = List<List<Rect>>;

        public:
            /// @brief Constructs new labeled dataset. 
            /// @param dbFolder Database folder recursively scanned for images.
            /// @param whRatio Width height ratio for ROIs.
            LabeledDataset(const string& dbFolder, float whRatio = -1)
            {
                this->dbFolder = dbFolder;
                FillData(whRatio);
                this->whRatio = whRatio;
            }

            virtual cv::Mat operator [](int index)
            {
                throw NotImplementedException((string)"A derived class must implement this method.");
            }

            virtual long Count()
            {
                throw NotImplementedException((string)"A derived class must implement this method.");
            }

        protected:
            /// @brief Database folder.
            string dbFolder;
            /// @brief Width height ratio for a ROI.
            float whRatio;

            /// @brief Image file collection.
            List<string> imgFiles;
            /// @brief A collection of object bounds for each image.
            Objects rois;

            /// @brief Constructs new labeled dataset using an already created set. 
            /// @param set Labeled dataset.
            LabeledDataset(LabeledDataset& set)
            {
                this->dbFolder = set.dbFolder;
                this->whRatio = set.whRatio;

                this->imgFiles = set.imgFiles;
                this->rois = set.rois;
            }

            /// @brief Loads an image as grayscale using a provided index.
            /// @param imFileIdx Image index.
            /// @return Grayscale image.
            cv::Mat ReadGrayImage(int imFileIdx)
            {
                var imFile = imgFiles[imFileIdx];
                var im = cv::imread(cv::String(imFile.Ptr(), imFile.Length()), cv::IMREAD_GRAYSCALE);
                if (im.empty())
                    throw Exception("Can not open the specified image: " + imFile);
        
                return im;
            }

            /// @brief Initializes dataset by reading image file names and parsing their label file.
            /// @param whRatio Width height ratio to enforce to a read object ROI.
            void FillData(float whRatio)
            {
                var allFiles = Directory::GetFiles(dbFolder, "", true);
                var nROIs = 0;

                for (var i = 0; i < allFiles.Count(); i++)
                {
                    var& file = allFiles[i];
                    var isIm = file.EndsWith(".jpg") || file.EndsWith(".bmp") || file.EndsWith(".png");
                    if (!isIm) 
                        continue;

                    imgFiles.Add(file);

                    var [p, f, e] = Path::FileParts(file);
                    var lblPath = Path::Combine(p, f + ".txt");

                    if (File::Exists(lblPath) == false)
                    {
                        rois.Add(List<Rect>());
                        continue;
                    }

                    var rects = ParseImageROIs(lblPath, whRatio);
                    rois.Add(rects);
                    nROIs += rects.Count();

                    Console::Progress((float)i / allFiles.Count(), (string)"\tReading data...");
                }

                Console::Progress(1.0f, (string)"\tReading data...");
                Console::WriteLine((string)"\tImage count:  " + imgFiles.Count());
                Console::WriteLine((string)"\tObject count: " + nROIs);
            }

            /// @brief Parses label file which stores object ROIs in YOLOv3 format.
            /// @param lblFile Label file.
            /// @param whRatio Width height ratio to enforce.
            /// @return A collection of object ROIs.
            List<Rect> ParseImageROIs(string& lblFile, float whRatio)
            {
                List<Rect> rects;
                var rows = File::ReadAllLines(lblFile);

                for (var row: rows)
                {
                    var elements = row<-Split({' '});
                    if (elements.Count() != 5)
                        throw Exception("Invalid ROI: " + row);

                    float c  = String::ParseInt32(elements[0]);
                    float cX = String::ParseDouble(elements[1]);
                    float cY = String::ParseDouble(elements[2]);
                    float w  = String::ParseDouble(elements[3]);
                    float h  = String::ParseDouble(elements[4]);

                    if (whRatio != -1)
                        w = whRatio * h;
                    
                    if (w == 0.0 || h == 0.0)
                        continue;

                    var rect = Rect { .CenterX = cX, .CenterY = cY, .Width = w, .Height = h };
                    rects.Add(rect);
                }

                return rects;
            }

            /// @brief Gets an image patch using the provided ROI.
            /// @param im Grayscale image.
            /// @param rect ROI for a patch.
            /// @return Image patch (data is cloned).
            cv::Mat GetPatch(cv::Mat& im, Rect& rect)
            {
                var [imW, imH] = im.size();
                int x = (rect.CenterX - rect.Width  / 2) * imW;
                int y = (rect.CenterY - rect.Height / 2) * imH;
                int w = rect.Width * imW;
                int h = rect.Height * imH;

                if (w == 0 || h == 0)
                    throw ArgumentException((string)"Can not get a patch. The specified ROI has zero size.");

                if (x < 0 || y < 0 || (x + w) >= imW || (y + h) >= imH)
                {
                    Console::Error((string)"Patch size: " + imW + "x" + imH);
                    Console::Error((string)"ROI       : " + x + ", " + y + ", " + w + ", " + h); //TODO: remove
                    throw ArgumentException((string)"Can not get a patch. The specified ROI is out of bounds.");
                }

                var patch = im(cv::Rect(x, y, w, h)).clone();
                return patch;
            }
    };

    /// @brief Dataset for positive samples.
    class PositiveDataset: public LabeledDataset
    {
        public:
            /// @brief Creates and initializes new dataset.
            /// @param dbFolder Database folder.
            /// @param whRatio Width height ratio for ROIs to enforce.
            /// @param roiTransform An optional ROI transform.
            PositiveDataset(const string& dbFolder, float whRatio = -1, RoiTransform* roiTransform = null)
                :LabeledDataset(dbFolder, whRatio)
            {
                this->roiTransform = roiTransform;
                FillObjIndices();
            }

            /// @brief Creates new dataset using an already initialized labeled dataset (base).
            /// @param set Labeled dataset.
            /// @param roiTransform An optional ROI transform.
            PositiveDataset(LabeledDataset& set, RoiTransform* roiTransform = null)
                :LabeledDataset(set)
            {
                this->roiTransform = roiTransform;
                FillObjIndices();
            }

            /// @brief Gets an grayscale image patch.
            /// @param index Object index.
            /// @return Grayscale image patch.
            cv::Mat operator [](int index) override
            {
                if (index >= Count())
                    throw IndexOutOfRangeException();
        
                var imgIdx = objImgIndices[index];
                var objIdx = objRoiIndices[index];

                var grayIm = ReadGrayImage(imgIdx);
                var rect = rois[imgIdx][objIdx];

                if (roiTransform != null)
                    rect = roiTransform->Transform(rect);

                var patch = GetPatch(grayIm, rect);
                return patch;
            }

            /// @brief Gets a number of objects.
            /// @return Object count.
            long Count() override
            {
                return this->objImgIndices.Count();
            }

        private:
            RoiTransform* roiTransform;
            List<int> objImgIndices;
            List<int> objRoiIndices;

            /// @brief Initializes image and ROI indices collections. 
            ///        A user provided index is then mapped to an image and its object.
            void FillObjIndices()
            {
                for (var i = 0; i < imgFiles.Count(); i++)
                {
                    if (rois[i].Count() == 0)
                        continue; //skip negative samples

                    for (var j = 0; j < rois[i].Count(); j++)
                    {
                        objImgIndices.Add(i);
                        objRoiIndices.Add(j);
                    }
                }
            }
    };

    /// @brief Dataset for negatives sampling.
    class NegativeDataset: public LabeledDataset
    {
        public:
            /// @brief Creates and initializes new dataset.
            /// @param dbFolder Database folder.
            /// @param whRatio Width height ratio for ROIs to enforce.
            /// @param minH Minimal height of a sampled patch.
            NegativeDataset(const string& dbFolder, float whRatio = -1, int minH = 50)
                :LabeledDataset(dbFolder, whRatio)
            {
                this->minH = minH;
            }

            /// @brief Creates new dataset using an already initialized labeled dataset (base).
            /// @param set Labeled dataset.
            /// @param minH Minimal height of a sampled patch.
            NegativeDataset(LabeledDataset& set, int minH = 50)
                :LabeledDataset(set)
            {
                this->minH = minH;
            }

            /// @brief Gets a grayscale image patch sampled from the image collection.
            /// @param _ Not used.
            /// @return Grayscale image patch.
            cv::Mat operator [](int _) override
            {
                var imIdx = rand.Next(0, imgFiles.Count() - 1);
                var grayIm = ReadGrayImage(imIdx);

                var objROIs = rois[imIdx];        
                Rect randROI;

                //if image does not contain objects, the first random ROI will be returned
                do
                {
                   randROI = GetRandomROI(grayIm.size(), minH, whRatio);
                } 
                while (GetMaxIOU(randROI, objROIs) >= 0.5); 
                      
                var patch = GetPatch(grayIm, randROI);
                return patch;
            }

            /// @brief Gets a number of dataset samples.
            /// @return Return max Int32 value, because we have an infinite pool of samples.
            long Count() override
            {
                return 0x7FFFFFFF; //Negative dataset has an infinite amount of samples.
            }

        private:
            int minH;
            Random rand;

            /// @brief Gets a random ROI provided the image size, minimal height and width/height ratio.
            /// @param imSize Image size.
            /// @param minH Minimal height to enforce.
            /// @param whRatio Width height ratio to enforce.
            /// @return ROI.
            Rect GetRandomROI(cv::Size imSize, int minH, float whRatio)
            {
                var [imW, imH] = imSize;

                int h = rand.Next(Math::Min(minH, imH), imH);
                int w = (int)(whRatio * h);

                int xC = rand.Next(0 + w/2, imW - w / 2);
                int yC = rand.Next(0 + h/2, imH - h / 2);

                return Rect 
                { 
                    .CenterX = (float)xC / imW, 
                    .CenterY = (float)yC / imH, 
                    .Width   = (float)w / imW, 
                    .Height  = (float)h / imH 
                };
            }

            /// @brief Gets a maximum IOU between the provided ROI and other object ROIs.
            /// @param roi ROI of an object.
            /// @param objRois Other ROIs.
            /// @return Max IOU.
            float GetMaxIOU(Rect& roi, List<Rect>& objRois)
            {
                var maxIOU = 0.0f;

                for (var& objRoi: objRois)
                {
                    var iou = GetIOU(roi, objRoi);
                    if (iou > maxIOU)
                        maxIOU = iou;
                }

                return maxIOU;
            }

            /// @brief Gets an IOU for two ROIs.
            /// @param roiA First ROI.
            /// @param roiB Second ROI.
            /// @return IOU.
            float GetIOU(Rect& roiA, Rect& roiB)
            {
                var [xCa, yCa, wa, ha] = roiA;
                var [xCb, yCb, wb, hb] = roiB;

                float overW = Math::Max(0.0f, Math::Min(xCa + wa / 2, xCb + wb / 2) - Math::Max(xCa - wa / 2, xCb - wb / 2));
                float overH = Math::Max(0.0f, Math::Min(yCa + ha / 2, yCb + hb / 2) - Math::Max(yCa - ha / 2, yCb - hb / 2));

                var iou = overW * overH / (wa * ha + wb * hb - overW * overH);
                return iou;
            }
    };
};