#include "aboutusdialog.h"
#include <QFont>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>

AboutUsDialog::AboutUsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("About us"));
    setMinimumWidth(300);

    auto *layout = new QVBoxLayout(this);

    titleLabel = new QLabel(tr("Program powstał w ramach projektu:"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    authorsLabel = new QLabel(tr("Embodying Math&Physics Education 2023-1-PL01-KA210-SCH-000165829"), this);

    closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addSpacing(10);
    layout->addWidget(authorsLabel);
    layout->addSpacing(5);

    // Dodanie obrazka w prawym dolnym rogu
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/FundedByEU.png");
    imageLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    bottomLayout->addWidget(imageLabel);
    layout->addLayout(bottomLayout);

    layout->addSpacing(10);
    layout->addWidget(closeButton);

    setLayout(layout);
}