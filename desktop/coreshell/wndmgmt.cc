#include "wndmgmt.h"

#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <qplatformdefs.h>
#include <cerrno>
#include <QtWaylandClient>

#define PWM_PROTO_VERSION       15
#define PW_PROTO_VERSION        16

PlasmaWindowManagement::PlasmaWindowManagement()
    : QWaylandClientExtensionTemplate(PWM_PROTO_VERSION) {}

PlasmaWindow *PlasmaWindowManagement::windowByUUID(QUuid &uuid)
{
    for(auto w : m_windows)
    {
        if(w->uuid() == uuid)
            return w;
    }
    return nullptr;
}

void PlasmaWindowManagement::org_kde_plasma_window_management_window_with_uuid(uint32_t id, const QString &uuid)
{
    Q_UNUSED(id);
    auto wl = get_window_by_uuid(uuid);
    auto pm = new PlasmaWindow(wl, this);
    pm->m_uuid = QUuid::fromString(uuid);
    m_windows.append(pm);
    Q_EMIT windowCreated(pm);
}

PlasmaWindow::PlasmaWindow(struct ::org_kde_plasma_window *window, PlasmaWindowManagement *p)
    : QWaylandClientExtensionTemplate(PW_PROTO_VERSION)
    , QtWayland::org_kde_plasma_window(window)
    ,  m_parent(p)
{
}

void PlasmaWindow::org_kde_plasma_window_initial_state()
{
    qDebug() << "org_kde_plasma_window_initial_state";
}

QUuid PlasmaWindow::uuid() const
{
    return m_uuid;
}

QIcon PlasmaWindow::icon() const
{
    return m_icon;
}

QString PlasmaWindow::windowTitle() const
{
    return m_wndTitle;
}

void PlasmaWindow::toggleMinimize()
{

}

void PlasmaWindow::org_kde_plasma_window_title_changed(const QString &title)
{
    m_wndTitle = title;
    emit windowTitleChanged();
}

void PlasmaWindow::org_kde_plasma_window_state_changed(uint32_t flags)
{
    if(flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZED)
    {
        m_minimized = true;
        emit minimizedChanged();
    }
}

static int readData(int fd, QByteArray &data)
{
    // this function borrowed from KWayland
    // implementation based on QtWayland file qwaylanddataoffer.cpp
    char buf[4096];
    int retryCount = 0;
    int n;
    while (true) {
        n = QT_READ(fd, buf, sizeof buf);
        if (n == -1 && (errno == EAGAIN) && ++retryCount < 1000) {
            usleep(1000);
        } else {
            break;
        }
    }
    if (n > 0) {
        data.append(buf, n);
        n = readData(fd, data);
    }
    return n;
}

void PlasmaWindow::org_kde_plasma_window_icon_changed()
{
    // this function borrowed from KWayland
    int pipeFds[2];
    if (pipe2(pipeFds, O_CLOEXEC | O_NONBLOCK) != 0)
        return;

    get_icon(pipeFds[1]);
    ::close(pipeFds[1]);
    const int pipeFd = pipeFds[0];
    auto readIcon = [pipeFd]() -> QIcon {
        QByteArray content;
        if (readData(pipeFd, content) != 0) {
            ::close(pipeFd);
            return QIcon();
        }
        ::close(pipeFd);
        QDataStream ds(content);
        QIcon icon;
        ds >> icon;
        return icon;
    };
    QFutureWatcher<QIcon> *watcher = new QFutureWatcher<QIcon>(this);
    QObject::connect(watcher, &QFutureWatcher<QIcon>::finished, this, [this, watcher] {
        watcher->deleteLater();
        QIcon icon = watcher->result();
        if (!icon.isNull()) {
            m_icon = icon;
        } else {
            m_icon = QIcon::fromTheme(QStringLiteral("wayland"));
        }
        qDebug() << "plasma-wm iconChanged()";
        Q_EMIT iconChanged();
    });
    watcher->setFuture(QtConcurrent::run(readIcon));
}

