#include "aboutusdialog.h"
#include <QFont>

AboutUsDialog::AboutUsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("About us"));
    setMinimumWidth(300);

    auto *layout = new QVBoxLayout(this);

    titleLabel = new QLabel(tr("Projekt powstał w ramach projektu:"), this);
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
    layout->addSpacing(10);
    layout->addWidget(closeButton);

    setLayout(layout);
}