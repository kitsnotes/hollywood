#include "chooser.h"
#include "wpmodel.h"
#include "shellfunc.h"
#include "wpapplet.h"

#include <hollywood/hollywood.h>

#include <QImageReader>
#include <QMimeType>
#include <QMimeDatabase>

WPChooserWidget::WPChooserWidget(QScreen *screen, QWidget *parent)
    : QWidget{parent}
    , m_screen(screen)
    , vl_main(new QVBoxLayout(this))
    , m_title(new QLabel(this))
    , m_cbo_wpstyle(new QComboBox(this))
    , m_wpcolor(new QToolButton(this))
    , m_lv_folders(new QListView(this))
    , m_lv_wpchoice(new QListView(this))
    , m_add_folder(new QToolButton(this))
    , m_remove_folder(new QToolButton(this))
    , m_rotate(new QCheckBox(this))
    , m_rotate_time(new QComboBox(this))
    , m_rotate_order(new QComboBox(this))
    , m_help(new QToolButton(this))
    , m_places(new QStandardItemModel(this))
    , m_colors(new WallpaperModel(QString(), this))
{
    QFile file(":/Colors");
    if(file.open(QFile::ReadOnly))
    {
        auto data = file.readAll().split('\n');
        for(QByteArray l : data)
        {
            auto vals = l.split(' ');
            auto color = vals.takeLast().toUpper();
            auto name = vals.join(' ');
            if(!name.isEmpty() && !color.isEmpty())
                m_colorList.append(QPair<QString,QString>(name,color));
        }
    }
    file.close();

    if(m_screen == QApplication::primaryScreen())
        m_primary = true;

    QSettings settings("originull", "hollywood");
    settings.beginGroup("PrimaryWallpaper");
    bool rotate = settings.value("Rotate", false).toBool();
    m_selectedFolder = settings.value("RotateFolder", "").toString();
    m_selectedFile = settings.value("Wallpaper").toString();
    m_selectedColor = settings.value("BackgroundColor", HOLLYWOOD_DEF_DESKTOP_BG).toString().toUpper();
    // TODO: verify file and color are sane
    auto dsm = settings.value("DisplayMode", 0).toUInt();
    if(dsm > 3)
        dsm = 0;
    int rotateTime = settings.value("RotateTime", 6).toInt();
    int rotateMode = settings.value("RotateMode", 0).toInt();

    setupUi();
    setupDirectories();
    connect(m_lv_wpchoice->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &WPChooserWidget::wpSelectionChanged);
    m_cbo_wpstyle->setCurrentIndex(dsm);
    m_rotate->setChecked(rotate);
    m_rotate_time->setCurrentIndex(rotateTime);
    m_rotate_order->setCurrentIndex(rotateMode);
    m_rotate_time->setEnabled(rotate);
    m_rotate_order->setEnabled(rotate);
    updateDisplay();
}

bool WPChooserWidget::primaryScreen() const
{
    return m_primary;
}

QString WPChooserWidget::selectedColor() const
{
    return m_selectedColor;
}

QString WPChooserWidget::selectedWallpaperFile() const
{
    return m_selectedFile;
}

QString WPChooserWidget::selectedFolder() const
{
    return m_selectedFolder;
}

uint WPChooserWidget::displayMode() const
{
    return m_cbo_wpstyle->currentIndex();
}

bool WPChooserWidget::rotate() const
{
    return m_rotate->isChecked();
}

uint WPChooserWidget::rotateMode() const
{
    return m_rotate_order->currentIndex();
}

uint WPChooserWidget::rotateTime() const
{
    return m_rotate_time->currentIndex();

}

QScreen* WPChooserWidget::screen() const
{
    return const_cast<QScreen*>(m_screen);
}

QString WPChooserWidget::matchColor(QString color)
{
    // see if our color string (hex code) is in the list

    for(QPair<QString,QString> i : m_colorList)
    {
        if(i.second == color)
            return i.first;
    }
    return tr("Custom Color");
}

