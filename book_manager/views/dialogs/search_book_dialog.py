from PyQt5.QtWidgets import QDialog, QFormLayout, QLineEdit, QPushButton, QVBoxLayout, QTableWidget, QTableWidgetItem, QSpinBox

class SearchBookDialog(QDialog):
    def __init__(self, parent, db):
        super().__init__(parent)
        self.setWindowTitle('Search Books')
        self.setFixedSize(500, 400)
        self.db = db

        layout = QVBoxLayout()

        form_layout = QFormLayout()
        self.title_input = QLineEdit()
        self.author_input = QLineEdit()
        self.publisher_input = QLineEdit()
        self.min_volumes_input = QSpinBox()
        self.min_volumes_input.setMinimum(1)
        self.max_volumes_input = QSpinBox()
        self.max_volumes_input.setMinimum(1)
        self.max_volumes_input.setValue(10)

        form_layout.addRow('Title:', self.title_input)
        form_layout.addRow('Author:', self.author_input)
        form_layout.addRow('Publisher:', self.publisher_input)
        form_layout.addRow('Min Volumes:', self.min_volumes_input)
        form_layout.addRow('Max Volumes:', self.max_volumes_input)

        search_button = QPushButton('Search')
        search_button.clicked.connect(self.search_books)

        self.results_table = QTableWidget()
        self.results_table.setColumnCount(5)
        self.results_table.setHorizontalHeaderLabels(['Title', 'Author', 'Publisher', 'Volumes', 'Circulation'])

        layout.addLayout(form_layout)
        layout.addWidget(search_button)
        layout.addWidget(self.results_table)

        self.setLayout(layout)

    def search_books(self):
        title = self.title_input.text()
        author = self.author_input.text()
        publisher = self.publisher_input.text()
        min_volumes = self.min_volumes_input.value()
        max_volumes = self.max_volumes_input.value()

        books = self.db.search_books(title, author, publisher, min_volumes, max_volumes)

        if not books:
            self.results_table.setRowCount(0)
            return

        self.results_table.setRowCount(len(books))
        for row, book in enumerate(books):
            self.results_table.setItem(row, 0, QTableWidgetItem(book.title))
            self.results_table.setItem(row, 1, QTableWidgetItem(book.author))
            self.results_table.setItem(row, 2, QTableWidgetItem(book.publisher))
            self.results_table.setItem(row, 3, QTableWidgetItem(str(book.volumes)))
            self.results_table.setItem(row, 4, QTableWidgetItem(str(book.circulation)))
