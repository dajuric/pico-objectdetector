#pragma once
#include "Definitions.h"
#include "String_.h"

namespace System
{
    class Exception
    {
    public:
        Exception(const string& message)
        {
            this->msg = message;
        }

        operator string()
        {
            return this->msg;
        }

    private:
        string msg;
    };


    class IndexOutOfRangeException : public Exception
    {
    public:
        IndexOutOfRangeException(const string& message = "")
            :Exception("IndexOutOfRangeException: " + message)
        { }
    };


    class NotImplementedException : public Exception
    {
    public:
        NotImplementedException(const string& message = "")
            :Exception("NotImplementedException: " + message)
        { }
    };


    class NotSupportedException : public Exception
    {
    public:
        NotSupportedException(const string& message = "")
            :Exception("NotSupportedException: " + message)
        { }
    };


    class InvalidOperationException : public Exception
    {
    public:
        InvalidOperationException(const string& message = "")
            :Exception("InvalidOperationException: " + message)
        { }
    };

    class ArgumentException : public Exception
    {
    public:
        ArgumentException(const string& message = "")
            :Exception("ArgumentException: " + message)
        { }
    };

    class KeyNotFoundException : public Exception
    {
    public:
        KeyNotFoundException(const string& message = "")
            :Exception("KeyNotFoundException: " + message)
        { }
    };

    class IOException : public Exception
    {
    public:
        IOException(const string& message = "")
            :Exception("IOException: " + message)
        { }
    };
}
