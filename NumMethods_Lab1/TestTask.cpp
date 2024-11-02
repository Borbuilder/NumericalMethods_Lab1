#include "TestTask.h"
#include "TrueSolution.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <queue>

#include <iomanip>
#include <iostream>


//Вспомогательные функции
bool TestTask::x_in_border(const double& B, const double& X, const double& BORDER) { // Функция проверяет , находится ли текущий X в окрестности правой границы
	return X >= B - BORDER && X <= B + BORDER;
}



//Основыне функции
TestTask::TestTask(const double& _A, const double& _B, const double& _STEP, const double& _E_ERROR, const double& _E_BORDER, const int& _MAX_STEPS, const double& _START_POINT) {
	parametrs.A = _A;
	parametrs.B = _B;
	parametrs.STEP = _STEP;
	parametrs.E_ERROR = _E_ERROR;
	parametrs.E_BORDER = _E_BORDER;
	parametrs.MAX_STEPS = _MAX_STEPS;
	parametrs.START_POINT_FOR_U = _START_POINT;
}

TestTask::~TestTask(){
	parametrs.A = 0.0;
	parametrs.B = 0.0;
	parametrs.STEP = 0.0;
	parametrs.E_ERROR = 0.0;
	parametrs.E_BORDER = 0.0;
	parametrs.MAX_STEPS = 0;
	parametrs.START_POINT_FOR_U = 0.0;

	TABLE_INFORMATION.clear();
	DISTANCE_Ui_Vi.clear();
	STEPS_and_Xs.clear();
	ERRORS_LIST.clear();
}


double TestTask::f(const double& X, const double& V)
{
	return (-1)*(V/2);
}

double TestTask::find_K1(const double& X, const double& V)
{
	return f(X,V);
}

double TestTask::find_K2(const double& X, const double& V, const double& STEP, const double& K1)
{
	return f(X+STEP/2,V+(STEP/2)*K1);
}

double TestTask::find_K3(const double& X, const double& V, const double& STEP, const double& K2)
{
	return f(X + STEP / 2, V + (STEP / 2) * K2);
}

double TestTask::find_K4(const double& X, const double& V, const double& STEP, const double& K3)
{
	return f(X + STEP, V + STEP * K3);
}

void TestTask::make_Step(double& X, double& V, const double& STEP)
{
	double K1 = find_K1(X, V);
	double K2 = find_K2(X, V, STEP, K1);
	double K3 = find_K3(X, V, STEP, K2);
	double K4 = find_K4(X, V, STEP, K3);
	X += STEP;
	V += (STEP / 6) * (K1 + 2 * K2 + 2 * K3 + K4);
}

void TestTask::control_Error(double& X,double& V, double& X_EXTRA, double& V_EXTRA, double& OLD_X, double& OLD_V, double& S, double& CURRENT_DOUBLING, double& CURRENT_REDUCTION)
{
	if (abs(S) < parametrs.E_ERROR / pow(2, 5)) { // Условие для удвоения
		parametrs.STEP *= 2;
		++reference.STEP_DOUBLING_COUNT;
		++CURRENT_DOUBLING;
	}
	else if (abs(S) > parametrs.E_ERROR) {        //Если условие для деления шага выполнилось, то возвращаемся назад,
		bool FLAG_TO_EXIT = false;
		double h = parametrs.STEP;  				  // выполняем расчёт с половинным шагом, если для него погрешность снова 
		while (!FLAG_TO_EXIT) {	
			                                 // оказалась большой, то снова возвращаемся и делим шаг, и так пока погрешность не будет допустимой
			h /= 2;
			++CURRENT_REDUCTION;
			++reference.STEP_REDUCTION_COUNT;

			V = OLD_V;
			X = OLD_X;
			V_EXTRA = V;
			X_EXTRA = X;

			for (int i = 0; i < 2; ++i) {
				make_Step(X_EXTRA, V_EXTRA, h / 2);
				++reference.ITERATIONS_COUNT;
			}
			make_Step(X, V, h);

			S = (V_EXTRA - V) / (pow(2, 4) - 1);
			if (abs(S) <= parametrs.E_ERROR) {
				FLAG_TO_EXIT = true;
				//V = V_EXTRA;
				parametrs.STEP = h;
			}
		}
	}
}

