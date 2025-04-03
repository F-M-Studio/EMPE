#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class AboutUsDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutUsDialog(QWidget *parent = nullptr);
    ~AboutUsDialog() override = default;

    void retranslateUi();

private:
    QLabel *titleLabel;
    QLabel *authorsLabel;
    QLabel *author1Label;
    QLabel *author2Label;
    QLabel *author3Label;
    QPushButton *closeButton;
};