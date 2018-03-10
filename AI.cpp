#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <set>
#include <climits>
#include <stdlib.h>
#include <ctime>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

vector <bool> move(vector<int>, vector<int>);
double count_recast(vector<int>, vector<int>, vector<bool>, int);
double eval_func(vector<int>, vector<int>, int);
int summa(vector<int>, int, int);
double line_util(vector<int>, int);
void table_print(vector <vector<int> >, vector <vector<int> >, int, int, vector<string>);
vector <vector<double> > util_table(4);
int tir_final;

// Correction coefficients to play 5s and 6s in upper part more intensively:
int k1 = 3; // 2~4
int k2 = 1000; // 500~2000
// Correction coefficient to play upper part more intensively:
int k3 = 0; // (-3~1)


int main()
{
	// Remember color preferences
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &csbi );

	vector <int> table(12, 0);
	vector <int> table_human(12, 0);
	vector <int> cast(5);
	vector <int> cast_human(5);
	vector <bool> recast;
	vector <bool> recast_human(5, 0);
	vector <vector<int> > final_table(12);
	vector <vector<int> > final_table_human(12);
	vector <string> table_headers(12);

	int points = 0;
	int points_human = 0;
	double res;
	int best_put; // computer's choice for line; 0~11
	int put; // human's choice for line; 1~12
	int i, j;
	bool input;
	int tir_human;
	int recast_human_int;
	srand (time(NULL));

	// filling the utility table and header texts
	for (int i = 0; i < 4; i++)
	{
		util_table[i].resize(48);
	}
	util_table[0][0] = 1.0/6;
	util_table[1][0] = 1.0/12;
	util_table[2][0] = 1.0/24;
	util_table[3][0] = 0.0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j < 45; j++)
		{
			util_table[i][j] = (1 - util_table[i][0]) / 44 * j + util_table[i][0];
		}
	}

	table_headers[0] = " <1> "; table_headers[ 1] = " <2> "; table_headers[ 2] = " <3> ";
	table_headers[3] = " <4> "; table_headers[ 4] = " <5> "; table_headers[ 5] = " <6> ";
	table_headers[6] = " pair"; table_headers[ 7] = " 2+2 "; table_headers[ 8] = " set ";
	table_headers[9] = " full"; table_headers[10] = " strt"; table_headers[11] = " four";


	for (int move1 = 0; move1 < 36; move1++)
	{
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0xF0 );  // set black text, white ground
		cout << "move #" << move1 + 1 << endl;
		for (int i = 0; i < 5; i++)
		{
			cast[i] = rand() % 6 + 1;
		}

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		cout << "Cast: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0C ); // set red text, black ground
		for (int i = 0; i < 5; i++)
		{
			cout << cast[i] << ' ';
		}
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		cout << endl << endl;

		//////////////////////////    Human's move   ///////////////////////////////////////////
		cast_human = cast;
		cout << "Human enters dice# to shoot again: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0B ); // Set light blue text, black ground
		input = 0;
		while(!input)
		{
			cin >> recast_human_int;
            for (int i = 0; i < 5; i++)
            {
                recast_human[i] = 0;
            }
			if(recast_human_int)
			{
				while(recast_human_int)
				{
					i = recast_human_int % 10;
					if(i >= 1 && i <= 5 && recast_human[i - 1] == 0)
					{
						recast_human[i - 1] = 1;
						recast_human_int /= 10;
						input = 1;
					}
					else
					{
						cout << "   Enter recast again: ";
						input = 0;
						break;
					}
				}
			}
			else
				input = 1;
		}
		if(!recast_human[0] && !recast_human[1] && !recast_human[2] && !recast_human[3] && !recast_human[4])
			tir_human = 1;
		else
			tir_human = 2;
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground


		for (int i = 0; i < 5; i++)
		{
			if(recast_human[i])
				cast_human[i] = rand() % 6 + 1;
		}

		cout << "Human's result: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0B ); // Set light blue text, black ground
		for (int i = 0; i < 5; i++)
		{
			cout << cast_human[i] << ' ';
		}
		cout << endl;
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground

		cout << "Human enters line#: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0B ); // Set light blue text, black ground
		cin >> put;
		while(put <= 0 || put > 13 || table_human[put - 1] >= 3)
		{
			cout << endl << "   Wrong line. Enter line# again: ";
			cin >> put;
		}

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		table_human[put - 1]++;
		j = summa(cast_human, put - 1, tir_human);
		if(put <= 6)
		{
			j -= (3 + k3) * put;
		}
		points_human += j;
		final_table_human[put - 1].push_back(j);
		cout << endl;


		//////////////////////////    Computer's move   ///////////////////////////////////////////
		recast = move(table, cast);
		for (int i = 0; i < 5; i++)
		{
			if(recast[i])
				cast[i] = rand() % 6 + 1;
		}

		cout << "Computer enters dice# to shoot again: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x03 ); // Set blue-green text, black ground

		input = 0;
		for (int i = 0; i < 5; i++)
		{
			input |= recast[i];
			if(recast[i])
                cout << i + 1;
		}
		if(!input)
			cout << 0; // Print "0" if no dice shoot again
		cout << endl;
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground

		cout << "Computer's result: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x03 ); // Set blue-green text, black ground
		for (int i = 0; i < 5; i++)
		{
			cout << cast[i] << ' ';
		}
		cout << endl;
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground

		res = -999;
		best_put = -1;
		for (int i = 0; i < 6; i++)
		{
			if(table[i] != 3)
			{
				if(res <= summa(cast, i, tir_final) * line_util(table, i))
				{
					best_put = i;
					res = summa(cast, i, tir_final) * line_util(table, i);
				}
			}
		}

		for (int i = 6; i < 12; i++)
		{
			if(table[i] != 3)
			{
				if(res <= summa(cast, i, tir_final) * line_util(table, i))
				{
					best_put = i;
					res = summa(cast, i, tir_final) * line_util(table, i);
				}
			}
		}


		j = summa(cast, best_put, tir_final);
		if(best_put < 6)
		{
			j -= (3 + k3) * (best_put + 1);
		}
		points += j;
		final_table[best_put].push_back(j);

		cout << "Computer enters line#: ";
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x03 ); // Set blue-green text, black ground
		cout << best_put + 1 << endl;
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		table[best_put]++;
		cout << endl;


		// Print current tables
		table_print(final_table_human, final_table, points_human, points, table_headers);
		cout << endl << endl;
	}



    // F I N A L   T A B L E
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x1B ); // Set light blue text, dark blue ground
    cout << "###########   I T ' S   F I N A L   T A B L E   ###############";
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
	cout << endl << endl << endl;

	// Restore console color preferences
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), csbi.wAttributes );
	system("pause");
	return 0;
}

