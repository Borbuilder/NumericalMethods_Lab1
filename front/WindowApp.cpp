#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

int main() {
    // ������������� �������������� Python
    py::scoped_interpreter guard{};

    try {
        // �������
        py::module os = py::module::import("os");
        py::print(os.attr("getcwd")());

        bool exists = os.attr("path").attr("exists")("gui.py").cast<bool>();
        std::cout << "gui.py exists: " << exists << std::endl;
        
        py::exec(R"(
            import sys
            print(sys.path)
            import gui
        )");

        // ��������� ������� ����� � sys.path
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")(".");

        // ����������� ������ GUI
        py::module gui_module = py::module::import("gui");

        // ������� �������� ������ Tk ��� �������� � ����� Window
        py::module tkinter = py::module::import("tkinter");
        py::object tk_instance = tkinter.attr("Tk")();  // ������� ��������� Tk

        // ������� ��������� ������ Window, ��������� tk_instance
        gui_module.attr("Window")(tk_instance);  // �������� ����� Window � ����������

        // ������ �������� ����� tkinter
        tkinter.attr("mainloop")();  // ��������� �������� ���� Tkinter

    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "C++ exception: " << e.what() << std::endl;
    }

    return 0;
}
