# ex02 — Array（クラステンプレート＋深いコピー＋例外）

このモジュールの **集大成** です。`std::vector` のような「動的配列」を、
クラステンプレートで自作します。`std::vector` や `<algorithm>` は使えません
（まさにそれを自作するのが目的）。

## 1. この演習でやること

任意の型 `T` の動的配列をラップするクラステンプレート `Array<T>` を実装します。

| 機能 | 内容 |
|------|------|
| デフォルトコンストラクタ | 空配列（要素数0）を作る |
| `Array(unsigned int n)` | n 個の要素を **デフォルト初期化** して持つ配列を作る（`new T[n]()`） |
| コピーコンストラクタ | **深いコピー**（中身まで複製） |
| コピー代入演算子 | **深いコピー**（自己代入チェック＋旧メモリ解放） |
| デストラクタ | 確保したメモリを解放（リークゼロ） |
| `operator[]`（非const版） | 書き換え可能な要素参照を返す。範囲外で例外 |
| `operator[]`（const版） | 読み取り専用の要素参照を返す。範囲外で例外 |
| `size()` | 要素数を返す。引数なし、インスタンスを変更しない（const） |

ルール:

- メモリ確保は **`new[]` 必須**。事前に余分な確保はしない（`_size` と確保数は常に一致）。
- 未確保のメモリにアクセスしない。
- 範囲外アクセス時は `std::exception`（派生の独自例外でOK）を投げる。
- **OCF（直交標準形）** をこのクラステンプレートに適用する。

---

## 2. 登場する新しい概念

### 2-1. クラステンプレート

クラス全体を「型 `T` を後から差し込める設計図」にします。

```cpp
template <typename T>
class Array
{
private:
	T*           _data;   // new[] で確保した要素配列（空なら NULL）
	unsigned int _size;   // 要素数
public:
	// ... コンストラクタ・operator[]・size() など ...
};
```

使うときは `Array<int>` のように `< >` で型を明示します
（関数テンプレートと違い、クラステンプレートは型推論されません）。

```cpp
Array<int>          a(5);   // int を5個持つ配列
Array<std::string>  b(3);   // string を3個持つ配列
```

### 2-2. `new T[n]` と `new T[n]()` の違い（デフォルト初期化）

n 要素コンストラクタの実装で **末尾の `()`** が決定的に重要です。

```cpp
new T[n]    // 各要素は「デフォルト初期化」。組み込み型(int等)は値が不定（未初期化）
new T[n]()  // 各要素は「値初期化」。組み込み型は 0 に、クラス型はデフォルト構築される
```

- `new int[3]`  → 中身は `?, ?, ?`（ゴミ値）。**読むと未定義動作**。
- `new int[3]()` → 中身は `0, 0, 0`。安全。

サブジェクトは「デフォルト初期化された n 要素」を求めているので、**`new T[n]()`** を使います。
本実装の `main` でも、`Array<int> arr(5)` の中身が `0 0 0 0 0` になることを確認しています。

```cpp
template <typename T>
Array<T>::Array(unsigned int n) : _data(new T[n]()), _size(n) {}
```

### 2-3. 浅いコピー（shallow copy）と深いコピー（deep copy）の違い ★最重要

`Array` はポインタ `_data` でヒープメモリを管理しています。
**コピーのとき、ポインタだけをコピーするか、中身まで複製するか** で挙動が変わります。

#### 浅いコピー（shallow copy）= ダメな例

コンパイラが自動生成するコピー（自分で書かない場合）は、メンバを単純にコピーします。
ポインタ `_data` も「アドレスの値」がそのままコピーされるだけです。

```
コピー前:
   original._data ──────► [1][2][3]   (ヒープ上の配列)

「ポインタの値だけ」コピーすると:
   original._data ──────► [1][2][3]
   copy._data    ──────►   ▲ 同じ配列を指してしまう！
```

これだと、

1. `copy[0] = 999` とすると `original[0]` まで `999` に変わる（同じ配列を共有しているため）。
2. `copy` と `original` のデストラクタが **両方とも** 同じ配列を `delete[]` する
   → **二重解放（double free）** でクラッシュ／未定義動作。

#### 深いコピー（deep copy）= 正しい実装

「相手と同じサイズのメモリを **新しく確保** し、要素を1つずつコピーする」。

```
深いコピー:
   original._data ──────► [1][2][3]
   copy._data    ──────► [1][2][3]   (別の、独立した配列)
```

