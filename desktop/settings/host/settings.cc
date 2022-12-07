#include "settings.h"
#include <aboutdialog.h>
#include <hollywood/hollywood.h>
#include <hollywood/appletinterface.h>
#include <QStyledItemDelegate>
#include <QHash>
#include <QSettings>
#include <QActionGroup>
#include <QApplication>
#include <QCommandLineParser>

#include "qcategorizedview/qcategorydrawer.h"

// this class borrowed from LXQT's lxconfig
class ConfigItemDelegate : public QStyledItemDelegate
{
public:
    ConfigItemDelegate(QCategorizedView* view) : mView(view) { }
    ~ConfigItemDelegate() override { }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        /* We let Qt calculate the real cell size but consider the 4-px margin
           around each cell and try to add a 2-px margin around the selection
           rectangle for styles that, unlike Fusion, highlight the whole item. */
        QStyleOptionViewItem opt = option;
        int delta = opt.rect.width() - (mView->gridSize().width() - 8);
        if (delta > 0)
          opt.rect.adjust(delta/2, 0 , -delta/2, 0);
        QSize defaultSize = QStyledItemDelegate::sizeHint(opt, index);
        return QSize(qMin(defaultSize.width() + 4, mView->gridSize().width() - 8),
                     qMin(defaultSize.height() + 4, mView->gridSize().height() - 8));
    }

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        if(!index.isValid())
            return;

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        const QSize & iconSize = option.decorationSize;
        int size = qMin(mView->gridSize().width() - 4, // 2-px margin around each cell
                        iconSize.height());
        opt.decorationSize = QSize(size, size);

        const QWidget* widget = opt.widget;
        QStyle* style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    }

private:
    QCategorizedView *mView;
};

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent)
    ,  m_menuBar(new QMenuBar(this))
    , m_toolBar(new QToolBar(this))
    , m_model(new SettingsAppletModel(this))
    , m_appletActionGroup(new QActionGroup(this))
{
    setMaximumWidth(m_homeWidth);
    setMinimumWidth(m_homeWidth);

    setupMenu();
    setupListView();
    QSettings settings;
    if(settings.contains("Cache/LastWindowPos"))
        restoreGeometry(settings.value("Cache/LastWindowPos").toByteArray());
    else
    {
        // fallback defaults
        resize(m_list->minimumSizeHint().height(),m_homeWidth);
        move(screen()->geometry().center() - frameGeometry().center());
    }
    showAll();
}

SettingsWindow::~SettingsWindow()
{
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    // preserve our view states
    QSettings settings;
    settings.setValue("Cache/LastWindowPos", saveGeometry());

    event->accept();
}

void SettingsWindow::menuAppletTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);

    showAppletWidget(m_appletActions.key(action));
}

void SettingsWindow::setSingleMode(bool single)
{
    m_menuBar->setVisible(!single);
    m_toolBar->setVisible(!single);
    m_single = single;
}

void SettingsWindow::clicked(const QModelIndex &index)
{
    if(!m_proxyModel->checkIndex(index))
        return;

    auto applet = m_model->applet(m_proxyModel->mapToSource(index));
    if(applet->init())
        showAppletWidget(applet);
}

void SettingsWindow::about()
{
    auto about = new HWAboutDialog(this);
    connect(about, SIGNAL(finished(int)), about, SLOT(deleteLater()));
    about->show();
}

void SettingsWindow::requestQuit()
{
    if(m_list == nullptr)
    {
        // we have an open settings page
        if(qobject_cast<SettingsAppletInterface*>(centralWidget())->canExit())
            qApp->quit();
    }
    qApp->quit();
}

void SettingsWindow::showAppletWidget(SettingsAppletInterface *applet)
{
    setWindowTitle(applet->name());
    setWindowIcon(applet->icon());
    m_showAll->setEnabled(true);

    if(!m_single)
    {
        auto wheight = applet->applet()->heightForWidth(m_homeWidth);
        auto animation = new QPropertyAnimation(this, "size");
        QSize start = size();
        QSize end = QSize(m_homeWidth, wheight+m_menuBar->height()+m_toolBar->height());
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->setDuration(190);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        m_appletActions.value(applet)->setChecked(true);
    }
    else
    {
        QSize sizeHint(m_homeWidth, applet->applet()->minimumHeight());
        setMaximumHeight(applet->applet()->minimumHeight());
        resize(sizeHint);
        qDebug() << "sizeHint" << sizeHint;
    }
    setCentralWidget(applet->applet());
    m_list = nullptr; // qmainwindow will destroy this
    applet->applet()->setFocus();
    m_current = applet;
}

void SettingsWindow::showAll()
{
    if(m_current != nullptr)
    {
        m_current->closeUp();
        m_current = nullptr;
    }
    setWindowTitle(tr("System Settings"));
    setWindowIcon(QIcon::fromTheme("preferences-system"));
    if(!m_list)
        setupListView();

    m_showAll->setEnabled(false);

    setCentralWidget(m_list);
    m_list->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_list->setMinimumWidth(size().width());
    auto animation = new QPropertyAnimation(this, "size");
    auto wheight = qMin(m_list->preferredSize().height()+70, 400);
    auto totalHeight = wheight+m_menuBar->height()+m_toolBar->height();
    QSize start = size();
    QSize end = QSize(m_homeWidth, totalHeight);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setDuration(120);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    m_toolBar->setFocus();
    setMaximumHeight(totalHeight);
}

