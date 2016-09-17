//
// cin_to_qt.cpp
//

#include "cin_to_qt.h"
#include <QDebug>

#include <QTimer>
#include <QEventLoop>

inline void do_events(int delay = 1) {
    QEventLoop lp;
    QTimer::singleShot(delay, &lp, &QEventLoop::quit);
    lp.exec();
}

cin_to_qt::cin_to_qt(std::istream &stream, QTextEdit* text_edit)
    : stream(stream),
      lastCursorSeen(-1)
{
    qDebug() << "cin_to_qt::cin_to_qt";

    con_window = text_edit;
    old_buf = stream.rdbuf();
    stream.rdbuf(this);

    //Q_ASSERT(C.movePosition(T::End));
    lastCursorSeen = text_edit->textCursor().position();
}

cin_to_qt::~cin_to_qt()
{
    qDebug() << "cin_to_qt::~cin_to_qt";

    // pass any input left...
    // ...

    stream.rdbuf(old_buf);
}

cin_to_qt::int_type cin_to_qt::underflow()
{
    if (gptr() == NULL || gptr() >= egptr()) {
        int gotted = fgetc_cstream_();
        if (gotted == EOF) {
            return traits_type::eof();
        } else {
            *inputBuffer_ = gotted;
            setg(inputBuffer_, inputBuffer_, inputBuffer_+1);
            return traits_type::to_int_type(*inputBuffer_);
        }
    } else {
        return traits_type::to_int_type(*inputBuffer_);
    }
    return 0;
}

cin_to_qt::int_type cin_to_qt::sync()
{
    qDebug() << "cin_to_qt::sync" << lastCursorSeen;
    return 0;
}

int cin_to_qt::fgetc_cstream_() {
    auto c = con_window->textCursor();

    c.movePosition(c.End);
    lastCursorSeen = c.position();

    for ( ; ; ) {
        c.movePosition(c.End);
        auto p = c.position();
//qDebug() << p << lastCursorSeen << "1 fgetc_cstream_";
        if (p > lastCursorSeen) {
            c.setPosition(lastCursorSeen);
            c.movePosition(c.Right, c.KeepAnchor);
            auto s = c.selectedText();
//qDebug() << s << s.length() << "2 fgetc_cstream_";
            if (s.length() >= 1) {
                lastCursorSeen = c.position();
                auto r = s[0];
                if (r == QChar(0x2029))
                    return '\n';
//qDebug() << r << r.toLatin1() << "3 fgetc_cstream_";
                return r.toLatin1();
            }
        }
        do_events();
    }

    /*
    c.movePosition(QTextCursor::End);
    //auto l = c.position();

    c.setPosition(lastCursorSeen);
    if (c.movePosition(c.Right, c.KeepAnchor)) {
        QChar q = c.selectedText()[0];
        return q.toLatin1();
    }
    return EOF;
    */
}
