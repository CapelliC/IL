//
// cin_to_qt.h
//

#ifndef CIN_TO_QT_H
#define CIN_TO_QT_H

#include <streambuf>
#include <iostream>
#include <string>

#include <QTextEdit>

class cin_to_qt : public std::basic_streambuf<char>
{
public:
    cin_to_qt(std::istream &stream, QTextEdit* text_edit);
    ~cin_to_qt();

protected:
    virtual int_type underflow();
    virtual int_type sync();

private:
    std::istream &stream;
    std::streambuf *old_buf;
    std::string buffer;
    QTextEdit* con_window;

    int lastCursorSeen;
    char inputBuffer_[1];
    int fgetc_cstream_();
};

#endif
