#include <iostream>
#include <string>
#include "whatever.hpp"

/*
** ::swap / ::min / ::max の "::" について
**
** "::" を関数名の前に付けると「グローバル名前空間の」関数を明示的に
** 指す。標準ライブラリにも std::swap / std::min / std::max が存在し、
** <string> など他のヘッダを include すると、引数依存名前探索(ADL)などで
** std 版が候補に入って曖昧（ambiguous）になることがある。
** "::swap(a, b)" と書くことで「自分が定義したグローバルの swap を呼ぶ」
** とコンパイラに伝え、衝突を確実に避けられる。
*/

int main(void)
{
	// --- PDF 指定のテスト（int） ---
	int a = 2;
	int b = 3;

	::swap(a, b);
	std::cout << "a = " << a << ", b = " << b << std::endl; // a = 3, b = 2
	std::cout << "min( a, b ) = " << ::min(a, b) << std::endl; // min = 2
	std::cout << "max( a, b ) = " << ::max(a, b) << std::endl; // max = 3

	// --- PDF 指定のテスト（std::string） ---
	std::string c = "chaine1";
	std::string d = "chaine2";

	::swap(c, d);
	std::cout << "c = " << c << ", d = " << d << std::endl; // c = chaine2, d = chaine1
	std::cout << "min( c, d ) = " << ::min(c, d) << std::endl; // chaine1
	std::cout << "max( c, d ) = " << ::max(c, d) << std::endl; // chaine2

	// --- 追加テスト：等しいときは「2番目」を返す仕様の確認 ---
	std::cout << "\n--- equal values (should return the 2nd arg) ---" << std::endl;
	int x = 5;
	int y = 5;
	// 値は同じだが、アドレスで「どちらが返ったか」を確認する。
	std::cout << "min returns &y (2nd)? "
			  << (&::min(x, y) == &y ? "yes" : "no") << std::endl;
	std::cout << "max returns &y (2nd)? "
			  << (&::max(x, y) == &y ? "yes" : "no") << std::endl;

	// --- 追加テスト：他の型（double, char） ---
	std::cout << "\n--- other types ---" << std::endl;
	double p = 3.14;
	double q = 2.72;
	::swap(p, q);
	std::cout << "p = " << p << ", q = " << q << std::endl; // p = 2.72, q = 3.14
	std::cout << "min( p, q ) = " << ::min(p, q) << std::endl; // 2.72
	std::cout << "max( p, q ) = " << ::max(p, q) << std::endl; // 3.14

	char e = 'z';
	char f = 'a';
	::swap(e, f);
	std::cout << "e = " << e << ", f = " << f << std::endl; // e = a, f = z
	std::cout << "min( e, f ) = " << ::min(e, f) << std::endl; // a
	std::cout << "max( e, f ) = " << ::max(e, f) << std::endl; // z

	return (0);
}