vector <bool> move(vector<int> table, vector<int> cast) // AI's turn with given table and cast
{
	double res1 = -999.9;
	double cur_res = -999.9;
	vector <bool> data_res(5);
	vector <bool> cur_recast(5);
	int tir;

	for(int first = 0; first <= 1; first++)
	{
		for(int second = 0; second <= 1; second++)
		{
			for(int third = 0; third <= 1; third++)
			{
				for(int fourth = 0; fourth <= 1; fourth++)
				{
					for(int fifth = 0; fifth <= 1; fifth++)
					{
						cur_recast[0] = first; cur_recast[1] = second; cur_recast[2] = third; cur_recast[3] = fourth; cur_recast[4] = fifth;
						if(first == second && first == third && first == fourth && first == fifth && first == 0)
							tir = 1;
						else
							tir = 2;
						cur_res = count_recast(table, cast, cur_recast, tir);
						if(cur_res > res1)
						{
							data_res = cur_recast;
							res1 = cur_res;
							tir_final = tir;
						}
					}}}}}

	return data_res;
}

double count_recast(vector<int> table, vector<int> cast, vector<bool> recast, int tir) // Max reached value with given table, cast and dice being recasted
{
	double res2 = 0;
	double sum = 0;
	int num = 0;
	vector <int> cur_data(5);

	vector <int> dice(5, 1);
	for (dice[0] = 1; dice[0] <= 5 * recast[0] + 1; dice[0]++)
	{
		for (dice[1] = 1; dice[1] <= 5 * recast[1] + 1; dice[1]++)
		{
			for (dice[2] = 1; dice[2] <= 5 * recast[2] + 1; dice[2]++)
			{
				for (dice[3] = 1; dice[3] <= 5 * recast[3] + 1; dice[3]++)
				{
					for (dice[4] = 1; dice[4] <= 5 * recast[4] + 1; dice[4]++)
					{
						for (int i = 0; i < 5; i++)
						{
							if(recast[i] == 0)
								cur_data[i] = cast[i];
							else
								cur_data[i] = dice[i];
						}

						num++;
						sum += eval_func(table, cur_data, tir);
					}
				}
			}
		}
	}

	res2 = sum / num;
	return res2;
}

double eval_func(vector<int> table, vector<int> cast, int tir) // Max reached value with given table and cast
{
	double res3 = -999.9;
	double x;
	for (int i = 0; i < 12; i++)
	{
		if(table[i] != 3)
		{
			x = summa(cast, i, tir) * line_util(table, i);
			if(i >= 0 && i <= 5)
				x *= 1 + (pow(i + 1, k1) / k2); // k1 and k2 are correction coefficients determined as globals
			if (x > res3)
				res3 = x;

		}
	}
	return res3;
}

