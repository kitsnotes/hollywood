#include "messagebox_p.h"
#include "messagebox.h"

#include <QLineEdit>
#include <QFile>
#include <QPropertyAnimation>
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <QSoundEffect>
#include <QParallelAnimationGroup>

MessageBoxPrivate::MessageBoxPrivate(HWMessageBox *parent)
    : d(parent)
{
    m_bell = new QSoundEffect(parent);
    m_bell->setSource(QUrl::fromLocalFile("/usr/share/sounds/Hollywood/Bell.wav"));

    verticalLayout = new QVBoxLayout(parent);
    m_icon = new QLabel(parent);
    m_text = new QLabel(parent);
    m_informative_text = new QLabel(parent);
    m_help = new QPushButton(parent);
    m_expand = new QPushButton(parent);
    buttonBox = new QDialogButtonBox(parent);
    m_description = new QTextBrowser(parent);

    m_icon->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    m_text->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_informative_text->setAlignment(Qt::AlignTop|Qt::AlignLeft);

    parent->setMinimumWidth(500);
    parent->setMinimumHeight(140);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    auto font = m_text->font();
    font.setBold(true);
    m_text->setFont(font);

    m_text->setWordWrap(true);
    m_informative_text->setWordWrap(true);
    m_informative_text->setMargin(10);
    m_description->setMaximumHeight(100);

    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setMaximumWidth(32);

    m_expand->setText(QApplication::tr("Details"));
    m_expand->setIcon(QIcon::fromTheme("arrow-down"));

    auto hl_main = new QHBoxLayout();
    auto vl_icon = new QVBoxLayout();
    auto vl_text = new QVBoxLayout();
    auto hl_buttons = new QHBoxLayout();
    vl_icon->addWidget(m_icon);
    vl_icon->addItem(new QSpacerItem(2, 2, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vl_text->addWidget(m_text);
    vl_text->addWidget(m_informative_text);
    vl_text->addItem(new QSpacerItem(2, 2, QSizePolicy::Minimum, QSizePolicy::Expanding));

    hl_main->addLayout(vl_icon);
    hl_main->addLayout(vl_text);
    hl_main->setStretch(1, 1);

    hl_buttons->addWidget(m_help);
    hl_buttons->addWidget(m_expand);
    hl_buttons->addWidget(buttonBox);

    verticalLayout->addLayout(hl_main);
    verticalLayout->addLayout(hl_buttons);
    verticalLayout->addWidget(m_description);

    QObject::connect(buttonBox, &QDialogButtonBox::clicked, d, &HWMessageBox::boxButtonClicked);
    QObject::connect(m_help, &QPushButton::pressed, d, &HWMessageBox::helpButtonClicked);

    QMetaObject::connectSlotsByName(parent);
}

void MessageBoxPrivate::generateStandardIcon()
{
    QIcon dialog_icon;
    QIcon app_icon = QApplication::windowIcon();

    switch(stdIcon)
    {
    case QMessageBox::Information:
        dialog_icon = QIcon::fromTheme("dialog-information");
        break;
    case QMessageBox::Warning:
        dialog_icon = QIcon::fromTheme("dialog-warning");
        break;
    case QMessageBox::Critical:
        dialog_icon = QIcon::fromTheme("dialog-error");
        break;
    case QMessageBox::Question:
        dialog_icon = QIcon::fromTheme("dialog-question");
        break;
    case QMessageBox::NoIcon:
        break;
    }

    if(app_icon.isNull() && dialog_icon.isNull())
    {
        m_icon->setVisible(false);
        return;
    }

    m_icon->setVisible(true);
    if(dialog_icon.isNull())
    {
        m_icon->setPixmap(app_icon.pixmap(64,64));
        return;
    }

    if(app_icon.isNull())
    {
        m_icon->setPixmap(dialog_icon.pixmap(64,64));
        return;
    }

    auto isz = 64;
    auto psz = 32;
    QPixmap i = dialog_icon.pixmap(isz,isz);
    QPixmap p = app_icon.pixmap(32,32);


    QPainter painter(&i);
    painter.drawPixmap(isz-psz,isz-psz,psz,psz,p);

    m_icon->setPixmap(i);
}

void MessageBoxPrivate::playBell()
{
    if(stdIcon == QMessageBox::Information ||
       stdIcon == QMessageBox::NoIcon)
        return; // don't bell on these two

    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));
    settings.beginGroup("Bell");
    auto bellFile = settings.value("AudioFile", "/usr/share/sounds/Hollywood/Bell.wav").toString();
    settings.endGroup();
    m_bell->setSource(QUrl::fromLocalFile(bellFile));

    if(stdIcon == QMessageBox::Critical && bellFile == QString("/usr/share/sounds/Hollywood/Bell.wav"))
    {
        if(QFile::exists("/usr/share/sounds/Hollywood/DoubleBell.wav"))
            m_bell->setSource(QUrl::fromLocalFile("/usr/share/sounds/Hollywood/DoubleBell.wav"));
    }
    //m_bell->play();
}

