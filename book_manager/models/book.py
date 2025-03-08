from datetime import date
from dataclasses import dataclass

@dataclass
class Book:
    title: str
    author: str
    publisher: str
    volume_count: int
    circulation: int

    @property
    def total_volumes(self):
        return self.volume_count * self.circulation

    def to_dict(self):
        return {
            'title': self.title,
            'author': self.author,
            'publisher': self.publisher,
            'volume_count': self.volume_count,
            'circulation': self.circulation,
            'total_volumes': self.total_volumes
        }

    @classmethod
    def from_dict(cls, data):
        return cls(
            title=data['title'],
            author=data['author'],
            publisher=data['publisher'],
            volume_count=int(data['volume_count']),
            circulation=int(data['circulation'])
        )