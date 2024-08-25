#pragma once
#include "Definitions.h"
#include "String_.h"

#include <stdio.h>

namespace System
{
    //https://www.codeproject.com/Tips/5255355/How-to-Put-Color-on-Windows-Console
    enum class ConsoleColor
    {
        Red = 31,
        Yellow = 33,
        Green = 92,
        Default = 0
    };

    class Console
    {
    public:
        static const int READ_BUFFER_SIZE = 255;
        inline static ConsoleColor ForegroundColor = ConsoleColor::Default;

        static void Write(const string& str)
        { 
            printf("\033[%dm", ForegroundColor);
            printf("%s", str.Ptr());
            fflush(stdout);
        }

        static void WriteLine(const string& str = "")
        {
            Write(str);
            printf("\r\n");
        }

        static string ReadLine()
        {
            char buff[READ_BUFFER_SIZE];
            fgets(buff, READ_BUFFER_SIZE, stdin);

            return String(buff);
        }

        static void Error(const string& str)
        {
            Console::ForegroundColor = ConsoleColor::Red;
            Console::WriteLine(str);
            Console::ForegroundColor = ConsoleColor::Default;
            Console::Write("");
        }

        static void Warning(const string& str)
        {
            Console::ForegroundColor = ConsoleColor::Yellow;
            Console::WriteLine(str);
            Console::ForegroundColor = ConsoleColor::Default;
            Console::Write("");
        }

        static void Progress(float percentage, const string& message)
        {
            if (percentage == 0)
                Console::WriteLine();

            Console::Write((string)"\r" + message);
            Console::ForegroundColor = (percentage < 1.0f) ? ConsoleColor::Yellow: ConsoleColor::Green;

            var pStr = ((string)(int)Math::Round(percentage * 100)).PadLeft(3, ' ');
            Console::Write((string)" [" + pStr + "%]");
            if (percentage == 1.0f)
                Console::WriteLine();

            Console::ForegroundColor = ConsoleColor::Default;
            Console::Write("");
        }

    };
}