void TestTask::find_Max_Step()
{
	reference.MAX_STEP_WITH_X = *std::max_element(STEPS_and_Xs.begin(), STEPS_and_Xs.end(),
		[](const std::pair<double, double>& a, const std::pair<double, double>& b) {
			return a.first < b.first;
		});
}

void TestTask::find_Min_Step()
{
	reference.MIN_STEP_WITH_X = *std::min_element(STEPS_and_Xs.begin(), STEPS_and_Xs.end(),
		[](const std::pair<double, double>& a, const std::pair<double, double>& b) {
			return a.first < b.first;
		});
}

void TestTask::find_Max_Error()
{
	reference.MAX_ERROR = *std::max_element(ERRORS_LIST.begin(), ERRORS_LIST.end());
}

void TestTask::Solve_Without_Error_Control()
{
	double X = parametrs.A;
	double U = find_True_Solution(X,parametrs.START_POINT_FOR_U); //Находим истинное решение на текущем шаге
	double V = U;
	
	std::vector<double> TABLE_ROW1 = {1.0, X, V, parametrs.STEP, U, abs(U-V)}; // Здесь и далее - строка итоговой таблицы в виде { i X_i V_i STEP_i U_i |U_i - V_i| }
	TABLE_INFORMATION.emplace_back(TABLE_ROW1);

	double OLD_X = X; //В переменых OLD храним значения с последного шага
	double OLD_V = V;
	double OLD_U = U;

	bool FLAG_TO_EXIT = false;
	for (int i = 2; i <= parametrs.MAX_STEPS; ++i)
	{
		make_Step(X, V, parametrs.STEP);
		U = find_True_Solution(X, parametrs.START_POINT_FOR_U);

		if (X >= parametrs.B) {           
			if (X > parametrs.B) {						// Если X вышел за правую границу, возвращаемся на шаг назад и делаем шаг,                                        
			    X = OLD_X;								//  равный разнице правой границы и последней точкой X и заканчиваем интегрирование
				V = OLD_V;
				U = OLD_U;
				parametrs.STEP = parametrs.B - OLD_X;
				make_Step(X, V, parametrs.STEP);
			}
			FLAG_TO_EXIT = true;                        //Если X совпал с правой границей, заканчиваем интегрирование 
			++reference.ITERATIONS_COUNT;
		}

		DISTANCE_Ui_Vi.emplace_back(abs(U - V));
		std::vector<double> TABLE_ROW = { static_cast<double>(i), X, V, parametrs.STEP, U, abs(U - V)};
		TABLE_INFORMATION.emplace_back(TABLE_ROW);

		if (FLAG_TO_EXIT)
			break;
		else {
			OLD_X = X;
			OLD_V = V;
			OLD_U = U;
		}

		++reference.ITERATIONS_COUNT;
	}
	find_Max_Distance_Ui_Vi();
}

