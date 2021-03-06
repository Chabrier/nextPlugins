/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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
#ifndef GVLE_VLEOMOD_H
#define GVLE_VLEOMOD_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QStaticText>
#include <QStyleOption>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlDefaultHandler>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/gvle/vleDomDiffStack.h>


namespace vle {
namespace gvle {

/**
 * @brief Class that implements vleDomObject especially for vleVm
 */
class vleDomVmVD : public vleDomObject
{
public:
    vleDomVmVD(QDomDocument* doc);
    ~vleDomVmVD();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

class vleVmVD : public QObject
{
    Q_OBJECT
public:
    vleVmVD(const QString& srcpath, const QString& smpath,
            QString pluginName);
    QString getSrcPath() const
    {return mFileNameSrc;};
    QString getVmPath() const
    {return mFileNameVm;};
    void setSrcPath(const QString& name)
    {mFileNameSrc = name;};
    void setVmPath(const QString& name)
    {mFileNameVm = name;};
    QString toQString(const QDomNode& node) const;
    void xCreateDom();
    void xReadDom();
    QString getDataPlugin();

    void setCurrentTab(QString tabName);

    void save();

    void undo();
    void redo();

private:
    QDomNode nodeVariable(const QString& varName);

public slots:
    void onUndoRedoVm(QDomNode oldValVm, QDomNode newValVm);
    void onUndoAvailable(bool);
signals:
    void undoRedo(QDomNode oldValVm, QDomNode newValVm);
    void undoAvailable(bool);

    void modified();


private:


    QDomDocument*    mDocVm;
    QString          mFileNameSrc;
    QString          mFileNameVm;
    vleDomVmVD*      mVdoVm;
    vleDomDiffStack* undoStackVm;
    bool             waitUndoRedoVm;
    QDomNode         oldValVm;
    QDomNode         newValVm;
    QString          mpluginName;
};

}}//namespaces

#endif
