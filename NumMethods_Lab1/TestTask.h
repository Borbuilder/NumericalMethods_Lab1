#pragma once
#include<list>
#include<vector>


class TestTask											// Класс реализует тестовую задчу
{
private:
	void find_Max_Distance_Ui_Vi();						// Метод для нахождения максимальной разницы между Ui и Vi для тестовой задачи
    std::vector<double> DISTANCE_Ui_Vi;					// Вектор, который хранит разницу между Ui и Vi на кождом шаге
protected:
	struct MethodParametrs				// Структура для параметров метода
	{
		double A{};						// Параметр из отрезка [a,b], задающий левую границу по x. Вводится пользователем
		double B{};						// Параметр из отрезка [a,b], задающий правую границу по x. Вводится пользователем
		double STEP{};					// Шаг метода
		double E_ERROR{};
		double E_BORDER{};				// Эпсилон граничный для котроля выхода за границу отрезка [a,b]
		int MAX_STEPS{};				// Максимальное число шагов метода
		double START_POINT_FOR_U{};		// Начальное условие для U(x)
	};

	MethodParametrs parametrs;							// Объект структуры MethodParametrs, в котором храняться парметры метода, описанные в структуре выше
	std::list<std::pair<double, double>> STEPS_and_Xs;	// Лист, который хранит шаги и соответсвующие им X
	std::list<double> ERRORS_LIST;						// Лист, который хранит ОЛП на каждом шаге
	 

	double f(const double& _X, const double& _V);														// Реализация нашей функции, в данном случае из тестовой задачи 
	double find_K1(const double& X, const double& V);													// Функция, которая считает K1 из метода
	double find_K2(const double& X, const double& V, const double& STEP, const double& K1);				// ...K2
	double find_K3(const double& X, const double& V, const double& STEP, const double& K2);				// ...K3
	double find_K4(const double& X, const double& V, const double& STEP, const double& K3);				// ...K4
	void make_Step(double& X, double& V, const double& STEP);                                           // Функция, выполняющая один шаг метиода, внутри меняются X и V
	void control_Error(double& X, double& V, double& X_EXTRA, double& V_EXTRA, double& OLD_X,
					   double& OLD_V, double& S, double& CURRENT_DOUBLING, double& CURRENT_REDUCTION);  // Функция, в которой реализован контроль ЛП, ничего внутри себя не меняет

	void find_Max_Step();		// Функция, которая ищет максимальный шаг, внутри параметру MAX_STEP_WITH_X структуры reference присваевается пара (STEP,X)
	void find_Min_Step();       // ... минимальный шаг
	void find_Max_Error();		// Функция, которая ищет максимальную ЛП, внутри параметру MAX_ERROR присваевается максимальный ERROR из структуры parametrs

public:
	struct FinalReference							 // Структура для конечной справки
	{
		int ITERATIONS_COUNT{};						 // Счётчик числа итераций 
		double DISTANCE_B_LAST_POINT{};				 // Расстояние между правой границей интегрирования B и последней точкой интегрирования
		double MAX_ERROR{};							 // Максимальное значение оценки ОЛП 
		int STEP_DOUBLING_COUNT{};					 // Счётчик числа удвоений шага
		int STEP_REDUCTION_COUNT{};					 // Счётчик числа уменьшений шага
		std::pair<double, double> MAX_STEP_WITH_X{}; // Максимальный шаг и Х, при котором этот шаг произошёл
		std::pair<double, double> MIN_STEP_WITH_X{}; // Минимальный шаг и Х, при котором этот шаг произошёл
		double MAX_DISTANCE_U_V{};                   // Максимальная разница между U_i и V_i (только для тестовой задачи)
	};
	FinalReference reference;							// Объект структуры Finalreference, в котором храняться парметры метода, описанные в структуре выше

	std::list<std::vector<double>> TABLE_INFORMATION;   // Список, который хранит строки итоговой таблицы
	
	TestTask(const double& _A, const double& _B, const double& _STEP, const double& _E_ERROR, 
			 const double& _E_BORDER, const int& _MAX_STEPS, const double& _START_POINT);    // Конструктор класса
	~TestTask();																			 // Деструктор

	void Solve_Without_Error_Control(); //Метод решения без ОЛП
	void Solve_With_Error_Control();	//Метод решения c ОЛП

	// Функции для проверки в main , в исходном варианте, они будут не нужны
	void PrintTable();     //Вывод итоговой таблицы на консоль, надо будет менять размер консоли, чтобы всё поместилось
	void Write_To_File();  //Запись X,Vi,Ui в файл
	void PrintReference(); //Вывод итоговой справки
};