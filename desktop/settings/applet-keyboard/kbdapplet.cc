#include "kbdapplet.h"
#include <hollywood/hollywood.h>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include "layoutselector.h"

KeyboardApplet::KeyboardApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface() {}

bool KeyboardApplet::init()
{
    setupWidget();
    loadXkbLayouts();
    loadSettings();

    connect(m_repeat, &QSlider::sliderReleased, this, &KeyboardApplet::widgetChanged);
    connect(m_delay, &QSlider::sliderReleased, this, &KeyboardApplet::widgetChanged);
    connect(keyboardModel, &QComboBox::currentIndexChanged, this, &KeyboardApplet::widgetChanged);
    connect(m_useFunction, &QCheckBox::toggled, this, &KeyboardApplet::widgetChanged);
    connect(m_numlock, &QCheckBox::toggled, this, &KeyboardApplet::widgetChanged);
    connect(m_backlight, &QCheckBox::toggled, this, &KeyboardApplet::widgetChanged);

    connect(m_addLayout, &QPushButton::pressed, this, &KeyboardApplet::addLayout);
    connect(m_removeLayout, &QPushButton::pressed, this, &KeyboardApplet::removeLayout);
    connect(m_moveUp, &QPushButton::pressed, this, &KeyboardApplet::moveLayoutUp);
    connect(m_moveDown, &QPushButton::pressed, this, &KeyboardApplet::moveLayoutDown);

    connect(m_layouts->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &KeyboardApplet::layoutSelectionChanged);
    return true;
}

bool KeyboardApplet::loadSettings()
{
    QSettings settings("originull","hollywood");
    settings.beginGroup("Keyboard");
    auto kbd = settings.value("KeyboardModel", HOLLYWOOD_DEFAULT_KBD).toString();
    auto layouts = settings.value("Layouts", HOLLYWOOD_DEFAULT_KBDL).toStringList();
    auto variants = settings.value("Variants", HOLLYWOOD_DEFAULT_KBDV).toStringList();

    if(layouts.count() != variants.count())
    {
        layouts = QStringList() << HOLLYWOOD_DEFAULT_KBDL;
        variants = QStringList() << HOLLYWOOD_DEFAULT_KBDV;
    }

    qDebug() << layouts << variants;
    auto repeatint = settings.value("RepeatInterval", HOLLYWOOD_DEF_KBD_INT).toUInt();
    if(repeatint < HOLLYWOOD_MIN_KBD_INT)
        repeatint = HOLLYWOOD_MIN_KBD_INT;
    if(repeatint > HOLLYWOOD_MAX_KBD_INT)
        repeatint = HOLLYWOOD_MAX_KBD_INT;

    auto delay = settings.value("RepeatDelay", HOLLYWOOD_DEF_KBD_DELAY).toUInt();
    if(delay > HOLLYWOOD_MAX_KBD_DELAY)
        delay = HOLLYWOOD_MAX_KBD_DELAY;
    if(delay < HOLLYWOOD_MIN_KBD_DELAY)
        delay = HOLLYWOOD_MIN_KBD_DELAY;
    auto numlock = settings.value("NumLockLogin", false).toBool();
    auto bl = settings.value("AutoBacklight", true).toBool();
    auto altkey = settings.value("AltFunction", true).toBool();

    settings.endGroup();

    m_repeat->setValue(repeatint);
    m_delay->setValue(delay);
    m_numlock->setChecked(numlock);
    m_backlight->setChecked(bl);
    m_useFunction->setChecked(altkey);
    for(int i = 0; i < keyboardModel->count(); i++)
    {
        if(keyboardModel->itemData(i) == kbd)
            keyboardModel->setCurrentIndex(i);
    }

    for(int i = 0; i < layouts.count(); i++)
    {
        auto layout = layouts.at(i);
        auto variant = variants.at(i);
        qDebug() << layout << variant;
        if(variant == "null")
            variant = "";

        if(!knownLayouts_.contains(layout))
            continue;
        const KeyboardLayoutInfo& info = knownLayouts_.value(layout);
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setData(0, Qt::DisplayRole, info.description);
        item->setData(0, Qt::UserRole, layout);
        QIcon ico = QIcon::fromTheme(QString("flag-us").arg(layout.toLower()), QIcon::fromTheme(("input-keyboard")));
        item->setIcon(0, ico);
        const LayoutVariantInfo* vinfo = info.findVariant(variant);
        if(vinfo)
        {
            item->setData(1, Qt::DisplayRole, vinfo->description);
            item->setData(1, Qt::UserRole, variant);
        }
        m_layouts->addTopLevelItem(item);
    }
    return true;
}

