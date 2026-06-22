#ifndef ITER_HPP
# define ITER_HPP

# include <cstddef> // std::size_t

/*
** iter.hpp
**
** 関数テンプレート iter を定義する。
**
** iter は次の3つを受け取る:
**   1. 配列の先頭アドレス（ポインタ）
**   2. 配列の長さ（要素数。const の値）
**   3. 各要素に対して呼び出す「関数」
**
** ポイントは「const 配列でも 非const 配列でも動く」こと。
** これを実現するために、テンプレート引数 T を 2つ用意する:
**   - T : 配列要素の型（int, char, const int ... などに推論される）
**   - F : 関数（関数ポインタ／関数オブジェクト）の型
**
** こうすると、
**   int arr[]       を渡せば T = int       に推論され
**   const int arr[] を渡せば T = const int に推論される。
** つまり「const か 非const か」を T 自身が吸収するので、
** const 版・非const 版を別々に書かなくても 1つのテンプレートで両対応できる。
**
** （別解として const T* を受け取る版を追加する書き方もあるが、
**   T を const にも推論させるこの方法が最もシンプルで汎用的。）
*/

/*
** length は const std::size_t で受ける（PDF の「const値」要件）。
** size_t は配列サイズを表すのに適した符号なし整数型。
*/
template <typename T, typename F>
void iter(T* array, std::size_t const length, F func)
{
	for (std::size_t i = 0; i < length; ++i)
		func(array[i]);
}

#endif
