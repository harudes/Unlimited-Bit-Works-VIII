#include "pch.h"
#include "Grammar.h"

void Gramatica::read(string texto) {
	if (texto.find("=?")) {
		readContx(texto);
		return;
	}
	string t1 = texto.substr(0, texto.find(":="));
	DeteleFromString(t1, ' ');
	Token temp1(t1);
	string temp2 = texto.substr(texto.find(":=") + 2, texto.length() - 1);
	stringstream ss(temp2);
	string token;
	while (getline(ss, token, '|')) {
		vector<Token> der;
		stringstream ss2(token);
		string token2;
		while (ss2 >> token2) {
			der.push_back(Token(token2));
		}
		Produccion* temp = new Produccion(temp1, der);
		production.push_back(temp);
	}
}
void Gramatica::readContx(string texto) {
	size_t separe = texto.find(":=");
	string nameIZQ = texto.substr(0, texto.find("["));
	DeteleFromString(nameIZQ, ' ');
	Token nn(nameIZQ);
	readVarContx(texto.substr(texto.find("[") + 1, separe - texto.find("[") - 1), nn);

	string dere = texto.substr(separe + 2);

	Produccion* ctp = new Produccion(nn);
	if (dere.find("?_") != string::npos) {
		vector<Token> *to_der = readMoreProduc(dere);
		ctp->der = to_der;
	}
	else {
		vector<Token> *to_der = readTerminals(dere);
		ctp->der = to_der;
	}
	production.push_back(ctp);
}
void Gramatica::readVarContx(string subtex, Token &toq) {
	DeteleFromString(subtex, ']');
	stringstream Saux(subtex);
	string temp;
	vector<string> *vari = new vector<string>;
	vector<string> *vals = new vector<string>;
	while (getline(Saux, temp, ',')) {
		vari->push_back(temp.substr(0, temp.find("=")));
		vals->push_back(temp.substr(temp.find("=") + 1));
	}
	toq.var = vari;
	toq.val = vals;
}
vector<Token>* Gramatica::readMoreProduc(string& subtex) {
	stringstream Saux(subtex);
	string temp;
	vector<Token> *otherProd = new vector<Token>;
	while (Saux >> temp) {
		string nameIZQ = temp.substr(0, temp.find("["));
		Token nn(nameIZQ);
		readVarContx(temp.substr(temp.find("[") + 1), nn);
		otherProd->push_back(nn);
		//			production.push_back(& otherProd[otherProd.size()-1]);
	}
	return otherProd;
}
vector<Token>* Gramatica::readTerminals(string& subtex) {
	stringstream Saux(subtex);
	string temp;
	vector<Token>* terminals = new vector<Token>;
	while (Saux >> temp) {
		terminals->push_back(temp);
	}
	return terminals;
}
vector<vector<string>> Gramatica::getProduction(string izq) {
	vector<vector<string>> result;
	for (size_t i = 0; i < production.size(); ++i) {
		if (production[i]->nombre.name == izq) {
			if (production[i]->der) {
				vector<string> t;
				for (size_t j = 0; j < production[i]->der->size(); ++j) {
					t.push_back(production[i]->der->at(j).name);
				}
				result.push_back(t);
			}
			else {
				vector<string> temp;
				Produccion* a = production[i];
				for (int j = 0; j < a->der->size(); j++) {
					string te = a->der->at(j).name;
					temp.push_back(te);
				}
				result.push_back(temp);
			}
		}
	}
	return result;
}
void Gramatica::printGrammar() {
	for (size_t i = 0; i < production.size(); ++i) {
		production[i]->nombre.print();
		for (size_t j = 0; j < production[i]->der->size(); ++j) {
			production[i]->der->at(j).print();
		}
	}
}

Produccion* Gramatica::get_Production(int pos) {
	return production[pos];
}
void Gramatica::insertProduction(Produccion* xtr, int pos) {
	production.insert(production.begin() + pos, xtr);
}

bool Gramatica::isTerminal(string token) {
	return terminales[token];
}

void Gramatica::setTerminals() {
	for (size_t i = 0; i < production.size(); ++i) {
		for (size_t j = 0; j < production[i]->der->size(); ++j) {
			terminales[production[i]->der->at(j).name]=true;
		}
	}
	for (size_t i = 0; i < production.size(); ++i) {
		terminales[production[i]->nombre.name] = false;
	}
}