bool KeyboardApplet::saveSettings()
{
    QSettings settings("originull","hollywood");
    settings.beginGroup("Keyboard");
    settings.setValue("KeyboardModel", keyboardModel->currentData().toString());
    settings.setValue("RepeatInterval", m_repeat->value());
    settings.setValue("RepeatDelay", m_delay->value());
    settings.setValue("NumLockLogin", m_numlock->isChecked());
    if(hasKeyboardBacklight())
       settings.setValue("AutoBacklight", m_backlight->isChecked());
    settings.setValue("AltFunction", m_useFunction->isChecked());

    int n = m_layouts->topLevelItemCount();
    QStringList layouts, variants;
    if(n > 0)
    {
        for(int row = 0; row < n; ++row)
        {
            QTreeWidgetItem* item = m_layouts->topLevelItem(row);
            layouts += item->data(0, Qt::UserRole).toString();
            auto data = item->data(1, Qt::UserRole).toString();
            variants += data.isEmpty() ? "null" : data;
        }
    }
    settings.setValue("Layouts", layouts);
    settings.setValue("Variants", variants);

    return true;
}

QString KeyboardApplet::id() const
{
    return QLatin1String("org.originull.keyboard");
}

QString KeyboardApplet::name() const
{
    return tr("Keyboard");
}

QString KeyboardApplet::description() const
{
    return tr("Keyboard & Input Methods");
}

QIcon KeyboardApplet::icon() const
{
    const QIcon mine(QIcon::fromTheme("preferences-desktop-keyboard"));
    return mine;
}

QWidget *KeyboardApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category KeyboardApplet::category() const
{
    return System;
}

void KeyboardApplet::addLayout()
{
    m_layoutSelector = new LayoutSelector(knownLayouts_, m_host);
    int result = m_layoutSelector->exec();
    if(result == 1)
    {
        auto layout = m_layoutSelector->selectedLayout();
        auto variant = m_layoutSelector->selectedVariant();
        const KeyboardLayoutInfo& info = knownLayouts_.value(layout);
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setData(0, Qt::DisplayRole, info.description);
        item->setData(0, Qt::UserRole, layout);
        QIcon ico = QIcon::fromTheme(QString("flag-%1").arg(layout.toLower()), QIcon::fromTheme(("input-keyboard")));
        item->setIcon(0, ico);
        const LayoutVariantInfo* vinfo = info.findVariant(variant);
        if(vinfo)
        {
            item->setData(1, Qt::DisplayRole, vinfo->description);
            item->setData(1, Qt::UserRole, variant);
        }
        m_layouts->addTopLevelItem(item);
        saveSettings();
        layoutSelectionChanged();
    }
    m_layoutSelector->deleteLater();
    m_layoutSelector = nullptr;
}

void KeyboardApplet::removeLayout()
{
    if(m_layouts->topLevelItemCount() > 1)
    {
        QTreeWidgetItem* item = m_layouts->currentItem();
        if(item)
        {
            delete item;
            saveSettings();
            layoutSelectionChanged();
        }
    }
}

void KeyboardApplet::moveLayoutUp()
{
    QTreeWidgetItem* item = m_layouts->currentItem();
    if(!item)
        return;
    int pos = m_layouts->indexOfTopLevelItem(item);
    if(pos > 0)
    {
        // not the first item
        m_layouts->takeTopLevelItem(pos);
        m_layouts->insertTopLevelItem(pos - 1, item);
        m_layouts->setCurrentItem(item);
        saveSettings();
    }
}

void KeyboardApplet::moveLayoutDown()
{
    QTreeWidgetItem* item = m_layouts->currentItem();
    if(!item)
        return;
    int pos = m_layouts->indexOfTopLevelItem(item);
    if(pos < m_layouts->topLevelItemCount() - 1)
    {
        // not the last item
        m_layouts->takeTopLevelItem(pos);
        m_layouts->insertTopLevelItem(pos + 1, item);
        m_layouts->setCurrentItem(item);
        saveSettings();
    }
}

void KeyboardApplet::widgetChanged()
{
    saveSettings();
}

void KeyboardApplet::layoutSelectionChanged()
{
    auto sm = m_layouts->selectionModel();
    if(!sm->selectedIndexes().isEmpty())
    {
        if(sm->model()->rowCount() > 1)
        {
            m_removeLayout->setDisabled(false);
            m_moveUp->setDisabled(false);
            m_moveDown->setDisabled(false);
            return;
        }
    }
    m_removeLayout->setDisabled(true);
    m_moveUp->setDisabled(true);
    m_moveDown->setDisabled(true);
}

void KeyboardApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("KeyboardAppletHost"));
    m_hostLayout = new QVBoxLayout(m_host);
    m_tabs = new QTabWidget(m_host);
    m_tabs->setTabShape(QTabWidget::Rounded);
    m_tabs->setUsesScrollButtons(false);

    tabKeyboard = new QWidget();
    m_layoutKeyboard = new QVBoxLayout(tabKeyboard);
    keyboardModel = new QComboBox(tabKeyboard);
    keyboardModel->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    auto hlkbd = new QHBoxLayout();
    auto l_kbd = new QLabel(tabKeyboard);
    hlkbd->addWidget(l_kbd, 0, Qt::AlignRight);
    hlkbd->addWidget(keyboardModel, 1);
    m_layoutKeyboard->addLayout(hlkbd);
    auto horizontalLayout = new QHBoxLayout();
    auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    auto vl_repeat = new QVBoxLayout();
    auto l_repeat = new QLabel(tabKeyboard);
    l_repeat->setAlignment(Qt::AlignCenter);

    vl_repeat->addWidget(l_repeat);

    m_repeat = new QSlider(tabKeyboard);
    m_repeat->setMinimumSize(QSize(120, 0));
    m_repeat->setOrientation(Qt::Horizontal);
    m_repeat->setTickPosition(QSlider::TicksBothSides);

    m_repeat->setRange(HOLLYWOOD_MIN_KBD_INT,HOLLYWOOD_MAX_KBD_INT);
    m_repeat->setTickInterval(20);
    m_repeat->setPageStep(10);

    vl_repeat->addWidget(m_repeat);

    auto hl_repeat = new QHBoxLayout();
    auto l_off = new QLabel(tabKeyboard);
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(l_off->sizePolicy().hasHeightForWidth());
    l_off->setSizePolicy(sizePolicy);

    hl_repeat->addWidget(l_off);

    auto l_rslow = new QLabel(tabKeyboard);
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(l_rslow->sizePolicy().hasHeightForWidth());
    l_rslow->setSizePolicy(sizePolicy1);

    hl_repeat->addWidget(l_rslow);

    auto l_rfast = new QLabel(tabKeyboard);
    l_rfast->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hl_repeat->addWidget(l_rfast);
    vl_repeat->addLayout(hl_repeat);

    horizontalLayout->addLayout(vl_repeat);

    auto hs_between = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

    horizontalLayout->addItem(hs_between);

    auto vl_delay = new QVBoxLayout();
    auto l_delay = new QLabel(tabKeyboard);
    l_delay->setAlignment(Qt::AlignCenter);

    vl_delay->addWidget(l_delay);

    m_delay = new QSlider(tabKeyboard);
    m_delay->setMinimumSize(QSize(120, 0));
    m_delay->setOrientation(Qt::Horizontal);
    m_delay->setTickPosition(QSlider::TicksBothSides);
    m_delay->setRange(HOLLYWOOD_MIN_KBD_DELAY, HOLLYWOOD_MAX_KBD_DELAY);
    m_delay->setTickInterval(100);
    m_delay->setPageStep(100);
    vl_delay->addWidget(m_delay);

    auto hl_delay = new QHBoxLayout();
    auto l_dslow = new QLabel(tabKeyboard);
    auto l_rfast_2 = new QLabel(tabKeyboard);
    auto horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_numlock = new QCheckBox(tabKeyboard);
    m_backlight = new QCheckBox(tabKeyboard);
    auto vs_end = new QSpacerItem(20, 24, QSizePolicy::Minimum, QSizePolicy::Expanding);
    auto vl_function = new QVBoxLayout();
    m_useFunction = new QCheckBox(tabKeyboard);
    auto lbl_function = new QLabel(tabKeyboard);
    lbl_function->setWordWrap(true);
    lbl_function->setIndent(25);
    hl_delay->addWidget(l_dslow);
    l_rfast_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    hl_delay->addWidget(l_rfast_2);
    vl_delay->addLayout(hl_delay);

    horizontalLayout->addLayout(vl_delay);
    horizontalLayout->addItem(horizontalSpacer_2);
    m_layoutKeyboard->addLayout(horizontalLayout);

    m_layoutKeyboard->addWidget(m_numlock);

    m_layoutKeyboard->addWidget(m_backlight);

    vl_function->addWidget(m_useFunction);

    vl_function->addWidget(lbl_function);
    m_layoutKeyboard->addLayout(vl_function);

    m_layoutKeyboard->addItem(vs_end);
    m_tabs->addTab(tabKeyboard, QString());

    // Create our tab "Layout"
    tabLayout = new QWidget();
    fl_tab_layout = new QFormLayout(tabLayout);
    fl_tab_layout->setObjectName(QString::fromUtf8("formLayout"));
    fl_tab_layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    auto hl_kbdmap = new QHBoxLayout();
    auto vl_kbdmap = new QVBoxLayout();
    auto spkbd = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    auto l_chghotkey = new QLabel(tabLayout);
    m_addLayout = new QPushButton(tabLayout);
    m_removeLayout = new QPushButton(tabLayout);
    m_moveUp = new QPushButton(tabLayout);
    m_moveDown = new QPushButton(tabLayout);
    m_layouts = new QTreeWidget(tabLayout);
    switchKey = new QComboBox(tabLayout);
    switchKey->addItem(QString());
    switchKey->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);


    QSizePolicy sp2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp2.setHorizontalStretch(1);
    sp2.setVerticalStretch(1);
    sp2.setHeightForWidth(m_layouts->sizePolicy().hasHeightForWidth());
    m_layouts->setSizePolicy(sp2);
    m_layouts->setDragEnabled(true);
    m_layouts->setDragDropMode(QAbstractItemView::InternalMove);
    m_layouts->setRootIsDecorated(false);
    m_layouts->setItemsExpandable(false);

    hl_kbdmap->addWidget(m_layouts);

    vl_kbdmap->addWidget(m_addLayout);
    vl_kbdmap->addWidget(m_removeLayout);
    vl_kbdmap->addWidget(m_moveUp);
    vl_kbdmap->addWidget(m_moveDown);
    vl_kbdmap->addItem(spkbd);
    vl_kbdmap->setStretch(4, 1);

    hl_kbdmap->addLayout(vl_kbdmap);

    hl_kbdmap->setStretch(0, 1);

    fl_tab_layout->setLayout(0, QFormLayout::SpanningRole, hl_kbdmap);
    fl_tab_layout->setWidget(2, QFormLayout::LabelRole, l_chghotkey);
    fl_tab_layout->setWidget(2, QFormLayout::FieldRole, switchKey);

    m_tabs->addTab(tabLayout, QString());

    // setup our "Shortcuts" tab
    tabSc = new QWidget(0);
    vl_sc = new QVBoxLayout(tabSc);
    m_splitter = new QSplitter(tabSc);
    m_category = new QListWidget(m_splitter);
    m_shortcuts = new QTreeView(m_splitter);
    m_restoredefaults = new QPushButton(tabSc);
    m_change = new QPushButton(tabSc);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(m_category);
    m_splitter->addWidget(m_shortcuts);

    vl_sc->addWidget(m_splitter);

    auto hl_sp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hl_sc = new QHBoxLayout();
    hl_sc->setSpacing(6);

    hl_sc->addItem(hl_sp);
    hl_sc->addWidget(m_restoredefaults);
    hl_sc->addWidget(m_change);
    vl_sc->addLayout(hl_sc);
    vl_sc->setStretch(0, 1);
    m_tabs->addTab(tabSc, QString());

    m_hostLayout->addWidget(m_tabs);
    m_tabs->setCurrentIndex(0);

    QTreeWidgetItem *___qtreewidgetitem = m_layouts->headerItem();
    ___qtreewidgetitem->setText(1, QCoreApplication::translate("KeyboardApplet", "Variant", nullptr));
    ___qtreewidgetitem->setText(0, QCoreApplication::translate("KeyboardApplet", "Layout", nullptr));
    m_addLayout->setText(QCoreApplication::translate("KeyboardApplet", "&Add", nullptr));
    m_removeLayout->setText(QCoreApplication::translate("KeyboardApplet", "&Remove", nullptr));
    m_moveUp->setText(QCoreApplication::translate("KeyboardApplet", "Up", nullptr));
    m_moveDown->setText(QCoreApplication::translate("KeyboardApplet", "Down", nullptr));
    l_kbd->setText(QCoreApplication::translate("KeyboardApplet", "Keyboard Model:", nullptr));
    l_chghotkey->setText(QCoreApplication::translate("KeyboardApplet", "Keys to change layout:", nullptr));
    switchKey->setItemText(0, QCoreApplication::translate("KeyboardApplet", "None", nullptr));

    m_restoredefaults->setText(QCoreApplication::translate("KeyboardApplet", "Restore Defaults", nullptr));
    m_change->setText(QCoreApplication::translate("KeyboardApplet", "Change Shortcut", nullptr));

    l_repeat->setText(QCoreApplication::translate("KeyboardApplet", "Repeat Interval", nullptr));
    l_off->setText(QCoreApplication::translate("KeyboardApplet", "Off", nullptr));
    l_rslow->setText(QCoreApplication::translate("KeyboardApplet", "Slow", nullptr));
    l_rfast->setText(QCoreApplication::translate("KeyboardApplet", "Fast", nullptr));
    l_delay->setText(QCoreApplication::translate("KeyboardApplet", "Delay Until Repeat", nullptr));
    l_dslow->setText(QCoreApplication::translate("KeyboardApplet", "Slow", nullptr));
    l_rfast_2->setText(QCoreApplication::translate("KeyboardApplet", "Fast", nullptr));
    m_numlock->setText(QCoreApplication::translate("KeyboardApplet", "Turn on NumLock key on login", nullptr));
    m_backlight->setText(QCoreApplication::translate("KeyboardApplet", "Automatically adjust keyboard backlight", nullptr));
    m_useFunction->setText(QCoreApplication::translate("KeyboardApplet", "Enable alternative function keys", nullptr));
    lbl_function->setText(QCoreApplication::translate("KeyboardApplet", "When this option is selected, press the fn key to use F1, F2, etc.", nullptr));
    m_tabs->setTabText(m_tabs->indexOf(tabKeyboard), QCoreApplication::translate("KeyboardApplet", "Keyboard", nullptr));
    m_tabs->setTabText(m_tabs->indexOf(tabLayout), QCoreApplication::translate("KeyboardApplet", "Layout", nullptr));
    m_tabs->setTabText(m_tabs->indexOf(tabSc), QCoreApplication::translate("KeyboardApplet", "Shortcuts", nullptr));

    // TODO: fix keyboard backlight adjustment
    if(!hasKeyboardBacklight())
        m_backlight->setVisible(false);

    m_removeLayout->setDisabled(true);
    m_moveUp->setDisabled(true);
    m_moveDown->setDisabled(true);

    // TODO: Temporary:
    l_chghotkey->setVisible(false);
    switchKey->setVisible(false);
    m_change->setDisabled(true);
    m_restoredefaults->setDisabled(true);
    m_shortcuts->setDisabled(true);
    m_category->setDisabled(true);
}

