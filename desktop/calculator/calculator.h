/****************************************************************************
** Arion Calculator
** Copyright (C) 2022 The Cat and Contributors to Arion
** Portions Copyright (C) 2016 The Qt Company Ltd.
** spdx-license-identifier: bsd-3-clause
****************************************************************************/

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QToolButton>

class QLineEdit;
class Button : public QToolButton
{
    Q_OBJECT

public:
    explicit Button(const QString &text, QWidget *parent = nullptr);
    QSize sizeHint() const override;
public slots:
    void setBackUp();
};

class QGridLayout;
class Calculator : public QWidget
{
    Q_OBJECT
public:
    enum OperationMode {
        Basic,
        Scientific
    };
    Calculator(QWidget *parent = nullptr);
    void setOperationMode(OperationMode mode);
signals:
    void resizeRequest();
protected:
    void keyPressEvent(QKeyEvent *e);
private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void clear();
    void clearAll();
    void clearMemory();
    void readMemory();
    void setMemory();
    void addToMemory();
    void undefined();
private:
    void additiveOperator(QString oper);
    void multiplicativeOperator(QString oper);
    void showScientificWidgets(bool visible);
private:
    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);
    double sumInMemory;
    double sumSoFar;
    double factorSoFar;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;
    QLineEdit *display;
    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];
    Button *equalButton;
    Button *plusButton;
    Button *minusButton;
    Button *timesButton;
    Button *divisionButton;
    Button *pointButton;
    Button *backspaceButton;
    Button *clearButton;
    QList<QWidget*> sciWidgets;
    QGridLayout *basiclayout;
    QGridLayout *scilayout;
    OperationMode currentMode = Basic;
};
#endif
