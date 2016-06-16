/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMenu>
#include <vle/gvle/gvle_widgets.h>

#include "VpzDummyPanel.h"
//#include "ui_leftWidget.h"
//#include "ui_rightWidget.h"

namespace vle {
namespace gvle {

VpzDummyPanel::VpzDummyPanel():
    PluginMainPanel(),  m_edit(0), m_file(""), vpzMetadata(0)
{
    m_edit = new QTextEdit();
    QObject::connect(m_edit, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));
}

VpzDummyPanel::~VpzDummyPanel()
{
    //delete right;
    //delete left;
}

QString
VpzDummyPanel::getname()
{
    return "vpz-dummy";
}

QWidget*
VpzDummyPanel::leftWidget()
{
    return m_edit;
}

QWidget*
VpzDummyPanel::rightWidget()
{
    return 0;
}

void
VpzDummyPanel::undo()
{
    vpzMetadata->undo();
    //reload();
}
void
VpzDummyPanel::redo()
{
    vpzMetadata->redo();
    //reload();
}

void
VpzDummyPanel::init(QString& relPath, utils::Package* pkg, Logger* ,
        gvle_plugins*)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    QString vpzpath = basepath + "/" + relPath;
    QString vmpath = basepath + "/metadata/" + relPath;
    vmpath.replace(".vpz",".vm");

    vpzMetadata = new vleVmVD(vpzpath, vmpath, getname());
    vpzData = new vleVpz(vpzpath);

    QObject::connect(vpzMetadata, SIGNAL(undoAvailable(bool)),
                     this, SLOT (onUndoAvailable(bool)));
    vpzMetadata->save();

    m_file = basepath+"/"+relPath;
    QFile dataFile (m_file);

    if (!dataFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Error DefaultDataPanel::init ";
    }

    QTextStream in(&dataFile);
    m_edit->setText(in.readAll());

}

QString
VpzDummyPanel::canBeClosed()
{
    return "";
}

void
VpzDummyPanel::save()
{
    vpzMetadata->save();


    if (m_file != "") {
        QFile file(m_file);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            file.write(m_edit->toPlainText().toStdString().c_str()) ;
            file.flush();
            file.close();
        }
    }

    // just to test if we can do something
    // stupid and not synchronized with the editor
    // But does work.
    vpzData->setAuthor("Le concombre Masqué");

    vpzData->save();

}

PluginMainPanel*
VpzDummyPanel::newInstance()
{
    return new VpzDummyPanel;
}

void
VpzDummyPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}} //namespaces