こうすれば、

1. `copy[0] = 999` しても `original` は無傷（別々の配列だから）。
2. それぞれのデストラクタは **自分の** 配列だけを `delete[]` する → 二重解放なし。

本実装ではコピーコンストラクタと代入演算子の両方で深いコピーを行います。

### 2-4. OCF（直交標準形 / Orthodox Canonical Form）

リソース（ヒープメモリ）を持つクラスは、次の4つを必ず定義します。

1. デフォルトコンストラクタ
2. コピーコンストラクタ
3. コピー代入演算子
4. デストラクタ

これは **「3つの法則（Rule of Three）」** とも呼ばれます。
「デストラクタ・コピーコンストラクタ・代入演算子のどれか1つを自分で書く必要があるなら、
残り2つも書け」という経験則です。`Array` は `delete[]` を持つので、コピー系も
自前で（深いコピーとして）書かないと、浅いコピーの事故が起きます。

### 2-5. `operator[]` の const オーバーロード

`operator[]` を **2つ** 用意します。const修飾の有無でオーバーロードします。

```cpp
T&        operator[](unsigned int index);        // 非const版（書き換え用）
T const&  operator[](unsigned int index) const;  // const版（読み取り用）
```

- 非const の `Array` には **非const版** が呼ばれ、`arr[i] = x;` と書き換えられます。
- const の `Array`（`const Array<int> a;`）には **const版** が呼ばれ、読むだけ。
  const版は戻り値も `T const&` なので、`const` 配列の要素を誤って書き換える事故を防げます。

末尾の `const`（メンバ関数の後ろ）は「このメンバ関数は **オブジェクトを変更しない**」
という宣言です。これがあるおかげで const なオブジェクトからでも呼べます。

### 2-6. 例外（exception）による範囲外アクセスの通知

範囲外の添字でアクセスされたら、`std::exception` 派生の独自例外を投げます。

```cpp
class OutOfBoundsException : public std::exception
{
public:
	virtual const char* what() const throw();   // 説明文字列を返す
};
```

- `std::exception` を継承し、`what()` をオーバーライドします。
- `throw()`（関数の後ろ）は C++98 の **例外仕様** で「この関数は例外を投げない」
  という意味。`std::exception::what()` のシグネチャに合わせる必要があります。
- 投げる側: `throw OutOfBoundsException();`
- 受ける側: `catch (std::exception const& e) { std::cout << e.what(); }`
  → 基底クラスの参照で受けられるので、`std::exception` で一括catchできます。

---

## 3. ファイル構成と各ファイルの役割

```
ex02/
├── Makefile      ビルド設定
├── main.cpp      テスト（空配列/n要素/深いコピー/範囲外例外/size/string/連鎖代入）
├── Array.hpp     Array<T> の【宣言】＋ 末尾で Array.tpp を include
├── Array.tpp     Array<T> のメンバ関数の【定義（実装）】
└── README.md     このファイル
```

### なぜ `.hpp` と `.tpp` に分けるのか

- テンプレートはインスタンス化のため **定義がヘッダ側に見えている必要** があります。
  そのため実装を `.cpp` に置けません。
- そのまま `Array.hpp` に宣言と実装を全部書くと長くて読みにくいので、
  **実装だけを `Array.tpp`** に切り出し、`Array.hpp` の末尾で `#include "Array.tpp"`
  しています。
- `.tpp` は **コンパイラから見ればただのテキスト**。`#include` は中身を貼り付けるだけ
  なので、最終的に「定義がヘッダ経由で見える」状態は保たれます。あくまで可読性のための分割です。

```cpp
// Array.hpp の末尾
#include "Array.tpp"
```

---

## 4. ビルド・実行方法

```bash
make            # ./array を生成
./array

# 直接コンパイルする場合（課題の必須条件）
c++ -Wall -Wextra -Werror -std=c++98 *.cpp -o array
./array
```

### 期待される出力（抜粋）

```
=== 1. empty array (default constructor) ===
size() = 0
accessing empty[0]...
caught: Array: index out of bounds

=== 2. n-element array (default-initialized to 0) ===
size() = 5
default-initialized values: 0 0 0 0 0
after assignment:           0 10 20 30 40

=== 3. deep copy via copy constructor ===
original: 1 2 3
copy:     1 2 3
after 'copy[0] = 999':
  original[0] = 1 (should stay 1)
  copy[0]     = 999 (should be 999)

=== 4. deep copy via assignment operator ===
b.size() after 'b = a' = 3
b: 10 20 30
after 'b[1] = 555':
  a[1] = 20 (should stay 20)
  b[1] = 555 (should be 555)
self-assignment 'a = a' ok, a[0] = 10

=== 5. out-of-bounds access throws ===
accessing arr[5] (size 2)...
caught: Array: index out of bounds
accessing arr[-1]...
caught: Array: index out of bounds
```