void WPChooserWidget::setupUi()
{
    auto hl_preview_top = new QHBoxLayout();
    auto hl_wp_preview = new QHBoxLayout();
    auto hl_space_top = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    auto m_wp_preview_frame = new QFrame(this);
    auto previewlayout = new QHBoxLayout(m_wp_preview_frame);
    auto horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    auto vl_previewname = new QVBoxLayout();
    auto hl_chooser = new QHBoxLayout();
    auto hl_bottom = new QHBoxLayout();
    auto btn_sp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto rotate_sp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto previewspacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    auto chooser_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hl_views = new QHBoxLayout();
    auto vl_left_tree = new QVBoxLayout();
    auto hl_lefttree_btns = new QHBoxLayout();

    setMinimumSize(QSize(50, 550));
    setMaximumSize(QSize(16777215, 550));

    // setup our top preview
    hl_preview_top->setSpacing(2);
    hl_preview_top->setSizeConstraint(QLayout::SetDefaultConstraint);
    hl_wp_preview->setSpacing(0);
    hl_wp_preview->addItem(hl_space_top);
    m_wp_preview_frame->setMaximumSize(QSize(180, 120));
    m_wp_preview_frame->setFrameShape(QFrame::StyledPanel);
    m_wp_preview_frame->setFrameShadow(QFrame::Raised);
    previewlayout->setSpacing(0);
    previewlayout->setContentsMargins(5, 5, 5, 5);
    m_preview = new QGraphicsView(m_wp_preview_frame);
    previewlayout->addWidget(m_preview);
    hl_wp_preview->addWidget(m_wp_preview_frame);
    hl_wp_preview->addItem(horizontalSpacer_3);
    hl_wp_preview->setStretch(1, 1);
    hl_preview_top->addLayout(hl_wp_preview);
    vl_previewname->addWidget(m_title);

    m_cbo_wpstyle->setMinimumSize(QSize(185, 0));
    hl_chooser->addWidget(m_cbo_wpstyle);
    hl_chooser->addWidget(m_wpcolor);

    m_cbo_wpstyle->addItems(QStringList()
                            << "Fill Screen"
                            << "Fit to Screen"
                            << "Stretch to Fill"
                            << "Center");

    hl_chooser->addItem(chooser_spacer);
    vl_previewname->addLayout(hl_chooser);
    vl_previewname->addItem(previewspacer);
    hl_preview_top->addLayout(vl_previewname);
    hl_preview_top->setStretch(1, 1);

    // setup our layout for our two views
    hl_views->setSpacing(2);
    vl_left_tree->setSpacing(0);
    m_lv_folders->setMaximumSize(QSize(200, 16777215));
    vl_left_tree->addWidget(m_lv_folders);
    hl_lefttree_btns->setSpacing(0);
    vl_left_tree->addLayout(hl_lefttree_btns);
    hl_views->addLayout(vl_left_tree);
    hl_views->addWidget(m_lv_wpchoice);
    hl_views->setStretch(1, 1);

    m_add_folder->setIcon(QIcon::fromTheme("list-add"));
    m_add_folder->setIconSize(QSize(22,22));
    m_remove_folder->setIcon(QIcon::fromTheme("list-remove"));
    m_remove_folder->setIconSize(QSize(22,22));
    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(22,22));

    connect(m_help, &QToolButton::pressed, this, []() {
        hw_shell::openHelpTopic(QString(HW_APP_HELP_TOPIC));
    });

    m_add_folder->setEnabled(false);
    m_rotate->setEnabled(false);

    // setup our bottom layout of options
    hl_bottom->addWidget(m_add_folder);
    hl_bottom->addWidget(m_remove_folder);
    hl_bottom->addItem(btn_sp);
    hl_bottom->addWidget(m_rotate);
    hl_bottom->addWidget(m_rotate_time);
    hl_bottom->addWidget(m_rotate_order);
    hl_bottom->addItem(rotate_sp);
    hl_bottom->addWidget(m_help);

    auto vs_end = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

    // setup our main layout
    vl_main->setSpacing(2);
    vl_main->setContentsMargins(9,9,9,9);
    vl_main->addLayout(hl_preview_top);
    vl_main->addLayout(hl_views);
    vl_main->addLayout(hl_bottom);
    vl_main->addItem(vs_end);
    vl_main->setStretch(1, 1);

    m_add_folder->setText(QLatin1String("+"));
    m_add_folder->setToolTip(tr("Add a folder for wallpaper selection"));

    m_title->setText(tr("No Wallpaper"));
    m_rotate->setText(tr("Rotate Wallpaper:"));
    m_rotate_time->addItems(QStringList()
            <<  tr("When I login")
            <<  tr("When I login and wake up from sleep")
            <<  tr("Every minute")
            <<  tr("Every 5 minutes")
            <<  tr("Every 10 minutes")
            <<  tr("Every 30 minutes")
            <<  tr("Every hour")
            <<  tr("Every 2 hours")
            <<  tr("Every 4 hours"));

    m_rotate_order->addItems(QStringList()
                            << tr("in order")
                            << tr("randomly"));

    m_lv_folders->setModel(m_places);

    connect(m_lv_folders->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &WPChooserWidget::folderRowChanged);

    connect(m_add_folder, &QToolButton::triggered, this, &WPChooserWidget::addNewFolder);
    connect(m_remove_folder, &QToolButton::triggered, this, &WPChooserWidget::removeSelectedFolder);

    connect(m_rotate, &QCheckBox::toggled, this, &WPChooserWidget::rotateChecked);
    connect(m_wpcolor, &QToolButton::triggered, this, &WPChooserWidget::selectCustomColor);

    connect(m_cbo_wpstyle, &QComboBox::currentIndexChanged,
            this, &WPChooserWidget::layoutStyleComboChanged);

    connect(m_rotate_time, &QComboBox::currentIndexChanged,
        this, &WPChooserWidget::wallpaperChanged);

    connect(m_rotate_order, &QComboBox::currentIndexChanged,
            this, &WPChooserWidget::wallpaperChanged);

}

