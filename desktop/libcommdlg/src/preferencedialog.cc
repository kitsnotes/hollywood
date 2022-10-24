#include "preferencedialog.h"
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QApplication>
#include <QDialog>
#include <QPropertyAnimation>

ADPreferenceDialog::ADPreferenceDialog(QWidget *parent)
    :QDialog(parent),
     m_tool(new QToolBar(this)),
     m_placeholder(new QWidget(this)),
     m_current(m_placeholder)
{
    setWindowTitle(tr("Preferences"));
    resize(400,40);
    m_tool->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    layout->addWidget(m_tool);
    layout->addWidget(m_placeholder, 1);
    auto lspacer = new QWidget(m_tool);
    auto rspacer = new QWidget(m_tool);

    auto p = lspacer->palette();
    p.setColor(QPalette::All, QPalette::Base, QColor(0,0,0,0));
    lspacer->setPalette(p);
    lspacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    lspacer->setMaximumHeight(3);
    rspacer->setPalette(p);
    rspacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    rspacer->setMaximumHeight(3);

    m_leftSpacerAct = m_tool->addWidget(lspacer);
    m_rightSpacerAct = m_tool->addWidget(rspacer);

}

ADPreferenceDialog::~ADPreferenceDialog()
{
    for(auto w : m_tabcontent)
        w->deleteLater();
}

void ADPreferenceDialog::addPage(QWidget *page, const QIcon &icon, const QString &title)
{
    bool rtl = false;
    if(qApp->layoutDirection() == Qt::RightToLeft)
        rtl = true;

    QAction *action = nullptr;
    if(icon.isNull())
        action = new QAction(title, this);
    else
        action = new QAction(icon, title, this);

    m_tabs.append(action);
    m_tabcontent.append(page);
    page->setVisible(false);
    if(rtl)
    {
        if(m_tabs.count() == 0)
            m_tool->insertAction(m_rightSpacerAct, action);
        else
            m_tool->insertAction(m_tabs.last(), action);
    }
    else
        m_tool->insertAction(m_rightSpacerAct, action);

    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &ADPreferenceDialog::tabActionTriggered);
}

void ADPreferenceDialog::setAnimated(bool animated)
{
    m_animate = animated;
    emit animatedChanged();
}

void ADPreferenceDialog::setCurrentIndex(int index)
{
    if(index > m_tabs.count()-1)
        return;

    if(index < 0)
        return;

    for(auto action : m_tabs)
        action->setChecked(false);

    for(auto view : m_tabcontent)
        view->setVisible(false);

    m_tabcontent[index]->setVisible(true);
    if(m_animate)
    {
        auto animation = new QPropertyAnimation(this, "size");
        QSize start = size();
        auto wheight = m_tabcontent[index]->minimumHeight();
        QSize end = QSize(width(), wheight+m_tool->height());
        qDebug() << "setCurrentIndex:" << end;
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->setDuration(190);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        auto wheight = m_tabcontent[index]->heightForWidth(width());
        QSize end = QSize(width(), wheight+m_tool->height());
        resize(end);
    }

    layout()->replaceWidget(m_current, m_tabcontent[index]);
    m_current = m_tabcontent[index];
    m_current->setVisible(true);
    m_index = index;
    m_tabs[index]->setChecked(true);
    emit currentIndexChanged(index);
}

void ADPreferenceDialog::show()
{
    if(m_tabs.count() < 1)
        return;

    setCurrentIndex(0);
    QDialog::show();
}

bool ADPreferenceDialog::animated()
{
    return m_animate;
}

int ADPreferenceDialog::currentIndex()
{
    return m_index;
}

void ADPreferenceDialog::tabActionTriggered()
{
    auto senderObj = qobject_cast<QAction*>(sender());
    Q_ASSERT(senderObj);

    if(m_tabs.contains(senderObj))
         setCurrentIndex(m_tabs.indexOf(senderObj));
}
