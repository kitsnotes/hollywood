/****************************************************************************
** Hollywood Calculator
** Copyright (C) 2022 The Cat and Contributors to Hollywood
** Portions Copyright (C) 2016 The Qt Company Ltd.
** spdx-license-identifier: bsd-3-clause
****************************************************************************/

#include "calculator.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QtMath>
#include <QKeyEvent>
#include <QTimer>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>

Button::Button(const QString &text, QWidget *parent)
    : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);
}

QSize Button::sizeHint() const
{
    QSize size = QToolButton::sizeHint();
    size.rheight() += 20;
    size.rwidth() = qMax(size.width(), size.height());
    return size;
}

void Button::setBackUp()
{
    setDown(false);
}

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), sumInMemory(0.0), sumSoFar(0.0)
    , factorSoFar(0.0), waitingForOperand(true)
{
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    for (int i = 0; i < NumDigitButtons; ++i)
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));

    pointButton = createButton(tr("."), SLOT(pointClicked()));
    Button *changeSignButton = createButton(tr("\302\261"), SLOT(changeSignClicked()));

    backspaceButton = createButton(tr("Backspace"), SLOT(backspaceClicked()));
    clearButton = createButton(tr("Clear"), SLOT(clear()));
    Button *clearAllButton = createButton(tr("Clear All"), SLOT(clearAll()));

    Button *clearMemoryButton = createButton(tr("MC"), SLOT(clearMemory()));
    Button *readMemoryButton = createButton(tr("MR"), SLOT(readMemory()));
    Button *setMemoryButton = createButton(tr("MS"), SLOT(setMemory()));
    Button *addToMemoryButton = createButton(tr("M+"), SLOT(addToMemory()));

    divisionButton = createButton(tr("\303\267"), SLOT(multiplicativeOperatorClicked()));
    timesButton = createButton(tr("\303\227"), SLOT(multiplicativeOperatorClicked()));
    minusButton = createButton(tr("-"), SLOT(additiveOperatorClicked()));
    plusButton = createButton(tr("+"), SLOT(additiveOperatorClicked()));

    Button *squareRootButton = createButton(tr("Sqrt"), SLOT(unaryOperatorClicked()));
    Button *powerButton = createButton(tr("x\302\262"), SLOT(unaryOperatorClicked()));
    Button *reciprocalButton = createButton(tr("1/x"), SLOT(unaryOperatorClicked()));
    equalButton = createButton(tr("="), SLOT(equalClicked()));

    Button *sinButton = createButton(tr("sin"), SLOT(undefined()));
    Button *cosButton = createButton(tr("cos"), SLOT(undefined()));
    Button *tanButton = createButton(tr("tan"), SLOT(undefined()));
    Button *sinhButton = createButton(tr("sinh"), SLOT(undefined()));
    Button *coshButton = createButton(tr("cosh"), SLOT(undefined()));
    Button *tanhButton = createButton(tr("tanh"), SLOT(undefined()));
    Button *leftpButton = createButton(tr("("), SLOT(undefined()));
    Button *rightpButton = createButton(tr(")"), SLOT(undefined()));
    Button *cubeButton = createButton(tr("x\302\263"), SLOT(undefined()));
    Button *lnButton = createButton(tr("ln"), SLOT(undefined()));
    Button *logButton = createButton(tr("log"), SLOT(undefined()));
    Button *factorialButton = createButton(tr("x!"), SLOT(undefined()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(display, 0, 0, 1, 2);

    basiclayout = new QGridLayout;
    basiclayout->setSizeConstraint(QLayout::SetFixedSize);

    basiclayout->addWidget(backspaceButton, 1, 0, 1, 2);
    basiclayout->addWidget(clearButton, 1, 2, 1, 2);
    basiclayout->addWidget(clearAllButton, 1, 4, 1, 2);

    basiclayout->addWidget(clearMemoryButton, 2, 0);
    basiclayout->addWidget(readMemoryButton, 2, 1);
    basiclayout->addWidget(setMemoryButton, 2, 2);
    basiclayout->addWidget(addToMemoryButton, 2, 3);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 3;
        int column = ((i - 1) % 3);
        basiclayout->addWidget(digitButtons[i], row, column);
    }

    basiclayout->addWidget(digitButtons[0], 6, 0, 1, 2);
    basiclayout->addWidget(pointButton, 6, 2);

    basiclayout->addWidget(divisionButton, 3, 3);
    basiclayout->addWidget(timesButton, 4, 3);
    basiclayout->addWidget(minusButton, 5, 3);
    basiclayout->addWidget(plusButton, 6, 3);

    basiclayout->addWidget(squareRootButton, 2, 5);
    basiclayout->addWidget(changeSignButton, 3, 5);
    basiclayout->addWidget(reciprocalButton, 4, 5);
    basiclayout->addWidget(equalButton, 5, 5, 2, 1);

    mainLayout->addLayout(basiclayout, 1, 1, Qt::AlignCenter);

    scilayout = new QGridLayout;
    scilayout->setSizeConstraint(QLayout::SetFixedSize);

    // setup deg/rad/grad box
    auto gb = new QGroupBox(this);
    gb->setMaximumHeight(41);
    gb->setMinimumHeight(41);
    auto hl = new QHBoxLayout(gb);
    auto deg = new QRadioButton(gb);
    deg->setText(tr("Deg"));
    auto rad = new QRadioButton(gb);
    rad->setText(tr("Rad"));
    auto grad = new QRadioButton(gb);
    grad->setText(tr("Grad"));
    hl->addWidget(deg);
    hl->addWidget(rad);
    hl->addWidget(grad);

    scilayout->addWidget(gb, 1, 1, 1, 5);

    sciWidgets.append(gb);
    sciWidgets.append(deg);
    sciWidgets.append(rad);
    sciWidgets.append(grad);

    scilayout->addWidget(sinhButton, 3,2);
    scilayout->addWidget(coshButton, 4,2);
    scilayout->addWidget(tanhButton, 5,2);
    scilayout->addWidget(lnButton, 6,2);
    sciWidgets.append(sinhButton);
    sciWidgets.append(coshButton);
    sciWidgets.append(tanhButton);
    sciWidgets.append(lnButton);

    scilayout->addWidget(sinButton, 3,3);
    scilayout->addWidget(cosButton, 4,3);
    scilayout->addWidget(tanButton, 5,3);
    scilayout->addWidget(logButton, 6,3);
    sciWidgets.append(sinButton);
    sciWidgets.append(cosButton);
    sciWidgets.append(tanButton);
    sciWidgets.append(logButton);

    scilayout->addWidget(leftpButton, 2,4);
    scilayout->addWidget(powerButton, 3,4);
    scilayout->addWidget(cubeButton, 4,4);
    sciWidgets.append(leftpButton);
    sciWidgets.append(powerButton);
    sciWidgets.append(cubeButton);
    //sciWidgets.append(logButton);

    scilayout->addWidget(rightpButton, 2,5);
    scilayout->addWidget(factorialButton, 3,5);
    sciWidgets.append(rightpButton);
    sciWidgets.append(factorialButton);

    mainLayout->addLayout(scilayout, 1, 0, Qt::AlignCenter);

    setLayout(mainLayout);

    setOperationMode(Calculator::Basic);
}

