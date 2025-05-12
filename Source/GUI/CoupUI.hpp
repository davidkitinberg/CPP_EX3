#pragma once
#include <QWidget>
#include "../../Headers/Game.hpp"

namespace Ui {
    class CoupUI;
}

class CoupUI : public QWidget {
    Q_OBJECT

public:
    explicit CoupUI(QWidget *parent = nullptr);
    ~CoupUI();

    void setGame(coup::Game* game);

private slots:
    void onArrestClicked();
    void appendLog(const QString& text);
    void updateActionButtons();

private:
    Ui::CoupUI* ui;
    coup::Game* game = nullptr;
};
