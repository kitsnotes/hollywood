#ifndef WPCHOOSERWIDGET_H
#define WPCHOOSERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtCore/QDir>
#include <QFile>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QSettings>
#include <QScreen>

inline QIcon createColorIcon(const QColor &color)
{
    QPixmap pm(100,100);
    pm.fill(color);
    QIcon i(pm);
    return i;
}

class WallpaperModel;
class WPChooserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WPChooserWidget(QScreen *screen, QWidget *parent = nullptr);
    QString selectedColor() const;
    QString selectedWallpaperFile() const;
    QString selectedFolder() const;
    uint displayMode() const;
    bool rotate() const;
    uint rotateMode() const;
    uint rotateTime() const;

    bool primaryScreen() const;
    QScreen* screen() const;
    QString matchColor(QString color);
signals:
    void wallpaperChanged();
private:
    void setupUi();
    void setupDirectories();
    void intWallpaperChanged();
    void updateDisplay();
private slots:
    void folderRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void wpSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void rotateChecked(bool rotate);
    void addNewFolder();
    void removeSelectedFolder();
    void selectCustomColor();
    void layoutStyleComboChanged(int);
private:
    bool m_primary = false;
    QScreen *m_screen;
    QString m_selectedFile;
    QString m_selectedColor;
    QString m_selectedFolder;
    // UI Objects
    QVBoxLayout *vl_main;
    QGraphicsView *m_preview;
    QLabel *m_title;
    QComboBox *m_cbo_wpstyle;
    QToolButton *m_wpcolor;
    QListView *m_lv_folders;
    QListView *m_lv_wpchoice;
    QToolButton *m_add_folder;
    QToolButton *m_remove_folder;
    QCheckBox *m_rotate;
    QComboBox *m_rotate_time;
    QComboBox *m_rotate_order;
    QToolButton *m_help;
    // Models etc
    QStandardItemModel *m_places;
    QList<QStandardItem*> m_placelist;
    WallpaperModel *m_colors;
    QMap<QString,WallpaperModel*> m_directories;
    QList<QPair<QString,QString>> m_colorList;
};

#endif // WPCHOOSERWIDGET_H
