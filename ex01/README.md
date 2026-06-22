# ex01 — iter（配列と「関数を渡す」）

## 1. この演習でやること

**配列の全要素に、渡された関数を順番に適用する** 関数テンプレート `iter` を実装します。

`iter` は次の3つの引数を取り、戻り値はありません。

| 引数 | 内容 |
|------|------|
| 第1引数 | 配列の **先頭アドレス**（ポインタ） |
| 第2引数 | 配列の **長さ**（要素数。const の値） |
| 第3引数 | 各要素に対して呼び出す **関数** |

要件:

- **どんな型の配列でも動く**（`int`, `char`, `std::string`, `double` ...）。
- 第3引数の関数は、それ自体が **インスタンス化済みの関数テンプレート** でもよい。
- **const配列でも非const配列でも動く**こと。
  関数は引数を const参照でも非const参照でも取りうる。

イメージ:

```cpp
int arr[] = {1, 2, 3};
iter(arr, 3, printElem<int>);   // -> 1 2 3 を表示する
iter(arr, 3, doubleElem<int>);  // -> 各要素を2倍に書き換える
```

---

## 2. 登場する新しい概念

### 2-1. 配列を関数に渡す ＝ 「先頭ポインタ＋長さ」

C++（C由来）では、配列を関数に渡すと **先頭要素へのポインタに減衰（decay）** します。
このとき配列の「長さ」の情報は失われます。だから長さを **別の引数** で渡します。

```cpp
template <typename T, typename F>
void iter(T* array, std::size_t const length, F func)
{
	for (std::size_t i = 0; i < length; ++i)
		func(array[i]);    // i 番目の要素に func を適用
}
```

- `T* array` … 配列の先頭ポインタ（`array[i]` で i 番目の要素にアクセス）。
- `std::size_t const length` … 要素数。`size_t` はサイズを表す符号なし整数型で、
  配列の長さに適しています。`const` を付けて「中で書き換えない」ことを明示します。
- `F func` … 各要素に適用する「呼べるもの」。

呼び出し側で長さを求めるときは、固定長配列なら次のイディオムが使えます。

```cpp
int arr[] = {1, 2, 3, 4, 5};
std::size_t len = sizeof(arr) / sizeof(arr[0]);  // = 5
```

### 2-2. 関数ポインタ・関数オブジェクトを「テンプレート引数」にする

第3引数 `F func` の型 `F` もテンプレートパラメータにしている点がミソです。
こうすると、`func` には **「呼べるもの（callable）」なら何でも** 渡せます。

渡せるものの例:

1. **通常の関数**（関数ポインタとして渡る）
   ```cpp
   void incrementInt(int& x) { x += 1; }
   iter(arr, len, incrementInt);
   ```

2. **インスタンス化済みの関数テンプレート**
   ```cpp
   template <typename T> void printElem(T const& x) { std::cout << x << " "; }
   iter(arr, len, printElem<int>);   // printElem<int> という具体化された関数を渡す
   ```

3. **関数オブジェクト（ファンクタ / functor）**
   `operator()` を持つクラスのインスタンス。関数ポインタと違い、**状態を持てる** のが強み。
   ```cpp
   class PrintWithIndex {
       int _i;
   public:
       PrintWithIndex() : _i(0) {}
       template <typename T> void operator()(T const& x) {
           std::cout << "[" << _i << "]=" << x << " ";
           ++_i;                 // 呼ばれるたびにカウンタが進む（状態を持つ）
       }
   };
   iter(arr, len, PrintWithIndex());   // [0]=.. [1]=.. [2]=..
   ```

`func(array[i])` という呼び出しは、関数ポインタでもファンクタでも **同じ書き方** で
動きます（どちらも `( )` で呼べるため）。これがテンプレートの柔軟さです。

### 2-3. const配列・非const配列の両対応（この演習の核心）

ここが一番の学びどころです。

「const配列」と「非const配列」の両方で動かすために、**要素型 `T` 自体を
const にも推論させる** 設計にします。`T*` という1つの形だけで両対応できます。

```cpp
int arr[];          iter(arr, ...)   → T = int       に推論（T* = int*）
const int carr[];   iter(carr, ...)  → T = const int に推論（T* = const int*）
```

- 非const配列を渡すと `T = int` になり、`func` は要素を **書き換えられます**
  （`doubleElem(int&)` のように非const参照で受ける関数もOK）。
- const配列を渡すと `T = const int` になり、要素は **読み取り専用**。
  この場合 `func` は const参照（`printElem(const int&)`）で受ける必要があります。
  もし非const参照で受ける関数を渡そうとすると、コンパイルエラーになり
  「const のものを書き換えようとしている」と気づけます（安全）。

```cpp
const int cints[] = {10, 20, 30};
iter(cints, 3, printElem<const int>);   // OK（読むだけ）
// iter(cints, 3, doubleElem<const int>); // これはコンパイルエラー（書き換え不可）→正しい挙動
```

