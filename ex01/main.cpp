#include <iostream>
#include <string>
#include "iter.hpp"

/*
** iter に渡す「各要素に適用する関数」をいくつか用意する。
**
** ・表示する関数（要素を変更しない → const参照で受けられる）
** ・要素を変更する関数（非const参照で受ける）
** ・それ自体が関数テンプレートのもの（インスタンス化済みでも渡せる）
*/

// (1) 表示する関数（テンプレート）。要素を書き換えないので const& で受ける。
template <typename T>
void	printElem(T const& x)
{
	std::cout << x << " ";
}

// (2) 要素を変更する関数（テンプレート）。非const& で受け、値を倍にする。
template <typename T>
void	doubleElem(T& x)
{
	x = x + x; // 数値なら2倍、文字列なら連結
}

// (3) 通常の（非テンプレート）関数も渡せることの確認。int 専用に +1 する。
void	incrementInt(int& x)
{
	x += 1;
}

// (4) 関数オブジェクト（ファンクタ）も「呼べるもの」なので渡せる。
//     operator() を持つクラス。状態（カウンタ）を持てるのが関数ポインタとの違い。
class PrintWithIndex
{
private:
	int	_i;

public:
	PrintWithIndex(void) : _i(0) {}

	template <typename T>
	void	operator()(T const& x)
	{
		std::cout << "[" << _i << "]=" << x << " ";
		++_i;
	}
};

template <typename T>
void	printArray(T* arr, std::size_t len)
{
	iter(arr, len, printElem<T>);
	std::cout << std::endl;
}

int main(void)
{
	std::cout << "=== int array (non-const): print, then double ===" << std::endl;
	int		ints[] = {1, 2, 3, 4, 5};
	std::size_t	intLen = sizeof(ints) / sizeof(ints[0]);

	printArray(ints, intLen);                // 1 2 3 4 5
	iter(ints, intLen, doubleElem<int>);     // 各要素を2倍に変更
	printArray(ints, intLen);                // 2 4 6 8 10
	iter(ints, intLen, incrementInt);        // 非テンプレート関数で +1
	printArray(ints, intLen);                // 3 5 7 9 11

	std::cout << "\n=== const int array: print only (no modification) ===" << std::endl;
	const int	cints[] = {10, 20, 30};
	std::size_t	cLen = sizeof(cints) / sizeof(cints[0]);
	// const 配列 → T が const int に推論される。printElem は const& 受けなのでOK。
	iter(cints, cLen, printElem<const int>);
	std::cout << std::endl;                  // 10 20 30

	std::cout << "\n=== std::string array: print, then double (concatenate) ===" << std::endl;
	std::string	strs[] = {"Hello", "42", "Tokyo"};
	std::size_t	sLen = sizeof(strs) / sizeof(strs[0]);
	iter(strs, sLen, printElem<std::string>);     // Hello 42 Tokyo
	std::cout << std::endl;
	iter(strs, sLen, doubleElem<std::string>);    // 文字列を連結
	iter(strs, sLen, printElem<std::string>);     // HelloHello 4242 TokyoTokyo
	std::cout << std::endl;

	std::cout << "\n=== char array ===" << std::endl;
	char	chars[] = {'a', 'b', 'c', 'd'};
	std::size_t	chLen = sizeof(chars) / sizeof(chars[0]);
	iter(chars, chLen, printElem<char>);          // a b c d
	std::cout << std::endl;

	std::cout << "\n=== function object (functor) with internal state ===" << std::endl;
	double	dbls[] = {1.1, 2.2, 3.3};
	std::size_t	dLen = sizeof(dbls) / sizeof(dbls[0]);
	iter(dbls, dLen, PrintWithIndex());           // [0]=1.1 [1]=2.2 [2]=3.3
	std::cout << std::endl;

	return (0);
}
