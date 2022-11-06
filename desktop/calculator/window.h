/****************************************************************************
** Arion Calculator
** Copyright (C) 2022 The Cat and Contributors to Arion
** spdx-license-identifier: bsd-3-clause
****************************************************************************/

#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#define ARION_CALCULATOR_VERSION  "1.0"

#include <QMainWindow>
#include <QPlainTextEdit>

class Calculator;
class CalculatorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CalculatorWindow(QWidget *parent = nullptr);
private slots:
    void copyTape();
    void copyValue();
    void pasteValue();
    void clipboardChanged();
    void togglePaperTape(bool showTape);
    void requestResize();
    void changeOperationMode();
    void saveTape();
    void printTape();
    void about();
private:
    Calculator *m_calc = nullptr;
    QPlainTextEdit *m_tape = nullptr;
    QAction *m_paste = nullptr;
    bool m_showtape = false;
};

#endif // CALCULATORWINDOW_H
