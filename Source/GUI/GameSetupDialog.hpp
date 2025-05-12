#pragma once
#include <QDialog>
#include <vector>
#include <QStringList>
#include "../../Headers/Game.hpp"

namespace Ui {
    class GameSetupDialog;
}

class GameSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit GameSetupDialog(coup::Game* game, QWidget* parent = nullptr);
    ~GameSetupDialog();

private slots:
    void onAddPlayerClicked();
    void onStartGameClicked();

private:
    Ui::GameSetupDialog* ui;
    coup::Game* game;
    QStringList playerNames;
};
