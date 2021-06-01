#ifndef DEBLOG_H
#define DEBLOG_H

#include <iostream>
using namespace std;

class deblog
{
public:
    deblog(string filepath);
    static void trace(const char *fmt, ...);
private:
    static string filepath;
};

#endif // DEBLOG_H
