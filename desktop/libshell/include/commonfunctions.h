// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QObject>
#include <QUndoStack>
#include <QMessageBox>

class OperationManager;
class LSUDisks;
class LSCommonFunctionsPrivate;
class LSCommonFunctions : public QObject
{
    Q_OBJECT
public:
    static LSCommonFunctions* instance();
    bool executeDesktopOverDBus(const QString &desktop);
    bool openFileOverDBusWithDefault(const QString &file);
    void showGetInfoDialog(const QUrl &target, QWidget *parent = nullptr);
    void showErrorDialog(const QString &heading, const QString &msg,
                         QWidget *parent = nullptr,
                         const QMessageBox::Icon icon = QMessageBox::Critical);

    QUrl newFolder(const QUrl &target, QWidget *parent = nullptr);
    OperationManager* operationManager();
    QUndoStack* undoStack();
    LSUDisks* udiskManager();
signals:
    void pasteAvailable(bool enable, uint count);
private:
    explicit LSCommonFunctions();
    LSCommonFunctionsPrivate *p = nullptr;
};
