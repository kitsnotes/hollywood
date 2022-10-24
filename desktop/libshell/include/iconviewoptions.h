#ifndef ICONVIEWOPTIONS_H
#define ICONVIEWOPTIONS_H

#include "libshell_int.h"

class LIBSHELL_EXPORT IconViewOptions : public QWidget
{
    Q_OBJECT
public:
    explicit IconViewOptions(QWidget *parent = nullptr);
    void attachView(QListView *view);
    void refreshViewOptions();
signals:
private:
    void setupWidget();
private:
    QListView *m_view;
    QVBoxLayout *layout_Main;
    QHBoxLayout *layout_Organize;
    QLabel *l_Organize;
    QComboBox *m_Organize;
    QFrame *line;
    QLabel *l_IconSize;
    QHBoxLayout *layout_IconSize;
    QLabel *l_IconSzSmall;
    QSlider *m_IconSizing;
    QLabel *l_IconSzLarge;
    QLabel *l_GridSpacing;
    QHBoxLayout *horizontalLayout_2;
    QLabel *l_GridSzSmall;
    QSlider *m_GridSizing;
    QLabel *l_GridSzLarge;
    QFrame *line_2;
    QHBoxLayout *layout_TextSize;
    QLabel *l_TextSize;
    QComboBox *m_TextSize;
    QSpacerItem *spTextSize;
    QLabel *l_LabelPos;
    QHBoxLayout *layout_LabelPos;
    QRadioButton *m_lblBottom;
    QRadioButton *m_lblRight;
    QSpacerItem *spLabelPos;
    QFrame *line_3;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QHBoxLayout *layoutDefaults;
    QSpacerItem *sp_DefLeft;
    QPushButton *m_setDefaults;
    QSpacerItem *sp_DefRight;
    QSpacerItem *spBottom;
};

#endif // ICONVIEWOPTIONS_H
