#include "tabhost.h"
#include <QVBoxLayout>
#include <QTabBar>
#include <QDateTime>
#include <QListView>
#include <QMessageBox>

#include "terminal.h"
#include "profile.h"

#if QT_VERSION >= 0x060000
#include <qtermwidget6/qtermwidget.h>
#else
#include <qtermwidget5/qtermwidget.h>
#include <QDebug>
#endif
#include "application.h"

TabHost::TabHost(QWidget *parent)
    : QWidget(parent),
      m_tabs(new QTabBar(this)),
      m_placeholder(new QWidget(this)),
      m_current(m_placeholder)
{
    connect(m_tabs, &QTabBar::tabCloseRequested, this, &TabHost::tabCloseRequest);
    connect(m_tabs, &QTabBar::currentChanged, this, &TabHost::tabChanged);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);
    layout->addWidget(m_placeholder, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    m_tabs->setDocumentMode(true);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
}

void TabHost::newDefaultTab()
{
    auto defProfile = TerminalApplication::instance()->defaultProfileName();
    auto term = TerminalHost::create(defProfile, this);
    m_terminals.insert(term->id(), term);
    int tabidx = m_tabs->addTab(tr("Untitled"));
    m_tabs->setTabData(tabidx, term->id());
    m_tabs->setCurrentIndex(tabidx);
    tabChanged(tabidx);
    connect(term, &TerminalHost::titleChanged,
            this, &TabHost::titleChanged);
    connect(term, &TerminalHost::requestClose,
            this, &TabHost::termRequestClose);
    connect(term->widget(), &QTermWidget::customContextMenuRequested,
            this, &TabHost::customContextMenuRequested);

    // TODO: fix this signal
    connect(term->widget(), &QTermWidget::copyAvailable, this,
            &TabHost::termCopyAvailable);

    if(m_tabs->count() == 1)
        m_tabs->setVisible(false);
    else
        m_tabs->setVisible(true);
}

bool TabHost::requestCloseAll()
{
    bool canclose = true;
    for(auto t : m_terminals)
    {
        if(!t->canClose())
            canclose = false;
    }

    return canclose;
}

void TabHost::copySelection()
{
    auto term = currentHost();
    if(!term)
        return;

    term->widget()->copyClipboard();
}

void TabHost::pasteClipboard()
{
    auto term = currentHost();
    if(!term)
        return;

    term->widget()->pasteClipboard();
}

void TabHost::pasteSelection()
{
    auto term = currentHost();
    if(!term)
        return;

    term->widget()->pasteSelection();
}

void TabHost::zoomIn()
{
    auto term = currentHost();
    if(!term)
        return;

    term->widget()->zoomIn();
}

void TabHost::zoomOut()
{
    auto term = currentHost();
    if(!term)
        return;

    term->widget()->zoomOut();

}

void TabHost::zoomNormal()
{
    auto term = currentHost();
    if(!term)
        return;

    //term->widget()->zoomIn();
}

void TabHost::clear()
{
    auto term = currentHost();
    if(!term)
        return;

    term->widget()->clear();
}

void TabHost::profileDeleted(const QString &profile)
{
    for(auto term : m_terminals)
    {
        if(term->profile()->profileName() == profile)
        {
            auto app = TerminalApplication::instance();
            term->setProfile(app->profileByName(app->defaultProfileName()));
        }
    }
}

void TabHost::tabChanged(int idx)
{
    auto id = m_tabs->tabData(idx).toByteArray();
    if(m_terminals.contains(id))
    {
        m_current->setVisible(false);
        layout()->replaceWidget(m_current, m_terminals[id]->widget());
        m_current = m_terminals[id]->widget();
        m_current->setVisible(true);
    }

    auto wndtitle = m_current->windowTitle();
    emit windowTitleChanged(wndtitle);
}

void TabHost::tabCloseRequest(int idx)
{
    auto id = m_tabs->tabData(idx).toByteArray();
    if(m_terminals.contains(id))
    {
        auto term = m_terminals[id];
        if(!term->canClose())
        {
            if(m_terminals[id]->profile()->closeConsentBehavior() == TerminalProfile::AskAlways)
            {
                QMessageBox question(this);
                question.setIcon(QMessageBox::Question);
                question.setWindowTitle("");
                question.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
                question.setText(tr("Do you want to close this terminal tab?"));
                int result = question.exec();
                if(result == QMessageBox::Yes)
                {
                    removeTab(idx);
                    return;
                }
                else
                    return;
            }
            if(m_terminals[id]->profile()->closeConsentBehavior() == TerminalProfile::AskSelective)
            {
                QMessageBox question(this);
                question.setIcon(QMessageBox::Question);
                question.setWindowTitle("");
                question.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
                question.setText(tr("Do you want to terminate all running processes in this tab?"));
                question.setInformativeText(tr("Process List Here"));
                int result = question.exec();
                if(result == QMessageBox::Yes)
                {
                    removeTab(idx);
                    return;
                }
                else
                    return;
            }
        }
        removeTab(idx);
    }
}

void TabHost::removeTab(int idx)
{
    auto id = m_tabs->tabData(idx).toByteArray();
    if(m_terminals.contains(id))
    {
        auto term = m_terminals[id];
        m_current->setVisible(false);
        layout()->replaceWidget(m_current, m_placeholder);
        m_current = m_placeholder;
        m_current->setVisible(true);
        m_terminals.remove(id);
        m_tabs->removeTab(idx);
        term->deleteLater();

        if(m_tabs->count() == 1)
            m_tabs->setVisible(false);
        else
            m_tabs->setVisible(true);

        if(m_tabs->count() == 0)
            emit requestWindowClose();
    }
}

void TabHost::termRequestClose()
{
    auto *host = qobject_cast<TerminalHost*>(sender());
    Q_ASSERT(host);

    auto tab = indexForTab(host->id());
    if(host->profile()->closeConsentBehavior() == TerminalProfile::AskAlways)
    {
        QMessageBox question(this);
        question.setIcon(QMessageBox::Question);
        question.setWindowTitle("");
        question.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        question.setText(tr("Do you want to close this terminal tab?"));
        int result = question.exec();
        if(result == QMessageBox::Yes)
        {
            removeTab(tab);
            return;
        }
        else
            return;
    }
    else
        removeTab(tab);
}

void TabHost::titleChanged()
{
    auto *host = qobject_cast<TerminalHost*>(sender());
    Q_ASSERT(host);

    auto tab = indexForTab(host->id());
    auto title = m_terminals.value(host->id())->tabTitle();
    auto wndtitle = m_terminals.value(host->id())->windowTitle();

    m_tabs->setTabText(tab,title);
    if(m_tabs->currentIndex() == tab)
        emit windowTitleChanged(wndtitle);
}

void TabHost::termCopyAvailable(bool copy)
{
    Q_UNUSED(copy);
    emit selectionChanged();
}

int TabHost::indexForTab(const QByteArray &tabId) const
{
    for(int i = 0; i < m_tabs->count(); ++i)
    {
        if(m_tabs->tabData(i).toByteArray() == tabId)
            return i;
    }

    return -1;
}

TerminalHost *TabHost::currentHost()
{
    auto id = m_tabs->tabData(m_tabs->currentIndex()).toByteArray();
    if(m_terminals.contains(id))
        return m_terminals[id];

    return nullptr;
}
