#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <hollywood/appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QWidget>
#include <QComboBox>
#include <QTreeView>
#include <QLineEdit>
#include <QSortFilterProxyModel>

class LSMimeApplications;
class MimeTypeModel;
class DefaultsApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit DefaultsApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings() { return true; }
    bool canExit() const { return true; }
    bool closeUp() { delete m_host; m_host = nullptr; return true; }
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
private:
    void setupWidget();
    LSMimeApplications *m_mime = nullptr;
    QWidget* m_host = nullptr;
    QVBoxLayout *m_mainlayout = nullptr;
    QFormLayout *m_layout = nullptr;

    QComboBox *m_browser = nullptr;
    QComboBox *m_email = nullptr;
    QComboBox *m_terminal = nullptr;
    QLineEdit *m_filter = nullptr;
    QTreeView *m_mimes = nullptr;

    MimeTypeModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxy = nullptr;
};

#endif // DEFAULTS_H
