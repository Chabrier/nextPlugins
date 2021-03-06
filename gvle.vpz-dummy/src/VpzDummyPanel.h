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

#ifndef GVLE_DATA_DUMMY_PANEL_H
#define GVLE_DATA_DUMMY_PANEL_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <vle/gvle/vlevpm.h>
#include <vle/gvle/vlevpz.h>
#include <vle/gvle/plugin_mainpanel.h>

//#include "VpzDummyLeftWidget.h"
//#include "VpzDummyRightWidget.h"
#include "vlevmvd.h"


namespace vle {
namespace gvle {

class VleTextEdit;

class VpzDummyPanel : public PluginMainPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel")
    Q_INTERFACES(vle::gvle::PluginMainPanel)

public:
    VpzDummyPanel();
    virtual ~VpzDummyPanel();
    QString  getname();
    QWidget* leftWidget();
    QWidget* rightWidget();
    void undo();
    void redo();
    void init(QString& file, utils::Package* pkg, Logger* ,gvle_plugins*);
    QString canBeClosed();
    void save();
    void discard(){};
    PluginMainPanel* newInstance();

public slots:
    //for both left and right
    void onUndoAvailable(bool);
    void onUndoRedoSm(QDomNode oldValVm, QDomNode newValVm);

private:

public:
    QTextEdit*  m_edit;
    QString     m_file;
    vleVmVD*    vpzMetadata;
    vleVpz*     vpzData;
};

}} //namespaces


#endif
