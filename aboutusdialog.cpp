#include "aboutusdialog.h"
#include <QFont>

AboutUsDialog::AboutUsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("About us"));
    setMinimumWidth(300);

    auto *layout = new QVBoxLayout(this);

    titleLabel = new QLabel(tr("Application Authors"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    authorsLabel = new QLabel(tr("This application was created by:"), this);

    author1Label = new QLabel("• Mateusz Korniak - " + tr("Lead Developer"), this);
    author2Label = new QLabel("• Mateusz Machowski - " + tr("Lead Developer"), this);
    author3Label = new QLabel("• Filip Leśnik - " + tr("Tester / 2nd Developer"), this);

    closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addSpacing(10);
    layout->addWidget(authorsLabel);
    layout->addSpacing(5);
    layout->addWidget(author1Label);
    layout->addWidget(author2Label);
    layout->addWidget(author3Label);
    layout->addSpacing(10);
    layout->addWidget(closeButton);

    setLayout(layout);
}

void AboutUsDialog::retranslateUi() {
    setWindowTitle(tr("About us"));
    titleLabel->setText(tr("Application Authors"));
    authorsLabel->setText(tr("This application was created by:"));

    author1Label->setText("• Jan Kowalski - " + tr("Lead Developer"));
    author2Label->setText("• Anna Nowak - " + tr("UI/UX Designer"));
    author3Label->setText("• Piotr Wiśniewski - " + tr("Testing Engineer"));

    closeButton->setText(tr("Close"));
}