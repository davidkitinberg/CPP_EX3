#include "GameSetupDialog.hpp"
#include "ui_GameSetupDialog.h"
#include "../../Headers/PlayerFactory.hpp"

GameSetupDialog::GameSetupDialog(coup::Game* game, QWidget* parent)
    : QDialog(parent), ui(new Ui::GameSetupDialog), game(game) {
    ui->setupUi(this);

    connect(ui->btnAddPlayer, &QPushButton::clicked, this, &GameSetupDialog::onAddPlayerClicked);
    connect(ui->btnStartGame, &QPushButton::clicked, this, &GameSetupDialog::onStartGameClicked);

    ui->btnStartGame->setEnabled(false);
}

GameSetupDialog::~GameSetupDialog() {
    delete ui;
}

void GameSetupDialog::onAddPlayerClicked() {
    QString name = ui->lineEditName->text();
    if (name.isEmpty()) return;

    try {
        game->addPlayerWithRandomRole(name.toStdString());
        playerNames.append(name);
        ui->listWidgetPlayers->addItem(name);

        if (playerNames.size() >= 2)
            ui->btnStartGame->setEnabled(true);

        ui->lineEditName->clear();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void GameSetupDialog::onStartGameClicked() {
    accept();  // closes dialog with QDialog::Accepted
}
