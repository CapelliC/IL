//
// cout_to_qt.cpp
//

#include "cout_to_qt.h"
#include <QDebug>

cout_to_qt::cout_to_qt(std::ostream &stream, QTextEdit* text_edit) : m_stream(stream)
{
    log_window = text_edit;
    m_old_buf = stream.rdbuf();
    stream.rdbuf(this);
}

cout_to_qt::~cout_to_qt()
{
    m_stream.rdbuf(m_old_buf);
}

cout_to_qt::int_type cout_to_qt::overflow(int_type v)
{
    auto c = log_window->textCursor();
    c.movePosition(c.End);
    c.insertText(QString(QChar(v)));
    c.movePosition(c.End);
    log_window->setTextCursor(c);
    return v;
}

std::streamsize cout_to_qt::xsputn(const char *p, std::streamsize n)
{
    std::string s(p, p + n);
    auto c = log_window->textCursor();
    c.movePosition(c.End);
    c.insertText(QString::fromStdString(s));
    c.movePosition(c.End);
    log_window->setTextCursor(c);
    return n;
}
