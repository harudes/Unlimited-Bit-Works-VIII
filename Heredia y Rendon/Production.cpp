#include "pch.h"
#include "Production.h"

void DeteleFromString(string& s, char d) {
	string result;
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == d)continue;
		result += s[i];
	}
	s = result;
}
template<class T>
void printMatrix(vector<vector<T>>& mat) {
	for (size_t i = 0; i < mat.size(); ++i) {
		for (size_t j = 0; j < mat[i].size(); ++j) {
			cout << mat[i][j] << '\t';
		}
		cout << endl;
	}
}

Token::Token(string nn) {

}

void Token::print() {

}
