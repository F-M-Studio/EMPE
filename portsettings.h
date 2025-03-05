#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

class PortSettings : public QDialog {
    Q_OBJECT

public:
    explicit PortSettings(QWidget *parent = nullptr);

    QString getPortName() const;
    int getBaudRate() const;
    int getDataBits() const;
    int getStopBits() const;
    int getParity() const;
    int getFlowControl() const;

private:
    QComboBox *portBox;
    QComboBox *baudRateBox;
    QComboBox *dataBitsBox;
    QComboBox *stopBitsBox;
    QComboBox *parityBox;
    QComboBox *flowControlBox;

    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUI();
};

#endif // PORTSETTINGS_H