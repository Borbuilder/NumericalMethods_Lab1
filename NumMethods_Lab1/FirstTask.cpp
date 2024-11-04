#include "FirstTask.h"
#include <iostream>
#include <iomanip>
#include <fstream>

double FirstTask::f(const double& X, const double& V)
{
    return  1/(1+pow(X,4)) * pow(V,2) + V - pow(V,3)*sin(10*X);
}

void FirstTask::Solve_With_Error_Control()
{
	double X = parametrs.A;
	double V = parametrs.START_POINT_FOR_U;
	STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

	std::vector<double> TABLE_ROW1 = { 1.0, X, V, V, 0.0, 0.0, parametrs.STEP, 0.0, 0.0 }; // ����� � ����� - ������ �������� ������� � ���� (9 ���������)
	TABLE_INFORMATION.emplace_back(TABLE_ROW1);											   // { i X_i V_i V_i^ V_i-V_i^ ���(S) STEP_i ���-�� ������� ���-�� ��������  }

	double OLD_X = X;
	double OLD_V = V;

	for (int i = 2; i <= parametrs.MAX_STEPS; ++i)
	{
		double CURRENT_DOUBLING{};  // ���-�� �������� �� ������� ����
		double CURRENT_REDUCTION{}; // ���-�� ������� �� ������� ����

		double V_EXTRA = V;         // V^ ��� ����������� ����
		double X_EXTRA = X;			// X^ ��� ����������� ����

		for (int j = 0; j < 2; ++j) {
			make_Step(X_EXTRA, V_EXTRA, parametrs.STEP / 2);
			++reference.ITERATIONS_COUNT;
		}
		make_Step(X, V, parametrs.STEP);

		double S = (V_EXTRA - V) / (pow(2, 4) - 1);
		ERRORS_LIST.emplace_back(S * pow(2, 4));
		control_Error(X, V, X_EXTRA, V_EXTRA, OLD_X, OLD_V, S, CURRENT_DOUBLING, CURRENT_REDUCTION); // ��������������� ��� �������� ��, ��������� ��. � ���������� �������
		STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

		std::vector<double> TABLE_ROW = { static_cast<double>(i), X, V, V_EXTRA, V - V_EXTRA, S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING };
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

					S = (V_EXTRA - V) / (pow(2, 4) - 1);
					ERRORS_LIST.emplace_back(S * pow(2, 4));
					STEPS_and_Xs.emplace_back(std::make_pair(parametrs.STEP, X));

					std::vector<double> TABLE_ROW = { static_cast<double>(++i), X, V, V_EXTRA, V - V_EXTRA, S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING};
					TABLE_INFORMATION.emplace_back(TABLE_ROW);
					CURRENT_REDUCTION = 0.0;
					CURRENT_DOUBLING = 0.0;

					OLD_X = X;
					OLD_V = V;
				}
				++reference.ITERATIONS_COUNT;
			}
			EXIT_FROM_FOR = true;
		}

		if (EXIT_FROM_FOR) {
			std::vector<double> TABLE_ROW = { static_cast<double>(++i), X, V, V_EXTRA, V - V_EXTRA, S, parametrs.STEP, CURRENT_REDUCTION, CURRENT_DOUBLING };
		    TABLE_INFORMATION.emplace_back(TABLE_ROW);
			break;
		}
		else {
			OLD_X = X;
			OLD_V = V;
		}

		++reference.ITERATIONS_COUNT;
	}
	reference.DISTANCE_B_LAST_POINT = parametrs.B - X; std::cout << "before max_s" << std::endl;
	find_Max_Step(); std::cout << "before mim_s" << std::endl;
	find_Min_Step(); std::cout << "before max_er" << std::endl;
	find_Max_Error(); std::cout << "end of solve" << std::endl;
}

void FirstTask::Solve_Without_Error_Control()
{
	double X = parametrs.A;
	double V = parametrs.START_POINT_FOR_U;

	std::vector<double> TABLE_ROW1 = { 1.0, X, V, parametrs.STEP}; // ����� � ����� - ������ �������� ������� � ���� { i X_i V_i STEP_i }
	TABLE_INFORMATION.emplace_back(TABLE_ROW1);

	double OLD_X = X; //� ��������� OLD ������ �������� � ���������� ����
	double OLD_V = V;

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

		std::vector<double> TABLE_ROW = { static_cast<double>(i), X, V, parametrs.STEP};
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



// ������� ��� �������� � main
void FirstTask::PrintTable()
{
	std::cout << std::setw(30) << "i" << std::setw(30) << "X" << std::setw(30) << "V" << std::setw(30) << "V^" << std::setw(30) << "V-V^" << std::setw(30) << "S" <<
		std::setw(30) << "Step" << std::setw(30) << "����������" << std::setw(30) << "��������" << std::setw(30) << std::endl;

	for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
		for (size_t i = 0; i < 9; ++i) {
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

void FirstTask::Write_To_File()
{
	std::ofstream outFile("points.txt");
	if (!outFile) {
		std::cerr << "������ �������� �����!" << std::endl;
	}

	for (auto it_list = TABLE_INFORMATION.begin(); it_list != TABLE_INFORMATION.end(); ++it_list) {
		outFile << std::fixed << std::setprecision(20) << (*it_list)[1] << "\t" << std::setw(30)
			                  << std::setprecision(20) << (*it_list)[2] << std::endl;  // ������ � ������� "x v "
	}
	outFile.close();
}

void FirstTask::PrintReference()
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
