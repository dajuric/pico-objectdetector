#pragma once
#include "System.h"
#include "List.h"

using namespace System::Collections::Generic;

//the idea for "extensions" is taken from: https://stackoverflow.com/questions/5463009/extension-methods-in-c
namespace System
{
    class ToString
    {
    public:
        ToString()
        { }

        ToString& operator-(void) 
        {
            return *this;
        }

        friend const String operator<(Collection<char>& collection, ToString& mthd)
        {
            char* buff = new char[collection.Count() + 1];
            buff[collection.Count()] = '\0';

            for (var i = 0; i < collection.Count(); i++)
                buff[i] = collection[i];

            String str(buff);
            delete[] buff;

            return str;
        }
    };

    class JoinChars
    {
    private:
        List<char>& collection;

    public:
        JoinChars(List<char>& collection)
            :collection(collection) 
        { }

        JoinChars& operator-(void) 
        {
            return *this;
        }

        friend const String operator<(const String& separator, JoinChars& mthd)
        {
            List<char> strBuilder;

            for (var item : mthd.collection)
            {
                strBuilder.Add(item);

                for (var chr : (String&)separator)
                    strBuilder.Add(chr);
            }

            var str = strBuilder<-ToString();
            return str;
        }
    };

    class JoinStrings
    {
    private:
        List<string>& collection;

    public:
        JoinStrings(List<string>& collection)
            :collection(collection)
        { }

        JoinStrings& operator-(void) 
        {
            return *this;
        }

        friend const String operator<(String& separator, JoinStrings& mthd)
        {
            List<char> strBuilder;

            for (var str : mthd.collection)
            {
                for (var chr : str)
                    strBuilder.Add(chr);

                for (var chr : separator)
                    strBuilder.Add(chr);
            }

            var str = strBuilder<-ToString();
            return str;
        }
    };

    class Split
    {
    public:
        Split(const InitializerList<char>& splitChars) 
            :splitChars((InitializerList<char>&)splitChars)
        { }

        Split& operator-(void) 
        {
            return *this;
        }

        friend const List<String> operator<(const String& obj, Split& mthd) 
        {
            List<String> splits;
            List<char> strBuilder;

            var _obj = (string&)obj;
            for (var chr : _obj)
            {
                var isSplit = ContainsChar(mthd.splitChars, chr);
                if (isSplit)
                {
                    splits.Add(strBuilder<-ToString());
                    strBuilder.Clear();
                    continue;
                }

                strBuilder.Add(chr);
            }

            splits.Add(strBuilder<-ToString());
            return splits;
        }

    private:
        InitializerList<char>& splitChars;

        static inline bool ContainsChar(InitializerList<char>& splitChars, char chr)
        {
            for (var splitChr : splitChars)
            {
                if (chr == splitChr)
                    return true;
            }

            return false;
        }
    };

    class AddString
    {
    public:
        AddString(const string& str)
            :str(str)
        { }

        AddString& operator-(void)
        {
            return *this;
        }

        friend const void operator<(List<char>& obj, AddString& mthd)
        {
            char* ptr = mthd.str.Ptr();
            while (*ptr != '\0')
            {
                obj.Add(*ptr);
                ptr++;
            }
        }

    private:
        const string& str;
    };
}