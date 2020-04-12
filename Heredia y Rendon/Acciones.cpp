#include "pch.h"
#include "Acciones.h"

Estado_Compilador::Estado_Compilador(int s_cht, int ast_pos, int pal_pos, Produccion* ptr_p, Estado_Compilador* ptr_e) {
	this->estadoChart = s_cht;
	this->PosAsterisco = ast_pos;
	this->PosPalabra = pal_pos;
	this->producRef = ptr_p;
	this->root = ptr_e;
}

bool Dummy::sePuedeAplicar(Estado_Compilador* stte) {
	return true;
}
void Dummy::aplica(Estado_Compilador* stte) {//, queue<Estado_Compilador*> &chrt){
	Produccion* fist = gramarSource->get_Production(0);
	vector<Token> v;
	v.push_back(fist->nombre);
	string na = "S";
	Token nn(na);
	Produccion* temp = new Produccion(nn, v);
	stte->PosAsterisco = 0;
	stte->PosPalabra = 0;
	stte->root = 0;
	stte->producRef = temp;

	gramarSource->production.insert(gramarSource->production.begin(), temp);
}
Dummy::Dummy(Estado_Compilador* state) {
	if (sePuedeAplicar(state)) {
		aplica(state);
	}
}
bool Expandir::sePuedeAplicar(Estado_Compilador* stte) {
	//        if (stte->producRef->isTerminal) return false;
	
	return true;
}
void Expandir::aplica(Estado_Compilador* stte) {//, queue<Estado_Compilador*> &chrt){
	size_t sizz = gramarSource->production.size();
	string nn = (stte->producRef->der->at(stte->PosAsterisco)).name;
	for (size_t i = 0; i < sizz; ++i) {
		if (gramarSource->production[i]->nombre.name == nn) {
			Estado_Compilador* EC = new Estado_Compilador;
			EC->producRef = gramarSource->production[i];
			//EC->gramarSource = stte->gramarSource;
			EC->PosAsterisco = 0;
			EC->PosPalabra = 0;
			EC->root = stte;
			chart->push(EC);
		}
	}
}
Expandir::Expandir(Estado_Compilador* state) {
	if (sePuedeAplicar(state)) {
		aplica(state);
	}
}

bool Aceptar::sePuedeAplicar(Estado_Compilador* stte) {
	if (gramarSource->isTerminal(tokensEntrada->at(stte->PosPalabra).name)) {
		Unificar uni;
		
		if (uni.sePuedeAplicar(stte))
			return true;
	}
	return false;
}
void Aceptar::aplica(Estado_Compilador* stte) {
	stte->estadoChart++;
	stte->PosAsterisco++;
}
Aceptar::Aceptar() {
}

bool Unificar::sePuedeAplicar(Estado_Compilador* stte) {
	if (stte->producRef->der->at(stte->PosAsterisco) == tokensEntrada->at(stte->PosPalabra)) {
		return true;
	}
	else
		return false;
}
void Unificar::aplica(Estado_Compilador* stte) {
	vector<string> *vec = stte->producRef->der->at(stte->PosAsterisco).var;
	vector<string> *vec2 = stte->producRef->der->at(stte->PosAsterisco).val;
	for (size_t i = 0; i < vec->size(); ++i) {
		if (stte->context[vec->at(i)] == "") {
			stte->context[vec->at(i)] == stte->context[vec2->at(i)];
		}
	}
}
Unificar::Unificar() {
}
