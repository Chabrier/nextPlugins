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



#include <QFileInfo>
#include <QDir>
#include <QtXml/QDomNode>

#include <vle/utils/Template.hpp>
#include "vlevmvd.h"

namespace vle {
namespace gvle {

vleDomVmVD::vleDomVmVD(QDomDocument* doc): vleDomObject(doc)

{

}

vleDomVmVD::~vleDomVmVD()
{

}

QString
vleDomVmVD::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    if (name == "dataPlugin") {
        return getXQuery(node.parentNode())+"/"+name;
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomVmVD::getNodeFromXQuery(const QString& query,
        QDomNode d)
{
    //handle last
    if (query.isEmpty()) {
        return d;
    }

    QStringList queryList = query.split("/");
    QString curr = "";
    QString rest = "";
    int j=0;
    if (queryList.at(0) == ".") {
        curr = queryList.at(1);
        j=2;
    } else {
        curr = queryList.at(0);
        j=1;
    }
    for (int i=j; i<queryList.size(); i++) {
        rest = rest + queryList.at(i);
        if (i < queryList.size()-1) {
            rest = rest + "/";
        }
    }
    //handle first
    if (d.isNull()) {
        QDomNode rootNode = mDoc->documentElement();
        if (curr != "vle_project_metadata" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return(getNodeFromXQuery(rest,rootNode));
    }

    //handle recursion with uniq node
    if ((curr == "dataPlugin")){
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
    }
    return QDomNode();
}

/************************************************************************/

vleVmVD::vleVmVD(const QString& srcpath, const QString& ompath,
        QString pluginName):
    mDocVm(0), mFileNameSrc(srcpath), mFileNameVm(ompath), mVdoVm(0),
    undoStackVm(0), waitUndoRedoVm(false),
    oldValVm(), newValVm(), mpluginName(pluginName)
{
    QFile file(mFileNameVm);
    if (file.exists()) {
        mDocVm = new QDomDocument("vle_project_metadata");
        QXmlInputSource source(&file);
        QXmlSimpleReader reader;
        mDocVm->setContent(&source, &reader);
    } else {
        xCreateDom();
    }

    mVdoVm = new vleDomVmVD(mDocVm);
    undoStackVm = new vleDomDiffStack(mVdoVm);
    undoStackVm->init(*mDocVm);

    QObject::connect(undoStackVm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoVm(QDomNode, QDomNode)));
    QObject::connect(undoStackVm,
                SIGNAL(undoAvailable(bool)),
                this, SLOT(onUndoAvailable(bool)));
}

QString
vleVmVD::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}

void
vleVmVD::xCreateDom()
{
    if (not mDocVm) {
        mDocVm = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDocVm->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDocVm->appendChild(pi);

        QDomElement vpmRoot = mDocVm->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "meto");
        QDomElement xCondPlug = mDocVm->createElement("vpzPlugin");
        xCondPlug.setAttribute("name", mpluginName);
        vpmRoot.appendChild(xCondPlug);
        mDocVm->appendChild(vpmRoot);
    }
}

void
vleVmVD::xReadDom()
{
    if (mDocVm) {
        QFile file(mFileNameVm);
        mDocVm->setContent(&file);
    }
}

QString
vleVmVD::getDataPlugin()
{
    QDomElement docElem = mDocVm->documentElement();

    QDomNode dataPluginNode =
        mDocVm->elementsByTagName("dataPlugin").item(0);
    return dataPluginNode.attributes().namedItem("name").nodeValue();
}

void
vleVmVD::setCurrentTab(QString tabName)
{
    undoStackVm->current_source = tabName;
}

void
vleVmVD::save()
{
    QFile file(mFileNameVm);
    QFileInfo fileInfo(file);
    if (not fileInfo.dir().exists()) {
        if (not QDir().mkpath(fileInfo.dir().path())) {
            return;
        }
    }
    if (not file.exists()) {
        if (not file.open(QIODevice::WriteOnly)) {
            return;
        }
        file.close();
    }
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QByteArray xml = mDocVm->toByteArray();
    file.write(xml);
    file.close();

    // dummy modification to make sur a UndoAivlable
    // is raised on the clear to be removed when
    // meta-data is going to be coherent with data
    QDomNode rootNode = mDocVm->documentElement();
    undoStackVm->snapshot(rootNode);

    undoStackVm->clear();
}

void
vleVmVD::undo()
{
    waitUndoRedoVm = true;
    undoStackVm->undo();
    emit modified();

}

void
vleVmVD::redo()
{
    waitUndoRedoVm = true;
    undoStackVm->redo();
    emit modified();
}

void
vleVmVD::onUndoRedoVm(QDomNode oldVm, QDomNode newVm)
{
    oldValVm = oldVm;
    newValVm = newVm;
    waitUndoRedoVm = false;
    emit undoRedo(oldValVm, newValVm);
}

void
vleVmVD::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}


}}//namespaces
