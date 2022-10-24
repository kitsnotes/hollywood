#ifndef WNDMGMT_H
#define WNDMGMT_H

#include <QUuid>
#include <QIcon>

#include <QtWaylandClient/QWaylandClientExtension>
#include "qwayland-plasma-window-management.h"

class PlasmaWindow;
class PlasmaWindowManagement : public QWaylandClientExtensionTemplate<PlasmaWindowManagement>
        , public QtWayland::org_kde_plasma_window_management
{
    Q_OBJECT
public:
    PlasmaWindowManagement();
    QList<PlasmaWindow*> allWindows() { return m_windows; }
    PlasmaWindow *windowByUUID(QUuid &uuid);
Q_SIGNALS:
    void windowCreated(PlasmaWindow *w);
    void activeWindowChanged();
    void showDesktopStateChanged(bool show);
protected:
    void org_kde_plasma_window_management_window_with_uuid(uint32_t id, const QString &uuid) override;
private:
    QList<PlasmaWindow*> m_windows;
    PlasmaWindow* m_lastWindow = nullptr;
};

class PlasmaWindow : public QWaylandClientExtensionTemplate<PlasmaWindow>
        , public QtWayland::org_kde_plasma_window
{
    Q_OBJECT
public:
    QUuid uuid() const;
    QIcon icon() const;
    QString windowTitle() const;
    void toggleMinimize();
Q_SIGNALS:
    void iconChanged();
    void windowTitleChanged();
    void minimizedChanged();
protected:
    friend class PlasmaWindowManagement;
    PlasmaWindow(struct ::org_kde_plasma_window *window, PlasmaWindowManagement *p);
    void org_kde_plasma_window_initial_state() override;
    void org_kde_plasma_window_title_changed(const QString &title) override;
    void org_kde_plasma_window_state_changed(uint32_t flags) override;
    void org_kde_plasma_window_icon_changed() override;
protected:
    QUuid m_uuid;
    QIcon m_icon;
    QString m_wndTitle;

    bool m_active = false;
    bool m_minimized = false;
    bool m_maximized = false;
    bool m_minimizable = false;
    bool m_maximizable = false;

    PlasmaWindowManagement *m_parent = nullptr;
};

#endif // WNDMGMT_H
