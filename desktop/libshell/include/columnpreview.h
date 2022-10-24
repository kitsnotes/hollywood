#ifndef LSCOLUMNPREVIEWWIDGET_H
#define LSCOLUMNPREVIEWWIDGET_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "libshell_int.h"

class LSColumnPreview : public QWidget
{
    Q_OBJECT
public:
    explicit LSColumnPreview(QWidget *parent = nullptr);
    void updateFileInfo(QFileInfo info);
signals:
private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_previewLayout;
    QSpacerItem *m_previewSpacerLeft;
    QGraphicsView *m_preview;
    QSpacerItem *m_previewSpacerRight;
    QSpacerItem *m_underPreviewSpacer;
    QLabel *m_fileTitle;
    QHBoxLayout *m_fileSubTitleLayout;
    QSpacerItem *m_fileSubTitleSpacer;
    QLabel *m_fileSubtitle;
    QTableWidget *m_fileProperties;
    QSpacerItem *m_endSpacer;
};

#endif // LSCOLUMNPREVIEWWIDGET_H
