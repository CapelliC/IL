//
// cout_to_qt.cpp
//

#include "cout_to_qt.h"
#include <QDebug>

cout_to_qt::cout_to_qt(std::ostream &stream, QTextEdit* text_edit) : m_stream(stream)
{
    qDebug() << "cout_to_qt::cout_to_qt";

    log_window = text_edit;
    m_old_buf = stream.rdbuf();
    stream.rdbuf(this);
}
cout_to_qt::~cout_to_qt()
{
    qDebug() << "cout_to_qt::~cout_to_qt";

    // output anything that is left
    if (!m_string.empty())
        log_window->append(m_string.c_str());

    m_stream.rdbuf(m_old_buf);
}

cout_to_qt::int_type cout_to_qt::overflow(int_type v)
{
    qDebug() << "cout_to_qt::overflow" << v;
    /*
    if (v == '\n')
    {
        log_window->append(m_string.c_str());
        m_string.erase(m_string.begin(), m_string.end());
    }
    else
        m_string += v;
    */
    log_window->append(QString(QChar(v)));

    auto c = log_window->textCursor();
    c.movePosition(c.End);
    log_window->setTextCursor(c);

    return v;
}

std::streamsize cout_to_qt::xsputn(const char *p, std::streamsize n)
{
    qDebug() << "cout_to_qt::xsputn" << QString(p);

    m_string.append(p, p + n);
    /*
    size_t pos = 0;
    while (pos != std::string::npos)
    {
        pos = m_string.find('\n');
        if (pos != std::string::npos)
        {
            std::string tmp(m_string.begin(), m_string.begin() + pos);
            log_window->append(tmp.c_str());
            m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
        }
    }
    */
    log_window->append(QString::fromStdString(m_string));
    m_string.clear();

    auto c = log_window->textCursor();
    c.movePosition(c.End);
    log_window->setTextCursor(c);

    return n;
}
