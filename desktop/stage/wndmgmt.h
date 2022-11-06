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
    QStringList stackingOrder();
    void removeWindow(PlasmaWindow *w);
Q_SIGNALS:
    void windowCreated(PlasmaWindow *w);
    void showDesktopStateChanged(bool show);
    void stackingOrderChanaged();
protected:
    void org_kde_plasma_window_management_window_with_uuid(uint32_t id, const QString &uuid) override;
    void org_kde_plasma_window_management_stacking_order_uuid_changed(const QString &uuids) override;
private:
    QList<PlasmaWindow*> m_windows;
    PlasmaWindow* m_lastWindow = nullptr;
    QStringList m_stackorder;
};

class PlasmaWindow : public QWaylandClientExtensionTemplate<PlasmaWindow>
        , public QtWayland::org_kde_plasma_window
{
    Q_OBJECT
public:
    void activate();
    QUuid uuid() const;
    QIcon icon() const;
    QString windowTitle() const;
    void toggleMinimize();
    bool minimized() const;
Q_SIGNALS:
    void iconChanged();
    void windowTitleChanged(const QString &title);
    void minimizedChanged();
    void windowClosed();
    void themeIconChanged(const QString &icon);
protected:
    friend class PlasmaWindowManagement;
    PlasmaWindow(struct ::org_kde_plasma_window *window, PlasmaWindowManagement *p);
    void org_kde_plasma_window_initial_state() override;
    void org_kde_plasma_window_title_changed(const QString &title) override;
    void org_kde_plasma_window_state_changed(uint32_t flags) override;
    void org_kde_plasma_window_icon_changed() override;
    void org_kde_plasma_window_themed_icon_name_changed(const QString &name) override;
    void org_kde_plasma_window_unmapped() override;
protected:
    QUuid m_uuid;
    QIcon m_icon;
    QString m_wndTitle;
    QString m_themedicon;

    uint m_state = 0;
    bool m_active = false;
    bool m_minimized = false;
    bool m_maximized = false;
    bool m_minimizable = false;
    bool m_maximizable = false;

    PlasmaWindowManagement *m_parent = nullptr;
};

#endif // WNDMGMT_H
