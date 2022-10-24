#include "regionapplet.h"

#include <QTextCodec>
#include <QStandardPaths>

const static QString lcLang = QStringLiteral("LANG");

const static QString lcNumeric = QStringLiteral("LC_NUMERIC");
const static QString lcTime = QStringLiteral("LC_TIME");
const static QString lcMonetary = QStringLiteral("LC_MONETARY");
const static QString lcMeasurement = QStringLiteral("LC_MEASUREMENT");
const static QString lcCollate = QStringLiteral("LC_COLLATE");
const static QString lcCtype = QStringLiteral("LC_CTYPE");

const static QString lcLanguage = QStringLiteral("LANGUAGE");


bool countryLessThan(const QLocale & c1, const QLocale & c2)
{
    // get the strings as in addLocaleToCombo() -> clabel
    return QString::localeAwareCompare(!c1.nativeCountryName().isEmpty()
                                           ? c1.nativeCountryName()
                                           : c1.countryToString(c1.country()),
                                       !c2.nativeCountryName().isEmpty()
                                           ? c2.nativeCountryName()
                                           : c2.countryToString(c2.country())) < 0;
}

void setCombo(QComboBox *combo, const QString &key)
{
    const int ix = combo->findData(key);
    if (ix > -1)
    {
        combo->setCurrentIndex(ix);
    }
}

RegionalApplet::RegionalApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool RegionalApplet::init()
{
    setupWidget();

    checkDetailed->setChecked(false);

    // restore user defaults from env vars
    setCombo(comboGlobal, QString::fromLocal8Bit(qgetenv(lcLang.toLatin1().constData())));
    setCombo(comboNumbers, QString::fromLocal8Bit(qgetenv(lcNumeric.toLatin1().constData())));
    setCombo(comboTime, QString::fromLocal8Bit(qgetenv(lcTime.toLatin1().constData())));
    setCombo(comboCollate, QString::fromLocal8Bit(qgetenv(lcCollate.toLatin1().constData())));
    setCombo(comboCurrency, QString::fromLocal8Bit(qgetenv(lcMonetary.toLatin1().constData())));
    setCombo(comboMeasurement, QString::fromLocal8Bit(qgetenv(lcMeasurement.toLatin1().constData())));
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    std::sort(allLocales.begin(), allLocales.end(), countryLessThan);
    for(QComboBox * combo : qAsConst(m_combos))
    {
        initCombo(combo, allLocales);
    }

    //readConfig();

    for(QComboBox * combo : qAsConst(m_combos))
    {
        connectCombo(combo);
    }

    connect(checkDetailed, &QGroupBox::toggled, [ = ]()
    {
        updateExample();
        hasChanged = true;
    });


    updateExample();
    hasChanged = false;
    return true;
}

QString RegionalApplet::id() const
{
    return QLatin1String("org.originull.regional");
}

QString RegionalApplet::name() const
{
    return tr("Language & Region");
}

QString RegionalApplet::description() const
{
    return tr("Configure language and regional settings.");
}

QIcon RegionalApplet::icon() const
{
    return QIcon::fromTheme("preferences-desktop-locale");
}

QWidget *RegionalApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category RegionalApplet::category() const
{
    return Personal;
}

