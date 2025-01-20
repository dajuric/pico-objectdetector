#pragma once
#include "System.h"
#include "List.h"

using namespace System::Collections::Generic;

namespace System
{
    static List<string> GetArguments(int argCount, char* argValues[], int skipFirst = true)
    {
        List<string> arguments;

        for (var i = skipFirst ? 1: 0; i < argCount; i++)
        {
            var arg = String(argValues[i]);
            arguments.Add(arg);
        }

        return arguments;
    }
}