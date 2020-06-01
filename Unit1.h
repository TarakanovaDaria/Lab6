//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <Vcl.DBGrids.hpp>
#include <Vcl.Grids.hpp>
#include <stdio.h>
#include <math.h>

enum  Tmeasure{tcharge = 1, tenergy, tvoltage, tmass, tnumb, tcap, tsquare,
tdistance, telectric,tdenergy, tdcap, tdvol, tdcharge,tddist, tdsquare};
enum  Ttask{tzero = 0, tC_energy, tC_cap, tC_vol, tC_charge, tC_dist, tC_elec,
			td_energy, td_cap, td_vol, td_charge, toverlay};
int global_task;

	struct TinputData{
		double value;
		Tmeasure measure;
	} inputData[10];
//---------------------------------------------------------------------------
class Tf_main : public TForm
{
__published:	// IDE-managed Components
	TMemo *chat;
	TButton *b_analyze;
	TButton *b_solve;
	TMemo *check_chat;
	TMemo *answer_chat;
	TADOQuery *qry1;
	TADOTable *tbl1;
	TDataSource *ds1;
	TDBGrid *task_table;
	TEdit *e_num;
	TLabel *lbl1;


	void __fastcall b_analyzeClick(TObject *Sender);
	void __fastcall b_solveClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall Tf_main(TComponent* Owner);

	int check_a_word(String word, int i, int flag);
	String solve(Ttask type);
	void Tf_main::dimention_install(String word, int i);
	Tmeasure Tf_main::dimention_from_db(String word);
	Ttask Tf_main::task_install(String number);
	void Tf_main::clear_inputData(int length);
	String Tf_main::delta_type(Ttask type);
	int Tf_main::check_type_dimention(Ttask type, String dimention);
	int Tf_main::check_delta_type(String  dimention);

};
	



//---------------------------------------------------------------------------
extern PACKAGE Tf_main *f_main;
//---------------------------------------------------------------------------
#endif
