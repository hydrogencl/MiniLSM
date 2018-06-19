#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <iterator>
#include <stdlib.h>

// Please compile with: 
// g++ -std=c++11 
//
// Please note: No warrenty for scientific using. 
//		Please be advised that though this program
//		follows the Penmen-Monteith equation for 
//		calculate the latent heat/evapotranspiration,
//		some surface variable should be used 
//		carefully, e.g. the net radiation. 

using namespace std;

//vector < vector<double> arr_all (double);
vector<double> arr_all (double);

//double ReadingWeatherCSV (string file_weather){
vector <double> ReadingWeatherCSV (string file_weather, int var){
        string line;
        ifstream file_in ( file_weather);
	vector <double> arr_all;
        if (file_in.is_open()){
                while (getline (file_in,line) ){
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
                        arr_all .push_back (arr_row[var]);
                }
	file_in.close();
        }
        else cout << "None this file";
        return arr_all ;
}

double SlopeVapourCurve (double T){
	double Delta; // kPa oC^-1
	Delta = 4098 * (0.6108 * exp(17.27 * (T-273.15)/(T))) / pow(T,2);
	return Delta;
}

double psychrometric (double P){
	// P = Pa
	double Gamma;  // kPa oC^-1
	double eta = 0.622;
	double lambda = 2.45; // MJ kg^-1
	double cp = 1.013 * pow(10,-3); // MJ kg^-1 oC^-1 
	Gamma = cp * P*0.001 / (eta * lambda );
	return Gamma;
}

double saturatedvapour (double T, double P) {
	// es = Pa
	double a0 =  6.11213476          ;
	double a1 =  0.444007856         ;
	double a2 =  0.143064234e-01     ;
	double a3 =  0.264461437e-03     ;
	double a4 =  0.305903558e-05     ;
	double a5 =  0.196237241e-07     ;
	double a6 =  0.892344772e-10     ;
	double a7 = -0.373208410e-12     ;
	double a8 =  0.209339997e-15     ;
	double c0 =  6.11123516          ;
	double c1 =  0.503109514         ;
	double c2 =  0.188369801e-01     ;
	double c3 =  0.420547422e-03     ;
	double c4 =  0.614396778e-05     ;
	double c5 =  0.602780717e-07     ;
	double c6 =  0.387940929e-09     ;
	double c7 =  0.149436277e-11     ;
	double c8 =  0.262655803e-14     ;
     
	double TD = T-273.15             ;

	double es;

	if (TD > 0.0){
		es = a0 + TD*(a1 + TD*(a2 + TD*(a3 + TD*(a4 + TD*(a5 + TD*(a6 + TD*(a7 + TD*a8)))))));
	} else {
		es = c0 + TD*(c1 + TD*(c2 + TD*(c3 + TD*(c4 + TD*(c5 + TD*(c6 + TD*(c7 + TD*c8)))))));
	}
	return es;

}

double surfaceRes(double uz, double zm, double zh, double VegHeight){
	double ra; //sm^-1
	double K = 0.41;
	double z0m = VegHeight * 0.123 ;
	double z0h = VegHeight * 0.1   ;
	double d   = VegHeight * 2 / 3 ;
	ra = log((zm-d)/z0m) * log((zh-d)/z0h) / (pow(K,2) * uz);
	return ra;
}

double vapour2humidity (double e, double P){
	double q = 0.622 * e / (P*0.001 - 0.378 * e);
	return q;
}

double humid2vapour (double q, double P){
	double e = P * q / 0.622 / 100 ;
	return e;
}

double canopyRes(double rsmin, double Rnet, double es, double ea,double T, double LAI, double P){
	double rs ; //sm^-1
	double rsmax= 20000 ;
	double hs   = 36.35 ;
	double rgl  = 100   ;
	double modt = 298.15;

	double ff   = 2.0 * Rnet / rgl;
	double q2    = vapour2humidity (ea, P  );
	double q2sat = vapour2humidity (es, P  );

	double f1   = (ff + rsmin/rsmax)/ (1.0+ff) ; 
	double f2   = 1.0 - 0.0016 * pow((T-modt),2);
	double f3   = 1.0 / (1.0 + hs * max((q2sat - q2),0.0 ));
	rs =  min(  rsmin /(f1 * f2 * f3), rsmax) / LAI;
	//cout << "rs: " << rs << endl;
	return rs;
}

double PenmanMonteith (double Rnet, double G, double Delta, double Gamma,double T,double u2, double es, double ea, double rho_air, double rs, double ra) {
	double LambdaET; // W M^-1
        double lambda = 2.45; // MJ kg^-1
        double cp = 1.013 * pow(10,-3); // MJ kg^-1 oC^-1 
	double EnergyTerm = Delta * (Rnet - G); 
	double HumidTerm  = rho_air * cp * (es - ea)/ra ; 
	double ResistTerm = Delta + Gamma * (1 + rs/ra) ; 
	LambdaET = (EnergyTerm + HumidTerm) / ResistTerm;
	return LambdaET;
}

int main (){
	// Missing : 1) Radiative transfer scheme
	//	     2) Soil Moisture
	//	     3) Leaf temperature


	vector <double> arr_temp = ReadingWeatherCSV ("./weather_input.csv",0);
	vector <double> arr_pres = ReadingWeatherCSV ("./weather_input.csv",1);
	vector <double> arr_humi = ReadingWeatherCSV ("./weather_input.csv",2);
	vector <double> arr_wind = ReadingWeatherCSV ("./weather_input.csv",3);
	vector <double> arr_fsds = ReadingWeatherCSV ("./weather_input.csv",4);
	vector <double> arr_prec = ReadingWeatherCSV ("./weather_input.csv",5);

	for (int n=0; n<arr_temp.size(); n++){
	double BoltzmannK = 5.67 * pow(10,-8);
	double TempAir = arr_temp[n] ;
	//  LWout =  BoltzmannK * pow(TempAir,4) ; 
	//double Rnet    = arr_fsds[n] - BoltzmannK * pow(Temp,4) ; 
	double Rnet    = arr_fsds[n] ; 
	double uz      = arr_wind[n] ;
	double pres    = arr_pres[n] ;
	double G       = arr_fsds[n] * 0.05 ;
	double rho_air = 1.2041      ;
	double ea      = humid2vapour(arr_humi[n], pres) ;
	double LAI     = 2.0                    ;
	double vegH    = 0.5                    ;
	double delta = SlopeVapourCurve(TempAir)   ;
	//cout << SlopeVapourCurve(283.6) << endl  ;
	double gamma = psychrometric(pres)         ;
	double es = saturatedvapour(TempAir, pres )       ;
	double rs = canopyRes( 40,   Rnet, es, ea, TempAir, LAI, pres) ;
	double ra = surfaceRes (uz, 2.0, 2.0, vegH);
	double ET = PenmanMonteith (Rnet, G, delta, gamma, TempAir, uz, es, ea, rho_air, rs, ra); 
	cout << "Time: " << n << " InSWRadia: " << Rnet <<  " ET= " << ET << " W m^-1" << endl   ;
	}
	return 0;
}
