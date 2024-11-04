#include "SecondTask.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>

std::vector<double> SecondTask::f(const double& X, const std::vector<double>& V)
{
    double f_1 = V[1];
    double f_2 = -parametrs.A * sin(V[0]);

    std::vector<double> function(2);
    function[0] = f_1;
    function[1] = f_2;

    return function;
}

std::vector<double> SecondTask::find_K1(const double& X, const std::vector<double>& V)
{
    return f(X,V);
}

std::vector<double> SecondTask::find_K2(const double& X, const std::vector<double>& V, const double& STEP, const std::vector<double>& K1)
{
    std::vector<double> new_V{ V[0] + (STEP / 2) * K1[0], V[1] + (STEP / 2) * K1[1] };
    return f(X + STEP / 2, new_V);
}

std::vector<double> SecondTask::find_K3(const double& X, const std::vector<double>& V, const double& STEP, const std::vector<double>& K2)
{
    std::vector<double> new_V{ V[0] + (STEP / 2) * K2[0], V[1] + (STEP / 2) * K2[1] };
    return f(X + STEP / 2, new_V);
}

std::vector<double> SecondTask::find_K4(const double& X, const std::vector<double>& V, const double& STEP, const std::vector<double>& K3)
{
    std::vector<double> new_V{ V[0] + STEP * K3[0], V[1] + STEP * K3[1] };
    return f(X + STEP , new_V);
}

void SecondTask::make_Step(double& X, std::vector<double>& V, const double& STEP)
{
    std::vector<double> K1 = find_K1(X, V);
    std::vector<double> K2 = find_K2(X, V, STEP, K1);
    std::vector<double> K3 = find_K3(X, V, STEP, K2);
    std::vector<double> K4 = find_K4(X, V, STEP, K3);

    X += STEP;
    V[0] += (STEP / 6) * (K1[0] + 2 * K2[0] + 2 * K3[0] + K4[0]);
    V[1] += (STEP / 6) * (K1[1] + 2 * K2[1] + 2 * K3[1] + K4[1]);
}

void SecondTask::control_Error(double& X, std::vector<double>& V, double& X_EXTRA, std::vector<double>& V_EXTRA, double& OLD_X, std::vector<double>& OLD_V, double& S, double& CURRENT_DOUBLING, double& CURRENT_REDUCTION)
{
	if (abs(S) < parametrs.E_ERROR / pow(2, 5)) { // ������� ��� ��������
		parametrs.STEP *= 2;
		++reference.STEP_DOUBLING_COUNT;
		++CURRENT_DOUBLING;
	}
	else if (abs(S) > parametrs.E_ERROR) {        //���� ������� ��� ������� ���� �����������, �� ������������ �����,
		bool FLAG_TO_EXIT = false;
		double h = parametrs.STEP;  				  // ��������� ������ � ���������� �����, ���� ��� ���� ����������� ����� 
		while (!FLAG_TO_EXIT) {
			// ��������� �������, �� ����� ������������ � ����� ���, � ��� ���� ����������� �� ����� ����������
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

			double s_1 = (V_EXTRA[0] - V[0]) / (pow(2, 4) - 1);
			double s_2 = (V_EXTRA[1] - V[1]) / (pow(2, 4) - 1);
			S = sqrt(s_1 * s_1 + s_2 * s_2);
			if (abs(S) <= parametrs.E_ERROR) {
				FLAG_TO_EXIT = true;
				parametrs.STEP = h;
			}
		}
	}
}

void SecondTask::Solve_Without_Error_Control()
{
	double X = parametrs.A;
	std::vector<double> V = {parametrs.START_POINT_FOR_U, 0.0}; //����� V[0] ��� U - ������� �������, V[1] ��� U' - � ����������� 

	std::vector<double> TABLE_ROW1 = { 1.0, X, V[0],V[1], parametrs.STEP}; // ����� � ����� - ������ �������� ������� � ���� (5 ���������)
	TABLE_INFORMATION.emplace_back(TABLE_ROW1);											       // { i; X_i; V0_i; V1_i; STEP_i }

	double OLD_X = X; //� ��������� OLD ������ �������� � ���������� ����
	std::vector<double> OLD_V = V;

	bool FLAG_TO_EXIT = false;
	for (int i = 2; i <= parametrs.MAX_STEPS; ++i)
	{
		make_Step(X, V, parametrs.STEP);
		
		if (X >= parametrs.B) {
			if (X > parametrs.B) {						// ���� X ����� �� ������ �������, ������������ �� ��� ����� � ������ ���,                                        
				X = OLD_X;								//  ������ ������� ������ ������� � ��������� ������ X � ����������� ��������������
				V = OLD_V;
				parametrs.STEP = parametrs.B - OLD_X;
				make_Step(X, V, parametrs.STEP);
			}
			FLAG_TO_EXIT = true;                        //���� X ������ � ������ ��������, ����������� �������������� 
			++reference.ITERATIONS_COUNT;
		}

		std::vector<double> TABLE_ROW = { static_cast<double>(i), X, V[0],V[1], parametrs.STEP};
		TABLE_INFORMATION.emplace_back(TABLE_ROW);

		if (FLAG_TO_EXIT)
			break;
		else {
			OLD_X = X;
			OLD_V = V;
		}

		++reference.ITERATIONS_COUNT;
	}
}