「深いコピーの証明」が眼目です。`copy`/`b` を変更しても `original`/`a` が変わらない
ことを出力で確認しています。

> メモリリークについて: macOS の `leaks` で確認済み（`0 leaks for 0 total leaked bytes`）。
> Linux なら `valgrind --leak-check=full ./array` でリークゼロになります。

---

## 5. コードの詳細解説（特に念入りに）

### 5-1. デフォルトコンストラクタ（空配列）

```cpp
template <typename T>
Array<T>::Array(void) : _data(NULL), _size(0) {}
```

要素数0。`_data` は `NULL` にしておきます。`delete[] NULL;` は何もしない安全な操作
なので、空配列のデストラクタも問題ありません。

### 5-2. n 要素コンストラクタ

```cpp
template <typename T>
Array<T>::Array(unsigned int n) : _data(new T[n]()), _size(n) {}
```

`new T[n]()` の `()` で全要素を **値初期化**（int等は0）します（→ 2-2参照）。

### 5-3. コピーコンストラクタ（深いコピー）

```cpp
template <typename T>
Array<T>::Array(Array const& other) : _data(NULL), _size(0)
{
	*this = other;   // 実体のコピーは operator= に委譲（コード重複を避ける）
}
```

- まず `_data(NULL), _size(0)` で安全な初期状態にしておきます。
  これは `operator=` が冒頭で `delete[] _data;` を呼ぶため、`_data` が未初期化の
  ゴミポインタだと不正な delete になるからです。NULLにしておけば安全に delete できます。
- 中身のコピーは `operator=` に任せます（DRY: 同じ深いコピー処理を二重に書かない）。

### 5-4. コピー代入演算子（深いコピー）★最重要

```cpp
template <typename T>
Array<T>& Array<T>::operator=(Array const& other)
{
	if (this == &other)        // 1. 自己代入チェック
		return (*this);

	delete[] _data;            // 2. 古いメモリを解放（リーク防止）

	_size = other._size;
	if (_size > 0)
		_data = new T[_size];  // 3. 新しいメモリを確保
	else
		_data = NULL;

	for (unsigned int i = 0; i < _size; ++i)   // 4. 要素を1つずつ深くコピー
		_data[i] = other._data[i];

	return (*this);            // 5. 連鎖代入のため自分自身を返す
}
```

5つの手順を順に説明します。

1. **自己代入チェック `if (this == &other)`**
   `a = a;`（同じオブジェクトへの代入）のとき。これを忘れて先に進むと、手順2で
   **自分のメモリを `delete[]` してから** そのメモリをコピー元として読もうとし、
   解放済みメモリを参照する（use-after-free）危険があります。`this`（自分のアドレス）と
   `&other`（相手のアドレス）が同じなら、何もせず自分を返して終了します。

2. **古いメモリを解放 `delete[] _data;`**
   代入先 `*this` が既に持っていた配列を解放します。これを忘れると、上書きで
   ポインタが差し替わり、元の配列が辿れなくなって **メモリリーク** します。
   `delete[] NULL` は安全なので、空配列でも問題ありません。

3. **新しいメモリを確保 `new T[_size]`**
   相手と同じ要素数のメモリを **新規に** 取ります。これにより相手とは独立した配列になります
   （深いコピーの肝）。`_size == 0` のときは確保せず `NULL` にして、無駄な確保を避けます。

4. **要素を1つずつコピー**
   `_data[i] = other._data[i];` で値を複製します。`T` が `std::string` のような
   クラスでも、各要素の `operator=` が呼ばれて正しく深くコピーされます。

5. **`return (*this);`**
   `c = b = a;` のような **連鎖代入** を成立させるため、自分自身への参照を返します。

### 5-5. デストラクタ

```cpp
template <typename T>
Array<T>::~Array(void) { delete[] _data; }
```

`new[]` で確保したものは必ず `delete[]`（角括弧つき）で解放します。
`new[]` と `delete`（角括弧なし）を取り違えると未定義動作になるので注意。
`_data` が `NULL`（空配列）でも `delete[] NULL;` は安全です。

