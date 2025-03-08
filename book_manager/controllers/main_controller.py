from models.player_database import PlayerDatabase
from views.dialogs.add_player_dialog import AddPlayerDialog
from views.dialogs.search_dialog import SearchDialog
from views.dialogs.delete_dialog import DeleteDialog

class MainController:
    def __init__(self, main_window):
        self.main_window = main_window
        self.db = PlayerDatabase()
        self.load_players()

    def load_players(self):
        players = self.db.get_all_players()
        self.main_window.update_table(players)

    def open_add_player_dialog(self):
        print("Opening add player dialog...")
        dialog = AddPlayerDialog(self.main_window)
        print("Dialog created.")
        if dialog.exec_():
            player = dialog.get_player()
            self.db.add_player(player)
            self.load_players()
        print("Dialog closed.")

    def open_search_dialog(self):
        print("Opening search dialog")
        dialog = SearchDialog(self.main_window, self.db)
        print("Dialog created")
        dialog.exec_()
        print("Dialog closed")

    def open_delete_dialog(self):
        dialog = DeleteDialog(self.main_window, self.db)
        if dialog.exec_():
            self.load_players()
