import sqlite3
import xml.etree.ElementTree as ET
from models.book import Book

class BookDatabase:
    def __init__(self, db_path='books.db'):
        self.db_path = db_path
        self._create_table()

    def _create_table(self):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute('''CREATE TABLE IF NOT EXISTS books (
                title TEXT,
                author TEXT,
                publisher TEXT,
                volume_count INTEGER,
                circulation INTEGER
            )''')
            conn.commit()

    def add_book(self, book: Book):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute('INSERT INTO books VALUES (?, ?, ?, ?, ?)',
                           (book.title, book.author, book.publisher, book.volume_count, book.circulation))
            conn.commit()

    def get_all_books(self):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT * FROM books')
            rows = cursor.fetchall()
            return [Book.from_dict(dict(zip(['title', 'author', 'publisher', 'volume_count', 'circulation'], row))) for row in rows]

    def search_books(self, title=None, author=None, publisher=None, min_volumes=None, max_volumes=None, min_circulation=None, max_circulation=None):
        query = 'SELECT * FROM books WHERE 1=1'
        params = []

        if title:
            query += ' AND title LIKE ?'
            params.append('%' + title + '%')
        if author:
            query += ' AND author LIKE ?'
            params.append('%' + author + '%')
        if publisher:
            query += ' AND publisher LIKE ?'
            params.append('%' + publisher + '%')
        if min_volumes is not None:
            query += ' AND volume_count >= ?'
            params.append(min_volumes)
        if max_volumes is not None:
            query += ' AND volume_count <= ?'
            params.append(max_volumes)
        if min_circulation is not None:
            query += ' AND circulation >= ?'
            params.append(min_circulation)
        if max_circulation is not None:
            query += ' AND circulation <= ?'
            params.append(max_circulation)

        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute(query, params)
            rows = cursor.fetchall()
            return [Book.from_dict(dict(zip(['title', 'author', 'publisher', 'volume_count', 'circulation'], row))) for row in rows]

    def delete_books(self, author=None, publisher=None, title=None):
        query = 'DELETE FROM books WHERE 1=1'
        params = []

        if author:
            query += ' AND author LIKE ?'
            params.append('%' + author + '%')
        if publisher:
            query += ' AND publisher LIKE ?'
            params.append('%' + publisher + '%')
        if title:
            query += ' AND title LIKE ?'
            params.append('%' + title + '%')

        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute(query, params)
            conn.commit()
            return cursor.rowcount