void RegionalApplet::setupWidget()
{
    m_host = new QWidget(0);
    m_host->resize(625, 439);

    mainLayout = new QVBoxLayout(m_host);
    gridLayout = new QGridLayout();
    formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setHorizontalSpacing(6);
    formLayout->setVerticalSpacing(6);
    labelGlobal = new QLabel(m_host);
    labelGlobal->setObjectName(QString::fromUtf8("labelGlobal"));
    labelGlobal->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    formLayout->setWidget(0, QFormLayout::LabelRole, labelGlobal);

    comboGlobal = new QComboBox(m_host);
    comboGlobal->setObjectName(QString::fromUtf8("comboGlobal"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(comboGlobal->sizePolicy().hasHeightForWidth());
    comboGlobal->setSizePolicy(sizePolicy);
    comboGlobal->setMinimumSize(QSize(300, 0));

    formLayout->setWidget(0, QFormLayout::FieldRole, comboGlobal);

    checkDetailed = new QGroupBox(m_host);
    checkDetailed->setObjectName(QString::fromUtf8("checkDetailed"));
    checkDetailed->setCheckable(true);
    checkDetailed->setChecked(false);
    formLayout_2 = new QFormLayout(checkDetailed);
    formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
    formLayout_2->setHorizontalSpacing(6);
    formLayout_2->setVerticalSpacing(6);
    labelNumbers = new QLabel(checkDetailed);
    labelNumbers->setObjectName(QString::fromUtf8("labelNumbers"));
    labelNumbers->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    formLayout_2->setWidget(0, QFormLayout::LabelRole, labelNumbers);

    comboNumbers = new QComboBox(checkDetailed);
    comboNumbers->setObjectName(QString::fromUtf8("comboNumbers"));
    sizePolicy.setHeightForWidth(comboNumbers->sizePolicy().hasHeightForWidth());
    comboNumbers->setSizePolicy(sizePolicy);
    comboNumbers->setMinimumSize(QSize(300, 0));

    formLayout_2->setWidget(0, QFormLayout::FieldRole, comboNumbers);

    labelTime = new QLabel(checkDetailed);
    labelTime->setObjectName(QString::fromUtf8("labelTime"));
    labelTime->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    formLayout_2->setWidget(1, QFormLayout::LabelRole, labelTime);

    comboTime = new QComboBox(checkDetailed);
    comboTime->setObjectName(QString::fromUtf8("comboTime"));
    sizePolicy.setHeightForWidth(comboTime->sizePolicy().hasHeightForWidth());
    comboTime->setSizePolicy(sizePolicy);
    comboTime->setMinimumSize(QSize(300, 0));

    formLayout_2->setWidget(1, QFormLayout::FieldRole, comboTime);

    labelCurrency = new QLabel(checkDetailed);
    labelCurrency->setObjectName(QString::fromUtf8("labelCurrency"));
    labelCurrency->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    formLayout_2->setWidget(2, QFormLayout::LabelRole, labelCurrency);

    comboCurrency = new QComboBox(checkDetailed);
    comboCurrency->setObjectName(QString::fromUtf8("comboCurrency"));
    sizePolicy.setHeightForWidth(comboCurrency->sizePolicy().hasHeightForWidth());
    comboCurrency->setSizePolicy(sizePolicy);
    comboCurrency->setMinimumSize(QSize(300, 0));

    formLayout_2->setWidget(2, QFormLayout::FieldRole, comboCurrency);

    labelMeasurement = new QLabel(checkDetailed);
    labelMeasurement->setObjectName(QString::fromUtf8("labelMeasurement"));
    labelMeasurement->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    formLayout_2->setWidget(3, QFormLayout::LabelRole, labelMeasurement);

    comboMeasurement = new QComboBox(checkDetailed);
    comboMeasurement->setObjectName(QString::fromUtf8("comboMeasurement"));
    sizePolicy.setHeightForWidth(comboMeasurement->sizePolicy().hasHeightForWidth());
    comboMeasurement->setSizePolicy(sizePolicy);
    comboMeasurement->setMinimumSize(QSize(300, 0));

    formLayout_2->setWidget(3, QFormLayout::FieldRole, comboMeasurement);

    labelCollate = new QLabel(checkDetailed);
    labelCollate->setObjectName(QString::fromUtf8("labelCollate"));
    labelCollate->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    formLayout_2->setWidget(4, QFormLayout::LabelRole, labelCollate);

    comboCollate = new QComboBox(checkDetailed);
    comboCollate->setObjectName(QString::fromUtf8("comboCollate"));
    sizePolicy.setHeightForWidth(comboCollate->sizePolicy().hasHeightForWidth());
    comboCollate->setSizePolicy(sizePolicy);
    comboCollate->setMinimumSize(QSize(300, 0));

    formLayout_2->setWidget(4, QFormLayout::FieldRole, comboCollate);


    formLayout->setWidget(1, QFormLayout::SpanningRole, checkDetailed);

    exampleBox = new QGroupBox(m_host);
    exampleBox->setObjectName(QString::fromUtf8("exampleBox"));
    formLayout_3 = new QFormLayout(exampleBox);
    formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
    formLayout_3->setHorizontalSpacing(6);
    formLayout_3->setVerticalSpacing(6);
    lexNumbers = new QLabel(exampleBox);
    lexNumbers->setObjectName(QString::fromUtf8("lexNumbers"));
    lexNumbers->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(0, QFormLayout::LabelRole, lexNumbers);

    exampleNumbers = new QLabel(exampleBox);
    exampleNumbers->setObjectName(QString::fromUtf8("exampleNumbers"));
    exampleNumbers->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(0, QFormLayout::FieldRole, exampleNumbers);

    lexTime = new QLabel(exampleBox);
    lexTime->setObjectName(QString::fromUtf8("lexTime"));
    lexTime->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(1, QFormLayout::LabelRole, lexTime);

    exampleTime = new QLabel(exampleBox);
    exampleTime->setObjectName(QString::fromUtf8("exampleTime"));
    exampleTime->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(1, QFormLayout::FieldRole, exampleTime);

    lexCurrency = new QLabel(exampleBox);
    lexCurrency->setObjectName(QString::fromUtf8("lexCurrency"));
    lexCurrency->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(2, QFormLayout::LabelRole, lexCurrency);

    exampleCurrency = new QLabel(exampleBox);
    exampleCurrency->setObjectName(QString::fromUtf8("exampleCurrency"));
    exampleCurrency->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(2, QFormLayout::FieldRole, exampleCurrency);

    lexMeasurement_2 = new QLabel(exampleBox);
    lexMeasurement_2->setObjectName(QString::fromUtf8("lexMeasurement_2"));
    lexMeasurement_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(3, QFormLayout::LabelRole, lexMeasurement_2);

    exampleMeasurement = new QLabel(exampleBox);
    exampleMeasurement->setObjectName(QString::fromUtf8("exampleMeasurement"));
    exampleMeasurement->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    formLayout_3->setWidget(3, QFormLayout::FieldRole, exampleMeasurement);


    formLayout->setWidget(2, QFormLayout::SpanningRole, exampleBox);


    gridLayout->addLayout(formLayout, 0, 0, 1, 1);


    mainLayout->addLayout(gridLayout);

    verticalSpacer_3 = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);

    mainLayout->addItem(verticalSpacer_3);

    #if QT_CONFIG(shortcut)
    labelGlobal->setBuddy(comboGlobal);
    labelNumbers->setBuddy(comboNumbers);
    labelTime->setBuddy(comboTime);
    labelCurrency->setBuddy(comboCurrency);
    labelMeasurement->setBuddy(comboMeasurement);
    labelCollate->setBuddy(comboCollate);
    #endif // QT_CONFIG(shortcut)

    labelGlobal->setText(QCoreApplication::translate("LocaleConfig", "Re&gion:", nullptr));
    checkDetailed->setTitle(QCoreApplication::translate("LocaleConfig", "De&tailed Settings", nullptr));
    labelNumbers->setText(QCoreApplication::translate("LocaleConfig", "&Numbers:", nullptr));
    labelTime->setText(QCoreApplication::translate("LocaleConfig", "&Time:", nullptr));
    labelCurrency->setText(QCoreApplication::translate("LocaleConfig", "Currenc&y:", nullptr));
    labelMeasurement->setText(QCoreApplication::translate("LocaleConfig", "Measurement &Units:", nullptr));
    labelCollate->setText(QCoreApplication::translate("LocaleConfig", "Co&llation and Sorting:", nullptr));
    exampleBox->setTitle(QCoreApplication::translate("LocaleConfig", "Examples", nullptr));
    lexNumbers->setText(QCoreApplication::translate("LocaleConfig", "Numbers:", nullptr));
    exampleNumbers->setText(QString());
    lexTime->setText(QCoreApplication::translate("LocaleConfig", "Time:", nullptr));
    exampleTime->setText(QString());
    lexCurrency->setText(QCoreApplication::translate("LocaleConfig", "Currency:", nullptr));
    exampleCurrency->setText(QString());
    lexMeasurement_2->setText(QCoreApplication::translate("LocaleConfig", "Measurement Units:", nullptr));
    exampleMeasurement->setText(QString());
}

void RegionalApplet::addLocaleToCombo(QComboBox *combo, const QLocale &locale)
{
    const QString clabel = !locale.nativeCountryName().isEmpty() ? locale.nativeCountryName() : locale.countryToString(locale.country());
    // This needs to use name() rather than bcp47name() or later on the export will generate a non-sense locale (e.g. "it" instead of
    // "it_IT")
    // TODO: Properly handle scripts (@foo)
    QString cvalue = locale.name();
    if (!cvalue.contains(QLatin1Char('.')))
    { // explicitely add the encoding, otherwise Qt doesn't accept dead keys and garbles the output as well
        cvalue.append(QLatin1Char('.') + QString::fromUtf8(QTextCodec::codecForLocale()->name()));
    }

    QString flagcode;
    const QStringList split = locale.name().split(QLatin1Char('_'));
    if (split.count() > 1)
    {
        flagcode = split[1].toLower();
    }
    /* TODO Find a better place for flags ... */
    QString flag(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kf5/locale/countries/%1/flag.png").arg(flagcode)));
    QIcon flagIcon;
    if (!flag.isEmpty())
    {
        flagIcon = QIcon(flag);
    }

    QString itemResult;
    itemResult = QStringLiteral("%1 - %2 (%3)")
                        .arg(clabel,
                        locale.nativeLanguageName(),
                        locale.name());

    combo->addItem(flagIcon, itemResult, cvalue);
}

void RegionalApplet::initCombo(QComboBox *combo, const QList<QLocale> &allLocales)
{
    combo->clear();
    const QString clabel = tr("No change");
    combo->setInsertPolicy(QComboBox::InsertAlphabetically);
    combo->addItem(clabel, QString());
    for(const QLocale & l : qAsConst(allLocales))
        addLocaleToCombo(combo, l);
}

void RegionalApplet::connectCombo(QComboBox *combo)
{
    connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ]()
    {
        hasChanged = true;
        updateExample();
    });
}

