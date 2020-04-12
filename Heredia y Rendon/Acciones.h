#pragma once
#include "Grammar.h"

class Estado_Compilador {
public:
	map<string, string> context;
	int estadoChart = 0;
	Produccion* producRef;
	int PosAsterisco = 0;
	int PosPalabra = 0;
	Estado_Compilador* root;
	Estado_Compilador() {}
	Estado_Compilador(int s_cht, int ast_pos, int pal_pos, Produccion* ptr_p, Estado_Compilador* ptr_e);
	~Estado_Compilador() {}
};

class Accion {
public:
	vector<Token> *tokensEntrada;
	queue<Estado_Compilador*> *chart;
	Gramatica* gramarSource;
	virtual bool sePuedeAplicar(Estado_Compilador* estado) = 0;
	virtual void aplica(Estado_Compilador* estado) = 0;//, queue<Estado_Compilador*> &chart) = 0;
};

class Dummy : public Accion {
public:
	bool sePuedeAplicar(Estado_Compilador* stte);
	void aplica(Estado_Compilador* stte);// {//, queue<Estado_Compilador*> &chrt){
	Dummy(Estado_Compilador* state);
	~Dummy() {}
};

class Expandir : public Accion {
public:
	bool sePuedeAplicar(Estado_Compilador* stte);
	void aplica(Estado_Compilador* stte);// {//, queue<Estado_Compilador*> &chrt){
	Expandir(Estado_Compilador* state);
	~Expandir() {}
};

class Aceptar : public Accion {
public:
	bool sePuedeAplicar(Estado_Compilador* stte);
	void aplica(Estado_Compilador* stte);
	Aceptar();
	~Aceptar() {}
};

class Unificar : public Accion {
public:
	bool sePuedeAplicar(Estado_Compilador* stte);
	void aplica(Estado_Compilador* stte);
	Unificar();
	~Unificar() {}
};

