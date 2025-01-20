#define PARALLEL 1 //execute training procedure in parallel where applicable

#include "Train.hpp"
#include <Extensions/ConsoleExtensions.h>

using namespace System;
using namespace ViolaJones;

/// @brief Outputs an error produced inside a thread - not caught in a main exception block.
/// @param ex An occurred exception.
static void OutputThreadError(Exception& ex)
{
    Console::Error((string)"Unexpected thread exception.");
    Console::Error(ex);
}

/// @brief Runs the app - parses the arguments and runs a training procedure.
/// @param args Console args.
static void RunApp(List<string>& args)
{
    Thread<>::SubscribeErrorHandler(OutputThreadError);

    if (args.Count() > 1)
        throw Exception((string)"Invalid number of arguments.");

    //db path
    var dbPath = (args.Count() == 1) ? args[0]: DATABASE_PATH;
    
    //config & cascade load
    bool isConfigCreated;
    var config = TrainConfig::LoadOrCreate(dbPath, isConfigCreated);
    if (isConfigCreated) //if created with default values give a user to change values
        return;

    var cascadeFile = Path::Combine(dbPath, CASCADE_FILE_NAME);
    var cascade = Cascade::LoadOrCreate(cascadeFile, config);

    Console::WriteLine((string)"---- Used config:");
    Console::Write(config);
    Console::WriteLine();

    //data init and loading
    Console::WriteLine((string)"Dataset:");
    var transform = RoiRandomJitterTransform();
    var baseSet   = LabeledDataset(dbPath, cascade.WidthHeightRatio);

    var posSet = PositiveDataset(baseSet, &transform);
    var negSet = NegativeDataset(baseSet);

    //training
    for (var i = cascade.StageCount(); i < config.MinTPRs.Count(); i++)
    {
        Console::WriteLine();
        Console::Warning((string)"------- Stage: "  + (i + 1) + " -------");

        var minTPR = config.MinTPRs[i];
        var isStageAppended = TryAppendStage(cascade, posSet, negSet, minTPR, 0.5f, config.MaxFPR, config.MaxTreeCount);
        if (isStageAppended == false)
            break;

        cascade.ToFile(cascadeFile);
    }

    Console::WriteLine((string)"Done!");
}

int main(int argCount, char* argValues[])
{
    Console::ForegroundColor = ConsoleColor::Green;
    Console::WriteLine((string)"Object detection (Viola Jones) training.");

    Console::ForegroundColor = ConsoleColor::Yellow;
    Console::WriteLine((string)"Argument: [database path] = 'database/'");
    Console::WriteLine((string)"\tExample: 'Train database/'");
    Console::WriteLine();

    Console::ForegroundColor = ConsoleColor::Default;
    Console::WriteLine((string)"Database folder must contain image files and their corresponding labels file (image name with '.txt' ext).");
    Console::WriteLine((string)"Labels file is in YOLOv3 format. Negative images will omit labels file.");
    Console::WriteLine((string)"Cascade will be written in '<databasePath>/cascade.bin' file (and read if exists).");
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