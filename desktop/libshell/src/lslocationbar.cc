#include "lslocationbar.h"
#include "filesystemmodel.h"

LSOldLocationBar::LSOldLocationBar(FilesystemModel *model, QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QStackedLayout(this)),
      m_model(model),
      m_completer(new QCompleter(model, this)),
      m_location(new QComboBox(this)),
      m_buttonBarHost(new QWidget(this))
{
    m_location->setEditable(true);
    m_location->setPlaceholderText(tr("Type a path or URL"));
    m_location->setCompleter(m_completer);
    m_location->addItem("");

    connect(m_location->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(editOnReturn()));

    m_mainLayout->addWidget(m_location);
    m_mainLayout->addWidget(m_buttonBarHost);
}

QUrl LSOldLocationBar::currentUrl() const
{
    return m_currentUrl;
}

void LSOldLocationBar::setCurrentUrl(const QUrl &url)
{
    m_currentUrl = url;
    emit currentUrlChanged(url);
    updateWidgetForUrl();
}

QIcon LSOldLocationBar::iconForUrl(const QUrl &url)
{
    if(url.isValid() && url.isLocalFile())
    {
        QModelIndex idx = m_model->index(url.toLocalFile());
        //return m_model->icon(idx);
    }

    return QIcon();
}

void LSOldLocationBar::updateWidgetForUrl()
{
    m_location->setItemText(0, m_currentUrl.toString());
    m_location->setItemIcon(0, iconForUrl(m_currentUrl));
    m_location->setCurrentIndex(0);
}

void LSOldLocationBar::editOnReturn()
{
    QString text = m_location->lineEdit()->text();
    qDebug() << "editTextChanged:" << text;
    QUrl url = QUrl::fromLocalFile(text);
    if(url.isValid())
    {
        emit navigationRequested(url);
    }
}