void MessageBoxPrivate::animateOpenDetails()
{
    QSize start = d->size();
    start.setHeight(start.height()+15);
    d->resize(start);
    auto size = m_description->size();
    size.setHeight(0);
    m_description->resize(size);
    m_description->setVisible(true);
    auto wnd_size_ani = new QPropertyAnimation(d, "size");
    auto box_size_ani = new QPropertyAnimation(m_description, "size");
    QSize end = start;
    end.setHeight(end.height()+105);
    wnd_size_ani->setStartValue(start);
    wnd_size_ani->setEndValue(end);
    wnd_size_ani->setDuration(190);
    wnd_size_ani->setEasingCurve(QEasingCurve::InOutQuad);

    start = QSize(m_description->size().width()-(verticalLayout->contentsMargins().left()-
                                     verticalLayout->contentsMargins().right()), 0);
    end = start;
    end.setHeight(100);
    box_size_ani->setStartValue(start);
    box_size_ani->setEndValue(end);
    box_size_ani->setDuration(190);
    box_size_ani->setEasingCurve(QEasingCurve::InOutQuad);

    auto group = new QParallelAnimationGroup(d);
    group->addAnimation(wnd_size_ani);
    group->addAnimation(box_size_ani);
    d->setMaximumHeight(d->minimumHeight()+105);
    group->start(QAbstractAnimation::DeleteWhenStopped);
    QObject::connect(group, &QAbstractAnimation::finished, [this] () {
        d->setMinimumHeight(d->minimumHeight()+105);
    });
}

void MessageBoxPrivate::animateCloseDetails()
{
    QSize start = d->size();
    auto wnd_size_ani = new QPropertyAnimation(d, "size");
    auto box_size_ani = new QPropertyAnimation(m_description, "size");
    QSize end = start;
    end.setHeight(end.height()-105);
    wnd_size_ani->setStartValue(start);
    wnd_size_ani->setEndValue(end);
    wnd_size_ani->setDuration(190);
    wnd_size_ani->setEasingCurve(QEasingCurve::InOutQuad);

    start = m_description->size();
    end = start;
    end.setHeight(0);
    box_size_ani->setStartValue(start);
    box_size_ani->setEndValue(end);
    box_size_ani->setDuration(190);
    box_size_ani->setEasingCurve(QEasingCurve::InOutQuad);

    auto group = new QParallelAnimationGroup(d);
    group->addAnimation(wnd_size_ani);
    group->addAnimation(box_size_ani);
    QObject::connect(group, &QAbstractAnimation::finished, [this] () {
        m_description->setVisible(false);
        d->setMinimumHeight(d->minimumHeight()-105);
        d->setMaximumHeight(d->minimumHeight()-105);
    });
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

HWMessageBox::HWMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text,
                       QMessageBox::StandardButtons buttons, QWidget *parent, Qt::WindowFlags flags)
          : QDialog(parent)
          , q(new MessageBoxPrivate(this))
{
    QObject::connect(q->m_expand, &QPushButton::pressed, this, &HWMessageBox::showDetailedText);
    q->m_expand->setVisible(false);
    q->m_description->setVisible(false);

    setWindowTitle(title);
    setWindowFlags(flags);
    setText(text);
    setIcon(icon);
    setStandardButtons(buttons);
}

