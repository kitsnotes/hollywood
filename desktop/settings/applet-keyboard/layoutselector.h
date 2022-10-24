#ifndef LAYOUTSELECTOR_H
#define LAYOUTSELECTOR_H

#include <QDialog>
#include <QWidget>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>

#include "kbdapplet.h"

class LayoutSelector : public QDialog
{
    Q_OBJECT
public:
    explicit LayoutSelector(QMap< QString, KeyboardLayoutInfo>& knownLayouts, QWidget *parent = nullptr);
    QString selectedLayout();
    QString selectedVariant();
private slots:
    void layoutChanged();
private:
    void setupUi();
    QGridLayout *gridLayout;
    QListWidget *layouts;
    QListWidget *variants;
    QDialogButtonBox *buttonBox;
    QMap< QString, KeyboardLayoutInfo>& knownLayouts_;
};

#endif // LAYOUTSELECTOR_H
