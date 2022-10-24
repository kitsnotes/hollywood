/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2011-2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QIcon>
#include "gui.h"
#include <unistd.h>
#include <QDebug>
#include <QPainter>

PolicykitAgentGUI::PolicykitAgentGUI(const QString &actionId,
                                     const QString &message,
                                     const QString &iconName,
                                     const PolkitQt1::Details &details,
                                     const PolkitQt1::Identity::List &identities)
   : QDialog(nullptr, Qt::WindowStaysOnTopHint)
{
    setupUi();
    setMinimumWidth(480);
    setMinimumHeight(220);
    Q_UNUSED(actionId);
    qDebug() << actionId;
    QStringList deets;
    for(auto d : details.keys())
        deets << QString("%1: %2").arg(d).arg(details.lookup(d));
    m_details->setText(deets.join('\n'));
    m_message->setText(message);
    QIcon icon = QIcon::fromTheme(iconName);
    if (icon.isNull())
        icon = QIcon::fromTheme(QLatin1String("dialog-question"));

    auto iz = 96;
    auto lz = 32;
    // TODO: high dpi

    auto pm = icon.pixmap(iz,iz);
    auto li = QIcon::fromTheme("emblem-locked");
    if(!li.isNull())
    {
        auto pmli = li.pixmap(lz,lz);
        QPainter p(&pm);
        p.drawPixmap(QPoint((iz-lz),(iz-lz)),pmli);
    }
    m_icon->setPixmap(pm);

    const uid_t current_uid = getuid();
    int current_user_index = -1;
    for (const PolkitQt1::Identity& identity : identities)
    {
        const int i = m_identity->count(); // index of the added item
        m_identity->addItem(identity.toString());
        PolkitQt1::UnixUserIdentity const * const u_id = static_cast<const PolkitQt1::UnixUserIdentity *>(&identity);
        if (u_id != nullptr && u_id->uid() == current_uid)
            current_user_index = i;
    }
    if (current_user_index != -1)
        m_identity->setCurrentIndex(current_user_index);

    m_details->setVisible(false);
    connect(m_identity, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PolicykitAgentGUI::onIdentityChanged);
    m_password->setFocus(Qt::OtherFocusReason);
}

PolicykitAgentGUI::~PolicykitAgentGUI() = default;

void PolicykitAgentGUI::setPrompt(const PolkitQt1::Identity &identity, const QString &text, bool echo)
{
    const int ix = m_identity->findText(identity.toString());
    if (ix != -1)
    {
        m_identity->setItemData(ix, text, RolePromptText);
        m_identity->setItemData(ix, echo, RolePromptEcho);

        if (ix == m_identity->currentIndex())
        {
            m_pwprompt->setText(text);
            m_password->setEchoMode(echo ? QLineEdit::Normal : QLineEdit::Password);
        }
    }
}

QString PolicykitAgentGUI::identity()
{
    Q_ASSERT(m_identity->currentIndex() != -1);
    return m_identity->currentText();
}

QString PolicykitAgentGUI::response() {
  QString response = m_password->text();
  m_password->setText(QString());
  return response;
}

void PolicykitAgentGUI::onIdentityChanged(int index)
{
    QVariant text = m_identity->itemData(index, RolePromptText);
    QVariant echo = m_identity->itemData(index, RolePromptEcho);
    if (text != QVariant{})
        m_pwprompt->setText(text.toString());
    if (echo != QVariant{})
        m_password->setEchoMode(echo.toBool() ? QLineEdit::Normal : QLineEdit::Password);
}

void PolicykitAgentGUI::setupUi()
{
    gridLayout = new QGridLayout(this);
    m_message = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_message->sizePolicy().hasHeightForWidth());
    m_message->setSizePolicy(sizePolicy);
    m_message->setWordWrap(true);

    m_icon = new QLabel(this);
    m_icon->setMinimumSize(QSize(96, 96));
    m_icon->setMaximumSize(QSize(96, 96));
    m_identity = new QComboBox(this);

    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);

    m_buttons = new QDialogButtonBox(this);
    m_pwprompt = new QLabel(this);
    auto l_identity = new QLabel(this);
    m_details = new QLabel(this);
    m_buttons->setOrientation(Qt::Horizontal);
    m_buttons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    //m_buttons->addButton(m_detaails, QDialogButtonBox::Reset);

    auto spacer = new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::Minimum);

    gridLayout->addWidget(m_message, 0, 1, 1, 1, Qt::AlignTop);
    gridLayout->addWidget(m_icon, 0, 0, 1, 1, Qt::AlignTop);
    gridLayout->addItem(spacer, 1, 1, 1, 2);

    gridLayout->addWidget(l_identity, 2, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(m_identity, 2, 1, 1, 1);

    gridLayout->addWidget(m_pwprompt, 3, 0, 1, 1,Qt::AlignRight);
    gridLayout->addWidget(m_password, 3, 1, 1, 1);

    gridLayout->addWidget(m_details, 4, 0, 1, 2);
    gridLayout->addWidget(m_buttons, 5, 0, 1, 2);

    setWindowTitle(tr("Authentication Required"));
    l_identity->setText(tr("Identity:"));
    m_pwprompt->setText(tr("Password:"));
    m_details->setText(tr("TextLabel"));

    QObject::connect(m_buttons, &QDialogButtonBox::accepted,
                     this, &QDialog::accept);
    QObject::connect(m_buttons, &QDialogButtonBox::rejected,
                     this, &QDialog::reject);

}
