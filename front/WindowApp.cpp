#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

int main() {
    // Инициализация интерпретатора Python
    py::scoped_interpreter guard{};

    try {
        // Отладка
        py::module os = py::module::import("os");
        py::print(os.attr("getcwd")());

        bool exists = os.attr("path").attr("exists")("gui.py").cast<bool>();
        std::cout << "gui.py exists: " << exists << std::endl;
        
        py::exec(R"(
            import sys
            print(sys.path)
            import gui
        )");

        // Добавляем текущую папку в sys.path
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")(".");

        // Импортируем модуль GUI
        py::module gui_module = py::module::import("gui");

        // Создаем основной объект Tk для передачи в класс Window
        py::module tkinter = py::module::import("tkinter");
        py::object tk_instance = tkinter.attr("Tk")();  // Создаем экземпляр Tk

        // Создаем экземпляр класса Window, передавая tk_instance
        gui_module.attr("Window")(tk_instance);  // Вызываем класс Window с аргументом

        // Запуск главного цикла tkinter
        tkinter.attr("mainloop")();  // Запускаем основной цикл Tkinter

    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "C++ exception: " << e.what() << std::endl;
    }

    return 0;
}
