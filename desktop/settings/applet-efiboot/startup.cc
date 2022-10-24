/* Hollywood Operating System
 * System Settings - System Startup Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "startup.h"
#include "bootentrylistmodel.h"
#include <QLabel>
#include <cctype>
#include <unordered_set>

EFIStartupSettingsApplet::EFIStartupSettingsApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

EFIStartupSettingsApplet::~EFIStartupSettingsApplet()
{

}

bool EFIStartupSettingsApplet::init()
{
    m_model = new BootEntryListModel(this);
    pollEFI();
    setupWidget();
    loadSettings();

    return true;
}

bool EFIStartupSettingsApplet::loadSettings()
{
    return true;
}

bool EFIStartupSettingsApplet::saveSettings()
{
    return true;
}

QString EFIStartupSettingsApplet::id() const
{
    return QLatin1String("org.originull.hwsettings.startup");
}

QString EFIStartupSettingsApplet::name() const
{
    return tr("System Startup");
}

QString EFIStartupSettingsApplet::description() const
{
    return tr("Select the operating system or device to use for system startup.");
}

QIcon EFIStartupSettingsApplet::icon() const
{
    const QIcon mine(QIcon::fromTheme("preferences-system-startup"));
    return mine;
}

QWidget *EFIStartupSettingsApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category EFIStartupSettingsApplet::category() const
{
    return System;
}

void EFIStartupSettingsApplet::polkitActivate()
{

}

void EFIStartupSettingsApplet::polkitActivated()
{

}

void EFIStartupSettingsApplet::setupWidget()
{
    m_host = new QWidget(0);
    m_host->setObjectName(QString::fromUtf8("StartupSettingsHost"));

    m_hostlayout = new QVBoxLayout(m_host);

    m_bootentries = new QListView(m_host);
    auto desc = new QLabel(m_host);
    desc->setText(tr("Select the device or operating system you want to start up your computer with:"));
    m_bootentries->setModel(m_model);
    m_bootentries->setFlow(QListView::LeftToRight);
    m_bootentries->setIconSize(QSize(32,32));
    m_polkit_lock = new QToolButton(m_host);
    m_polkit_label = new QLabel(m_host);
    m_polkit_lock->setIconSize(QSize(32,32));
    m_polkit_lock->setAutoRaise(true);
    m_polkit_label->setText(tr("Click the lock to authenticate for changes."));

    m_bt = new PolkitQt1::Gui::ActionButton(m_polkit_lock,
               "org.originull.hwsettings.datetime.auth", this);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::triggered,
            this,  &EFIStartupSettingsApplet::polkitActivate);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::clicked,
            m_bt, &PolkitQt1::Gui::ActionButton::activate);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::authorized,
            this, &EFIStartupSettingsApplet::polkitActivated);
    m_bt->setIcon(QIcon::fromTheme("emblem-locked"));

    auto polkit_layout = new QHBoxLayout();
    polkit_layout->addWidget(m_polkit_lock);
    polkit_layout->addWidget(m_polkit_label, 1);


    m_hostlayout->addWidget(desc);
    m_hostlayout->addWidget(m_bootentries, 1);
    m_hostlayout->addLayout(polkit_layout);
}

void EFIStartupSettingsApplet::pollEFI(bool showHidden)
{
    int next_boot = -1;
    std::vector<uint16_t> order;
    std::unordered_set<unsigned long> ordered_entry;
    QStringList invalid_keys;

    auto name_to_guid = EFIBoot::get_variables(
        [](const EFIBoot::efi_guid_t &guid, const std::tstring_view name)
        { return guid == EFIBoot::efi_guid_global && (name.substr(0, 4) == _T("Boot") || name == _T("Timeout")); },
        [&](size_t step, size_t total)
        {
            //show_progress_bar(step, total + 1u, tr("Searching EFI Boot Manager entries..."));
        });

    size_t step = 0;
    const size_t total_steps = name_to_guid.size() + 1u;
    if(name_to_guid.count(_T("Timeout")))
    {
        auto variable = EFIBoot::get_variable<uint16_t>(name_to_guid[_T("Timeout")], _T("Timeout"));
        if(!variable)
            invalid_keys.push_back("Timeout");
        else
        {
            const auto &[value, attributes] = *variable;
            (void)attributes;
            //ui->timeout_number->setValue(value);
        }
    }

    if(name_to_guid.count(_T("BootNext")))
    {
        auto variable = EFIBoot::get_variable<int32_t>(name_to_guid[_T("BootNext")], _T("BootNext"));
        if(!variable)
            invalid_keys.push_back("BootNext");
        else
        {
            const auto &[value, attributes] = *variable;
            (void)attributes;
            next_boot = value;
        }
    }

    if(name_to_guid.count(_T("BootOrder")))
    {
        auto variable = EFIBoot::get_list_variable<uint16_t>(name_to_guid[_T("BootOrder")], _T("BootOrder"));
        if(!variable)
            invalid_keys.push_back("BootOrder");
        else
        {
            const auto &[value, attributes] = *variable;
            (void)attributes;
            order = value;

            for(const auto &index: order)
                ordered_entry.insert(index);
        }
    }

    // Add entries not in BootOrder at the end
    for(const auto &[name, guid]: name_to_guid)
    {
        (void)guid;
        if(name.length() != 8 || name.substr(0, 4) != _T("Boot"))
            continue;

        auto suffix = name.substr(4);
        if(!isxnumber(suffix))
            continue;

        auto index = std::stoul(suffix, nullptr, HEX_BASE);
        if(ordered_entry.count(index))
            continue;

        order.push_back(static_cast<uint16_t>(index));
        ordered_entry.insert(index);
    }

    m_model->clear();
    for(const auto &index: order)
    {
        auto qname = toHex(index, 4, "Boot");
        auto name = QStringToStdTString(qname);
        auto variable = EFIBoot::get_variable<EFIBoot::Load_option>(name_to_guid[name], name);

        if(!variable)
        {
            invalid_keys.push_back(qname);
            continue;
        }

        const auto &[value, attributes] = *variable;

        // Translate STL to QTL
        if(!(attributes & EFIBoot::LOAD_OPTION_HIDDEN))
        {
            auto entry = BootEntry::fromEFIBootLoadOption(value);
            entry.index = index;
            entry.efi_attributes = attributes;
            entry.is_next_boot = next_boot == static_cast<int>(index);
            m_model->appendRow(entry);
        }
    }

    /*if(!invalid_keys.isEmpty())
        show_error(tr("Error loading entries!"), tr("Couldn't deserialize keys: %1").arg(invalid_keys.join(", "))); */

}
