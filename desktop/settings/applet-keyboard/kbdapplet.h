#ifndef KBDAPPLET_H
#define KBDAPPLET_H

#include <hollywood/appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QSlider>
#include <QFormLayout>
#include <QComboBox>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QListWidget>

#define HW_APP_HELP_TOPIC "settings/22-keyboard"

struct LayoutVariantInfo {
  QString name;
  QString description;
  LayoutVariantInfo(QString _name, QString desc): name(_name), description(desc) {
  }
};

struct KeyboardLayoutInfo {
  QString description;
  QList<LayoutVariantInfo> variants;

  KeyboardLayoutInfo(QString desc = QString()): description(desc) {
  }

  const LayoutVariantInfo* findVariant(QString name) const {
    if(!name.isEmpty()) {
      for(const LayoutVariantInfo& vinfo : qAsConst(variants)) {
        if(vinfo.name == name)
          return &vinfo;
      }
    }
    return nullptr;
  }
};

class LayoutSelector;
class KeyboardApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet-kbd.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    enum ListSection{
      NoSection,
      ModelSection,
      LayoutSection,
      VariantSection,
      OptionSection
    };
    explicit KeyboardApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings();
    bool canExit() const { return true; }
    bool closeUp() { delete m_host; m_host = nullptr; return true; }
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
    QString helpTopic() const  { return QString(HW_APP_HELP_TOPIC); }
    QStringList searchTokens() const;
private slots:
    void addLayout();
    void removeLayout();
    void moveLayoutUp();
    void moveLayoutDown();
    void widgetChanged();
    void layoutSelectionChanged();
private:
    void setupWidget();
    void loadXkbLayouts();
    bool hasKeyboardBacklight();
    QWidget* m_host = nullptr;
    QVBoxLayout *m_hostLayout;
    QTabWidget *m_tabs;
    QWidget *tabKeyboard;
    QVBoxLayout *m_layoutKeyboard;
    QSlider *m_repeat;
    QSlider *m_delay;
    QCheckBox *m_numlock;
    QCheckBox *m_backlight;
    QCheckBox *m_useFunction;
    QComboBox *keyboardModel;

    // layout tab
    QWidget *tabLayout;
    QFormLayout *fl_tab_layout;
    QTreeWidget *m_layouts;
    QPushButton *m_addLayout;
    QPushButton *m_removeLayout;
    QPushButton *m_moveUp;
    QPushButton *m_moveDown;
    QComboBox *switchKey;

    // shortcuts tab
    QWidget *tabSc;
    QVBoxLayout *vl_sc;
    QSplitter *m_splitter;
    QListWidget *m_category;
    QTreeView *m_shortcuts;
    QPushButton *m_restoredefaults;
    QPushButton *m_change;

    QList<QPair<QString, QString> > currentLayouts_;
    QMap<QString, KeyboardLayoutInfo> knownLayouts_;

    LayoutSelector *m_layoutSelector = nullptr;
};

#endif // KBDAPPLET_H
