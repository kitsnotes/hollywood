/*
 * horizonwizardpage.cc - Implementation of our custom QWizardPage subclass
 * horizon-qt5, the Qt 5 user interface for
 * Project Horizon
 *
 * Copyright (c) 2019 Adélie Linux and contributors.  All rights reserved.
 * This code is licensed under the AGPL 3.0 license, as noted in the
 * LICENSE-code file in the root directory of this repository.
 *
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#include "horizonwizardpage.hh"
#include <QtSvg/QSvgRenderer>

#include <QPainter>
#include <QPair>
#include <QApplication>

using std::string;

QPixmap HorizonWizardPage::loadDPIAwarePixmap(string pixmap, string type) {
    string path = ":/wizard_pixmaps/resources/";
    path += pixmap;
    path += "-";

    if(window()->devicePixelRatioF() <= 1.0) {
        path += "low";
    } else {
        path += "high";
    }

    path += type;
    return QPixmap(path.c_str());
}

void HorizonWizardPage::loadWatermark(string page)
{
    QPalette p = qApp->palette();
    auto tc = p.color(QPalette::Active, QPalette::WindowText);
    auto bc = p.color(QPalette::Active, QPalette::Window);

    bool dark = false;
    if(tc.name() == "#ffffff")
    {
        qDebug() << "dark mode";
        dark = true;
    }
    QSvgRenderer renderer(this);
    if(!dark)
        renderer.load(QString(":/AssistantBackground"));
    else
        renderer.load(QString(":/AssistantBackgroundDark"));

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    QSvgRenderer logo(QString(":/HollywoodLogo"), this);
    logo.setAspectRatioMode(Qt::KeepAspectRatio);

    QPixmap pm(700,500);
    pm.fill(bc);
    QPainter painter(&pm);
    painter.setOpacity(0.8);
    painter.setBrush(QBrush(bc));
    renderer.render(&painter, pm.rect());
    painter.setBrush(QBrush(tc));
    int startx = 25;
    int starty = 35;
    int img = painter.fontMetrics().height();

    int lw = 120;
    int lh = 60;
    QRect lr(startx,starty,lw,lh);
    logo.render(&painter, lr);

    starty += lh + 10;

    QList<QPair<QString,QString>> pages;
    pages << QPair<QString,QString>("intro", tr("Welcome"));
    pages << QPair<QString,QString>("keyboard", tr("Keyboard"));
    pages << QPair<QString,QString>("disk", tr("Disk"));
    pages << QPair<QString,QString>("datetime", tr("Date & Time"));
    pages << QPair<QString,QString>("acct", tr("User Account"));
    pages << QPair<QString,QString>("commit", tr("Install"));

    QIcon completed(":/wizard_pixmaps/resources/status-success-high.svg");
    QIcon current(":/wizard_pixmaps/resources/status-current-high.svg");
    QStringList levels;
    for(auto i : pages)
        levels.append(i.second);

    QString currentLevel;
    for(auto i : pages)
    {
        if(i.first == QString::fromStdString(page))
            currentLevel = i.second;
    }

    painter.setBrush(QBrush(tc));
    painter.setBackground(QBrush(tc));
    int levelInt = levels.indexOf(currentLevel);
    for(int i = 0; i < levels.count(); ++i)
    {
        if(levelInt == i)
        {
            painter.drawPixmap(QPoint(startx,starty),
                               current.pixmap(QSize(img,img)));
            QFont font = painter.font();
            font.setBold(true);
            painter.setFont(font);
        }
        else
        {
            if(i < levelInt)
            {
                painter.drawPixmap(QPoint(startx,starty),
                                   completed.pixmap(QSize(img,img)));
            }
            QFont font = painter.font();
            font.setBold(false);
            painter.setFont(font);
        }

        painter.setPen(tc);
        painter.drawText(QPoint(startx+img+5,starty+15), levels[i]);
        starty += painter.fontMetrics().height() + 5;
    }

    setPixmap(QWizard::BackgroundPixmap, pm);
}

#ifndef IN_RUNNER
HorizonWizard *HorizonWizardPage::horizonWizard() const {
    return dynamic_cast<HorizonWizard *>(this->wizard());
}
#endif  /* !IN_RUNNER */
