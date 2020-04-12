#pragma once
#include "Production.h"

class Gramatica {
public:
	string estadoInicial;
	vector<Produccion*> production;
	map<string, bool> terminales;

	void read(string texto);
	void readContx(string texto);
	void readVarContx(string subtex, Token &toq);
	vector<Token>* readMoreProduc(string& subtex);
	vector<Token>* readTerminals(string& subtex);
	vector<vector<string>> getProduction(string izq);
	void printGrammar();
	Gramatica() {}
	~Gramatica() {}

	Produccion* get_Production(int pos);
	void insertProduction(Produccion* xtr, int pos);
	bool isTerminal(string token);
	void setTerminals();
};