void WPChooserWidget::setupDirectories()
{
    // see what kind of image formats we can support
    QMimeDatabase mime;
    auto support = QImageReader::supportedMimeTypes();
    // to get these two stock appearing in translations
    auto tr1 = tr("Originull Wallpapers");
    auto tr2 = tr("Scenes of California");

    QDir dir("/usr/share/wallpapers");
    for(auto di : dir.entryInfoList(QDir::Dirs|QDir::Executable|QDir::NoDotAndDotDot, QDir::Name))
    {
        auto d = di.fileName();
        if(d == tr1.toUtf8().data())
            d = tr(d.toUtf8().data());

        if(d == tr2.toUtf8().data())
            d = tr(d.toUtf8().data());

        bool canPlace = false;
        QDir check(di.canonicalFilePath());
        for(auto f : check.entryInfoList(QDir::Files))
        {
            if(support.contains(mime.mimeTypeForFile(f).name()))
            {
                canPlace = true;
                break;
            }
        }

        if(canPlace)
        {
            auto model = new WallpaperModel(di.canonicalFilePath(), this);
            m_directories.insert(di.absoluteFilePath(), model);
            auto si = new QStandardItem(QIcon::fromTheme("folder"), d);
            si->setData(777); // magic number for a non-removable element
            si->setData(di.canonicalFilePath(), Qt::UserRole+2);
            m_places->appendRow(si);
            m_placelist.append(si);
            if(!m_selectedFile.isEmpty())
            {
                auto arr = m_selectedFile.split('/');
                arr.removeLast();
                auto dir = arr.join('/');
                if(dir == di.absoluteFilePath())
                {
                    m_lv_folders->selectionModel()->select(m_places->indexFromItem(si), QItemSelectionModel::ClearAndSelect);
                    m_lv_wpchoice->setModel(model);
                    m_remove_folder->setDisabled(true);
                    m_lv_wpchoice->selectionModel()->select(model->indexForItem(m_selectedFile), QItemSelectionModel::ClearAndSelect);
                    m_selectedFolder = model->selectedDirectory();

                }
            }
        }
    }

    auto cgi = new QStandardItem(QIcon::fromTheme("color-management"),
                                             tr("Colors & Gradients"));
    cgi->setData(777); // magic number for a non-removable element
    cgi->setData("COLOR", Qt::UserRole+2);
    m_places->appendRow(cgi);
    m_placelist.append(cgi);
    if(m_selectedFile.isEmpty())
    {
        m_lv_folders->selectionModel()->select(m_places->indexFromItem(cgi), QItemSelectionModel::ClearAndSelect);
        m_lv_wpchoice->setModel(m_colors);
        m_cbo_wpstyle->setVisible(false);
        m_remove_folder->setDisabled(true);
        m_lv_wpchoice->selectionModel()->select(m_colors->indexForItem(m_selectedColor), QItemSelectionModel::ClearAndSelect);
    }

    // TODO: customized directories
}

void WPChooserWidget::intWallpaperChanged()
{
    // re-draw in our preview
    updateDisplay();
    // emit our signal for our host to
    // save data to our settings
    Q_EMIT wallpaperChanged();
}

