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
#include <QFile>
#include <QItemSelectionModel>

#include <QMessageBox>
#include <QProcess>

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

bool EFIStartupSettingsApplet::available() const
{
    QFile f("/sys/firmware/efi/");
    if(f.exists())
        return true;

    return false;
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
    PolkitQt1::Gui::Action *action = qobject_cast<PolkitQt1::Gui::Action *>(sender());
    qDebug() << "pretending to be the mechanism for action:" << action->actionId();

    PolkitQt1::Authority::Result result;
    PolkitQt1::UnixProcessSubject subject(static_cast<uint>(QCoreApplication::applicationPid()));

    result = PolkitQt1::Authority::instance()->checkAuthorizationSync(action->actionId(), subject,
             PolkitQt1::Authority::AllowUserInteraction);
    if (result == PolkitQt1::Authority::Yes) {
        // in the helper you will do the action
        qDebug() << "caller is authorized to do:" << action->actionId();
        m_bt->setIcon(QIcon::fromTheme("emblem-unlocked"));
        m_polkit_label->setText(tr("Click the lock to prevent further changes."));
        enableElevated(true);
    } else {
        // OR return false to notify the caller that the action is not authorized.
        qDebug() << "caller is NOT authorized to do:" << action->actionId();
    }
}

void EFIStartupSettingsApplet::restartSelected()
{
    QMessageBox msg;
    auto idx = m_bootentries->selectionModel()->selectedIndexes().first();

    if(idx.isValid())
    {
        QString os = m_model->data(idx, Qt::DisplayRole).toString();
        msg.setWindowTitle(tr("Reboot to %1").arg(os));
        msg.setText(tr("<b>Are you ready to reboot to %1?</b>").arg(os));
        msg.setInformativeText(tr("If you have open documents, you should save your work before proceeding."));
        msg.setIcon(QMessageBox::Question);
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        if(msg.exec() == QMessageBox::Yes)
        {
            if(getuid() == 0)
            {
                // root level function
                QVector<uint16_t> test = QVector<uint16_t>(order.begin(), order.end());
                int id = m_model->data(idx, Qt::UserRole+3).toInt();
                test.removeOne(id);
                test.prepend(id);

                QStringList ids;

                for(auto &i : test)
                    ids.append(QString::number((int)i));

                auto bootorder = ids.join(',');

                QProcess proc;
                proc.setProgram("/usr/sbin/efibootmgr");
                proc.setArguments(QStringList() << "-o" << bootorder);
                proc.start();
                for(;;)
                {
                    if(proc.waitForFinished())
                        break;
                }

                int res = proc.exitCode();

                if(res == 0)
                {
                    QProcess proc;
                    proc.setProgram("/usr/sbin/openrc-shutdown");
                    proc.setArguments(QStringList() << "-r" << "now");
                    proc.startDetached();
                }
                else
                {
                    QMessageBox error;
                    error.setWindowTitle(tr("Reboot into %1").arg(os));
                    error.setText(tr("Failed to set EFI boot variables.").arg(os));
                    error.setIcon(QMessageBox::Critical);
                    error.setStandardButtons(QMessageBox::Ok);
                    error.exec();
                }
            }
            else
            {
                // TODO: handle the setting of things via polkit
            }
        }
    }
}

void EFIStartupSettingsApplet::currentIndexChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    if(selected.indexes().empty())
    {
        m_desc->setText("");
        m_reboot->setEnabled(false);
        m_advanced->setEnabled(false);
        return;
    }

    auto index = selected.indexes().first();

    m_desc->setText(m_model->data(index, Qt::UserRole+1).toString());
    m_reboot->setEnabled(true);
    /* if(m_model->data(index, Qt::UserRole+2).toBool() == true)
        m_advanced->setEnabled(true);
    else */
        m_advanced->setEnabled(false);

}