void Calculator::setOperationMode(OperationMode mode)
{
    auto gl = qobject_cast<QGridLayout*>(layout());
    switch(mode)
    {
    case Scientific:
        currentMode = Scientific;
        gl->removeWidget(display);
        gl->removeItem(gl->itemAtPosition(1,0));
        gl->removeItem(gl->itemAtPosition(1,1));
        gl->addWidget(display, 0, 0, 1, 2);
        gl->addLayout(scilayout, 1, 0);
        gl->addLayout(basiclayout, 1, 1);
        showScientificWidgets(true);
        break;
    case Basic:
    default:
        currentMode = Basic;
        gl->removeWidget(display);
        gl->removeItem(gl->itemAtPosition(1,0));
        gl->removeItem(gl->itemAtPosition(1,1));
        gl->addWidget(display, 0, 0);
        gl->addLayout(basiclayout, 1, 0);
        showScientificWidgets(false);
        break;
    }

    emit resizeRequest();
}

void Calculator::keyPressEvent(QKeyEvent *e)
{
    if(e->key() >= Qt::Key_0 &&
            e->key() <= Qt::Key_9)
    {
        int key = e->key()-Qt::Key_0;
        digitButtons[key]->setDown(true);
        if (waitingForOperand) {
            display->clear();
            waitingForOperand = false;
        }
        display->setText(display->text() + QString::number(key));
        QTimer::singleShot(120, digitButtons[key], &Button::setBackUp);
        e->accept();
    }

    if(e->key() == Qt::Key_Equal ||
       e->key() == Qt::Key_Return ||
       e->key() == Qt::Key_Enter)
    {
        equalButton->setDown(true);
        equalClicked();
        QTimer::singleShot(120, equalButton, &Button::setBackUp);
        e->accept();
    }

    if(e->key() == Qt::Key_Period)
    {
        pointButton->setDown(true);
        pointClicked();
        QTimer::singleShot(120, pointButton, &Button::setBackUp);
        e->accept();
    }

    if(e->key() == Qt::Key_Plus)
    {
        plusButton->setDown(true);
        additiveOperator("+");
        QTimer::singleShot(120, plusButton, &Button::setBackUp);
        e->accept();
    }

    if(e->key() == Qt::Key_Minus)
    {
        minusButton->setDown(true);
        additiveOperator("-");
        QTimer::singleShot(120, minusButton, &Button::setBackUp);
        e->accept();
    }

    if(e->key() == Qt::Key_Backspace ||
       e->key() == Qt::Key_Delete)
    {
        backspaceButton->setDown(true);
        backspaceClicked();
        QTimer::singleShot(120, backspaceButton, &Button::setBackUp);
        e->accept();
    }

    if(e->key() == Qt::Key_Escape)
    {
        clearButton->setDown(true);
        clear();
        QTimer::singleShot(120, clearButton, &Button::setBackUp);
        e->accept();
    }

    if(currentMode == Scientific)
    {

    }

    e->ignore();
}