HWMessageBox::~HWMessageBox()
{
    delete q;
}

void HWMessageBox::showDetailedText()
{
    bool show = !q->m_description->isVisible();
    if(show)
    {
        q->animateOpenDetails();
        q->m_expand->setIcon(QIcon::fromTheme("arrow-up"));
    }
    else
    {
        q->animateCloseDetails();
        q->m_expand->setIcon(QIcon::fromTheme("arrow-down"));
    }
}

void HWMessageBox::addButton(QAbstractButton *button, QMessageBox::ButtonRole role)
{
    q->buttonBox->addButton(button, QDialogButtonBox::ButtonRole(role));
}

QPushButton *HWMessageBox::addButton(const QString &text, QMessageBox::ButtonRole role)
{
    auto btn = new QPushButton(text, this);
    q->buttonBox->addButton(btn, QDialogButtonBox::ButtonRole(role));
    return btn;
}

QPushButton *HWMessageBox::addButton(QMessageBox::StandardButton button)
{
    auto btn = q->buttonBox->addButton((QDialogButtonBox::StandardButton)button);
    return btn;
}

void HWMessageBox::removeButton(QAbstractButton *button)
{
    if(qobject_cast<QAbstractButton*>(q->m_help) == button)
    {
        q->m_help->setVisible(false);
        return;
    }

    q->buttonBox->removeButton(button);
}

QList<QAbstractButton *> HWMessageBox::buttons() const
{
    QList<QAbstractButton*> buttons = q->buttonBox->buttons();
    if(q->m_help->isVisible())
        buttons.append(q->m_help);

    return buttons;
}

QMessageBox::ButtonRole HWMessageBox::buttonRole(QAbstractButton *button) const
{
    if(qobject_cast<QAbstractButton*>(q->m_help) == button)
        return QMessageBox::ActionRole;

    return (QMessageBox::ButtonRole)q->buttonBox->buttonRole(button);
}

void HWMessageBox::setStandardButtons(QMessageBox::StandardButtons buttons)
{
    bool use_help = false;
    if(buttons.testFlag(QMessageBox::Help))
    {
        use_help = true;
        buttons.setFlag(QMessageBox::Help, false);
    }

    int btnint = (int)buttons;
    QDialogButtonBox::StandardButtons btns = (QDialogButtonBox::StandardButtons)btnint;
    q->buttonBox->setStandardButtons(btns);
    q->m_help->setVisible(use_help);
}

QMessageBox::StandardButtons HWMessageBox::standardButtons() const
{
    auto btns = q->buttonBox->standardButtons();
    int btnint = (int)btns;
    if(q->m_help->isVisible())
        btns.setFlag(QDialogButtonBox::Help, true);

    QMessageBox::StandardButtons buttons = (QMessageBox::StandardButtons)btnint;
    return buttons;
}

QMessageBox::StandardButton HWMessageBox::standardButton(QAbstractButton *button) const
{
    if(qobject_cast<QAbstractButton*>(q->m_help) == button)
        return QMessageBox::Help;

    auto btn = q->buttonBox->standardButton(button);
    int btnint = int(btn);
    QMessageBox::StandardButton retBtn = (QMessageBox::StandardButton)btnint;
    return retBtn;
}

QAbstractButton *HWMessageBox::button(QMessageBox::StandardButton which) const
{
    if(which == QMessageBox::Help)
        return q->m_help;

    return q->buttonBox->button((QDialogButtonBox::StandardButton)which);
}

QPushButton *HWMessageBox::defaultButton() const
{
    return q->m_default;
}

void HWMessageBox::setDefaultButton(QPushButton *button)
{
    q->m_default = button;
    button->setDefault(true);
}