> ポイント: 別解として「`const T*` を受け取るオーバーロードを追加する」方法もありますが、
> `T*` 一本で `T` を const にも推論させるこの方法が最もシンプルで汎用的です。

---

## 3. ファイル構成と各ファイルの役割

```
ex01/
├── Makefile     ビルド設定
├── main.cpp     テスト（int/const int/string/char/double、表示・変更・ファンクタ）
├── iter.hpp     iter の【定義】（テンプレートなのでヘッダに書く）
└── README.md    このファイル
```

### なぜ `iter.hpp` に実装を書くのか

ex00 と同じ理由です。`iter` はテンプレートなので、`main.cpp` で使われた型ごとに
インスタンス化されます。コンパイル時に定義が見えている必要があるため、
ヘッダに定義を書いて include します。

---

## 4. ビルド・実行方法

```bash
make            # ./iter を生成
./iter

# 直接コンパイルする場合（課題の必須条件）
c++ -Wall -Wextra -Werror -std=c++98 *.cpp -o iter
./iter
```

### 期待される出力

```
=== int array (non-const): print, then double ===
1 2 3 4 5
2 4 6 8 10
3 5 7 9 11

=== const int array: print only (no modification) ===
10 20 30

=== std::string array: print, then double (concatenate) ===
Hello 42 Tokyo
HelloHello 4242 TokyoTokyo

=== char array ===
a b c d

=== function object (functor) with internal state ===
[0]=1.1 [1]=2.2 [2]=3.3
```

- 非const `int` 配列: 表示 → 2倍に変更 → +1 に変更、と書き換えが効いています。
- const `int` 配列: 表示のみ（書き換えないので const でもOK）。
- `std::string`: `doubleElem` で `s = s + s`（文字列連結）になります。
- ファンクタ `PrintWithIndex`: 内部カウンタで `[0] [1] [2]` と添字が増えます。

---

## 5. コードの詳細解説

### iter 本体

```cpp
template <typename T, typename F>
void iter(T* array, std::size_t const length, F func)
{
	for (std::size_t i = 0; i < length; ++i)
		func(array[i]);
}
```

- テンプレートパラメータは2つ: `T`（要素の型）と `F`（呼べるものの型）。
- `array[i]` は `*(array + i)` と同じで、i 番目の要素そのものを表します。
  これを `func` に渡すので、`func` が非const参照で受ければ要素を書き換えられ、
  const参照で受ければ読むだけになります。
- ループ変数 `i` も `std::size_t` にして、`length` との比較で符号付き／符号なしの
  警告（`-Wsign-compare`）が出ないようにしています。

### テストで使う関数たち（main.cpp）

```cpp
template <typename T> void printElem(T const& x)  { std::cout << x << " "; }     // 表示（変更しない）
template <typename T> void doubleElem(T& x)       { x = x + x; }                 // 変更（2倍/連結）
void                       incrementInt(int& x)   { x += 1; }                    // 非テンプレート関数も渡せる
```

- `printElem` は const参照で受けるので **const配列にも非const配列にも** 使えます。
- `doubleElem` は非const参照で受けるので **非const配列専用**（要素を書き換えるため）。
- `incrementInt` は通常関数。テンプレートでなくても `iter` に渡せることの確認です。

---

## 6. つまずきポイント / 評価で訊かれること（Q&A）

**Q. なぜ配列の長さを別の引数で渡すのですか？ 配列自体に長さは無いの？**
A. 関数に配列を渡すと先頭ポインタに減衰し、長さの情報が失われます。
そのため長さを別途渡す必要があります。`sizeof(arr)/sizeof(arr[0])` で長さを
取れるのは、配列がポインタに減衰する **前**（同じスコープ内）だけです。

**Q. const配列でも動くようにする工夫は？**
A. 要素型 `T` 自体を const にも推論させています。`const int arr[]` を渡すと
`T = const int` となり、`T*` は `const int*` になります。これで const 配列でも
そのまま動きます。書き換えようとする関数を渡せばコンパイルエラーで弾かれるので安全です。

**Q. 第3引数の型もテンプレートにする理由は？**
A. 関数ポインタ・インスタンス化済みテンプレート関数・関数オブジェクト（ファンクタ）の
どれでも受けられるようにするためです。型を `F` にしておけば、`func(array[i])` という
共通の呼び出しでまとめて扱えます。

**Q. 関数ポインタとファンクタの違いは？**
A. 関数ポインタは「ただの関数」を指すだけで状態を持てません。ファンクタは
`operator()` を持つクラスのインスタンスなので、メンバ変数として **状態（カウンタなど）**
を持てます（`PrintWithIndex` の `_i` がその例）。

**Q. `printElem<int>` のように `<int>` を明示しているのはなぜ？**
A. 関数テンプレートを「値（関数）として渡す」ときは、どの型版を渡すのかを確定させる
必要があるため、明示的にインスタンス化（`printElem<int>`）して渡しています。
