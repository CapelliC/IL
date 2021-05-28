#    IL : Prolog interpreter
#    Copyright (C) 1992-2021 - Ing. Capelli Carlo

#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

QT += core
QT -= gui

TARGET = mini_console
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle

INCLUDEPATH += ..

SOURCES += main.cpp \
    ../write.cpp \
    ../unify.cpp \
    ../tracer.cpp \
    ../term.cpp \
    ../sysdata.cpp \
    ../strftime.cpp \
    ../slist.cpp \
    ../scanner.cpp \
    ../reduce.cpp \
    ../query.cpp \
    ../qstack.cpp \
    ../qdata.cpp \
    ../proios.cpp \
    ../parse.cpp \
    ../operator.cpp \
    ../message.cpp \
    ../kstr.cpp \
    ../iafx.cpp \
    ../hasht.cpp \
    ../findall.cpp \
    ../fastree.cpp \
    ../eng.cpp \
    ../defsys.cpp \
    ../dbintlog.cpp \
    ../dbdisp.cpp \
    ../constr.cpp \
    ../clause.cpp \
    ../builtin.cpp \
    ../bttrace.cpp \
    ../btmix.cpp \
    ../btmeta.cpp \
    ../btmath.cpp \
    ../btio.cpp \
    ../btil.cpp \
    ../btexec.cpp \
    ../btdb.cpp \
    ../btcmp.cpp \
    ../btclass.cpp \
    ../btarit.cpp \
    ../btswi0.cpp \
    ../binstr.cpp \
    ../binlib.cpp \
    ../argali.cpp \
    ../actios.cpp

OTHER_FILES += \
    ../README.md

HEADERS += \
    ../vectb.h \
    ../unify.h \
    ../tracer.h \
    ../term.h \
    ../stack.h \
    ../srcpos.h \
    ../slist.h \
    ../scanner.h \
    ../reduce.h \
    ../query.h \
    ../qdata.h \
    ../proios.h \
    ../parsemsg.h \
    ../parse.h \
    ../operator.h \
    ../mycont.h \
    ../message.h \
    ../memstore.h \
    ../membuf.h \
    ../kstr.h \
    ../iafx.h \
    ../hasht.h \
    ../fastree.h \
    ../eng.h \
    ../dlist.h \
    ../defsys.h \
    ../dbintlog.h \
    ../constr.h \
    ../clause.h \
    ../chkrel.h \
    ../builtin.h \
    ../btmix.h \
    ../btil.h \
    ../bterr.h \
    ../binstr.h \
    ../binlib.h \
    ../aritval.h \
    ../argali.h \
    ../actios.h

DISTFILES += \
    ac.il \
    ansidrv.il \
    bordo.il \
    deriv.il \
    eliza.il \
    genAlberi.il \
    hanoi.il \
    hanoiv0.il \
    hanoiv1.il \
    hanoiv2.il \
    kalah.il \
    keybdrv.il \
    linee.il \
    listproc.il \
    lookup.il \
    menu.il \
    mescola.il \
    mousedrv.il \
    msort.il \
    nim.il \
    nla.il \
    play.il \
    qsort.il \
    readwl.il \
    regine.il \
    rwl.il \
    s2pc.il \
    scacch.il \
    sm.il \
    tabsim.il \
    test_dcg.il \
    tgf.il \
    utili.il \
    vectuty.il \
    dcg.il \
    vrmluty.il \
    masmind.il
