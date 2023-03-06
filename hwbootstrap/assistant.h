#ifndef SETUPASSISTANT_H
#define SETUPASSISTANT_H

#include <QWizard>
#include <QWidget>
#include <QSplitter>
#include <QCheckBox>
#include <QTextBrowser>

class ThemedPage : public QWizardPage
{
    Q_OBJECT
public:
    ThemedPage(QWidget *parent = 0) : QWizardPage(parent) {}
    void loadWatermark(std::string page);
};

class PartitionPage : public ThemedPage
{
    Q_OBJECT
public:
    PartitionPage(QWidget *parent = nullptr);
private:
    QSplitter *m_splitter = nullptr;
};

class SettingsPage : public ThemedPage
{
    Q_OBJECT
public:
    SettingsPage(QWidget *parent = nullptr);
    QCheckBox *m_keyboard = nullptr;
    QCheckBox *m_mouse = nullptr;
    QCheckBox *m_display = nullptr;
    QCheckBox *m_icc = nullptr;
    QCheckBox *m_appearance = nullptr;
    QCheckBox *m_wallpaper = nullptr;
    QCheckBox *m_regional = nullptr;
};

class ReviewPage : public ThemedPage
{
    Q_OBJECT
public:
    ReviewPage(QWidget *parent = nullptr);
private:
    QTextBrowser *m_review;
};

class SetupAssistant : public QWizard
{
    Q_OBJECT
public:
    SetupAssistant(QWidget *parent = nullptr);
private:
    PartitionPage *m_disk = nullptr;
    SettingsPage *m_settings = nullptr;

    ReviewPage *m_review = nullptr;
};

#endif // SETUPASSISTANT_H
