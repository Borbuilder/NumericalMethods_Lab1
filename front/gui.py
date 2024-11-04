import tkinter as tk
from tkinter import messagebox
from tkinter import ttk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
from Lab_1_Module import TestTask, FirstTask, SecondTask

class Window:
    def __init__(self, master):
        self.master = master
        self.master.title("Задача Коши для ОДУ")
        self.master.geometry("1200x600")

        # Create interface elements for task selection
        tk.Label(self.master, text="Задача").grid(row=0, column=0, sticky='w')
        
        # Dropdown for task selection
        self.task_selector = ttk.Combobox(self.master, values=["Тестовая", "Первая", "Вторая"])
        self.task_selector.grid(row=1, column=0, sticky='w')
        self.task_selector.set("Тестовая")  # Default value
        self.task_selector.bind("<<ComboboxSelected>>", self.on_task_selected)

        # Display widgets for "Тестовая задача" by default
        self.create_test_task_widgets()

    def on_task_selected(self, event):
        selected_task = self.task_selector.get()
        self.clear_widgets()

        # Re-add dropdown and label after clearing
        tk.Label(self.master, text="Задача").grid(row=0, column=0, sticky='w')
        self.task_selector.grid(row=1, column=0, sticky='w')

        # Choose which widgets to display based on selected task
        if selected_task == "Тестовая":
            self.create_test_task_widgets()
        elif selected_task == "Первая":
            self.create_first_task_widgets()
        elif selected_task == "Вторая":
            self.create_second_task_widgets()

    def clear_widgets(self):
        # Remove all widgets except the dropdown
        for widget in self.master.winfo_children():
            if widget not in [self.task_selector, tk.Label(self.master, text="Задача")]:
                widget.destroy()

    def create_test_task_widgets(self):
        self.master.grid_columnconfigure(0, weight=100)
        self.master.grid_columnconfigure(1, weight=2)
        self.master.grid_columnconfigure(2, weight=2)
        self.master.grid_columnconfigure(3, weight=2)

        # Input fields for test task
        tk.Label(self.master, text="A:").grid(row=0, column=1, sticky='e')
        tk.Label(self.master, text="B:").grid(row=1, column=1, sticky='e')
        tk.Label(self.master, text="U0:").grid(row=2, column=1, sticky='e')
        tk.Label(self.master, text="Начальный шаг:").grid(row=3, column=1, sticky='e')
        tk.Label(self.master, text="Контроль лок. погрешности:").grid(row=4, column=1, sticky='e')
        tk.Label(self.master, text="Точность выхода на границу:").grid(row=5, column=1, sticky='e')
        tk.Label(self.master, text="Макс. число шагов:").grid(row=6, column=1, sticky='e')

        # Input fields
        self.entry_a = tk.Entry(self.master)
        self.entry_b = tk.Entry(self.master)
        self.entry_u0 = tk.Entry(self.master)
        self.entry_step_size = tk.Entry(self.master)
        self.entry_max_e_error = tk.Entry(self.master)
        self.entry_e_border = tk.Entry(self.master)
        self.entry_max_steps = tk.Entry(self.master)

        # Set default values
        self.entry_a.insert(0, "0.0")
        self.entry_b.insert(0, "1.0")
        self.entry_u0.insert(0, "1.0")
        self.entry_step_size.insert(0, "0.001")
        self.entry_max_e_error.insert(0, "1e-6")
        self.entry_e_border.insert(0, "1e-6")
        self.entry_max_steps.insert(0, "10000")

        # Placement of input fields
        self.entry_a.grid(row=0, column=2, sticky='ew')
        self.entry_b.grid(row=1, column=2, sticky='ew')
        self.entry_u0.grid(row=2, column=2, sticky='ew')
        self.entry_step_size.grid(row=3, column=2, sticky='ew')
        self.entry_max_e_error.grid(row=4, column=2, sticky='ew')
        self.entry_e_border.grid(row=5, column=2, sticky='ew')
        self.entry_max_steps.grid(row=6, column=2, sticky='ew')

        # Toggle button
        tk.Label(self.master, text="Режим работы").grid(row=3, column=0, sticky='w')
        self.button_mode = tk.Button(self.master, text="С ОЛП", command=self.toggle_mode)
        self.button_mode.grid(row=4, column=0, sticky='we')

        # Calculate button
        self.calculate_button = tk.Button(self.master, text="Вычислить", command=self.calculate_values)
        self.calculate_button.grid(row=6, column=0, sticky='we')

        # Create Treeview to display the table
        self.tree = ttk.Treeview(self.master, columns=["Col"+str(i) for i in range(10)], show="headings")
        self.tree.grid(row=8, column=0, columnspan=3, sticky='nsew', padx=5, pady=5)  # Place the table under input fields

        # Add horizontal and vertical scrolling
        vsb = ttk.Scrollbar(self.master, orient="vertical", command=self.tree.yview)
        vsb.grid(row=7, column=2, sticky='sne', rowspan=3)
        self.tree.configure(yscrollcommand=vsb.set)

        hsb = ttk.Scrollbar(self.master, orient="horizontal", command=self.tree.xview)
        hsb.grid(row=9, column=0, columnspan=3, sticky='esw')
        self.tree.configure(xscrollcommand=hsb.set)

        # Configure stretching
        self.master.grid_rowconfigure(8, weight=1)  # Allows the table to expand vertically
        self.master.grid_columnconfigure(0, weight=1)  # Allows the table to expand horizontally

        # Create figure for the plot
        self.figure = Figure(figsize=(5, 4), dpi=100)
        self.ax = self.figure.add_subplot(111)
        self.canvas = FigureCanvasTkAgg(self.figure, master=self.master)

        # Create a Text widget for final report
        self.final_report_text = tk.Text(self.master, height=10, width=50)
        self.final_report_text.grid(row=0, column=4, rowspan=7, sticky='nwe', padx=5, pady=5)

        # Place the canvas (the graph) in the bottom right
        self.canvas.get_tk_widget().grid(row=6, column=4, rowspan=5, sticky='se', padx=5, pady=5)

    def create_first_task_widgets(self):
        self.master.grid_columnconfigure(0, weight=100)
        self.master.grid_columnconfigure(1, weight=2)
        self.master.grid_columnconfigure(2, weight=2)
        self.master.grid_columnconfigure(3, weight=2)

        # Input fields for test task
        tk.Label(self.master, text="A:").grid(row=0, column=1, sticky='e')
        tk.Label(self.master, text="B:").grid(row=1, column=1, sticky='e')
        tk.Label(self.master, text="U0:").grid(row=2, column=1, sticky='e')
        tk.Label(self.master, text="Начальный шаг:").grid(row=3, column=1, sticky='e')
        tk.Label(self.master, text="Контроль лок. погрешности:").grid(row=4, column=1, sticky='e')
        tk.Label(self.master, text="Точность выхода на границу:").grid(row=5, column=1, sticky='e')
        tk.Label(self.master, text="Макс. число шагов:").grid(row=6, column=1, sticky='e')

        # Input fields
        self.entry_a = tk.Entry(self.master)
        self.entry_b = tk.Entry(self.master)
        self.entry_u0 = tk.Entry(self.master)
        self.entry_step_size = tk.Entry(self.master)
        self.entry_max_e_error = tk.Entry(self.master)
        self.entry_e_border = tk.Entry(self.master)
        self.entry_max_steps = tk.Entry(self.master)

        # Set default values
        self.entry_a.insert(0, "0.0")
        self.entry_b.insert(0, "1.0")
        self.entry_u0.insert(0, "1.0")
        self.entry_step_size.insert(0, "0.1")
        self.entry_max_e_error.insert(0, "1e-5")
        self.entry_e_border.insert(0, "1e-6")
        self.entry_max_steps.insert(0, "10000")

        # Placement of input fields
        self.entry_a.grid(row=0, column=2, sticky='ew')
        self.entry_b.grid(row=1, column=2, sticky='ew')
        self.entry_u0.grid(row=2, column=2, sticky='ew')
        self.entry_step_size.grid(row=3, column=2, sticky='ew')
        self.entry_max_e_error.grid(row=4, column=2, sticky='ew')
        self.entry_e_border.grid(row=5, column=2, sticky='ew')
        self.entry_max_steps.grid(row=6, column=2, sticky='ew')

        # Toggle button
        tk.Label(self.master, text="Режим работы").grid(row=3, column=0, sticky='w')
        self.button_mode = tk.Button(self.master, text="С ОЛП", command=self.toggle_mode)
        self.button_mode.grid(row=4, column=0, sticky='we')

        # Calculate button
        self.calculate_button = tk.Button(self.master, text="Вычислить", command=self.calculate_values)
        self.calculate_button.grid(row=6, column=0, sticky='we')

        # Create Treeview to display the table
        self.tree = ttk.Treeview(self.master, columns=["Col"+str(i) for i in range(10)], show="headings")
        self.tree.grid(row=8, column=0, columnspan=3, sticky='nsew', padx=5, pady=5)  # Place the table under input fields

        # Add horizontal and vertical scrolling
        vsb = ttk.Scrollbar(self.master, orient="vertical", command=self.tree.yview)
        vsb.grid(row=7, column=2, sticky='sne', rowspan=3)
        self.tree.configure(yscrollcommand=vsb.set)

        hsb = ttk.Scrollbar(self.master, orient="horizontal", command=self.tree.xview)
        hsb.grid(row=9, column=0, columnspan=3, sticky='esw')
        self.tree.configure(xscrollcommand=hsb.set)

        # Configure stretching
        self.master.grid_rowconfigure(8, weight=1)  # Allows the table to expand vertically
        self.master.grid_columnconfigure(0, weight=1)  # Allows the table to expand horizontally

        # Create figure for the plot
        self.figure = Figure(figsize=(5, 4), dpi=100)
        self.ax = self.figure.add_subplot(111)
        self.canvas = FigureCanvasTkAgg(self.figure, master=self.master)

        # Create a Text widget for final report
        self.final_report_text = tk.Text(self.master, height=10, width=50)
        self.final_report_text.grid(row=0, column=4, rowspan=7, sticky='nwe', padx=5, pady=5)

        # Place the canvas (the graph) in the bottom right
        self.canvas.get_tk_widget().grid(row=6, column=4, rowspan=5, sticky='se', padx=5, pady=5)

    def create_second_task_widgets(self):
        # Here you can add input fields and logic for the second task
        tk.Label(self.master, text="Это интерфейс для Второй задачи (реализация в процессе)").grid(row=2, columnspan=2)

    def calculate_values(self):
        try:
            A = float(self.entry_a.get())
            B = float(self.entry_b.get())
            U0 = float(self.entry_u0.get())
            step_size = float(self.entry_step_size.get())
            max_e_error = float(self.entry_max_e_error.get())
            e_border = float(self.entry_e_border.get())
            max_steps = int(self.entry_max_steps.get())
            
            # select the task number
            selected_task = self.task_selector.get()
            if selected_task == "Тестовая":
                task = TestTask(A, B, step_size, max_e_error, e_border, max_steps, U0)
            elif selected_task == "Первая":
                task = FirstTask(A, B, step_size, max_e_error, e_border, max_steps, U0)
            elif selected_task == "Вторая":
                task = SecondTask(A, B, step_size, max_e_error, e_border, max_steps, U0)
        
            # Compute
            if self.button_mode.cget('text') == 'С ОЛП':
                task.Solve_With_Error_Control()
                self.update_table(task.get_table_information())
            else:
                task.Solve_Without_Error_Control()
                self.update_table(task.get_table_information())

            # Get the final report
            final_ref = task.get_final_reference()
            self.show_final_reference(final_ref)

            # Update the plot with U(x) and V(x)
            self.plot_graph(U0, A, B, task.get_table_information())

        except ValueError:
            messagebox.showerror("Ошибка", "Пожалуйста, введите корректные значения.")

    def toggle_mode(self):
        # Toggle button text
        if self.button_mode.cget('text') == 'С ОЛП':
            self.button_mode.config(text='Без ОЛП')
        else:
            self.button_mode.config(text='С ОЛП')

    def update_table(self, table_data):
        # Clear Treeview before updating
        for row in self.tree.get_children():
            self.tree.delete(row)

        # Set column headers
        if self.button_mode.cget('text') == 'Без ОЛП':
            columns = ["i", "X", "V", "h"]
            arr_width = [50, 100, 100, 100]
        else:
            columns = ["i", "X", "V", "V^", "V-V^", "S", "h", "/2", "*2"]
            arr_width = [50, 100, 100, 100, 100, 100, 100, 50, 50]
        
        if self.task_selector.get() == "Тестовая":
            columns += ["U", "|U-V|"]
            arr_width += [100, 100]

        if list(self.tree["columns"]) != columns:
            self.tree["columns"] = columns
            for i in range(len(columns)):
                self.tree.heading(columns[i], text=columns[i])
                self.tree.column(columns[i], width=arr_width[i], minwidth=arr_width[i])  # Set column width

        # Add data to the table with formatting
        for row in table_data:
            formatted_row = [f"{value:.6g}" if isinstance(value, float) else value for value in row]
            self.tree.insert("", "end", values=formatted_row)

    def plot_graph(self, U0, A, B, table_data):
        # Clear the plot
        self.ax.clear()
    
        # Generate data for the first plot
        if self.task_selector.get() == "Тестовая":
            x = np.linspace(A, B, 100)
            U = U0 * np.exp(-x / 2)

            # Plot U(x)
            self.ax.plot(x, U, label=f'U(x) = {U0} * exp(-x/2)', color='blue')

        # Extract X and V values from table data for the second plot
        X_values = [row[1] for row in table_data]  # Assuming the second column is X
        V_values = [row[2] for row in table_data]  # Assuming the third column is V

        # Plot V(x)
        self.ax.plot(X_values, V_values, label='V(x)', color='red')

        # Customize the plot
        self.ax.set_xlabel('x')
        self.ax.legend()
        self.ax.grid()

        # Update the plot
        self.canvas.draw()

    def show_final_reference(self, final_ref):
        # Clear the previous report text
        self.final_report_text.delete(1.0, tk.END)

        # Insert new information into the Text widget
        info = (
            f"Количество итераций: {final_ref.ITERATIONS_COUNT}\n"
            f"Расстояние до последней точки: {final_ref.DISTANCE_B_LAST_POINT}\n"
            f"Максимальная ошибка: {final_ref.MAX_ERROR}\n"
            f"Количество удвоений шага: {final_ref.STEP_DOUBLING_COUNT}\n"
            f"Количество уменьшений шага: {final_ref.STEP_REDUCTION_COUNT}\n"
            f"Макс. шаг с X: {final_ref.MAX_STEP_WITH_X}\n"
            f"Мин. шаг с X: {final_ref.MIN_STEP_WITH_X}\n"
            f"Макс. расстояние U-V: {final_ref.MAX_DISTANCE_U_V}"
        )
        self.final_report_text.insert(tk.END, info)

def create_gui():
    root = tk.Tk()
    app = Window(root)
    root.mainloop()

if __name__ == "__main__":
    create_gui()
