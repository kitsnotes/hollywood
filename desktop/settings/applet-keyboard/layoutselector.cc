// derived fromlxqt-config-input/selectkeyboardlayoutdialog.cpp
// License: LGPL2

#include "layoutselector.h"

LayoutSelector::LayoutSelector(QMap< QString, KeyboardLayoutInfo>& knownLayouts, QWidget *parent)
    : QDialog(parent)
    , gridLayout(new QGridLayout(this))
    , layouts(new QListWidget(this))
    , variants(new QListWidget(this))
    , buttonBox(new QDialogButtonBox(this))
    , knownLayouts_(knownLayouts)
{
    setWindowTitle(tr("Select Keyboard Layout"));
    setupUi();
    setMinimumHeight(350);
    setMinimumWidth(300);
    connect(layouts, &QListWidget::currentItemChanged,
            this, &LayoutSelector::layoutChanged);
    QMap<QString, KeyboardLayoutInfo >::const_iterator it;
    for(it = knownLayouts_.constBegin(); it != knownLayouts_.constEnd(); ++it) {
      const QString& name = it.key();
      const KeyboardLayoutInfo& info = *it;
      QListWidgetItem * item = new QListWidgetItem(info.description);
      item->setData(Qt::UserRole, name);
      layouts->addItem(item);
    }
    layouts->setCurrentItem(layouts->item(0));

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString LayoutSelector::selectedLayout()
{
    QListWidgetItem* layoutItem = layouts->currentItem();
    if(layoutItem)
        return layoutItem->data(Qt::UserRole).toString();
    return QString();
}

QString LayoutSelector::selectedVariant()
{
    QListWidgetItem* variantItem = variants->currentItem();
    if(variantItem)
        return variantItem->data(Qt::UserRole).toString();
    return QString();
}

void LayoutSelector::layoutChanged()
{
    QListWidgetItem* item = layouts->currentItem();
    variants->clear();

    variants->addItem(QStringLiteral("None"));
    variants->setCurrentItem(variants->item(0));
    if(item)
    {
        // add variants of this layout to the list view
        QString name = item->data(Qt::UserRole).toString();
        const KeyboardLayoutInfo& info = knownLayouts_[name];
        for(const LayoutVariantInfo& vinfo : qAsConst(info.variants))
        {
            QListWidgetItem * vitem = new QListWidgetItem(vinfo.description);
            vitem->setData(Qt::UserRole, vinfo.name);
            variants->addItem(vitem);
        }
    }
}

void LayoutSelector::setupUi()
{
    auto lbl_layouts = new QLabel(this);
    auto lbl_variants = new QLabel(this);
    layouts->setSortingEnabled(true);

    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(lbl_layouts, 0, 0, 1, 1);
    gridLayout->addWidget(lbl_variants, 0, 2, 1, 1);
    gridLayout->addWidget(layouts, 1, 0, 1, 2);
    gridLayout->addWidget(variants, 1, 2, 1, 1);
    gridLayout->addWidget(buttonBox, 2, 0, 1, 3);

    lbl_layouts->setText(QCoreApplication::translate("KeyboardApplet", "Keyboard layout", nullptr));
    lbl_variants->setText(QCoreApplication::translate("KeyboardApplet", "Variant", nullptr));
}
