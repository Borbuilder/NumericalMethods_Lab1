import matplotlib.pyplot as plt

# Списки для хранения значений x и v
x_values = []
v_values = []

# Чтение данных из текстового файла
with open('Points.txt', 'r') as datafile:
    for line in datafile:
        # Разделяем строку по пробелам или другим разделителям
        values = line.split()  # Если разделитель другой, укажите его, например, line.split(';')
        if len(values) >= 2:  # Убедимся, что в строке есть хотя бы два значения
            x_values.append(float(values[0]))  # Первый столбец
            v_values.append(float(values[1]))  # Второй столбец

# Построение графика
plt.plot(x_values, v_values, marker='o', linestyle='-')
plt.title('График зависимости v(x)')
plt.xlabel('x')
plt.ylabel('v')
plt.grid()
plt.show()