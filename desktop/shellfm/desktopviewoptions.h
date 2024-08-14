#ifndef DESKTOPVIEWOPTIONS_H
#define DESKTOPVIEWOPTIONS_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QListView>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPushButton>

class DesktopViewOptions : public QDialog
{
    Q_OBJECT
public:
    explicit DesktopViewOptions(QWidget *parent = nullptr);
    void updateViewOptions(QListView *view);
    uint iconSize() const;
    uint spacing() const;
    uint textsize() const;
signals:
    void optionsChanged();
private slots:
    void updateIconSizeLabel();
    void updateSpacingLabel();
private:
    QVBoxLayout *layout_Main = nullptr;
    QComboBox *m_Organize = nullptr;
    QLabel *m_IconSizeLabel = nullptr;
    QLabel *m_GridSpacingLabel = nullptr;
    QSlider *m_IconSizing = nullptr;
    QSlider *m_GridSizing = nullptr;
    QComboBox *m_TextSize = nullptr;
    QRadioButton *m_lblBottom = nullptr;
    QRadioButton *m_lblRight = nullptr;
    QCheckBox *m_iconPreview = nullptr;
    QCheckBox *m_itemInfo = nullptr;
    QPushButton *m_setDefaults = nullptr;
};

#endif // DESKTOPVIEWOPTIONS_H