### 5-6. operator[] の境界チェック ★最重要

```cpp
template <typename T>
T& Array<T>::operator[](unsigned int index)
{
	if (index >= _size)              // 境界チェック
		throw OutOfBoundsException();
	return (_data[index]);
}
```

- `index >= _size` で範囲外を検出します。`index` は `unsigned int` なので
  **負の値を渡しても巨大な正の値に変換** され、結局 `>= _size` に引っかかって
  例外になります（`arr[-1]` も安全に弾けます）。
- 範囲内なら `_data[index]` への参照を返し、`arr[i] = x;` のように書き換えられます。
- const版も同じ境界チェックを行い、`T const&` を返します（読み取り専用）。

```cpp
template <typename T>
T const& Array<T>::operator[](unsigned int index) const
{
	if (index >= _size)
		throw OutOfBoundsException();
	return (_data[index]);
}
```

### 5-7. size()

```cpp
template <typename T>
unsigned int Array<T>::size(void) const { return (_size); }
```

要素数を返すだけ。**const メンバ関数** なので、const な `Array` からも呼べ、
オブジェクトを変更しないことが保証されます。

---

## 6. つまずきポイント / 評価で訊かれること（Q&A）

**Q. 浅いコピーだと何が起きますか？**
A. 2つの `Array` が **同じヒープ配列を共有** してしまいます。結果、(1) 片方を変更すると
もう片方も変わる、(2) 両方のデストラクタが同じ配列を `delete[]` して **二重解放**
（クラッシュ／未定義動作）になります。だから深いコピーが必須です。

**Q. `new T[n]` と `new T[n]()` の違いは？**
A. `new T[n]` は組み込み型を **未初期化（ゴミ値）** のまま確保します。
`new T[n]()` は **値初期化** し、int等は `0`、クラス型はデフォルト構築されます。
本演習は「デフォルト初期化された n 要素」が要件なので `new T[n]()` を使います。

**Q. 代入演算子の自己代入チェック（`if (this == &other)`）はなぜ必要？**
A. `a = a;` のとき、先に `delete[] _data;` してしまうと、コピー元（=自分）の配列が
解放済みになり、それを読みに行って **解放済みメモリ参照（use-after-free）** を起こす
危険があります。自己代入なら何もせず返すことで安全にします。

**Q. 代入演算子で古いメモリを解放しないとどうなりますか？**
A. 代入先が既に持っていた配列のポインタが上書きされ、辿れなくなって **メモリリーク**
します。手順2の `delete[] _data;` でこれを防ぎます。

**Q. なぜ `operator[]` を const版と非const版の2つ用意するのですか？**
A. const な `Array`（`const Array<int> a;`）からは const版が呼ばれ、読み取り専用に
なります。非const版は書き換え可能な参照（`T&`）を返し、`a[i] = x;` を可能にします。
const版がないと、const オブジェクトの要素にまったくアクセスできません。

**Q. なぜ範囲外で「例外」を投げるのですか？戻り値ではダメ？**
A. `operator[]` は要素の参照を返す演算子で、「失敗を表す特別な戻り値」を持てません。
そこで C++ では **例外** でエラーを通知します。`std::exception` を継承した独自例外を投げ、
呼び出し側が `catch (std::exception const&)` で受け取れるようにしています。

**Q. なぜテンプレートの実装を `.cpp` に分けず `.tpp`（ヘッダ側）に置くのですか？**
A. テンプレートは使われた型ごとにインスタンス化されます。`main.cpp` のコンパイル時に
`Array<int>` の各メンバ関数の **定義が見えている必要** があるため、定義を `.cpp` に
隠せません。`.tpp` はヘッダから include され、実質的にヘッダの一部として展開されます
（可読性のために宣言と実装を分けているだけ）。

**Q. `throw()` が `what()` に付いているのはなぜ？**
A. C++98 の例外仕様で「この関数は例外を投げない」という意味です。基底クラス
`std::exception::what()` のシグネチャが `const char* what() const throw();` なので、
オーバーライドするにはこのシグネチャに合わせる必要があります。

**Q. 事前に余分なメモリを確保していないことはどう保証されますか？**
A. `_size` と「実際に確保した要素数」が常に一致するように実装しています。
`new T[_size]` で必要数ちょうどを確保し、容量（capacity）と要素数を別管理する
ような仕組み（`std::vector` のような余分確保）はしていません。
