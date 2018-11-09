#
#    IL : Intlog Language
#    Object Oriented Prolog Project
#    Copyright (C) 1992-2016 - Ing. Capelli Carlo
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#

QT += core gui widgets
CONFIG += c++11

TARGET = iostreams_qt
TEMPLATE = app

INCLUDEPATH += ..

SOURCES += \
    main.cpp \
    main_window.cpp \
    cin_to_qt.cpp \
    cout_to_qt.cpp \
    iostream_widget.cpp \
    ../write.cpp \
    ../waittask.cpp \
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
    ../binstr.cpp \
    ../binlib.cpp \
    ../argali.cpp \
    ../actios.cpp \
    ../btswi0.cpp

OTHER_FILES += \
    ../README.md

HEADERS += \
    cin_to_qt.h \
    cout_to_qt.h \
    main_window.h \
    iostream_widget.h \
    ../xtrace.h \
    ../vectb.h \
    ../unify.h \
    ../tracer.h \
    ../term.h \
    ../syntaxcolor.h \
    ../stdafx.h \
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
    ../actios.h \
    ../btswi0.h

DISTFILES += \
    README.md \
    ../scripts/ISO/isoTestSuite.pl \
    ../scripts/ac.il \
    ../scripts/ansidrv.il \
    ../scripts/between.il \
    ../scripts/bordo.il \
    ../scripts/deriv.il \
    ../scripts/genAlberi.il \
    ../scripts/hanoi.il \
    ../scripts/hanoiv0.il \
    ../scripts/keybdrv.il \
    ../scripts/linee.il \
    ../scripts/listproc.il \
    ../scripts/lookup.il \
    ../scripts/menu.il \
    ../scripts/mousedrv.il \
    ../scripts/msort.il \
    ../scripts/nla.il \
    ../scripts/qsort.il \
    ../scripts/readwl.il \
    ../scripts/regine.il \
    ../scripts/s2pc.il \
    ../scripts/scacch.il \
    ../scripts/sm.il \
    ../scripts/test_dcg.il \
    ../scripts/tgf.il \
    ../scripts/utili.il \
    ../scripts/vectuty.il \
    ../scripts/vrmluty.il \
    ../scripts/dcg.il \
    ../scripts/eliza.il \
    ../scripts/hanoiv1.il \
    ../scripts/hanoiv2.il \
    ../scripts/kalah.il \
    ../scripts/masmind.il \
    ../scripts/mescola.il \
    ../scripts/nim.il \
    ../scripts/play.il \
    ../scripts/rwl.il \
    ../scripts/tabsim.il \
    ../scripts/length.il