void RegionalApplet::updateExample()
{
    const bool useDetailed = checkDetailed->isChecked();

    QLocale nloc;
    QLocale tloc;
    QLocale cloc;
    QLocale mloc;

    if (useDetailed)
    {
        nloc = QLocale(comboNumbers->currentData().toString());
        tloc = QLocale(comboTime->currentData().toString());
        cloc = QLocale(comboCurrency->currentData().toString());
        mloc = QLocale(comboMeasurement->currentData().toString());
    }
    else
    {
        nloc = QLocale(comboGlobal->currentData().toString());
        tloc = QLocale(comboGlobal->currentData().toString());
        cloc = QLocale(comboGlobal->currentData().toString());
        mloc = QLocale(comboGlobal->currentData().toString());
    }

    const QString numberExample = nloc.toString(1000.01);
    const QString timeExample = tloc.toString(QDateTime::currentDateTime());
    const QString currencyExample = cloc.toCurrencyString(24);

    QString measurementSetting;
    if (mloc.measurementSystem() == QLocale::ImperialUKSystem)
    {
        measurementSetting = tr("Imperial UK");
    }
    else if (mloc.measurementSystem() == QLocale::ImperialUSSystem)
    {
        measurementSetting = tr("Imperial US");
    }
    else
    {
        measurementSetting = tr("Metric");
    }

    exampleNumbers->setText(numberExample);
    exampleTime->setText(timeExample);
    exampleCurrency->setText(currencyExample);
    exampleMeasurement->setText(measurementSetting);
}

