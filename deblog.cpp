#include "deblog.h"
#include <fstream>
#include <cstdarg>
#include <iomanip>
#include <ctime>

string deblog::filepath;

static void a_line(ofstream &f, string s) {
    time_t t = time(nullptr);
    tm tm = *localtime(&t);
    f << put_time(&tm, "%T: ") << s << endl;
}

deblog::deblog(string filepath_) {
    filepath = filepath_;
    ofstream f(filepath);
    a_line(f, "deblog to " + filepath);
}

void deblog::trace(const char *fmt, ...) {
    if (!filepath.empty()) {
        char buffer[1 << 12];
        va_list a;
        va_start(a, fmt);
        vsnprintf(buffer, sizeof buffer, fmt, a);
        ofstream s(filepath, ios_base::out|ios_base::app);
        a_line(s, buffer);
    }
}
