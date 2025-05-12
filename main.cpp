#include <QApplication>
#include "Source/GUI/CoupUI.hpp"
#include "Headers/Game.hpp"

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     coup::Game* game = new coup::Game();
//     CoupUI window;
//     window.setGame(game);  // <-- this line connects logic to UI

//     window.show();
//     return app.exec();
// }

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    coup::Game game;

    GameSetupDialog setupDialog(&game);
    if (setupDialog.exec() == QDialog::Accepted) {
        CoupUI mainWindow;
        mainWindow.setGame(&game);
        mainWindow.show();
        return app.exec();
    }

    return 0;
}