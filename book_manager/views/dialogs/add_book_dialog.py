from PyQt5.QtWidgets import QDialog, QFormLayout, QLineEdit, QPushButton, QDateEdit, QComboBox
from models.book import Player
from datetime import date

class AddPlayerDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle('Add Player')
        self.setFixedSize(300, 300)

        layout = QFormLayout()

        self.full_name_input = QLineEdit()
        self.birth_date_input = QDateEdit()
        self.birth_date_input.setCalendarPopup(True)
        self.birth_date_input.setDate(date.today())

        self.team_input = QLineEdit()
        self.home_city_input = QLineEdit()
        self.squad_input = QComboBox()
        self.squad_input.addItems(['Main', 'Reserve', 'Youth'])
        self.position_input = QLineEdit()

        layout.addRow('Full Name:', self.full_name_input)
        layout.addRow('Birth Date:', self.birth_date_input)
        layout.addRow('Team:', self.team_input)
        layout.addRow('Home City:', self.home_city_input)
        layout.addRow('Squad:', self.squad_input)
        layout.addRow('Position:', self.position_input)

        submit_button = QPushButton('Add')
        submit_button.clicked.connect(self.accept)
        layout.addWidget(submit_button)

        self.setLayout(layout)

    def get_player(self):
        return Player(
            full_name=self.full_name_input.text(),
            birth_date=self.birth_date_input.date().toPyDate(),
            team=self.team_input.text(),
            home_city=self.home_city_input.text(),
            squad=self.squad_input.currentText(),
            position=self.position_input.text()
        )
