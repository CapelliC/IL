#ifndef CENTERWIDGETS_H
#define CENTERWIDGETS_H

#include <QScreen>
#include <QWidget>
#include <QGuiApplication>

inline void CenterWidgets(QWidget *widget, QWidget *host = Q_NULLPTR) {
    if (!host)
        host = widget->parentWidget();

    if (host) {
        auto hostRect = host->geometry();
        widget->move(hostRect.center() - widget->rect().center());
    }
    else {
        QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
        int x = (screenGeometry.width() - widget->width()) / 2;
        int y = (screenGeometry.height() - widget->height()) / 2;
        widget->move(x, y);
    }
}

#endif // CENTERWIDGETS_H