void WPChooserWidget::updateDisplay()
{
    if(m_selectedFile.isEmpty())
    {
        // display a solid color
        m_title->setText(matchColor(m_selectedColor));
        m_cbo_wpstyle->setVisible(false);
        m_wpcolor->setText(QString(tr("Select Custom Color...")));
        m_wpcolor->setIcon(QIcon());
    }
    else
    {
        // display our wallpaper file
        QFileInfo f(m_selectedFile);
        auto fn = f.fileName();
        auto fns = fn.split('.');
        fns.removeLast();
        fn = fns.join('.');

        m_title->setText(fn);
        m_cbo_wpstyle->setVisible(true);
        m_wpcolor->setText(QString(""));
        auto ci = createColorIcon(QColor(m_selectedColor));
        m_wpcolor->setIcon(ci);
        m_wpcolor->setIconSize(QSize(18,18));
    }
}


void WPChooserWidget::folderRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    // our selection of folders (on the left) has changed
    // if it's invalid select our old one
    if(!current.isValid())
        return m_lv_folders->selectionModel()->select(previous, QItemSelectionModel::ClearAndSelect);

    // else let's deal with changing our model;
    auto val = m_places->itemFromIndex(current);
    if(val->data().toInt() == 777)
        m_remove_folder->setDisabled(true);
    else
        m_remove_folder->setDisabled(false);

    auto file = val->data(Qt::UserRole+2).toString();

    if(file == QString("COLOR"))
    {
        m_lv_wpchoice->setModel(m_colors);
        connect(m_lv_wpchoice->selectionModel(), &QItemSelectionModel::currentRowChanged,
                this, &WPChooserWidget::wpSelectionChanged);
        if(m_selectedFile.isEmpty())
        {
            m_lv_wpchoice->selectionModel()->select(m_colors->indexForItem(m_selectedColor), QItemSelectionModel::ClearAndSelect);
        }
    }
    else
    {
        m_lv_wpchoice->setModel(m_directories[file]);
        connect(m_lv_wpchoice->selectionModel(), &QItemSelectionModel::currentRowChanged,
                this, &WPChooserWidget::wpSelectionChanged);
        if(!m_selectedFile.isEmpty())
        {
            if(m_directories[file]->hasItem(m_selectedFile))
                 m_lv_wpchoice->selectionModel()->select(m_directories[file]->indexForItem(m_selectedFile), QItemSelectionModel::ClearAndSelect);
        }
        m_selectedFolder = m_directories[file]->selectedDirectory();
    }
}

void WPChooserWidget::wpSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    // a new wallpaper image or color has been selected by the user
    // if it's invalid select our old one
    if(!current.isValid())
        return m_lv_wpchoice->selectionModel()->select(previous, QItemSelectionModel::ClearAndSelect);

    auto data = m_lv_wpchoice->model()->data(current, Qt::UserRole+1).toString();

    if(m_lv_wpchoice->model() == m_colors)
    {
        m_selectedColor = data;
        m_selectedFile = QString("");
        m_selectedFolder = QString("");
        m_cbo_wpstyle->setVisible(false);
        m_wpcolor->setVisible(true);
        m_wpcolor->setText(QString(tr("Select Custom Color...")));
    }
    else
    {
        m_wpcolor->setText(QString(""));
        m_selectedFile = data;
        m_selectedFolder = qobject_cast<WallpaperModel*>(m_lv_wpchoice->model())->selectedDirectory();
        m_cbo_wpstyle->setVisible(true);
        layoutStyleComboChanged(m_cbo_wpstyle->currentIndex());
    }
    intWallpaperChanged();

}

void WPChooserWidget::rotateChecked(bool rotate)
{
    m_rotate_time->setEnabled(rotate);
    m_rotate_order->setEnabled(rotate);
    Q_EMIT wallpaperChanged();
}

void WPChooserWidget::addNewFolder()
{

}

void WPChooserWidget::removeSelectedFolder()
{

}

void WPChooserWidget::selectCustomColor()
{

}

void WPChooserWidget::layoutStyleComboChanged(int val)
{
    m_wpcolor->setText(QString(""));
    switch(val)
    {
    case 2:
    case 3:
        m_wpcolor->setVisible(true);
        break;
    case 0:
    case 1:
    default:
        m_wpcolor->setVisible(false);
    }
    Q_EMIT wallpaperChanged();
}