void HWMessageBox::setDefaultButton(QMessageBox::StandardButton button)
{
    QPushButton *btn;
    if(button == QMessageBox::Help)
        btn = q->m_help;
    else
        btn = q->buttonBox->button((QDialogButtonBox::StandardButton)button);
    q->m_default = btn;
    btn->setDefault(true);
}

QAbstractButton *HWMessageBox::escapeButton() const
{
    return q->m_escape;
}

void HWMessageBox::setEscapeButton(QAbstractButton *button)
{
    q->m_escape = button;
}

void HWMessageBox::setEscapeButton(QMessageBox::StandardButton button)
{
    QAbstractButton *btn;
    if(button == QMessageBox::Help)
        btn = qobject_cast<QAbstractButton*>(q->m_help);
    else
        btn = qobject_cast<QAbstractButton*>(q->buttonBox->button((QDialogButtonBox::StandardButton)button));
    q->m_escape = btn;
}

QAbstractButton *HWMessageBox::clickedButton() const
{
    return q->m_clicked;
}

QString HWMessageBox::text() const
{
    return q->m_text->text();
}

void HWMessageBox::setText(const QString &text)
{
    q->m_text->setText(text);
}

QString HWMessageBox::detailedText() const
{
    return q->m_description->toPlainText();
}

void HWMessageBox::setDetailedText(const QString &text)
{
    q->m_description->setText(text);
    if(!text.isEmpty())
        q->m_expand->setVisible(true);
    else
    {
        q->m_expand->setVisible(false);
        q->m_description->setVisible(false);
    }
}

QString HWMessageBox::informativeText() const
{
    return q->m_informative_text->text();
}

void HWMessageBox::setInformativeText(const QString &text)
{
    q->m_informative_text->setText(text);
}

QMessageBox::Icon HWMessageBox::icon() const
{
    return q->stdIcon;
}

void HWMessageBox::setIcon(QMessageBox::Icon stdIcon)
{
    q->stdIcon = stdIcon;
    q->generateStandardIcon();
}

QPixmap HWMessageBox::iconPixmap() const
{
    return q->icon;
}

void HWMessageBox::setIconPixmap(const QPixmap &pixmap)
{
    q->icon = pixmap;
    q->m_icon->setPixmap(pixmap);
}

Qt::TextFormat HWMessageBox::textFormat() const
{
    return q->m_format;
}

void HWMessageBox::setTextFormat(Qt::TextFormat format)
{
    q->m_format = format;
    q->m_text->setTextFormat(format);
    q->m_informative_text->setTextFormat(format);
    if(format == Qt::PlainText)
        q->m_description->setAcceptRichText(false);
    else
        q->m_description->setAcceptRichText(true);
}

void HWMessageBox::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    q->m_text->setTextInteractionFlags(flags);
    q->m_informative_text->setTextInteractionFlags(flags);
    q->m_description->setTextInteractionFlags(flags);
}

Qt::TextInteractionFlags HWMessageBox::textInteractionFlags() const
{
    return q->m_text->textInteractionFlags();
}

void HWMessageBox::setCheckBox(QCheckBox *cb)
{
    q->m_cb = cb;
}

QCheckBox *HWMessageBox::checkBox() const
{
    return q->m_cb;
}

int HWMessageBox::exec()
{
    QDialog::exec();

    return standardButton(q->m_clicked);
}

void HWMessageBox::showEvent(QShowEvent *event)
{
    //q->playBell();
    QDialog::showEvent(event);
}

void HWMessageBox::boxButtonClicked(QAbstractButton *button)
{
    q->m_clicked = button;
    setResult(standardButton(button));
    emit buttonClicked(button);
    if(q->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole ||
       q->buttonBox->buttonRole(button) == QDialogButtonBox::YesRole)
        done(QMessageBox::Yes);
    if(q->buttonBox->buttonRole(button) == QDialogButtonBox::RejectRole ||
       q->buttonBox->buttonRole(button) == QDialogButtonBox::NoRole)
        done(QMessageBox::No);
}

void HWMessageBox::helpButtonClicked()
{
    q->m_clicked = qobject_cast<QAbstractButton*>(q->m_help);
}
