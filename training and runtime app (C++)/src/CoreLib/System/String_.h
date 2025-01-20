#pragma once
#include "Definitions.h"
#include "Enumerable.h"
#include "Math_.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

namespace System
{
    class String : public Enumerable<char*, char>
    {
    public:
        String()
            :String((unsigned)1, (long)0)
        { }

        String(const char* _str)
            :String((unsigned)1, (long)0)
        {
            if (_str != null)
            {
                delete[] this->data;
                CopyFromCharArray(_str);
            }
        }

        String(const char val)
            :String((unsigned)(1+1), (long)1)
        {
            this->data[0] = val;
        }

        String(const int val)
            :String((unsigned)255, (long)0)
        {
            sprintf(this->data, "%d", val);
            this->length = (int)strlen(this->data);
        }

        String(const long val)
            :String((unsigned)255, (long)0)
        {
            sprintf(this->data, "%ld", val);
            this->length = (int)strlen(this->data);
        }

        String(const UInt64 val)
            :String((unsigned)255, (long)0)
        {
#if defined(_MSC_VER)
            sprintf(this->data, "%llu", val);
#else
            sprintf(this->data, "%lu", val);
#endif
            this->length = (int)strlen(this->data);
        }

        String(const bool val)
        {
            CopyFromCharArray(val ? "true" : "false");
        }

        String(const float val, int decimals = 7)
            :String((unsigned)255, (long)0)
        {
            char formatBuf[255] = { '\0' };
            sprintf(&formatBuf[strlen(formatBuf)], "%%.");
            sprintf(&formatBuf[strlen(formatBuf)], "%d", decimals);
            sprintf(&formatBuf[strlen(formatBuf)], "f");

            sprintf(this->data, formatBuf, val);
            this->length = (int)strlen(this->data);
        }

        String(const double val, int decimals = 14)
            :String((unsigned)255, (long)0)
        {
            char formatBuf[255] = { '\0' };
            sprintf(&formatBuf[strlen(formatBuf)], "%%.");
            sprintf(&formatBuf[strlen(formatBuf)], "%d", decimals);
            sprintf(&formatBuf[strlen(formatBuf)], "lf");

            sprintf(this->data, formatBuf, val);
            this->length = (int)strlen(this->data);
        }

        String(const String& src)
        {
            CopyFromCharArray(src.data);
        }

        int Length() const
        {
            return this->length; 
        }

        ~String() 
        {
            delete[] this->data;
            this->data = null;
        }

        String& operator = (const String& rhs)
        {
            if (this == &rhs)
                return *this;

            delete[] this->data;
            CopyFromCharArray(rhs.data);
            return *this;
        }

        String& operator = (const char* _str)
        {
            delete[] this->data;
            CopyFromCharArray(_str);
            return *this;
        }

        const char operator [](int index)
        {
            if (index < 0 || index >= this->length)
                return '\0';
                //throw IndexOutOfRangeException("Index out of range.");

            return this->data[index];
        }

        char* begin()
        {
            return this->data;
        }

        char* end()
        {
            return this->data + this->length;
        }

        friend String operator + (const String& strA, const String& strB)
        {
            var len = strlen(strA.data) + strlen(strB.data);
            
            var newStr = String((unsigned)len + 1, (long)len);
            strcpy(newStr.data, strA.data);
            strcat(newStr.data, strB.data);

            return newStr;
        }

        friend String operator * (const String& str, const int repetition)
        {
            var len = str.length * repetition;

            var newStr = String(len + 1, len);
            for (var i = 0; i < len; i += str.length)
                strcpy(&newStr.data[i], str.data);

            return newStr;
        }

        friend bool operator == (const String& strA, const String& strB)
        {
            return strcmp(strA.data, strB.data) == 0;
        }

        friend bool operator != (const String& strA, const String& strB)
        {
            return strcmp(strA.data, strB.data) != 0;
        }

        friend bool operator > (const String& strA, const String& strB)
        {
            return strcmp(strA.data, strB.data) > 0;
        }

        friend bool operator < (const String& strA, const String& strB)
        {
            return strcmp(strA.data, strB.data) < 0;
        }

        char* Ptr() const { return this->data; }

        bool StartsWith(const String& subStr) const
        {
            if (this->length < subStr.length)
                return false;

            return memcmp(this->data, subStr.data, subStr.length) == 0;
        }

        bool EndsWith(const String& subStr) const
        {
            if (this->length < subStr.length)
                return false;

            int offset = this->length - subStr.length;
            return memcmp(&this->data[offset], subStr.data, subStr.length) == 0;
        }

        bool Contains(const String& subStr) const
        {
            return strstr(this->data, subStr.data) != null;
        }

        String PadLeft(int totalLength, char paddingChar = ' ') const
        {
            totalLength = Math::Max(totalLength, (int)this->length);
            var paddedStr = String((unsigned)totalLength + 1, (long)totalLength);    
            var padLen = Math::Max(0, totalLength - (int)this->length);

            for (var i = 0; i < padLen; i++)
                paddedStr.data[i] = paddingChar;

            for (var i = padLen; i < totalLength; i++)
                paddedStr.data[i] = this->data[i - padLen];
            
            return paddedStr;
        }

