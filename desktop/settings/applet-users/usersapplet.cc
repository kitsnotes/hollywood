#include "usersapplet.h"
#include <QString>
#include <QFormLayout>
#include <QLineEdit>
#include <QFile>
#include <QTimer>
#include "shellfunc.h"

UsersGroupsApplet::UsersGroupsApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool UsersGroupsApplet::init()
{
    setupWidget();
    setupAccountService();
    QTimer::singleShot(20, [this]() {
        m_users->setCurrentIndex(m_model->index(0,0));
    });
    return true;
}

void UsersGroupsApplet::setupAccountService()
{
    m_m = new QtAccountsService::AccountsManager();
    m_model = new UsersModel();
    m_users->setModel(m_model);
    connect(m_users->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection &selected, const QItemSelection &deselected) {
        auto idx = m_users->selectionModel()->selectedIndexes().first();
        if(idx.isValid())
        {
            qDebug() << idx.row() << idx.column();
            auto user = m_model->userAccount(idx);
            if(user)
                updateViewForUser(user);
        }
    });
}

void UsersGroupsApplet::updateViewForUser(QtAccountsService::UserAccount *user)
{
    if(!user)
        return;

    m_dname->setText(user->displayName());
    m_linuxname->setText(user->userName());

    e_dname->setText(user->displayName());
    e_fname->setText(user->realName());
    e_email->setText(user->email());
}

void UsersGroupsApplet::setUserIcon(QtAccountsService::UserAccount *user)
{
    if(!user)
    {
        m_icon->setIcon(QIcon::fromTheme("user-none"));
        return;
    }

    auto fn = user->iconFileName();
    QFile file(fn);
    if(file.exists())
    {
        QPixmap img(fn);
        if(!img.isNull())
        {
            m_icon->setIcon(QIcon(img));
            return;
        }
    }

    m_icon->setIcon(QIcon::fromTheme("user-none"));
    return;
}

QString UsersGroupsApplet::id() const
{
    return QString("org.originull.users");
}

QString UsersGroupsApplet::name() const
{
    return tr("Users & Groups");
}

QString UsersGroupsApplet::description() const
{
    return tr("Manage local users & groups");
}

QIcon UsersGroupsApplet::icon() const
{
    return QIcon::fromTheme("system-users");
}

QWidget *UsersGroupsApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category UsersGroupsApplet::category() const
{
    return System;
}

QStringList UsersGroupsApplet::searchTokens() const
{
    QStringList tokens;
    tokens << tr("Change account password");
    tokens << tr("Create local user account");

    return tokens;
}

void UsersGroupsApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("UsersGroups"));
    m_host->resize(625, 439);

    m_help = new QToolButton(m_host);
    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(22,22));

    connect(m_help, &QToolButton::pressed, this, []() {
        hw_shell::openHelpTopic(QString(HW_APP_HELP_TOPIC));
    });

    mainLayout = new QVBoxLayout(m_host);
    auto sl = new QHBoxLayout;
    mainLayout->addLayout(sl);
    m_delegate = new SettingsSidebarDelegate(this);
    m_users = new QListView(m_host);
    m_users->setMaximumWidth(250);
    m_users->setMinimumHeight(250);
    m_users->setItemDelegate(m_delegate);
    m_users->setIconSize(QSize(48,48));
    m_users->setSelectionMode(QAbstractItemView::SingleSelection);
    sl->addWidget(m_users);

    auto il = new QVBoxLayout;
    sl->addLayout(il);

    auto hl_top = new QHBoxLayout;

    m_icon = new QToolButton(m_host);
    m_icon->setIconSize(QSize(64,64));
    setUserIcon(nullptr);
    m_dname = new QLabel(m_host);

    auto font = m_dname->font();
    font.setBold(true);
    font.setPointSize(font.pointSize()+1);
    m_dname->setFont(font);
    m_dname->setText(tr("User"));
    m_icon->setMinimumSize(QSize(64,64));

    auto ic_v = new QVBoxLayout;
    hl_top->addWidget(m_icon);
    hl_top->addLayout(ic_v);
    ic_v->addWidget(m_dname);
    ic_v->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));
    il->addLayout(hl_top);
    hl_top->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));

    auto vl_btns = new QVBoxLayout;
    m_chgpass = new QPushButton(m_host);
    m_startup = new QPushButton(m_host);
    vl_btns->addWidget(m_chgpass);
    vl_btns->addWidget(m_startup);

    hl_top->addLayout(vl_btns);
    auto fl = new QFormLayout;
    il->addLayout(fl);

    fl->setLabelAlignment(Qt::AlignRight);
    auto l_linuxname = new QLabel(m_host);
    auto l_fullname = new QLabel(m_host);
    auto l_displayname = new QLabel(m_host);
    auto l_email = new QLabel(m_host);

    m_linuxname = new QLabel(m_host);
    e_dname = new QLineEdit(m_host);
    e_fname = new QLineEdit(m_host);
    e_email = new QLineEdit(m_host);

    m_admin = new QCheckBox(m_host);
    m_autologin = new QCheckBox(m_host);

    fl->addRow(l_linuxname, m_linuxname);
    fl->addRow(l_displayname, e_dname);
    fl->addRow(l_fullname, e_fname);
    fl->addRow(l_email, e_email);

    fl->setContentsMargins(25,0,25,0);
    il->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));

    il->addWidget(m_admin);
    il->addWidget(m_autologin);
    l_linuxname->setText(tr("Linux System Name:"));
    l_fullname->setText(tr("Full Name:"));
    l_displayname->setText(tr("Display Name:"));
    l_email->setText(tr("E-Mail Address:"));
    m_admin->setText(tr("This account is a system administrator"));
    m_autologin->setText(tr("Automatically log in with this account"));

    m_chgpass->setText(tr("Change Password"));
    m_startup->setText(tr("Manage Startup Items"));

    auto btnl = new QHBoxLayout;
    mainLayout->addLayout(btnl);
    btnl->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    btnl->addWidget(m_help);
}


QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DecorationRole)
    {
        return QIcon::fromTheme("user-none");
    }
    if(role == Qt::UserRole+1)
    {
        return tr("Administrator");
    }
    return QtAccountsService::UsersModel::data(index, role);
}
