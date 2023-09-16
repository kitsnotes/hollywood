#include "helpwindow.h"
#include "application.h"

#include <QHelpContentWidget>
#include <QMessageBox>
#include <QActionGroup>
#include <QApplication>
#include <QStatusBar>

HelpWindow::HelpWindow(QString helpFile, QWidget *parent)
    : QMainWindow(parent)
    , m_menubar(new QMenuBar(this))
    , m_toolbar(new QToolBar(this))
    , m_splitter(new QSplitter(this))
    , m_dummy(new QWidget(m_splitter))
    , m_html(new QLiteHtmlWidget(m_splitter))
    , m_status(new QStatusBar(this))
    , m_helpPath(helpFile)
{
    setWindowTitle(tr("Hollywood Help"));
    setMinimumSize(QSize(640,480));
    setCentralWidget(m_splitter);

    auto file = m_menubar->addMenu(tr("&File"));
    auto print = file->addAction(tr("&Print"));
    print->setIcon(QIcon::fromTheme("file-print"));
    print->setIconVisibleInMenu(false);
    print->setShortcut(QKeySequence::Print);
    print->setEnabled(false);

    auto edit = m_menubar->addMenu(tr("&Edit"));
    auto copy = edit->addAction(tr("&Copy Selection"));
    copy->setIcon(QIcon::fromTheme("edit-copy"));
    copy->setShortcut(QKeySequence::Copy);
    copy->setIconVisibleInMenu(false);

    auto view = m_menubar->addMenu(tr("&View"));

    m_contents = view->addAction(tr("&Contents"));
    m_contents->setIcon(QIcon::fromTheme("view-list-tree"));
    m_contents->setIconVisibleInMenu(false);
    m_contents->setCheckable(true);
    m_index = view->addAction(tr("&Index"));
    m_index->setIcon(QIcon::fromTheme("view-list-text"));
    m_index->setIconVisibleInMenu(false);
    m_index->setCheckable(true);
    m_index->setEnabled(false);

    m_ag = new QActionGroup(this);
    m_ag->setExclusive(true);
    m_ag->addAction(m_contents);
    m_ag->addAction(m_index);

    auto help = m_menubar->addMenu(tr("&Help"));
    auto about = help->addAction(tr("&About Help Viewer"));
    about->setEnabled(false);

    m_toolbar->setMovable(false);
    m_toolbar->addAction(m_contents);
    m_toolbar->addAction(m_index);

    setMenuBar(m_menubar);
    setStatusBar(m_status);
    addToolBar(m_toolbar);

    m_splitter->addWidget(m_dummy);
    m_splitter->addWidget(m_html);

    QList<int> sizes;
    sizes << 125 << 300;
    m_splitter->setSizes(sizes);

    loadHelp();
}

HelpWindow::~HelpWindow()
{
}

void HelpWindow::loadHelp()
{
    m_help = new QHelpEngine(m_helpPath, this);
    m_help->setReadOnly(true);
    if(m_help->setupData())
    {
        //m_help->contentWidget()->setRootIsDecorated(false);
        m_splitter->replaceWidget(0, m_help->contentWidget());
        m_html->setResourceHandler([this](const QUrl &url) {
            return m_help->fileData(url);
        });
        connect(m_help->contentWidget(), &QHelpContentWidget::linkActivated,
                this, &HelpWindow::loadUrl);
        connect(m_html, &QLiteHtmlWidget::linkClicked,
                this, &HelpWindow::loadUrl);

        m_dummy->deleteLater();
        m_dummy = nullptr;
        m_contents->setChecked(true);
        openIndex();

        m_help->contentWidget()->expand(m_help->contentModel()->index(1,1));
    }
    else
    {
        QMessageBox::critical(this, tr("Error loading help file"),
                              tr("Hollywood Help could not load the specified help file: %1.<br>%2").arg(m_helpPath, m_help->error()));
        qApp->exit();
    }
}

void HelpWindow::openIndex()
{
    QString str("qthelp://%1/doc/index.html");
    QUrl url(str.arg(HW_GUIDE_URN));

    loadUrl(url);
}

void HelpWindow::loadUrl(const QUrl &url)
{
    if(!m_help)
        return;

    if(!m_html)
        return;

    auto data = m_help->fileData(url);
    m_html->setUrl(url);
    m_html->setHtml(QString::fromUtf8(data));

    m_help->contentWidget()->expandRecursively(m_help->contentWidget()->indexOf(url));
    m_help->contentWidget()->selectionModel()->select(m_help->contentWidget()->indexOf(url), QItemSelectionModel::ClearAndSelect);
    m_status->showMessage(url.toString());
}

