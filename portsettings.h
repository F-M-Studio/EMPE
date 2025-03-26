#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QTabWidget>

class PortSettings : public QDialog {
    Q_OBJECT

public:
    explicit PortSettings(QWidget *parent = nullptr);

    QString getPortName1() const;
    int getBaudRate1() const;
    int getDataBits1() const;
    int getStopBits1() const;
    int getParity1() const;
    int getFlowControl1() const;

    QString getPortName2() const;
    int getBaudRate2() const;
    int getDataBits2() const;
    int getStopBits2() const;
    int getParity2() const;
    int getFlowControl2() const;

    private slots:
        void refreshPorts();

private:
    // Port 1 controls
    QComboBox *portBox1;
    QComboBox *baudRateBox1;
    QComboBox *dataBitsBox1;
    QComboBox *stopBitsBox1;
    QComboBox *parityBox1;
    QComboBox *flowControlBox1;

    // Port 2 controls
    QComboBox *portBox2;
    QComboBox *baudRateBox2;
    QComboBox *dataBitsBox2;
    QComboBox *stopBitsBox2;
    QComboBox *parityBox2;
    QComboBox *flowControlBox2;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *refreshButton;
    QTabWidget *tabWidget;

    void setupUI();
    QWidget* createPortTab(const QString& tabName, QComboBox* &portBox,
                          QComboBox* &baudRateBox, QComboBox* &dataBitsBox,
                          QComboBox* &stopBitsBox, QComboBox* &parityBox,
                          QComboBox* &flowControlBox);
};

#endif // PORTSETTINGS_H