void KeyboardApplet::loadXkbLayouts()
{
    // from  lxqt-config/lxqt-config-input/keyboardlayoutconfig.cpp
    // LGPL 2.1
    QFile f(HOLLYWOOD_XKBDBL_PATH);
    if(f.open(QIODevice::ReadOnly))
    {
        ListSection section = NoSection;
        while(!f.atEnd()) {
            QByteArray line = f.readLine().trimmed();
            if(section == NoSection)
            {
                if(line.startsWith("! model"))
                    section = ModelSection;
                else if(line.startsWith("! layout"))
                    section = LayoutSection;
                else if(line.startsWith("! variant"))
                    section = VariantSection;
                else if(line.startsWith("! option"))
                    section = OptionSection;
            }
            else
            {
                if(line.isEmpty())
                {
                section = NoSection;
                continue;
                }
                int sep = line.indexOf(' ');
                QString name = QString::fromLatin1(line.constData(), sep);
                while(line[sep] ==' ') // skip spaces
                    ++sep;
                QString description = QString::fromUtf8(line.constData() + sep);

                switch(section)
                {
                case ModelSection:
                    keyboardModel->addItem(description, name);
                    break;
                case LayoutSection:
                    knownLayouts_[name] = KeyboardLayoutInfo(description);
                    break;
                case VariantSection:
                {
                    // the descriptions of variants are prefixed by their language ids
                    sep = description.indexOf(QLatin1String(": "));
                    if(sep >= 0)
                    {
                        QString lang = description.left(sep);
                        QMap<QString, KeyboardLayoutInfo>::iterator it = knownLayouts_.find(lang);
                        if(it != knownLayouts_.end()) {
                        KeyboardLayoutInfo& info = *it;
                        info.variants.append(LayoutVariantInfo(name, description.mid(sep + 2)));
                        }
                    }
                    break;
                }
                case OptionSection:
                    if(line.startsWith("grp:")) // key used to switch to another layout
                        switchKey->addItem(description, name);
                    break;
                default:;
                }
            }
        }
        f.close();
    }
}

bool KeyboardApplet::hasKeyboardBacklight()
{
    QFile f(HOLLYWOOD_KEYBOARD_BL);
    if(!f.exists())
        return false;

    return true;
}

