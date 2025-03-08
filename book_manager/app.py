import sys
from PyQt5.QtWidgets import QApplication
from views.main_window import MainWindow
from controllers.main_controller import MainController


if __name__ == '__main__':
    app = QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    main_window.controller.load_players()  # Вызов после show
    sys.exit(app.exec_())
