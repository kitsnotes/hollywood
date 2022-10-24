#include "clipboardwindow.h"

ClipboardWindow::ClipboardWindow(QWidget *parent)
    : QMainWindow(parent),
      m_status(new QStatusBar(this))
{
    setWindowTitle(tr("Clipboard"));
    setWindowIcon(QIcon());

    setStatusBar(m_status);

    FMApplication *app = FMApplication::instance();
    connect(app->clipboard(), SIGNAL(dataChanged()),
            this, SLOT(clipboardDataChanged()));
}

ClipboardWindow::~ClipboardWindow()
{
    disconnect(this, SLOT(clipboardDataChanged()));
}

void ClipboardWindow::clipboardDataChanged()
{

}
