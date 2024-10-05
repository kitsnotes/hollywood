#pragma once

#include <QListView>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class LSIconViewOptions;
class LSIconViewOptionsPrivate
{
private:
    friend class LSIconViewOptions;
    LSIconViewOptions *d;
    LSIconViewOptionsPrivate(LSIconViewOptions *parent);
    ~LSIconViewOptionsPrivate() = default;
    QListView *m_view;
    QVBoxLayout *layout_Main;
    QComboBox *m_Organize;
    QLabel *m_IconSizeLabel;
    QLabel *m_GridSpacingLabel;
    QSlider *m_IconSizing;
    QSlider *m_GridSizing;
    QComboBox *m_TextSize;
    QRadioButton *m_lblBottom;
    QRadioButton *m_lblRight;
    QCheckBox *m_iconPreview;
    QCheckBox *m_itemInfo;
    QPushButton *m_setDefaults;
};
