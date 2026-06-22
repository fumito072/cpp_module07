#ifndef ARRAY_HPP
# define ARRAY_HPP

# include <cstddef>   // std::size_t
# include <exception> // std::exception

/*
** Array.hpp
**
** クラステンプレート Array<T>。任意の型 T の動的配列をラップする。
**
** 設計の要点:
**   - 内部に T* _data（new[] で確保したヒープ領域）と
**     unsigned int _size（要素数）を持つ。
**   - メモリ確保は new[] のみ。事前に余分には確保しない
**     （_size と確保した数は常に一致する）。
**   - OCF（直交標準形 / Orthodox Canonical Form）を実装する:
**       デフォルトコンストラクタ / コピーコンストラクタ /
**       コピー代入演算子 / デストラクタ。
**   - コピーは「深いコピー（deep copy）」。ポインタだけをコピーする
**     「浅いコピー（shallow copy）」だと、2つの Array が同じメモリを
**     共有してしまい、片方の delete[] でもう片方が壊れる/二重解放になる。
**   - operator[] は範囲外で例外を投げる（const版・非const版の両方）。
**
** クラステンプレートはインスタンス化時に T が決まって初めてコードが
** 生成される。そのためメンバ関数の定義もヘッダ側に置く必要がある。
** ここでは「宣言は Array.hpp に」「定義（実装）は Array.tpp に」分けて、
** Array.hpp の末尾で Array.tpp を #include する構成にしている。
** （.tpp は "template implementation" の意味で使われる慣習的な拡張子。
**   コンパイラにとっては普通のヘッダと同じ。読みやすさのための分割。）
*/

template <typename T>
class Array
{
public:
	/* ---- OCF（直交標準形） ---- */

	// デフォルトコンストラクタ: 要素数0の空配列を作る。
	Array(void);

	// n 個の要素を「デフォルト初期化」して持つ配列を作る。
	// new T[n]() の () により、組み込み型(int 等)も 0 に初期化される。
	Array(unsigned int n);

	// コピーコンストラクタ: 深いコピー。
	Array(Array const& other);

	// コピー代入演算子: 深いコピー（自己代入チェックと旧メモリ解放を行う）。
	Array&	operator=(Array const& other);

	// デストラクタ: 確保したメモリを解放する。
	~Array(void);

	/* ---- 要素アクセス ---- */

	// 非const版: 書き換え可能な参照を返す。範囲外で例外を投げる。
	T&			operator[](unsigned int index);

	// const版: const な Array に対して読み取り専用アクセスを提供する。
	T const&	operator[](unsigned int index) const;

	/* ---- 情報取得 ---- */

	// 要素数を返す。引数なし、インスタンスを変更しない(const)。
	unsigned int	size(void) const;

	/* ---- 例外クラス ---- */

	// 範囲外アクセス時に投げる、std::exception 派生の独自例外。
	class OutOfBoundsException : public std::exception
	{
	public:
		// what() は例外の説明文字列を返す。throw() は「例外を投げない」指定
		// （C++98 の例外仕様）。std::exception::what() のシグネチャに合わせる。
		virtual const char*	what(void) const throw();
	};

private:
	T*				_data; // new[] で確保した要素の配列。空配列なら NULL。
	unsigned int	_size; // 要素数。
};

// テンプレートの定義（実装）を取り込む。
# include "Array.tpp"

#endif
