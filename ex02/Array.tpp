#ifndef ARRAY_TPP
# define ARRAY_TPP

/*
** Array.tpp — Array<T> のメンバ関数の定義（実装）。
**
** このファイルは Array.hpp の末尾から #include される。
** 単独で #include して使うものではない（Array.hpp 経由で使う）。
** テンプレートはインスタンス化のために定義がヘッダ側にある必要があるので、
** .cpp ではなくこちらに実装を書いている。
*/

// ---- デフォルトコンストラクタ: 空配列 ----
// 要素数0。_data は NULL にしておき、サイズ0を表す。
template <typename T>
Array<T>::Array(void) : _data(NULL), _size(0)
{
}

// ---- n 要素コンストラクタ ----
// new T[n]() の末尾の () が重要。
//   new T[n]   → 各要素は「デフォルト初期化」だが、組み込み型(int等)は
//                未初期化（値が不定）のまま。
//   new T[n]() → 各要素は「値初期化」され、組み込み型は 0 に、
//                クラス型はデフォルトコンストラクタで初期化される。
// 42 のサブジェクトが求める「デフォルト初期化」はこの () 付きの動作。
// n == 0 のときは new T[0]() でも有効（要素0の有効なポインタが返る）だが、
// ここでは _size を n に合わせて保持する。
template <typename T>
Array<T>::Array(unsigned int n) : _data(new T[n]()), _size(n)
{
}

// ---- コピーコンストラクタ（深いコピー） ----
// other と同じ要素数のメモリを新しく確保し、要素を1つずつコピーする。
// ポインタをそのままコピーする浅いコピーにはしない。
// （浅いコピーだと両者が同じ配列を指し、二重解放やデータ破壊が起きる。）
template <typename T>
Array<T>::Array(Array const& other) : _data(NULL), _size(0)
{
	*this = other; // 実体のコピーは operator= に任せて重複を避ける。
}

// ---- コピー代入演算子（深いコピー） ----
// 手順:
//   1. 自己代入チェック（a = a; のとき何もしない）。
//      これを忘れて自分のメモリを delete してからコピーしようとすると、
//      解放済みメモリを読むことになり危険。
//   2. 古いメモリを解放する（メモリリーク防止）。
//   3. 新しいサイズ分のメモリを確保する。
//   4. 要素を1つずつ深くコピーする。
//   5. *this を返す（a = b = c; のような連鎖代入のため）。
template <typename T>
Array<T>&	Array<T>::operator=(Array const& other)
{
	if (this == &other) // 1. 自己代入チェック
		return (*this);

	delete[] _data;     // 2. 古いメモリを解放

	_size = other._size;
	if (_size > 0)
		_data = new T[_size]; // 3. 新しいメモリを確保
	else
		_data = NULL;

	for (unsigned int i = 0; i < _size; ++i) // 4. 要素ごとに深くコピー
		_data[i] = other._data[i];

	return (*this); // 5. 連鎖代入のため自分自身を返す
}

// ---- デストラクタ ----
// delete[] は NULL に対しても安全（何もしない）。
template <typename T>
Array<T>::~Array(void)
{
	delete[] _data;
}

// ---- operator[]（非const版） ----
// 範囲外なら独自例外を投げる。範囲内なら書き換え可能な参照を返す。
template <typename T>
T&	Array<T>::operator[](unsigned int index)
{
	if (index >= _size)
		throw OutOfBoundsException();
	return (_data[index]);
}

// ---- operator[]（const版） ----
// const な Array からの読み取り専用アクセス。返り値も const参照。
template <typename T>
T const&	Array<T>::operator[](unsigned int index) const
{
	if (index >= _size)
		throw OutOfBoundsException();
	return (_data[index]);
}

// ---- size() ----
// 要素数を返す。const メンバ関数なのでインスタンスを変更しない。
template <typename T>
unsigned int	Array<T>::size(void) const
{
	return (_size);
}

// ---- 例外クラスの what() ----
template <typename T>
const char*	Array<T>::OutOfBoundsException::what(void) const throw()
{
	return ("Array: index out of bounds");
}

#endif
