#include "logviewer.h"

LogViewer::LogViewer(QWidget *parent)
    : QWidget(parent),
      vl_main(new QVBoxLayout(this)),
      m_splitter(new QSplitter(this)),
      m_sidebar(new QTreeView(m_splitter)),
      m_log(new QTextBrowser(m_splitter))
{
    vl_main->setSpacing(0);
    vl_main->setContentsMargins(0, 0, 0, 0);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(m_sidebar);
    m_splitter->addWidget(m_log);

    vl_main->addWidget(m_splitter);
}
