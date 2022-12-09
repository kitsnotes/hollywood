#ifndef ICONVIEWOPTIONS_H
#define ICONVIEWOPTIONS_H

#include "libshell_int.h"

class LSIconViewOptionsPrivate;
class LIBSHELL_EXPORT LSIconViewOptions : public QWidget
{
    Q_OBJECT
public:
    explicit LSIconViewOptions(QWidget *parent = nullptr);
    void attachView(QListView *view);
    void refreshViewOptions();
private:
    LSIconViewOptionsPrivate *p;
};

#endif // ICONVIEWOPTIONS_H
