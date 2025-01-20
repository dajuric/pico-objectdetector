#pragma once

#define TC template<typename T>
#define null nullptr
#define var auto
//#define ref
//#define out
#define abstract

typedef signed char sbyte;
typedef unsigned char byte;
typedef signed int Int32;
typedef unsigned int UInt32;

#if defined(_MSC_VER)
//  MSVC 
#define EXPORT __declspec(dllexport)
typedef signed   long long Int64;
typedef unsigned long long UInt64;
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
typedef signed   long Int64;
typedef unsigned long UInt64;
#elif defined(__clang__)
//  CLang
#define EXPORT __attribute__((used))
typedef signed   long Int64;
typedef unsigned long UInt64;
#else
//  default
#define EXPORT
#pragma warning Unknown dynamic link import/export semantics.
typedef signed   long Int64;
typedef unsigned long UInt64;
#endif