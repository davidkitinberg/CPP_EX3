#pragma once
#include <QDialog>
#include <QStringList>
#include "../../Headers/Player.hpp"  // make sure you have access

namespace coup { class Player; }

class TargetDialog : public QDialog {
    Q_OBJECT

public:
    explicit TargetDialog(const QStringList& playerNames,
                          const std::vector<coup::Player*>& playerRefs,
                          QWidget* parent = nullptr);

    QString getSelectedPlayerName() const;
    coup::Player* getSelectedPlayer() const;

private:
    std::vector<coup::Player*> players;
    QListWidget* listWidget;
};
