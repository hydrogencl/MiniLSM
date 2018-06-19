#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <iterator>
#include <stdlib.h>
// g++ -std=c++11 Readcsv.cpp
using namespace std;

int main(){
	string line;
	ifstream file_in ("./weather_input.csv");
	if (file_in.is_open()){
		vector < vector <double> > arr_all  ;
		vector <double> arr_fsds ;
		vector <double> arr_pres ;
		vector <double> arr_prec ;
		vector <double> arr_temp ;
		vector <double> arr_humi ;
		vector <double> arr_wind ;
		while (getline (file_in,line) ){
		//cout << line <<'\n';
		// try to deal with lines
			regex ws_re(",");
			sregex_token_iterator chk_line (line.begin(), line.end(), ws_re, -1);
        	        sregex_token_iterator reg_end;
			vector <double> arr_row;
			for (; chk_line != reg_end; ++chk_line) {
				//cout << chk_line ->str() << endl;
				//chk_num = atof(chk_line)
				double chk_num = stod(chk_line->str());
				arr_row.push_back (chk_num);
			}
			arr_temp.push_back (arr_row[0]);
			arr_pres.push_back (arr_row[1]);
			arr_humi.push_back (arr_row[2]);
			arr_wind.push_back (arr_row[3]);
			arr_fsds.push_back (arr_row[4]);
			arr_prec.push_back (arr_row[5]);
			cout << arr_row[0] << "  " << arr_row[1] << endl;
		}
	for (int n=0; n<95; n++){
		cout << arr_temp[n] << endl;
	}

	arr_all.push_back (arr_temp);
	arr_all.push_back (arr_pres);
	arr_all.push_back (arr_humi);
	arr_all.push_back (arr_wind);
	arr_all.push_back (arr_fsds);
	arr_all.push_back (arr_prec);
  // Check if the output of code is right.
  //cout << arr_all.size();
	//for (int n=0; n<95; n++){
	//	cout << arr_all[2][n] << endl;
	//}

	file_in.close();
	}
	else cout << "None this file";
	return 0;
}
