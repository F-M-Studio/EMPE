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

    [[nodiscard]] QString getPortName() const;
    [[nodiscard]] int getBaudRate() const;
    [[nodiscard]] int getDataBits() const;
    [[nodiscard]] int getStopBits() const;
    [[nodiscard]] int getParity() const;
    [[nodiscard]] int getFlowControl() const;

private slots:
    void refreshPorts() const;

private:
    QComboBox *portBox{};
    QComboBox *baudRateBox{};
    QComboBox *dataBitsBox{};
    QComboBox *stopBitsBox{};
    QComboBox *parityBox{};
    QComboBox *flowControlBox{};

    QPushButton *okButton{};
    QPushButton *cancelButton{};
    QPushButton *refreshButton{};

    void setupUI();
};

#endif // PORTSETTINGS_H