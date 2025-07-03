/*
* Nazwa Projektu: EMPE
 * Plik: aboutusdialog.cpp
 *
 * Krótki opis pliku: Implementacja okna dialogowego informującego o autorach aplikacji i projekcie.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 10 Marca 2025
 * Ostatnia Modyfikacja: 18 Czerwaca 2025
 *
 * Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub
 * modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
 * opublikowanej przez Free Software Foundation, w wersji 3 tej Licencji
 * lub (według twojego wyboru) dowolnej późniejszej wersji.
 *
 * Ten program jest rozpowszechniany w nadziei, że będzie użyteczny, ale
 * BEZ ŻADNEJ GWARANCJI; nawet bez domyślnej gwarancji PRZYDATNOŚCI
 * HANDLOWEJ lub PRZYDATNOŚCI DO OKREŚLONEGO CELU. Zobacz Powszechną
 * Licencję Publiczną GNU, aby uzyskać więcej szczegółów.
 *
 * Powinieneś otrzymać kopię Powszechnej Licencji Publicznej GNU wraz z
 * tym programem. Jeśli nie, zobacz <http://www.gnu.org/licenses/>.
*/



#include "aboutusdialog.h"
#include <QEvent>
#include <QFont>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>

AboutUsDialog::AboutUsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("About Us"));
    setMinimumWidth(400);

    auto *layout = new QVBoxLayout(this);

    /*titleLabel = new QLabel(tr("This program was created as part of the project:"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    authorsLabel = new QLabel(tr("Embodying Math&Physics Education 2023-1-PL01-KA210-SCH-000165829"), this);
    authorsLabel->setAlignment(Qt::AlignCenter);
    authorsLabel->setWordWrap(true);
    */
    QLabel *authorsHeaderLabel = new QLabel(tr("The program was written as part of the Procedural Programming course under the supervision of Dr. Hab. Roman Rosiek and was developed by:"), this);
    QFont authorsFont = authorsHeaderLabel->font();
    authorsFont.setBold(true);
    authorsHeaderLabel->setFont(authorsFont);
    authorsHeaderLabel->setAlignment(Qt::AlignCenter);

    QLabel *authorsList = new QLabel(tr("Mateusz Korniak <mkorniak04@gmail.com>\n"
                                      "Mateusz Machowski <machowskimateusz51@gmail.com>\n"
                                      "Filip Leśnik <filip.lesnik170@gmail.com>"), this);
    authorsList->setAlignment(Qt::AlignCenter);

    closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    //layout->addWidget(titleLabel);
    //layout->addSpacing(10);
    //layout->addWidget(authorsLabel);
    layout->addSpacing(15);
    layout->addWidget(authorsHeaderLabel);
    layout->addSpacing(5);
    layout->addWidget(authorsList);
    layout->addSpacing(15);

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