        String PadRight(int totalLength, char paddingChar = ' ') const
        {
            totalLength = Math::Max(totalLength, (int)this->length);
            var paddedStr = String((unsigned)totalLength + 1, (long)totalLength);     
            var padLen = Math::Max(0, totalLength - (int)this->length);
            
            for (var i = 0; i < this->length; i++)
                paddedStr.data[i] = this->data[i];

            for (var i = this->length; i < totalLength; i++)
                paddedStr.data[i] = paddingChar;
            
            return paddedStr;
        }

        String ToUpper() const
        {
            var upperStr = String(this->length + 1, this->length);

            for (var i = 0; i < this->length; i++)
                upperStr.data[i] = (char)toupper(this->data[i]);

            return upperStr;
        }

        String TrimStart(const char trimChar) const
        {
            if (this->length == 0)
                return *this;

            var startPos = 0;
            while (this->data[startPos] == trimChar)
            {
                if (startPos == this->length)
                    break;

                startPos++;
            }

            var trimmedStr = String(&this->data[startPos]);
            return trimmedStr;
        }

        String TrimEnd(const char trimChar) const
        {
            if (this->length == 0)
                return *this;

            var endPos = this->length - 1;
            while (this->data[endPos] == trimChar)
            {
                if (endPos == 0)
                    break;

                endPos--;
            }

            var trimmedStr = String(endPos + 1 + 1, endPos + 1);
            strncpy(trimmedStr.data, this->data, endPos + 1);

            return trimmedStr;
        }

        bool IsEmptyOrWhiteSpace() const
        {
            for (var i = 0; i < length; i++)
            {
                if (data[i] != ' ' && data[i] != '\0')
                    return false;
            }

            return true;
        }

        int Find(const String& str, int startPos = 0) const
        {
            if (str.Length() == 0)
                return -1;

            for (var i = startPos; i < length; i++)
            {
                if (this->data[i] != str.data[0] || (i + str.Length()) >= this->length)
                    continue;

                var containsStr = true;
                for (var iCh = 0; iCh < str.length; iCh++)
                {
                    if (this->data[i + iCh] != str.data[iCh])
                    {
                        containsStr = false;
                        break;
                    }
                }

                if (containsStr)
                    return i;
            }

            return -1;
        }

        int Find(char chr, int startPos = 0, bool last = false) const
        {
            var pos = -1;

            for (var i = startPos; i < length; i++)
            {
                if (data[i] == chr)
                {
                    pos = i;
                    if (last == false)
                        break;
                }
            }

            return pos;
        }

        String Replace(char oldChar, char newChar) const
        {
            var newStr = *this;
            for (var i = 0; i < newStr.length; i++)
            {
                if (newStr.data[i] == oldChar)
                    newStr.data[i] = newChar;
            }

            return newStr;
        }

        String Replace(const String& oldStr, const String& newStr) const
        {
            var result = *this;

            int startPos = -1;
            while ((startPos = Find(oldStr, startPos + 1)) != -1)
            {             
                for (var i = startPos; i < oldStr.length; i++)
                    result.data[i] = newStr.data[i - startPos];
            }

            return result;
        }

        String Slice(int start, int end = -1) const
        {
            if (end == -1)
                end = this->length - 1;

            if (start < 0 || start >= this->length)
                return String(); //throw IndexOutOfRangeException();

             if (end < 0 || end >= this->length)
                return String(); //throw IndexOutOfRangeException();

            var len = end - start + 1;
            var slicedStr = String((long)len + 1, (long)len);

            for (var i = 0; i < len; i++)
                slicedStr.data[i] = this->data[i + start];

            return slicedStr;
        }

        bool IsNumber() const
        {
            if (this->data[0] == '\0')
                return false;

            var containsDot = false;
            for (var i = 1; i < this->length; i++)
            {
                var isValid = this->data[i] >= '0' && this->data[i] <= '9';
                isValid = isValid || (this->data[i] == '.' && containsDot == false);
                if (!isValid)
                    return false;

                if (this->data[i] == '.') 
                    containsDot = true;
            }

            var firstChar = this->data[0];
            var isFirstCharSign = (firstChar == '-' || firstChar == '+' || firstChar == '.');
            var isFirstCharNum  = (firstChar >= '0' && firstChar <= '9');

            return (isFirstCharSign && this->length > 1) || (isFirstCharNum);
        }

        static int ParseInt32(const String& str)
        {
            var result = atoi(str.Ptr());
            /*if (result == 0 && (str.length > 0 && str.data[0] != '0'))
                throw NotSupportedException("The string is not a number.");*/

            return result;
        }

        static double ParseDouble(const String& str)
        {
            var result = atof(str.Ptr());
            /*if (result == 0.0 && (str.length > 0 && str.data[0] != '0'))
                throw NotSupportedException("The string is not a number.");*/

            return result;
        }

        static String Alphanum()
        {
            var str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
            return str;
        }

        const static String Empty;

    private:
        char* data;
        long length;

        String(unsigned capacity, long length)
        {
            this->data = new char[capacity];
            memset(this->data, 0, capacity);

            this->length = length;
        }

        void CopyFromCharArray(const char* src)
        {
            var len = (long)strlen(src);
            this->data = new char[len + 1];

            strcpy(this->data, src);
            this->data[len] = '\0';

            this->length = len;
        }
    };
}

using string = System::String;