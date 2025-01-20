#pragma once

#include <System.h>
#include <System.Collections.h>
#include <System.IO.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

namespace ViolaJones
{
    /// @brief Config used for training.
    class TrainConfig
    {   
    public:
        /// @brief Patch width to height ratio.
        float WidthHeightRatio = 1.0f;
        /// @brief Max tree depth for each tree of a cascade.
        int MaxTreeDepth = 5;
        /// @brief Max tree count per stage.
        int MaxTreeCount = 64;
        /// @brief Max FPR rate for a cascade.
        float MaxFPR = 1e-3;
        /// @brief Minimum TPR to retain for each stage.
        List<float> MinTPRs = { 0.980f, 0.990f, 0.995f, 0.995f, 0.997f, 0.997f, 0.997f, 0.997f, 0.997f, 0.997f, 0.997f, 0.997f, 0.997f };
        
        /// @brief Loads (if exists) or creates a config file.
        /// @param dbPath Database path - config file name is predefined.
        /// @param isCreated Is set to true, if a cnfig file does not exist and is just created with default values.
        /// @return Train config.
        static TrainConfig LoadOrCreate(const string& dbPath, bool& isCreated)
        {
            if (Directory::Exists(dbPath) == false)
                throw Exception("Cannot write config file because the specified directory is not found: " + dbPath); 

            var configFile = Path::Combine(dbPath, (string)"trainConfig.txt");  
            if (File::Exists(configFile) == false)
            {
                TrainConfig().ToFile(configFile);

                Console::Warning((string)"New config file is created: " + configFile);
                Console::Warning((string)"Edit the config file if required and re-run the application.");
                isCreated = true;
                return TrainConfig();
            }

            var config = FromFile(configFile);
            isCreated = false;
            return config;
        }

        operator string()
        {
            const int PADDING = 20;
            var str = (string)"";

            var minTPRsStr = (string)"";
            for (var i = 0; i < MinTPRs.Count(); i++)
                minTPRsStr = minTPRsStr + (String(MinTPRs[i], 3) + (string)" ");

            str = str + ((string)"widthHeightRatio:").PadRight(PADDING) + String(WidthHeightRatio, 3) + (string)"\n";
            str = str + ((string)"maxTreeDepth:").PadRight(PADDING)     + MaxTreeDepth                + (string)"\n";
            str = str + ((string)"maxTreeCount:").PadRight(PADDING)     + MaxTreeCount                + (string)"\n";
            str = str + ((string)"maxFPR:").PadRight(PADDING)           + String(MaxFPR, 5)           + (string)"\n";
            str = str + ((string)"minTPRs:").PadRight(PADDING)          + minTPRsStr                  + (string)"\n";

            return str;
        }

    private:
        /// @brief Writes the config to a file.
        /// @param configFile Path to a config file.
        void ToFile(const string& configFile)
        {
            var str = (string)*this;

            var fs = FileStream(configFile, FileMode::WriteOnly);
            fs.Write((byte*)str.Ptr(), str.Length());
            fs.Close();
        }

        /// @brief Loads a config from a file.
        /// @param configFile Config file path.
        /// @return Config.
        static TrainConfig FromFile(const string& configFile)
        {
            var config = TrainConfig();
            var keys = List<string>();
            var values = List<string>();

            //----------read config file
            var fs = FileStream(configFile, FileMode::ReadOnly); 

            while (fs.IsEOF() == false)
            {
                var row = fs.ReadLine();
                if (row.IsEmptyOrWhiteSpace())
                    continue;

                var kv = row<-Split({':'});
                if (kv.Count() != 2)
                {
                    fs.Close();
                    throw Exception("Error reading config file - row: " + row);
                }

                keys.Add(kv[0].TrimStart(' ').TrimEnd(' '));
                values.Add(kv[1].TrimStart(' ').TrimEnd(' '));
            }  

            fs.Close();  

            //----------parse values
            var keyIdx = -1;

            //widthHeightRatio
            keyIdx = keys.FindIndex("widthHeightRatio");
            if (keyIdx != -1)
            {
                var val = String::ParseDouble(values[keyIdx]);
                config.WidthHeightRatio = ValidateValue(val, 0.01, 10.0, "widthHeightRatio");
            }

            //maxTreeDepth
            keyIdx = keys.FindIndex("maxTreeDepth");
            if (keyIdx != -1)
            {
                var val = String::ParseInt32(values[keyIdx]);
                config.MaxTreeDepth = ValidateValue(val, 1, 256, "maxTreeDepth");
            }

            //maxTreeCount
            keyIdx = keys.FindIndex("maxTreeCount");
            if (keyIdx != -1)
            {
                var val = String::ParseInt32(values[keyIdx]);
                config.MaxTreeCount = ValidateValue(val, 1, 256, "maxTreeCount");
            }

            //maxFPR
            keyIdx = keys.FindIndex("maxFPR");
            if (keyIdx != -1)
            {
                var val = String::ParseDouble(values[keyIdx]);
                config.MaxFPR = ValidateValue(val, 0.0, 1.0, "maxFPR");
            }

            //minTPRs
            keyIdx = keys.FindIndex("minTPRs");
            if (keyIdx != -1)
            {
                config.MinTPRs.Clear();
                var tprVals = values[keyIdx]<-Split({' '});

                for (var i = 0; i < tprVals.Count(); i++)
                {
                    var valStr = tprVals[i].TrimStart(' ').TrimEnd(' ');
                    var val = String::ParseDouble(valStr);
                    config.MinTPRs.Add(ValidateValue(val, 0.01, 1.0, "minTPRs"));
                }
            }

            return config;
        }

        TC static T ValidateValue(T val, T min, T max, const string& arg)
        {
            if (val < min || val > max)
                throw ArgumentException("Config file: " + arg + " has an unsupported value: " + val + ". Valid range: (" + min + ", " + max + ").");

            return val;
        }
    };

    //----shared
    /// @brief Cascade file name.
    const static string CASCADE_FILE_NAME = "cascade.bin";

    //----train
    /// @brief Default database path (if not specified by cmd arguments).
    const static string DATABASE_PATH = "database/";
    /// @brief Number of random features to generate while training a single node.
    const int RANDOM_FEATURE_COUNT = 1024;

    //----test
    /// @brief Min scale factor (it multiplies image size) when detecting objects.
    const float MIN_SCALE_FACTOR = 0.1f;
    /// @brief Step (offset) scale multiplied by a current scale when scanning an image during detection.
    const float STEP_SCALE = 0.1f;
    /// @brief Scale increase - multiplies a previous scale. Used during object detection.
    const float SCALE_INCREASE = 1.1f;
}