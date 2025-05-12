#include "TargetDialog.hpp"
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

TargetDialog::TargetDialog(const QStringList& playerNames,
    const std::vector<coup::Player*>& playerRefs,
    QWidget* parent)
: QDialog(parent), players(playerRefs)
{
setWindowTitle("Choose a Target");
QVBoxLayout* layout = new QVBoxLayout(this);

listWidget = new QListWidget(this);
listWidget->addItems(playerNames);
layout->addWidget(listWidget);

QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
layout->addWidget(buttons);

connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString TargetDialog::getSelectedPlayerName() const {
QListWidgetItem* selected = listWidget->currentItem();
return selected ? selected->text() : "";
}

coup::Player* TargetDialog::getSelectedPlayer() const {
int index = listWidget->currentRow();
return (index >= 0 && index < players.size()) ? players[index] : nullptr;
}