void TestTask::Solve_With_Error_Control()
{
	double X = parametrs.A;
	double U = find_True_Solution(X, parametrs.START_POINT_FOR_U);
	double V = U;
	STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

	std::vector<double> TABLE_ROW1 = { 1.0, X, V, V, 0.0, 0.0, parametrs.STEP, 0.0, 0.0, U, abs(U - V) }; // Здесь и далее - строка итоговой таблицы в виде 
	TABLE_INFORMATION.emplace_back(TABLE_ROW1);											// { i X_i V_i V_i^ V_i-V_i^ ОЛП(S) STEP_i Кол-во делений Кол-во удвоений U_i |U_i - V_i| }

	double OLD_X = X;
	double OLD_V = V;
	double OLD_U = U;

	for (int i = 2; i <= parametrs.MAX_STEPS; ++i)
	{   
		double CURRENT_DOUBLING{};  // Кол-во удвоений на текущем шаге
		double CURRENT_REDUCTION{}; // Кол-во делений на текущем шаге
		
		double V_EXTRA = V;         // V^ для половинного шага
		double X_EXTRA = X;			// X^ для половинного шага

		for (int i = 0; i < 2; ++i) {
			make_Step(X_EXTRA, V_EXTRA, parametrs.STEP / 2);
		}
		make_Step(X, V, parametrs.STEP);
		double S = (V_EXTRA - V) / (pow(2, 4) - 1);
		ERRORS_LIST.emplace_back(S*pow(2,4));
		control_Error(X, V, X_EXTRA, V_EXTRA, OLD_X, OLD_V, S, CURRENT_DOUBLING, CURRENT_REDUCTION); // Непосредственно сам контроль ЛП, подробнее см. в реализации функции
		STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));
		U = find_True_Solution(X, parametrs.START_POINT_FOR_U);

		bool EXIT_FROM_FOR = false;
		if (x_in_border(parametrs.B,X,parametrs.E_BORDER)) {                       //Проверка на попадание в окрестность правой границы по X. 
			EXIT_FROM_FOR = true;												   //Если X попал в окрестность, завершаем интегрирование, выходя из for по флагу
		}
		else if (X > parametrs.B + parametrs.E_BORDER) {						//Если оказались правее окрестности, то возвращаемся на шаг назад, делим шаг и выплняем
			while (!x_in_border(parametrs.B, X, parametrs.E_BORDER)) {			//шаг интегрирования снова. Если после него оказались снова правее, то повторяем деление,так 
				X = OLD_X;														//пока не попадём в границу или не окажемся левее её. Если оказались левее границы 
				V = OLD_V;														//после деления шага, то производим обычный шаг (1*) интегрированя с подсчётом погрешности, но саму
				parametrs.STEP /= 2;											//погрешность не контролируем, так как она автоматически попадает под условие деления, и так 
				++CURRENT_REDUCTION;											//повторяем, пока не попадём в окрестность
				++reference.STEP_REDUCTION_COUNT;

				make_Step(X, V, parametrs.STEP);
				if (X < parametrs.B - parametrs.E_BORDER) { //(1*)
					X_EXTRA = X;
					V_EXTRA = V;
					for (int i = 0; i < 2; ++i) {
						make_Step(X_EXTRA, V_EXTRA, parametrs.STEP / 2);
						++reference.ITERATIONS_COUNT;
					}
					S = (V_EXTRA - V) / (pow(2, 4) - 1);
					ERRORS_LIST.emplace_back(S*pow(2,4));
					STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));
					U = find_True_Solution(X, parametrs.START_POINT_FOR_U);

					DISTANCE_Ui_Vi.emplace_back(abs(U - V));
					std::vector<double> TABLE_ROW = { static_cast<double>(++i), X, V, V_EXTRA, V - V_EXTRA, S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING, U, abs(U - V) };
					TABLE_INFORMATION.emplace_back(TABLE_ROW);

					OLD_X = X;
					OLD_V = V;
				}
				++reference.ITERATIONS_COUNT;
			}
				EXIT_FROM_FOR = true;
		}

		if (EXIT_FROM_FOR) {
			break;
		}
		else {
			DISTANCE_Ui_Vi.emplace_back(abs(U - V));
			std::vector<double> TABLE_ROW = { static_cast<double>(i), X, V, V_EXTRA, V - V_EXTRA, S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING, U, abs(U - V) };
			TABLE_INFORMATION.emplace_back(TABLE_ROW);

			OLD_X = X;
			OLD_V = V;
			OLD_U = U;
		}

		++reference.ITERATIONS_COUNT;
	}
	reference.DISTANCE_B_LAST_POINT = parametrs.B - X;
	find_Max_Distance_Ui_Vi();
	find_Max_Step();
	find_Min_Step();
}