void Calculator::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if (display->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}

void Calculator::unaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = std::sqrt(operand);
    } else if (clickedOperator == tr("x\302\262")) {
        result = std::pow(operand, 2.0);
    } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
            abortOperation();
            return;
        }
        result = 1.0 / operand;
    }
    display->setText(QString::number(result));
    waitingForOperand = true;
}

void Calculator::additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if (!clickedButton)
      return;
    QString clickedOperator = clickedButton->text();
    additiveOperator(clickedOperator);
}

void Calculator::additiveOperator(QString oper)
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = oper;
    waitingForOperand = true;
}
void Calculator::multiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if (!clickedButton)
      return;
    QString clickedOperator = clickedButton->text();
    multiplicativeOperator(clickedOperator);
}

void Calculator::multiplicativeOperator(QString oper)
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = oper;
    waitingForOperand = true;
}

void Calculator::showScientificWidgets(bool visible)
{
    for(QWidget* w : sciWidgets)
        w->setVisible(visible);
}

void Calculator::equalClicked()
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}

void Calculator::pointClicked()
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains('.'))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}

void Calculator::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}

void Calculator::backspaceClicked()
{
    if (waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    display->setText(text);
}

void Calculator::clear()
{
    if (waitingForOperand)
        return;

    display->setText("0");
    waitingForOperand = true;
}

void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}

void Calculator::clearMemory()
{
    sumInMemory = 0.0;
}

void Calculator::readMemory()
{
    display->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

void Calculator::setMemory()
{
    equalClicked();
    sumInMemory = display->text().toDouble();
}

void Calculator::addToMemory()
{
    equalClicked();
    sumInMemory += display->text().toDouble();
}

void Calculator::undefined()
{

}

Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("####"));
}

bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\303\267")) {
        if (rightOperand == 0.0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}

