import tkinter as tk
from gui import Window  # Импортируем класс Window из модуля gui


def main():
    # Создаем основной объект Tk
    root = tk.Tk()

    # Создаем экземпляр класса Window, передавая root как аргумент
    app = Window(root)

    # Запуск основного цикла tkinter
    root.mainloop()


if __name__ == "__main__":
    main()