void SettingsWindow::setupMenu()
{
    setMenuBar(m_menuBar);
    m_toolBar->setFloatable(false);
    m_toolBar->setMovable(false);
    m_toolBar->setContextMenuPolicy(Qt::NoContextMenu);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addToolBar(Qt::TopToolBarArea, m_toolBar);

    auto spacer = new QWidget(m_toolBar);
    auto p = spacer->palette();
    p.setColor(QPalette::All, QPalette::Base, QColor(0,0,0,0));
    spacer->setPalette(p);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    spacer->setMaximumHeight(3);

    m_search = new QLineEdit(this);
    auto *icon = new QIcon(QIcon::fromTheme("search"));
    m_search->addAction(*icon, QLineEdit::LeadingPosition);
    m_search->setPlaceholderText(tr("Search"));

    auto settings = m_menuBar->addMenu(tr("&Settings"));
    for(auto applet : m_model->applets())
    {
        auto action = new QAction(applet->icon(), applet->name());
        action->setCheckable(true);
        m_appletActionGroup->addAction(action);
        m_appletActions.insert(applet,action);
        connect(action, &QAction::triggered, this, &SettingsWindow::menuAppletTriggered);
        settings->addAction(action);
    }
    settings->addSeparator();
    auto quit = settings->addAction(tr("&Quit"));
    quit->setShortcut(QKeySequence::Quit);

    auto view = m_menuBar->addMenu(tr("&View"));
    auto back = view->addAction(tr("&Back"));
    back->setIcon(QIcon::fromTheme("go-previous"));
    back->setIconVisibleInMenu(false);
    back->setEnabled(false);
    back->setPriority(QAction::LowPriority);
    back->setShortcut(QKeySequence::Back);

    auto forward = view->addAction(tr("&Forward"));
    forward->setIcon(QIcon::fromTheme("go-next"));
    forward->setIconVisibleInMenu(false);
    forward->setEnabled(false);
    forward->setPriority(QAction::LowPriority);
    forward->setShortcut(QKeySequence::Forward);


    m_showAll = view->addAction(tr("&Show All"));
    m_showAll->setIcon(QIcon::fromTheme("view-grid"));
    m_showAll->setPriority(QAction::HighPriority);
    m_showAll->setIconVisibleInMenu(false);
    m_showAll->setIconText(tr("Show All"));
    m_showAll->setShortcut(QKeySequence("Ctrl+L"));
    auto help = m_menuBar->addMenu(tr("&Help"));
    help->addAction(tr("Get System &Help..."));
    auto about = help->addAction(tr("&About System Settings"));
    connect(about, &QAction::triggered, this, &SettingsWindow::about);
    m_toolBar->addAction(back);
    m_toolBar->addAction(forward);
    m_toolBar->addAction(m_showAll);

    m_toolBar->addWidget(spacer);
    m_toolBar->addWidget(m_search);

    connect(m_showAll, &QAction::triggered,
            this, &SettingsWindow::showAll);

    connect(quit, &QAction::triggered, this, &SettingsWindow::requestQuit);
}

void SettingsWindow::setupListView()
{
    m_list = new QCategorizedView(this);
    m_list->setViewMode(QListView::IconMode);
    m_list->setAlternatingBlockColors(true);
    m_list->setWordWrap(true);
    m_list->setUniformItemSizes(true);
    m_list->setCategoryDrawer(new QCategoryDrawerV3(m_list));
    m_list->setCategorySpacing(5);
    m_list->setFrameShape(QFrame::NoFrame);
    m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QPalette p = m_list->palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    m_list->setPalette(p);
    connect(m_list, &QListView::clicked,
            this, &SettingsWindow::clicked);
    m_list->hide();

    // borrowed from lxqt
    int iconSize = qBound(16, m_list->decorationSize().height(), 256);
    /* To have an appropriate grid size, we suppose that
     *
     * (1) The text has 3 lines and each line has 16 chars (for languages like German), at most;
     * (2) The selection rect has a margin of 2 px, at most;
     * (3) There is, at most, a 3-px spacing between text and icon; and
     * (4) There is a 4-px margin around each cell.
     */
    QFontMetrics fm = fontMetrics();
    int textWidth = fm.averageCharWidth() * 16;
    int textHeight = fm.lineSpacing() * 3;
    QSize grid;
    grid.setWidth(qMax(iconSize, textWidth) + 4);
    grid.setHeight(iconSize + textHeight + 4 + 3);
    m_list->setGridSize(grid + QSize(4, 4));
    //m_list->setCategorySpacing(8);
    m_proxyModel = new QCategorizedSortFilterProxyModel();
    m_proxyModel->setCategorizedModel(true);
    m_proxyModel->setSourceModel(m_model);
    m_list->setModel(m_proxyModel);
    m_list->setItemDelegate(new ConfigItemDelegate(m_list));
    m_list->show();
}

uint SettingsWindow::homeHeight() const
{
    return m_homeHeight;
}

void SettingsWindow::showAppletById(const QString &id)
{
    auto applet = m_model->appletForId(id);
    if(applet == nullptr)
        return;

    if(applet->init())
        showAppletWidget(applet);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(HOLLYWOOD_OS_VERSION);
    a.setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    a.setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    a.setApplicationName("System Settings");
    a.setWindowIcon(QIcon::fromTheme("preferences-system"));

    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood System Settings"));
    p.addHelpOption();
    p.addVersionOption();

    p.addOptions({
     {{"S", "single"},
         QCoreApplication::translate("main", "Start in single applet mode.")},
    });
    p.process(a);

    auto execapplet = QString();


    if(p.positionalArguments().count() > 0)
        execapplet = p.positionalArguments().first();

    bool single = false;
    if(p.isSet("single"))
        single = true;

    SettingsWindow w;
    w.setSingleMode(single);
    if(!execapplet.isEmpty())
        w.showAppletById(execapplet);
    w.show();

    return a.exec();
}