void EFIStartupSettingsApplet::setupWidget()
{
    m_host = new QWidget(0);
    //m_host->setMinimumHeight(160);
    m_host->setObjectName(QString::fromUtf8("StartupSettingsHost"));
    m_host->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

    m_hostlayout = new QVBoxLayout(m_host);

    m_bootentries = new QListView(m_host);
    auto desc = new QLabel(m_host);
    desc->setText(tr("Select the device or operating system you want to start up your computer with:"));
    m_bootentries->setModel(m_model);
    m_bootentries->setViewMode(QListView::IconMode);
    m_bootentries->setFlow(QListView::LeftToRight);
    m_bootentries->setEditTriggers(QListView::NoEditTriggers);
    m_bootentries->setSelectionMode(QAbstractItemView::SingleSelection);
    m_bootentries->setIconSize(QSize(32,32));
    connect(m_bootentries->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &EFIStartupSettingsApplet::currentIndexChanged);
    connect(m_bootentries, &QListView::activated, this, &EFIStartupSettingsApplet::restartSelected);
    m_polkit_lock = new QToolButton(m_host);
    m_polkit_label = new QLabel(m_host);
    m_polkit_lock->setIconSize(QSize(32,32));
    m_polkit_lock->setAutoRaise(true);
    m_polkit_label->setText(tr("Click the lock to authenticate for changes."));

    m_bt = new PolkitQt1::Gui::ActionButton(m_polkit_lock,
               "org.originull.hwsettings.efiboot.auth", this);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::triggered,
            this,  &EFIStartupSettingsApplet::polkitActivate);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::clicked,
            m_bt, &PolkitQt1::Gui::ActionButton::activate);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::authorized,
            this, &EFIStartupSettingsApplet::polkitActivated);
    m_bt->setIcon(QIcon::fromTheme("emblem-locked"));

    m_advanced = new QPushButton(m_host);
    m_advanced->setText(tr("Advanced..."));
    m_reboot = new QPushButton(m_host);
    m_reboot->setText(tr("Restart..."));
    connect(m_reboot, &QPushButton::pressed, this, &EFIStartupSettingsApplet::restartSelected);
    m_advanced->setEnabled(false);
    m_reboot->setEnabled(false);

    bool isroot = false;
    if(getuid() == 0)
        isroot = true;

    m_desc = new QLabel(m_host);
    auto desc_layout = new QHBoxLayout();
    desc_layout->addWidget(m_desc, 1);
    desc_layout->addWidget(m_advanced);
    desc_layout->addWidget(m_reboot);

    auto polkit_layout = new QHBoxLayout();
    polkit_layout->addWidget(m_polkit_lock);
    polkit_layout->addWidget(m_polkit_label, 1);


    m_hostlayout->addWidget(desc);
    m_hostlayout->addWidget(m_bootentries, 1);
    m_hostlayout->addLayout(desc_layout);
    m_hostlayout->addLayout(polkit_layout);

    if(isroot)
    {
        m_polkit_lock->setVisible(false);
        m_polkit_label->setVisible(false);
        m_bootentries->setEnabled(true);
    }
    else
        enableElevated(false);
}

void EFIStartupSettingsApplet::enableElevated(bool enable)
{
    m_bootentries->setEnabled(enable);
}

void EFIStartupSettingsApplet::pollEFI(bool showHidden)
{
    Q_UNUSED(showHidden);
    int next_boot = -1;

    std::unordered_set<unsigned long> ordered_entry;
    QStringList invalid_keys;

    auto name_to_guid = EFIBoot::get_variables(
        [](const EFIBoot::efi_guid_t &guid, const std::tstring_view name)
        { return guid == EFIBoot::efi_guid_global && (name.substr(0, 4) == _T("Boot") || name == _T("Timeout")); },
        [&](size_t step, size_t total)
        {
            Q_UNUSED(step);
            Q_UNUSED(total);
            //show_progress_bar(step, total + 1u, tr("Searching EFI Boot Manager entries..."));
        });

    //size_t step = 0;
    //const size_t total_steps = name_to_guid.size() + 1u;
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