void SecondTask::Solve_With_Error_Control()
{
	double X = parametrs.A;
	std::vector<double> V = { parametrs.START_POINT_FOR_U,0.0 };
	STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

	std::vector<double> TABLE_ROW1 = { 1.0, X, V[0], V[1], V[0], V[1], 0.0, 0.0, 0.0, parametrs.STEP, 0.0, 0.0}; // ����� � ����� - ������ �������� ������� � ���� (12 ���������)
	TABLE_INFORMATION.emplace_back(TABLE_ROW1);									// { i; X_i; V[0]_i; V[1]_i; V[0]_i^; V[1]_i^; V[0]_i-V[0]_i^; V[1]_i-V[1]_i^; ���(S); STEP_i; ���-�� �������; ���-�� ��������  }

	double OLD_X = X;
	std::vector<double> OLD_V = V;

	for (int i = 2; i <= parametrs.MAX_STEPS; ++i)
	{
		double CURRENT_DOUBLING{};  // ���-�� �������� �� ������� ����
		double CURRENT_REDUCTION{}; // ���-�� ������� �� ������� ����

		std::vector<double> V_EXTRA = V;         // V^ ��� ����������� ����
		double X_EXTRA = X;			// X^ ��� ����������� ����

		for (int j = 0; j < 2; ++j) {
			make_Step(X_EXTRA, V_EXTRA, parametrs.STEP / 2);
			++reference.ITERATIONS_COUNT;
		}
		make_Step(X, V, parametrs.STEP);

		double s_1 = (V_EXTRA[0] - V[0]) / (pow(2, 4) - 1);
		double s_2 = (V_EXTRA[1] - V[1]) / (pow(2, 4) - 1);
		double S = sqrt(s_1 * s_1 + s_2 * s_2);
		ERRORS_LIST.emplace_back(S * pow(2, 4));
		control_Error(X, V, X_EXTRA, V_EXTRA, OLD_X, OLD_V, S, CURRENT_DOUBLING, CURRENT_REDUCTION); // ��������������� ��� �������� ��, ��������� ��. � ���������� �������
		STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

		std::vector<double> TABLE_ROW = { static_cast<double>(i), X, V[0],V[1], V_EXTRA[0],V_EXTRA[1], V[0] - V_EXTRA[0],V[1] - V_EXTRA[1], S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING};
		TABLE_INFORMATION.emplace_back(TABLE_ROW);

		bool EXIT_FROM_FOR = false;
		if (x_in_border(parametrs.B, X, parametrs.E_BORDER)) {                       //�������� �� ��������� � ����������� ������ ������� �� X. 
			EXIT_FROM_FOR = true;												   //���� X ����� � �����������, ��������� ��������������, ������ �� for �� �����
		}
		else if (X > parametrs.B + parametrs.E_BORDER) {						//���� ��������� ������ �����������, �� ������������ �� ��� �����, ����� ��� � ��������
			while (!x_in_border(parametrs.B, X, parametrs.E_BORDER)) {			//��� �������������� �����. ���� ����� ���� ��������� ����� ������, �� ��������� �������,��� 
				X = OLD_X;														//���� �� ������ � ������� ��� �� �������� ����� �. ���� ��������� ����� ������� 
				V = OLD_V;														//����� ������� ����, �� ���������� ������� ��� (1*) ������������� � ��������� �����������, �� ����
				parametrs.STEP /= 2;											//����������� �� ������������, ��� ��� ��� ������������� �������� ��� ������� �������, � ��� 
				++CURRENT_REDUCTION;											//���������, ���� �� ������ � �����������
				++reference.STEP_REDUCTION_COUNT;

				make_Step(X, V, parametrs.STEP);
				if (X < parametrs.B - parametrs.E_BORDER) { //(1*)
					X_EXTRA = X;
					V_EXTRA = V;

					for (int j = 0; j < 2; ++j) {
						make_Step(X_EXTRA, V_EXTRA, parametrs.STEP / 2);
						++reference.ITERATIONS_COUNT;
					}

					s_1 = (V_EXTRA[0] - V[0]) / (pow(2, 4) - 1);
					s_2 = (V_EXTRA[1] - V[1]) / (pow(2, 4) - 1);
					double S = sqrt(s_1 * s_1 + s_2 * s_2);
					ERRORS_LIST.emplace_back(S * pow(2, 4));
					STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

					std::vector<double> TABLE_ROW = { static_cast<double>(++i), X, V[0],V[1], V_EXTRA[0],V_EXTRA[1], V[0] - V_EXTRA[0],V[1] - V_EXTRA[1],
													  S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING };
					TABLE_INFORMATION.emplace_back(TABLE_ROW);
					CURRENT_DOUBLING = 0.0;
					CURRENT_REDUCTION = 0.0;

					OLD_X = X;
					OLD_V = V;
				}
				++reference.ITERATIONS_COUNT;
			}
			EXIT_FROM_FOR = true;
		}

		if (EXIT_FROM_FOR) {
			TABLE_ROW = { static_cast<double>(++i), X, V[0],V[1], V_EXTRA[0],V_EXTRA[1], V[0] - V_EXTRA[0],V[1] - V_EXTRA[1],
											  S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING };
			TABLE_INFORMATION.emplace_back(TABLE_ROW);
			break;
		}
		else {
			OLD_X = X;
			OLD_V = V;
		}

		++reference.ITERATIONS_COUNT;
	}
	reference.DISTANCE_B_LAST_POINT = parametrs.B - X;
	find_Max_Step();
	find_Min_Step();
	find_Max_Error();
}



