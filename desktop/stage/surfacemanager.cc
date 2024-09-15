// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

/* void SurfaceManager::newPlasmaWindow(PlasmaWindow *wnd)
{
    auto btn = new TaskButton(wnd, m_parent);
    btn->setText(wnd->windowTitle());
    btn->setToolTip(wnd->windowTitle());

    m_group->addButton(btn);
    m_windows.append(btn);

    connect(wnd, &PlasmaWindow::windowClosed, this, &SurfaceManager::windowClosed);
    connect(wnd, &PlasmaWindow::minimizedChanged, this, &SurfaceManager::minimizedChanged);
    connect(wnd, &PlasmaWindow::destroyed, btn, &TaskButton::deleteLater);

    connect(btn, &QAbstractButton::pressed, this, &SurfaceManager::buttonClicked);

    emit windowCreated(btn);
} */

