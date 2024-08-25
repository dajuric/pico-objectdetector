#define PARALLEL 1 //execute test procedure in parallel where applicable

#include "Test.hpp"
#include <System.Diagnostics.h>
#include <Extensions/ConsoleExtensions.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace ViolaJones;
using namespace System::Diagnostics;

/// @brief Outputs an error produced inside a thread - not caught in a main exception block.
/// @param ex An occurred exception.
static void OutputThreadError(Exception& ex)
{
    Console::Error((string)"Unexpected thread exception.");
    Console::Error(ex);
}

/// @brief Converts an bgr image to a grayscale one.
/// @param bgrIm Bgr image to convert.
/// @return Grayscale image.
static cv::Mat BgrToGray(cv::Mat& bgrIm)
{
    var grayIm = cv::Mat(bgrIm.rows, bgrIm.cols, CV_8UC1, cv::Scalar(0));

    for (var r = 0 ; r < bgrIm.rows; r++)
    {
        for (var c = 0; c < bgrIm.cols; c++)
        { 
            var& bgr = bgrIm.at<cv::Vec3b>(r, c);
            grayIm.at<byte>(r, c) = (0.114f * bgr.val[0] + 0.587f * bgr.val[1] + 0.299f * bgr.val[2]);
        }
    }

    return grayIm;
}

/// @brief Draws detections onto a provided image.
/// @param detections Detections to draw.
/// @param image Bgr image.
static void DrawDetections(List<Detection>& detections, cv::Mat& image)
{
    for (var& d: detections)
    {
        var r = d.Scale / 2;
        var cX = d.Col + r;
        var cY = d.Row + r;

        cv::circle(image, cv::Point2d(cX, cY), r, cv::Scalar(0, 0, 255), 5);
    }
}

/// @brief Gets a camera capture API depending on a Windows / other OS.
/// @return Camera capture API.
static int GetCameraCaptureAPI()
{
    #if defined(_MSC_VER)
        return cv::CAP_DSHOW;
    #else
        return 0;
    #endif
}

/// @brief Detects objects in a video stream.
/// @param cap Video stream.
/// @param flipFrame True to flip frame, false otherwise.
static void DetectObjectsVideo(cv::VideoCapture& cap, bool flipFrame = false)
{
    if (cap.isOpened() == false)
        throw Exception((string)"Error opening video stream or file.");

    var cascade = Cascade::FromFile(CASCADE_FILE_NAME);
    cv::Mat frame;
    cap >> frame;

    cv::namedWindow("Frame", cv::WINDOW_AUTOSIZE);

    while (frame.empty() == false)
    {
        if (flipFrame) cv::flip(frame, frame, 1);

        var tic = Stopwatch::TotalMilliseconds();
        {
            var grayIm = BgrToGray(frame);
            var detections = DetectObjects(cascade, grayIm);
            DrawDetections(detections, frame);
        }
        var toc = Stopwatch::TotalMilliseconds();

        var fps = 1000.0f / (toc - tic);
        var txt = (string)"FPS: " + (int)fps;
        cv::putText(frame, cv::String(txt.Ptr()), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);

        cv::imshow("Frame", frame);
        cap >> frame;

        var c = (char)cv::waitKey(5);
        if (c == 27) //ESC
            break;
    }

    cap.release();
    cv::destroyAllWindows();
}

static void DetectObjectsImage(const string& imFile)
{
    var im = cv::imread(cv::String(imFile.Ptr(), imFile.Length()), cv::IMREAD_COLOR);
    if (im.empty())
        throw ArgumentException("Can not open the specified image: " + imFile);

    var cascade = Cascade::FromFile(CASCADE_FILE_NAME);
    cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);

    var grayIm = BgrToGray(im);
    var detections = DetectObjects(cascade, grayIm);
    
    DrawDetections(detections, im);
    cv::imshow("Image", im);

    cv::waitKey();
    cv::destroyAllWindows();
}

/// @brief Runs the app - parses the arguments and runs a detection procedure.
/// @param args Console args.
static void RunApp(List<string>& args)
{
    Thread<>::SubscribeErrorHandler(OutputThreadError);

    if (args.Count() == 0)
    {
        Console::WriteLine((string)"Capture from camera: 0");
        var cap = cv::VideoCapture(0, GetCameraCaptureAPI());
        DetectObjectsVideo(cap, true);
        return;
    }

    if (args.Count() > 1)
    {
        throw NotSupportedException((string)"Invalid number of arguments.");
    }

    var imSource = args[0];
    if (imSource.IsNumber())
    {
        var cameraId = String::ParseInt32(imSource);
        Console::WriteLine((string)"Capture from camera: " + cameraId);
        var cap = cv::VideoCapture(cameraId, GetCameraCaptureAPI());
        DetectObjectsVideo(cap, true);
        return;
    }

    if (File::Exists(imSource) && 
        (imSource.EndsWith(".mp4") || imSource.EndsWith(".webm") || imSource.EndsWith(".avi")))
    {
        Console::WriteLine((string)"Capture from video: " + imSource);
        var cap = cv::VideoCapture(cv::String(imSource.Ptr()));
        DetectObjectsVideo(cap, false);
        return;
    }

    if (File::Exists(imSource) && 
        (imSource.EndsWith(".jpg") || imSource.EndsWith(".jpeg") || imSource.EndsWith(".png") || imSource.EndsWith(".bmp")))
    {
        Console::WriteLine((string)"Image file source: " + imSource);
        DetectObjectsImage(imSource);
        return;
    }

    throw NotSupportedException((string)"The specified source is not supported");
}

int main(int argCount, char* argValues[])
{
    Console::ForegroundColor = ConsoleColor::Green;
    Console::WriteLine((string)"Object detection (Viola Jones) from camera, video or image.");

    Console::ForegroundColor = ConsoleColor::Yellow;
    Console::WriteLine((string)"Argument: camera index, video path or image path. If nothing is provided, camera with index 0 is assumed.");
    Console::WriteLine((string)"\tExample camera: 'Test 0'");
    Console::WriteLine((string)"\tExample video:  'Test video.mp4'");
    Console::WriteLine((string)"\tExample image:  'Test image.jpg'");
    Console::WriteLine();

    Console::ForegroundColor = ConsoleColor::Default;
    Console::WriteLine((string)"Use ESC to stop the application.");
    Console::WriteLine();
    Console::WriteLine();

    try
    {
        var arguments = GetArguments(argCount, argValues);
        RunApp(arguments);
    }
    catch (Exception& ex)
    {
        Console::Error(ex);
        return -1;
    }
    
    return 0;
}