int summa(vector<int> cast, int line, int tir) // Value with given cast into line
{
	if(line < 6)
	{
		int n2 = 0;
		for (int i = 0; i < 5; i++)
		{
			n2 += (cast[i] - 1 == line);
		}
		return (n2 + k3) * (line + 1);
	}
	else
	{
		int res4 = 0;
		sort(cast.begin(), cast.end());
		if(line == 6) // Pair
		{
			for (int i = 0; i < 4; i++)
			{
				if(cast[i] == cast[i + 1])
					res4 = 2 * cast[i];
			}
		}
		else if (line == 7) // Two pairs
		{
			if(cast[0] == cast[1] && cast[2] == cast[3] && cast[0] != cast[2])
				res4 = 2 * cast[0] + 2 * cast[2];
			else if(cast[0] == cast[1] && cast[3] == cast[4] && cast[0] != cast[3])
				res4 = 2 * cast[0] + 2 * cast[3];
			else if(cast[1] == cast[2] && cast[3] == cast[4] && cast[1] != cast[3])
				res4 = 2 * cast[1] + 2 * cast[3];
		}
		else if (line == 8) // Set
		{
			if((cast[0] == cast[1] && cast[0] == cast[2]) || (cast[1] == cast[2] && cast[1] == cast[3]) || (cast[2] == cast[3] && cast[2] == cast[4]))
				res4 = cast[2] * 3;
		}
		else if (line == 9) // Full house
		{
			if(cast[0] == cast[1] && cast[0] == cast[2] && cast[3] == cast[4] && cast[0] != cast[3])
				res4 = cast[0] * 3 + cast[3] * 2;
			else if(cast[0] == cast[1] && cast[2] == cast[3] && cast[2] == cast[4] && cast[0] != cast[2])
				res4 = cast[0] * 2 + cast[2] * 3;
		}
		else if (line == 10) // Straight
		{
			if((cast[0] == 1 && cast[1] == 2 && cast[2] == 3 && cast[3] == 4 && cast[4] == 5) || (cast[0] == 2 && cast[1] == 3 && cast[2] == 4 && cast[3] == 5 && cast[4] == 6))
			{
				for (int i = 0; i < 5; i++)
				{
					res4 += cast[i];
				}
			}
		}
		else if (line == 11) // Four of a kind
		{
			if((cast[0] == cast[1] && cast[0] == cast[2] && cast[0] == cast[3]) || (cast[1] == cast[2] && cast[1] == cast[3] && cast[1] == cast[4]))
				res4 = cast[1] * 4;
		}
		return res4 * (3 - tir);
	}
}

double line_util(vector<int> table, int line) // Utility to put a cast into given table into given line
{
	int fullness = 0;
	for(int i = 0; i < 12; i++)
	{
		if(i != line)
			fullness += table[i];
	}
	return util_table[table[line]][fullness];
}

