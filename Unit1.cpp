//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <math.h>
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tf_main *f_main;
String info = "";
//---------------------------------------------------------------------------
__fastcall Tf_main::Tf_main(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall Tf_main::b_analyzeClick(TObject *Sender)
{
	int p;           //space
	int q;           //point
	int d;           //comma
	int b;			 //question
	int l;
	double number;
	Ttask type =  task_install(global_task);

	qry1->Close();
	qry1->SQL->Clear();
	qry1->SQL->Add("select * from tasks where number='"+ FloatToStr(global_task) + "'");
	qry1->Open();
	String text = qry1->FieldByName("task")->AsString;

	String word;
	int i = 0;
	char *measureText[]={"","Кл","Дж","В","кг"," ", "Ф","м^2","м","В/м"};
	bool flag = false;
	int numb = 0;

	do{
		p = text.Pos(" ");

		if (p == 0) {
			word = text;
		}
		else{
			word = text.SubString(1, p-1);
			text = text.SubString(p+1,text.Length());
		}

		q = word.Pos(".");
		d = word.Pos(",");
		b = word.Pos("?");
		l = word.Length();
		if (l == q || l == d || l == b) {
			word = word.SubString(1,l-1);
		}

		if(flag == true){
			dimention_install(word, i-1);
		}

		check_chat->Lines->Add(word);

		try{
			number = word.ToDouble();

			if(type == td_energy || type == td_cap || type == td_vol || type == td_charge){
				inputData[i].value = inputData[i].value*number;
			}
			else{
				inputData[i].measure = tnumb;
				inputData[i].value = number;
				flag = true;
			}
			i++;
		  //	chat->Lines->Add("Число");
		}
		catch(const Exception& e)
		{
			if(flag == true){
				flag = false;
			}
			else{

				numb = check_a_word(word,i,numb);

				if(numb == 3){
					i++;
				}
				if(numb > 0 && numb < 3){
					flag = true;
					if(numb == 1){
						i++;
					}
				}
				//словарь
			}
		}
	} while(p);

	for(int j = 0; j < i; j++){
		check_chat->Lines->Add(FloatToStr(inputData[j].value) + " "
		 + measureText[inputData[j].measure]);
	}
}
//---------------------------------------------------------------------------

void __fastcall Tf_main::b_solveClick(TObject *Sender)
{
	int p;
	bool flag = false;
	int number;
	String task_num = e_num->Text;

	if (task_num.Length() == 0) {
		number = 1;
		flag = true;
	}
	else{
		try{
			number = task_num.ToDouble();
		}
		catch(const Exception& e){
			answer_chat->Lines->Add("Неверный номер задания");
		}
	}

	do{
		Ttask type = task_install(number);
		if(type != toverlay){
			if(type != tzero){
				global_task = number;
				b_analyzeClick(e_num);
				answer_chat->Lines->Add(FloatToStr(global_task)+")"+"Ответ: " + solve(type));
				clear_inputData(10);
			}
			number++;
		}
		else{
			flag = false;
		}
	}while(flag);
}
//---------------------------------------------------------------------------
int  Tf_main::check_a_word(String word, int i, int flag){

	//если  numb = 1 - word - первое числительное, 3 - размерность.
	//если 2, составное числительное, 0 - не числительное.
	// 4 - отношение

	int number;

	AnsiString sign;
	AnsiString str;

	Ttask type =  task_install(global_task);

	qry1->Close();
	qry1->SQL->Clear();
	qry1->SQL->Add("select * from numbers where word_db='"+word+"'");
	qry1->Open();
	sign = qry1->FieldByName("number")->AsString;
	str = qry1->FieldByName("dimention")->AsString;


	try{
		number = sign.ToDouble();
	}
	catch(const Exception& e)
	{
		return 0;
	}

	if(type == td_energy || type == td_cap || type == td_vol || type == td_charge){
		if(check_type_dimention(type, str)){
			if (str == "d_number") {
				inputData[i].value = number;
				return 4;
			}
			else if(str == "tdenergy"||str == "tdcap"||str == "tdvol"||str == "tdcharge"){
				if(number == 0){
					inputData[i].value = 1;
					inputData[i].measure = dimention_from_db(str);
					return 3;
				}
				inputData[i].measure = dimention_from_db(str);
				return 4;
			}
		}
	}

	if(str != "number" && flag == 0 && check_delta_type(str) == 1){
		inputData[i].value = 1;
		inputData[i].measure = dimention_from_db(str);
		return 3;
	}

	if(flag == true){
		if(number == 1000){
			inputData[i-1].value = number * inputData[i-1].value;
		}
		else{
			inputData[i-1].value = number + inputData[i-1].value;
		}
		return 2;
	}
	else if(str == "number"){
		inputData[i].value = number;
		inputData[i].measure = tnumb;
		return 1;
	}

	return 0;
}

String  Tf_main::solve(Ttask type){
	if(type == td_energy || type == td_cap || type == td_vol || type == td_charge){
		return delta_type(type);
	}
	double Q = 0;
	double U = 0;
	double C = 0;
	double E = 0;
	double e = 1;
	double s = 0;
	double d = 0;
	double W = 0;
	double e0 = 8.854187812813*0.000000000001;


	for(int i = 0; i < 10; i++){
		if(inputData[i].measure == tcharge){
			Q = inputData[i].value;
		}
		if(inputData[i].measure == tvoltage){
			U = inputData[i].value;
		}
		if(inputData[i].measure == tcap){
			C = inputData[i].value;
		}
		if(inputData[i].measure == tnumb){
			e = inputData[i].value;
		}
		if(inputData[i].measure == tdistance){
			d = inputData[i].value;
		}
		if(inputData[i].measure == tsquare){
			s = inputData[i].value;
		}
		if(inputData[i].measure == telectric){
			E = inputData[i].value;
		}
		if(inputData[i].measure == tenergy){
			W = inputData[i].value;
		}
	}

	if(C == 0 && !(d == 0 || s == 0)){
		C = e0*e*s/d;
		if(U == 0 && E != 0){
			U = E*d;
		}
	}

	if(type == tC_energy){

		if(Q == 0){
			if( U == 0){
				 return -1;
			}
			W = C*U*U/2;
		}
		else if(U == 0){
			if( Q == 0 ){
				 return -1;
			}
			W = Q*Q/(2*C);
		}
		else{
			W = Q*U/2;
		}

		return FloatToStr(W)+" Дж";
	}
	else if(type == tC_cap) {
		if((Q == 0 || U == 0) && C == 0){
			return -1;
		}
		else if( C == 0){
			C = Q/U;
		}
		return FloatToStr(C)+" Ф";
	}
	else if(type == tC_vol) {
		if(W == 0){
			if(Q == 0 || C == 0){
				return -1;
			}
			U = Q/C;
		}
		else{
			if(Q == 0){
				return -1;
			}
			U = 2*W/Q;
		}
		return FloatToStr(U)+" В";
	}
	else if(type == tC_charge) {
		if(U == 0 || C == 0){
			return -1;
		}
		Q = U*C;
		return FloatToStr(Q)+" Кл";
	}
	else if(type == tC_dist) {
		if(s == 0 || C == 0){
			return -1;
		}
		d = e*e0*s/C;
		return FloatToStr(d)+" м";
	}
	else if(type == tC_elec) {
		if(s == 0 || Q == 0){
			return -1;
		}
		E = Q/e*e0*s;
		return FloatToStr(E)+" В/м";
	}
	return -1;
}

void Tf_main::dimention_install(String word, int i){

    qry1->Close();
	qry1->SQL->Clear();
	qry1->SQL->Add("select * from numbers where word_db='"+word+"'");
	qry1->Open();
	String sign = qry1->FieldByName("number")->AsString;
	String str = qry1->FieldByName("dimention")->AsString;

	inputData[i].value = inputData[i].value*sign.ToDouble();
	inputData[i].measure = dimention_from_db(str);
	
}

Tmeasure Tf_main::dimention_from_db(String word){
	if(word.Pos("tcharge") > 0){
		return tcharge;
	}
	else if(word.Pos("tenergy") > 0){
		return tenergy;
	}
	else if(word.Pos("tvoltage") > 0 ){
		return tvoltage;
	}
	else if(word.Pos("tmass") > 0){
		return tmass;
	}
	else if(word.Pos("tcap") > 0){
		return tcap;
	}
	else if(word.Pos("tsquare") > 0){
		return tsquare;
	}
	else if(word.Pos("tdistance") > 0){
		return tdistance;
	}
	else if(word.Pos("telectric") > 0){
		return telectric;
	}
	else if(word.Pos("tdenergy") > 0){
		return tdenergy;
	}
	else if(word.Pos("tdvol") > 0){
		return tdvol;
	}
	else if(word.Pos("tddist") > 0){
		return tddist;
	}
	else if(word.Pos("tdcap") > 0){
		return tdcap;
	}
	else if(word.Pos("tdchargec") > 0){
		return tdcharge;
	}
	else if(word.Pos("tdsquare") > 0){
		return tdsquare;
	}
}

 Ttask Tf_main::task_install(String number){
	qry1->Close();
	qry1->SQL->Clear();
	qry1->SQL->Add("select * from tasks where number='"+number+"'");
	qry1->Open();
	String text = qry1->FieldByName("task")->AsString;
	if (text.Length() == 0){
		return toverlay;
	}

	if(text.Pos("акой энергией ") > 0 ||
	text.Pos("Определите энергию") > 0||
	text.Pos("Определить энергию") > 0||
	text.Pos("акое количество теплоты") > 0||
	text.Pos("акова энергия") > 0){
		return tC_energy;
	}
	else if(text.Pos("акова электроемкость") > 0||
	text.Pos("Определить электроемкость") > 0||
	text.Pos("Определите емкость") > 0||
	text.Pos("акова емкость") > 0){
		return tC_cap;
	}
	else if(text.Pos("Определите напряжение") > 0||
	text.Pos("Какова разность потенциалов") > 0||
	text.Pos("До какого напряжения") > 0){
		return tC_vol;
	}
	else if(text.Pos("акой наибольший допустимый электрический заряд") > 0||
	text.Pos("Какой заряд") > 0){
		return tC_charge;
	}
	else if(text.Pos("Определите толщину ") > 0||
	text.Pos("На каком расстоянии") > 0){
		return tC_dist;
	}
	else if(text.Pos("Какова напряженность ") > 0||
	text.Pos("Найти напряженность ") > 0){
		return tC_elec;
	}else if(text.Pos("Как изменится энергия ") > 0||
	text.Pos("Во сколько раз изменится энергия") > 0){
		return td_energy;
	}
	else if(text.Pos("Как изменится емкость ") > 0||
	text.Pos("Во сколько раз изменится емкость")||
	text.Pos("Как изменится электроемкость ") > 0 ||
	text.Pos("Во сколько раз изменится электроемкость") > 0||
	text.Pos("Как изменилась емкость") > 0 ||
	text.Pos("Как изменилась электроемкость") > 0 ||
	text.Pos("Как изменилась электроемкость") > 0){
		return td_cap;
	}
	else if(text.Pos("Как изменится напряжение ") > 0||
	text.Pos("Во сколько раз изменится напряжение") > 0||
	text.Pos("Во сколько раз изменилось напряжение") > 0){
		return td_vol;
	}
	else if(text.Pos("Как изменится заряд ") > 0||
	text.Pos("Во сколько раз изменится заряд") > 0||
	text.Pos("Во сколько раз изменился заряд") > 0){
		return td_charge;
	}
	else{
		answer_chat->Lines->Add("Не знаю, как решить.");
		return tzero;
	}
 }
//---------------------------------------------------------------------------
void Tf_main::clear_inputData(int length){
	int i;
	for(i = 0; i < length; i++){
		inputData[i].value = NULL;
		inputData[i].measure = NULL;
	}
}

String Tf_main::delta_type(Ttask type){
// если увеличивается number>0, иначе <0. добавить в анализ, чек и базу number
//"увеличили"  и "уменьшили" + в таск инстол
	int e[2];
	e[0] = 1;
	e[1] = 1;
	int j;
	double N;
	double dQ = 1;
	double dU = 1;
	double dC = 1;
	double ds = 1;
	double dd = 1;
	double de = 1;
	double dW = 1;

	for(int i = 0; i < 10; i++){
		if(inputData[i].measure == tdcharge){
			dQ = inputData[i].value;
		}
		if(inputData[i].measure == tdvol){
			dU = inputData[i].value;
		}
		if(inputData[i].measure == tdcap){
			dC = inputData[i].value;
		}
		if(inputData[i].measure == tnumb){
			e[j] = inputData[i].value;
			j++;
		}
		if(inputData[i].measure == tddist){
			dd = inputData[i].value;
		}
		if(inputData[i].measure == tdsquare){
			ds = inputData[i].value;
		}
		if(inputData[i].measure == tdenergy){
			dW = inputData[i].value;
		}
	}

	if (e[0] != e[1]){
		de = e[0]/e[1];
	}

	if(dd !=1 || ds !=1 || de!=1){
		dC = (-1*dd)*ds*de;
	}

	if(type == td_energy){
		if(dU == 1){
			 dW = dW*dQ*dQ*(-1*dC);
		}
		else if (dQ == 1) {
			dW = dW*dU*dU*dC;

		}
		else if (dC == 1) {
			dW = dW*dU*dQ;
		}
		N = fabs(dW);
		if (dW > 1){
			return "увеличилось в " + FloatToStr(N)+" раз";
		}
		if (dW < 1){
			return "уменьшилось в " + FloatToStr(N)+ " раз";
		}
		return "Не изменилось";
	}
	if(type == td_cap){
		N = fabs(dC);

		if (dC > 1){
			return "увеличилось в " + FloatToStr(N)+" раз";
		}
		if (dC < 1){
			return "уменьшилось в " + FloatToStr(N)+ " раз";
		}
		return "Не изменилось";
	}
	if(type == td_vol){
		if(dU == 1){
			return "Не изменилось";
		}
		else {
			dU = -1*dU*dC;

			N = fabs(dU);

			if (dU > 1){
				return "увеличилось в " + FloatToStr(N)+" раз";
			}
			if (dU < 1){
				return "уменьшилось в " + FloatToStr(N)+ " раз";
			}
			return "Не изменилось";
		}
	}
	if(type == td_charge){
		if(dQ == 1){
			return "Не изменилось";
		}
		else {
			dQ = dU*dC;

			N = fabs(dQ);

			if (dQ > 1){
				return "увеличилось в " + FloatToStr(N)+" раз";
			}
			if (dQ < 1){
				return "уменьшилось в " + FloatToStr(N)+ " раз";
			}
			return "Не изменилось";
		}
	}
}

int Tf_main::check_type_dimention(Ttask type, String  dimention){

	if(type == td_energy && dimention == "tdenergy"){
		return 0;
	}
	if(type == td_cap && dimention == "tdcap"){
		return 0;
	}
	if(type == td_vol && dimention == "tdvol"){
		return 0;
	}
	if(type == td_charge && dimention == "tdcharge"){
		return 0;
	}
	return 1;
}

int Tf_main::check_delta_type(String  dimention){

	if(dimention == "tdenergy"){
		return 0;
	}
	if(dimention == "tdcap"){
		return 0;
	};
	if(dimention == "tdvol"){
		return 0;
	}
	if(dimention == "tdcharge"){
		return 0;
	}
	if(dimention == "tdsquare"){
		return 0;
	}
	if(dimention == "tddist"){
		return 0;
	}
	return 1;
}
