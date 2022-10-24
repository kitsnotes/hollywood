#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#include <QMainWindow>

class QListView;
class QStandardItemModel;
class LSMimeApplications;
class ProgramManager : public QMainWindow
{
    Q_OBJECT
public:
    ProgramManager(QWidget *parent = nullptr);
    ~ProgramManager();
signals:
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void activated(const QModelIndex &idx);
    void refreshPrograms();
private:
    QListView *m_view = nullptr;
    LSMimeApplications *m_mime = nullptr;
    QStandardItemModel *m_model = nullptr;
};

#endif // PROGRAMMANAGER_H
