#ifndef LSPROCFSMODEL_H
#define LSPROCFSMODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QRegularExpression>

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/sysinfo.h>

class LSDesktopEntry;
class LSMimeApplications;
class LSProcessItem
{
public:
    enum ProcessState {
        PROC_RUNNING,
        PROC_SLEEPING,
        PROC_SLEEP_UNINTERUPT,
        PROC_ZOMBIE,
        PROC_TRACED,
        PROC_UNKNOWN
    };
    LSProcessItem(uint pid = 0);
protected:
    bool valid = false;
    uid_t pid = 0;
    uid_t ppid= 0;
    uint user= 0;
    qulonglong rss= 0;
    qulonglong vsize= 0;
    qulonglong shmem= 0;
    qulonglong threads= 0;
    qulonglong utime= 0;
    qulonglong stime= 0;
    qulonglong cutime= 0;
    qulonglong cstime= 0;
    qulonglong starttime= 0;
    ProcessState state;
    QString processName;
    QString commandLine;
    QString tty;
    LSDesktopEntry* desktopEntry = nullptr;
    int priority= 0;
    int nice= 0;
    bool process();
    void secondProcess();
    friend class LSProcfsModel;
};

class LSProcfsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ModelMode {
        FlatMode,
        TreeMode
    };

    enum ColumnItem {
        ProcessId,
        ParentProcessId,
        UserId,
        UserName,
        ResidentMemory,
        VirtualMemory,
        SharedMemory,
        ThreadCount,
        CPUTime,
        SharedCPUTime,
        ProcessState,
        ProcessName,
        FullCommandLine,
        Terminal,
        Priority,
        NiceValue
    };

    explicit LSProcfsModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant valueForIndex(const ColumnItem item, const QModelIndex &index) const;
    pid_t pidForIndex(const QModelIndex &index);
    QString desktopEntryIconForIndex(const QModelIndex &index);
public slots:
    void reload();
private slots:
private:
    LSProcessItem *itemForIndex(const QModelIndex &index) const;
signals:
    void polled();
private:
    void clearStale();
    void setupTimer();
    QString cpuPercent(LSProcessItem* item) const;
    void poll();
    void sortItems();
    void createDefaultColumnOrder();
    QString formatMemory(LSProcessItem *item) const;
    QString formatState(LSProcessItem::ProcessState state) const;
    QString username(uid_t uid) const;
    LSProcessItem m_root;
    bool m_initial = false;
    QTimer *m_timer = nullptr;
    ModelMode m_mode = FlatMode;
    ColumnItem m_sort = ProcessId;
    QList<LSProcessItem*> m_flat;
    QList<pid_t> m_flatpids;
    QHash<uint,LSProcessItem*> m_byid;
    QList<ColumnItem> m_visibleColumnOrder;
    QMap<uid_t,QString> m_usercache;
    ulong m_clockticks;
    uint m_pagesize;
    bool m_pollprocess = true;
    LSMimeApplications *m_mime = nullptr;
    friend class LSProcessItem;
};



#endif // LSPROCFSMODEL_H
