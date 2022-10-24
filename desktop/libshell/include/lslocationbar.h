#ifndef LSLOCATIONBAR_H
#define LSLOCATIONBAR_H

#include "libshell_int.h"
#include "shellhost.h"

class LSFSModel;
class LIBSHELL_EXPORT LSOldLocationBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QUrl currentUrl READ currentUrl WRITE setCurrentUrl NOTIFY currentUrlChanged);
public:
    explicit LSOldLocationBar(FilesystemModel *model, QWidget *parent = nullptr);
    QUrl currentUrl() const;
    void setCurrentUrl(const QUrl &url);
signals:
    void currentUrlChanged(const QUrl &url);
    void navigationRequested(const QUrl &url);
private slots:
    void editOnReturn();
private:
    QIcon iconForUrl(const QUrl &url);
    void updateWidgetForUrl();
private:
    QUrl m_currentUrl;
    QStackedLayout *m_mainLayout;
    QIcon m_currentIcon;
    LSFSModel *m_model;
    QCompleter *m_completer;
    QComboBox *m_location;
    QWidget *m_buttonBarHost;
};

#endif // LSLOCATIONBAR_H
