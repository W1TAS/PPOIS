from PyQt5.QtWidgets import QDialog, QFormLayout, QLineEdit, QPushButton, QMessageBox

class DeleteBookDialog(QDialog):
    def __init__(self, parent, db):
        super().__init__(parent)
        self.setWindowTitle('Delete Books')
        self.setFixedSize(300, 200)
        self.db = db

        layout = QFormLayout()

        self.author_input = QLineEdit()
        self.publisher_input = QLineEdit()
        layout.addRow('Author:', self.author_input)
        layout.addRow('Publisher:', self.publisher_input)

        delete_button = QPushButton('Delete')
        delete_button.clicked.connect(self.delete_books)
        layout.addWidget(delete_button)

        self.setLayout(layout)

    def delete_books(self):
        author = self.author_input.text()
        publisher = self.publisher_input.text()
        deleted_count = self.db.delete_books(author, publisher)

        if deleted_count > 0:
            QMessageBox.information(self, 'Success', f'Deleted {deleted_count} books.')
        else:
            QMessageBox.information(self, 'No Results', 'No books found matching criteria.')

        self.accept()