void table_print(vector <vector<int> > final_table_human, vector <vector<int> > final_table, int points_human, int points, vector<string> table_headers) // Print out current and final tables
{
	int q1 = 7, q2 = 5;
	string str1 = "   Human's table", str2 = "   Computer's table";
	int len1 = str1.length(), len2 = str2.length();
    string str3 = "  Human's score: ", str4 = "  Computer's score: ";
	int len3 = str3.length(), len4 = str4.length();

    ///  #1   ///
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
    cout << "    " << char(201);
	for (int j = 0; j < q1 + q2 * 3 + 3; j++)
	{
        cout << char(205);
	}
    cout << char(203);
    for (int j = 0; j < q1 + q2 * 3 + 3; j++)
	{
        cout << char(205);
	}
    cout << char(187);
    cout << endl;



    ///  #2 HEADER   ///
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
    cout << "    " << char(186);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x1B ); // Set light blue text, dark blue ground
	cout << str1;
	for (int j = 0; j < q1 + q2 * 3 + 3 - len1; j++)
	{
        cout << ' ';
	}
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
	cout << char(186);
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x13 ); // Set blue-green text, dark blue ground
	cout << str2;
	for (int j = 0; j < q1 + q2 * 3 + 3 - len2; j++)
	{
        cout << ' ';
	}
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
    cout << char(186);
    cout << endl;



    ///  #3   ///
    cout << "    " << char(204);
    //
	for (int j = 0; j < q1; j++)
	{
        cout << char(205);
	}
    cout << char(203);
    for (int j = 0; j < q2; j++)
    {
        cout << char(205);
    }
    cout << char(209);
    for (int j = 0; j < q2; j++)
    {
        cout << char(205);
    }
    cout << char(209);
    for (int j = 0; j < q2; j++)
    {
        cout << char(205);
    }
    //
    cout << char(206);
    //
	for (int j = 0; j < q1; j++)
	{
        cout << char(205);
	}
    cout << char(203);
    for (int j = 0; j < q2; j++)
    {
        cout << char(205);
    }
    cout << char(209);
    for (int j = 0; j < q2; j++)
    {
        cout << char(205);
    }
    cout << char(209);
    for (int j = 0; j < q2; j++)
    {
        cout << char(205);
    }
    cout << char(185);
    //
    cout << endl;



    ///  #4 SCORE   ///
    for (int i = 0; i < 12; i++)
	{
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		if(i < 9)
			cout << ' ';
		cout << ' ' << i + 1 << ' ';

		cout << char(186);
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0E ); // Set yellow text, black ground
		cout << ' ' << table_headers[i] << ' ';
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		cout << char(186);

		for (int j = 0; j < 3; j++)
		{
			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0B ); // Set light blue text, black ground
			if(j < final_table_human[i].size())
			{
                cout << setw(4) << final_table_human[i][j];
                cout << ' ';
			}
			else
			{
                cout << "     ";
			}
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
            if(j < 2)
            {
                cout << char(179);
            }
            else
            {
                cout << char(186);
            }
		}

        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0E ); // Set yellow text, black ground
		cout << ' ' << table_headers[i] << ' ';
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
		cout << char(186);

		for (int j = 0; j < 3; j++)
		{
			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x03 ); // Set blue-green text, black ground
			if(j < final_table[i].size())
			{
                cout << setw(4) << final_table[i][j];
                cout << ' ';
			}
			else
			{
                cout << "     ";
			}
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
            if(j < 2)
            {
                cout << char(179);
            }
            else
            {
                cout << char(186);
            }
		}
        cout << endl;


        if(i == 5)
        {
            ///  #6   ///
            cout << "    " << char(204);
            //
            for (int j = 0; j < q1; j++)
            {
                cout << char(205);
            }
            cout << char(206);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(216);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(216);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            //
            cout << char(206);
            //
            for (int j = 0; j < q1; j++)
            {
                cout << char(205);
            }
            cout << char(206);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(216);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(216);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(185);
            //
            cout << endl;
        }

        else if(i == 11)
        {
            ///  #7   ///
            cout << "    " << char(204);
            //
            for (int j = 0; j < q1; j++)
            {
                cout << char(205);
            }
            cout << char(202);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(207);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(207);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            //
            cout << char(206);
            //
            for (int j = 0; j < q1; j++)
            {
                cout << char(205);
            }
            cout << char(202);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(207);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(207);
            for (int j = 0; j < q2; j++)
            {
                cout << char(205);
            }
            cout << char(185);
            //
            cout << endl;
        }

        else
        {
            ///  #5   ///
            cout << "    " << char(199);
            //
            for (int j = 0; j < q1; j++)
            {
                cout << char(196);
            }
            cout << char(215);
            for (int j = 0; j < q2; j++)
            {
                cout << char(196);
            }
            cout << char(197);
            for (int j = 0; j < q2; j++)
            {
                cout << char(196);
            }
            cout << char(197);
            for (int j = 0; j < q2; j++)
            {
                cout << char(196);
            }
            //
            cout << char(215);
            //
            for (int j = 0; j < q1; j++)
            {
                cout << char(196);
            }
            cout << char(215);
            for (int j = 0; j < q2; j++)
            {
                cout << char(196);
            }
            cout << char(197);
            for (int j = 0; j < q2; j++)
            {
                cout << char(196);
            }
            cout << char(197);
            for (int j = 0; j < q2; j++)
            {
                cout << char(196);
            }
            cout << char(182);
            //
            cout << endl;
        }
	}



    ///  #8 SCORE   ///
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
    cout << "    " << char(186);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x1B ); // Set light blue text, dark blue ground
	cout << str3 << setw(4) << points_human;
	for (int j = 0; j < q1 + q2 * 3 - 1 - len3; j++)
	{
        cout << ' ';
	}
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
	cout << char(186);
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x13 ); // Set blue-green text, dark blue ground
	cout << str4 << setw(4) << points;
	for (int j = 0; j < q1 + q2 * 3 - 1 - len4; j++)
	{
        cout << ' ';
	}
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
    cout << char(186);
    cout << endl;


    ///  #9   ///
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F ); // Set white text, black ground
    cout << "    " << char(200);
	for (int j = 0; j < q1 + q2 * 3 + 3; j++)
	{
        cout << char(205);
	}
    cout << char(202);
    for (int j = 0; j < q1 + q2 * 3 + 3; j++)
	{
        cout << char(205);
	}
    cout << char(188);
    cout << endl;

}
