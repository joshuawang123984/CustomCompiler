#pragma once
#include <iostream>
#include <string>

class ErrorReporter
{
public:
    static bool hadError;

    static void error(int line, const std::string &message)
    {
        report(line, "", message);
    }

    static void report(int line, const std::string &where, const std::string &message)
    {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
        hadError = true;
    }
};

inline bool ErrorReporter::hadError = false;