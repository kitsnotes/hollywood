#include "preferencedialog.h"
#include "preferencedialog_p.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QPropertyAnimation>

HWPreferenceDialogPrivate::HWPreferenceDialogPrivate(HWPreferenceDialog *parent)
    : d(parent)
    , m_tool(new QToolBar(parent))
    , m_placeholder(new QWidget(parent))
    , m_current(m_placeholder)
{

}

HWPreferenceDialog::HWPreferenceDialog(QWidget *parent)
    : QDialog(parent)
    , p(new HWPreferenceDialogPrivate(this))
{
    setWindowTitle(tr("Preferences"));
    resize(400,40);
    p->m_tool->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    layout->addWidget(p->m_tool);
    layout->addWidget(p->m_placeholder, 1);
    auto lspacer = new QWidget(p->m_tool);
    auto rspacer = new QWidget(p->m_tool);

    auto pal = lspacer->palette();
    pal.setColor(QPalette::All, QPalette::Base, QColor(0,0,0,0));
    lspacer->setPalette(pal);
    lspacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    lspacer->setMaximumHeight(3);
    rspacer->setPalette(pal);
    rspacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    rspacer->setMaximumHeight(3);

    p->m_leftSpacerAct = p->m_tool->addWidget(lspacer);
    p->m_rightSpacerAct = p->m_tool->addWidget(rspacer);

}

HWPreferenceDialog::~HWPreferenceDialog()
{
    for(auto w : p->m_tabcontent)
        w->deleteLater();
}

void HWPreferenceDialog::addPage(QWidget *page, const QIcon &icon, const QString &title)
{
    bool rtl = false;
    if(qApp->layoutDirection() == Qt::RightToLeft)
        rtl = true;

    QAction *action = nullptr;
    if(icon.isNull())
        action = new QAction(title, this);
    else
        action = new QAction(icon, title, this);

    p->m_tabs.append(action);
    p->m_tabcontent.append(page);
    page->setVisible(false);
    if(rtl)
    {
        if(p->m_tabs.count() == 0)
            p->m_tool->insertAction(p->m_rightSpacerAct, action);
        else
            p->m_tool->insertAction(p->m_tabs.last(), action);
    }
    else
        p->m_tool->insertAction(p->m_rightSpacerAct, action);

    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &HWPreferenceDialog::tabActionTriggered);
}

void HWPreferenceDialog::setAnimated(bool animated)
{
    p->m_animate = animated;
    emit animatedChanged();
}

void HWPreferenceDialog::setCurrentIndex(int index)
{
    if(index > p->m_tabs.count()-1)
        return;

    if(index < 0)
        return;

    for(auto action : p->m_tabs)
        action->setChecked(false);

    for(auto view : p->m_tabcontent)
        view->setVisible(false);

    p->m_tabcontent[index]->setVisible(true);
    if(p->m_animate)
    {
        auto animation = new QPropertyAnimation(this, "size");
        QSize start = size();
        auto wheight = p->m_tabcontent[index]->minimumHeight();
        QSize end = QSize(width(), wheight+p->m_tool->height());
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->setDuration(190);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        auto wheight = p->m_tabcontent[index]->heightForWidth(width());
        QSize end = QSize(width(), wheight+p->m_tool->height());
        resize(end);
    }

    layout()->replaceWidget(p->m_current, p->m_tabcontent[index]);
    p->m_current = p->m_tabcontent[index];
    p->m_current->setVisible(true);
    p->m_index = index;
    p->m_tabs[index]->setChecked(true);
    emit currentIndexChanged(index);
}

void HWPreferenceDialog::show()
{
    if(p->m_tabs.count() < 1)
        return;

    setCurrentIndex(0);
    QDialog::show();
}

bool HWPreferenceDialog::animated()
{
    return p->m_animate;
}

int HWPreferenceDialog::currentIndex()
{
    return p->m_index;
}

void HWPreferenceDialog::tabActionTriggered()
{
    auto senderObj = qobject_cast<QAction*>(sender());
    Q_ASSERT(senderObj);

    if(p->m_tabs.contains(senderObj))
         setCurrentIndex(p->m_tabs.indexOf(senderObj));
}

