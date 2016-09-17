//
// cout_to_qt.h
// modified hint from Jochen Ulrich: see
//  http://www.archivum.info/qt-interest@trolltech.com/2005-06/00040/Redirecting-std-cout-to-QTextEdit--Using-QTextEdit-as-a-Log-Window.html
//

#ifndef COUT_TO_QT_H
#define COUT_TO_QT_H

#include <streambuf>
#include <iostream>
#include <string>

#include <QTextEdit>

class cout_to_qt : public std::basic_streambuf<char>
{
public:
    cout_to_qt(std::ostream &stream, QTextEdit* text_edit);
    ~cout_to_qt();

protected:
    virtual int_type overflow(int_type v);
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    QTextEdit* log_window;
};

#endif
