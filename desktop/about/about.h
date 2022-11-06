#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class AboutWindow : public QDialog
{
    Q_OBJECT

public:
    AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow() = default;

    struct CpuID {
        uint processor = 0;
        uint cores = 1;
        float mhz = 0;
        QByteArray cache = NULL;
        float mips = 0;
        QByteArray vendor = NULL;
        QByteArray model = NULL;
        QByteArray modelname = NULL;
        QByteArray stepping = 0;
        int revision = 0;
        QByteArray x86vendor = NULL;
        QByteArray physicalPackage = NULL;
        QByteArray coreSiblings = NULL;
    };
protected:
    void paintEvent(QPaintEvent *event);
private:
    void setupUi();
    void populateKernel();
    void populateApi();
    void populateSMBIOS();
    void populateCPUID();
    void processCpuidBlock(QList<QByteArray> d);
    void arm_cpu_decode(struct CpuID *desc);
    void populateMemory();
    void populateSysDisk();
    void populateGPU();
    void populateDeviceTree();
private:
    QPixmap *m_lightBg;
    QPixmap *m_darkBg;

    QVBoxLayout *vl_main;
    QHBoxLayout *hl_branding;
    QLabel *logo_icon;
    QLabel *hollywood_label;
    QLabel *hw_train;
    QFormLayout *fl_versions;
    QLabel *l_hwsh;
    QLabel *m_shellVersion;
    QLabel *l_kv;
    QLabel *m_kernelVersion;
    QLabel *l_api;
    QLabel *m_apiVersions;
    QLabel *l_opengl;
    QLabel *m_opengl;

    QLabel *l_hw;
    QLabel *m_hardware;
    QLabel *l_proc;
    QLabel *m_proc;
    QLabel *l_mem;
    QLabel *m_memory;
    QLabel *l_gpu;
    QLabel *m_gpu;

    QLabel *l_sysdisk;
    QLabel *m_systemdisk;
    QLabel *lbl_copyright;
    QHBoxLayout *hl_buttons;
    QPushButton *m_sysreport;
    QPushButton *m_update;
    QPushButton *m_rights;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_help;

    QList<CpuID> m_cpus;
};
#endif // ABOUTWINDOW_H
