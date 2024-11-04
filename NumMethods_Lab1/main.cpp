#include "TestTask.h"
#include "FirstTask.h"
#include "SecondTask.h"
#include <iostream>
#include <locale>


int main()
{
	setlocale(LC_ALL, "Russian");
	double A{};
	double B{};
	double STEP{};
	double E_BORDER{};
	double E_ERROR{};
	int MAX_STEPS{};
	double START_POINT_FOR_U{};

	std::cout << "0 - тестовая задача   1 - первая задача  2 - вторая задача " << std::endl;
	int task;
	std::cin >> task;
	
	int option;
	
	std::cout << "1 - без ОЛП     2 - с ОЛП" << std::endl << std::endl;
	std::cout << "Выбирите вариант решения: ";
	std::cin >> option;
	std::cout << std::endl;
	


	std::cout << "Введите левую границу интегрирования А: " << std::endl;
	std::cin >> A;

	std::cout << "Введите правую границу интегрирования B: " << std::endl;
	std::cin >> B;

	std::cout << "Введите шаг интегрирования STEP: " << std::endl;
	std::cin >> STEP;

	std::cout << "Введите E для оценки ЛП E_ERROR: " << std::endl;
	std::cin >> E_ERROR;

	std::cout << "Введите E-граничный E_BORDER: " << std::endl;
	std::cin >> E_BORDER;

	std::cout << "Введите максимальное число шагов MAX_STEPS: " << std::endl;
	std::cin >> MAX_STEPS;

	std::cout << "Введите начальное условие U(A): " << std::endl;
	std::cin >> START_POINT_FOR_U;

	if (task == 0) {
		TestTask Solution(A, B, STEP, E_ERROR, E_BORDER, MAX_STEPS, START_POINT_FOR_U);
		if (option == 1) {

			Solution.Solve_Without_Error_Control();
		}
		if (option == 2) {
			Solution.Solve_With_Error_Control();
		}

		Solution.Write_To_File();
		Solution.PrintTable();
		Solution.PrintReference();
	}

	if (task == 1) {
		FirstTask Solution1(A, B, STEP, E_ERROR, E_BORDER, MAX_STEPS, START_POINT_FOR_U);
		if (option == 1) {

			Solution1.Solve_Without_Error_Control();
		}
		if (option == 2) {
			Solution1.Solve_With_Error_Control();
		}

		Solution1.Write_To_File();
		Solution1.PrintTable();
		Solution1.PrintReference();
	}

	if (task == 2) {
		SecondTask Solution2(A, B, STEP, E_ERROR, E_BORDER, MAX_STEPS, START_POINT_FOR_U);
		if (option == 1) {

			Solution2.Solve_Without_Error_Control();
		}
		if (option == 2) {
			Solution2.Solve_With_Error_Control();
		}

		Solution2.Write_To_File();
		Solution2.PrintTable();
		Solution2.PrintReference();
	}

}