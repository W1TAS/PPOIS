import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QAction, QMenuBar, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget
from controllers.main_controller import MainController

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('Football Player Manager')
        self.setGeometry(100, 100, 800, 600)

        self._create_table()
        self.controller = MainController(self)

        self._create_menu()

    def _create_menu(self):
        menubar = self.menuBar()

        player_menu = menubar.addMenu('Players')

        add_action = QAction('Add Player', self)
        add_action.triggered.connect(self.controller.open_add_player_dialog)
        player_menu.addAction(add_action)

        search_action = QAction('Search Players', self)
        search_action.triggered.connect(self.controller.open_search_dialog)
        player_menu.addAction(search_action)

        delete_action = QAction('Delete Players', self)
        delete_action.triggered.connect(self.controller.open_delete_dialog)
        player_menu.addAction(delete_action)

    def _create_table(self):
        self.table = QTableWidget()
        self.table.setColumnCount(6)
        self.table.setHorizontalHeaderLabels(['Full Name', 'Birth Date', 'Team', 'Home City', 'Squad', 'Position'])

        layout = QVBoxLayout()
        layout.addWidget(self.table)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def update_table(self, players):
        self.table.setRowCount(len(players))
        for row, player in enumerate(players):
            self.table.setItem(row, 0, QTableWidgetItem(player.full_name))
            self.table.setItem(row, 1, QTableWidgetItem(player.birth_date.isoformat()))
            self.table.setItem(row, 2, QTableWidgetItem(player.team))
            self.table.setItem(row, 3, QTableWidgetItem(player.home_city))
            self.table.setItem(row, 4, QTableWidgetItem(player.squad))
            self.table.setItem(row, 5, QTableWidgetItem(player.position))

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