void TestTask::find_Max_Distance_Ui_Vi()
{ 
	reference.MAX_DISTANCE_U_V =  *std::max_element(DISTANCE_Ui_Vi.begin(), DISTANCE_Ui_Vi.end());
}





//функции для проверки
void TestTask::PrintTable(){
	if (TABLE_INFORMATION.front().size() == 6) {
		std::cout << std::setw(30) << "i" << std::setw(30) << "X" << std::setw(30) << "V" << std::setw(30) << "Step" << std::setw(30) << "U" << std::setw(30) << "|U-V|" << std::endl;
		for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
			for (size_t i = 0; i < 6; ++i) {
				if (i == 1) {
					std::cout << std::setw(30) << (*it_list)[i];
				}
				else {
					std::cout << std::setprecision(16) << std::setw(30) << (*it_list)[i];
				}
			}
			std::cout << std::endl;
		}
	}
	else {
		std::cout << std::setw(30) << "i" << std::setw(30) << "X" << std::setw(30) << "V" << std::setw(30) << "V^" << std::setw(30) << "V-V^" << std::setw(30) << "S" <<
			std::setw(30) << "Step" << std::setw(30) << "Уменьшения" << std::setw(30) << "Удвоения" << std::setw(30) << "U_i" <<std::setw(30) << "|U_i-V_i|" << std::endl;
		for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
			for (size_t i = 0; i < 11; ++i) {
				if (i == 1) {
					std::cout << std::setw(30) << (*it_list)[i];
				}
				else if (i == 7 || i == 8) {
					std::cout << std::setw(30) << static_cast<int>((*it_list)[i]);
				}
				else {
					std::cout << std::setprecision(16) << std::setw(30) << (*it_list)[i];
				}
			}
			std::cout << std::endl;
		}
	}
}

void TestTask::Write_To_File()
{
	std::ofstream outFile("points.txt");
	if (!outFile) {
		std::cerr << "Ошибка открытия файла!" << std::endl;
	}
	else {
		if (TABLE_INFORMATION.front().size() == 6) { //Если считали без контроля ЛП
			for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
				outFile << std::fixed << std::setprecision(20) << (*it_list)[1] << "\t" << std::setw(30) 
						<< std::setprecision(20) << (*it_list)[2] << "\t" << std::setw(30) << std::setprecision(20) 
						<< (*it_list)[4] << std::endl;  // Запись в формате "x v u"
			}
			outFile.close();
		}
		else { //Если считали с контролем ЛП
			for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
				outFile << std::fixed << std::setprecision(20) << (*it_list)[1] << "\t" << std::setw(30) 
						<< std::setprecision(20) << (*it_list)[2] << "\t" << std::setw(30) << std::setprecision(20) 
						<< (*it_list)[9] << std::endl;  // Запись в формате "x v u"
			}
			outFile.close();
		}
	}
}

void TestTask::PrintReference()
{
	std::cout<< std::endl;
	std::cout << "DISTANCE_B_LAST_POINT :" << reference.DISTANCE_B_LAST_POINT << std::endl;
	std::cout << "ITERATIONS_COUNT :" << reference.ITERATIONS_COUNT << std::endl;
	std::cout << "MAX_DISTANCE_U_V :" << reference.MAX_DISTANCE_U_V << std::endl;
	std::cout << "MAX_ERROR :" << reference.MAX_ERROR << std::endl;
	std::cout << "MAX_STEP_WITH_X :" << "STEP = " << reference.MAX_STEP_WITH_X.first <<"  X = " << reference.MAX_STEP_WITH_X.second << std::endl;
	std::cout << "MIN_STEP_WITH_X :" << "STEP = " << reference.MIN_STEP_WITH_X.first <<"  X = " << reference.MIN_STEP_WITH_X.second << std::endl;
	std::cout << "STEP_DOUBLING_COUNT :" << reference.STEP_DOUBLING_COUNT << std::endl;
	std::cout << "STEP_REDUCTION_COUNT :" << reference.STEP_REDUCTION_COUNT << std::endl;
}