void SecondTask::PrintTable()
{
	if (TABLE_INFORMATION.front().size() == 5) {
		std::cout << std::setw(30) << "i" << std::setw(30) << "X" << std::setw(30) << "V[0]" << std::setw(30) << "V[1]"<< std::setw(30) << "Step" << std::endl;
		for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
			for (size_t i = 0; i < 5; ++i) {
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
		std::cout << std::setw(30) << "i" << std::setw(30) << "X" << std::setw(30) << "V[0]" << std::setw(30) << "V[1]" << std::setw(30) << "V[0]^" << std::setw(30) << "V[1]^"
			      << std::setw(30) << "V[0]-V[0]^" << std::setw(30) << "V[1]-V[1]^" << std::setw(30) << "S"
			      << std::setw(30) << "Step" << std::setw(30) << "����������" << std::setw(30) << "��������" << std::setw(30)<< std::endl;
		for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
			for (size_t i = 0; i < 12; ++i) {
				if (i == 1) {
					std::cout << std::setw(30) << (*it_list)[i];
				}
				else if (i == 10 || i == 11) {
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

void SecondTask::Write_To_File()
{
	// { i; X_i; V[0]_i; V[1]_i; V[0]_i^; V[1]_i^; V[0]_i-V[0]_i^; V[1]_i-V[1]_i^; ���(S); STEP_i; ���-�� �������; ���-�� ��������  }
	std::ofstream outFile("points.txt");
	if (!outFile) {
		std::cerr << "������ �������� �����!" << std::endl;
	}
	else {
		
		for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
			outFile << std::fixed << std::setprecision(20) << (*it_list)[1] << "\t" << std::setw(30)
					              << std::setprecision(20) << (*it_list)[2] << "\t" << std::setw(30) 
					              << std::setprecision(20) << (*it_list)[3] << std::endl;  // ������ � ������� "x v[0] v[1]"
		}
		outFile.close();
	}
}

void SecondTask::PrintReference()
{
	std::cout << std::endl;
	std::cout << "DISTANCE_B_LAST_POINT :" << reference.DISTANCE_B_LAST_POINT << std::endl;
	std::cout << "ITERATIONS_COUNT :" << reference.ITERATIONS_COUNT << std::endl;
	std::cout << "MAX_ERROR :" << reference.MAX_ERROR << std::endl;
	std::cout << "MAX_STEP_WITH_X :" << "STEP = " << reference.MAX_STEP_WITH_X.first << "  X = " << reference.MAX_STEP_WITH_X.second << std::endl;
	std::cout << "MIN_STEP_WITH_X :" << "STEP = " << reference.MIN_STEP_WITH_X.first << "  X = " << reference.MIN_STEP_WITH_X.second << std::endl;
	std::cout << "STEP_DOUBLING_COUNT :" << reference.STEP_DOUBLING_COUNT << std::endl;
	std::cout << "STEP_REDUCTION_COUNT :" << reference.STEP_REDUCTION_COUNT << std::endl;
}



