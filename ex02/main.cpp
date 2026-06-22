#include <iostream>
#include <string>
#include "Array.hpp"

int main(void)
{
	std::cout << "=== 1. empty array (default constructor) ===" << std::endl;
	{
		Array<int>	empty;
		std::cout << "size() = " << empty.size() << std::endl; // 0
		try
		{
			std::cout << "accessing empty[0]..." << std::endl;
			empty[0] = 42; // 範囲外 → 例外
		}
		catch (std::exception const& e)
		{
			std::cout << "caught: " << e.what() << std::endl;
		}
	}

	std::cout << "\n=== 2. n-element array (default-initialized to 0) ===" << std::endl;
	{
		Array<int>	arr(5);
		std::cout << "size() = " << arr.size() << std::endl; // 5
		std::cout << "default-initialized values: ";
		for (unsigned int i = 0; i < arr.size(); ++i)
			std::cout << arr[i] << " "; // すべて 0 のはず
		std::cout << std::endl;

		for (unsigned int i = 0; i < arr.size(); ++i)
			arr[i] = static_cast<int>(i) * 10;
		std::cout << "after assignment:           ";
		for (unsigned int i = 0; i < arr.size(); ++i)
			std::cout << arr[i] << " "; // 0 10 20 30 40
		std::cout << std::endl;
	}

	std::cout << "\n=== 3. deep copy via copy constructor ===" << std::endl;
	{
		Array<int>	original(3);
		original[0] = 1;
		original[1] = 2;
		original[2] = 3;

		Array<int>	copy(original); // コピーコンストラクタ

		std::cout << "original: " << original[0] << " " << original[1] << " " << original[2] << std::endl;
		std::cout << "copy:     " << copy[0] << " " << copy[1] << " " << copy[2] << std::endl;

		// copy を変更しても original に影響しないことを確認（深いコピーの証明）。
		copy[0] = 999;
		std::cout << "after 'copy[0] = 999':" << std::endl;
		std::cout << "  original[0] = " << original[0] << " (should stay 1)" << std::endl;
		std::cout << "  copy[0]     = " << copy[0] << " (should be 999)" << std::endl;
	}

	std::cout << "\n=== 4. deep copy via assignment operator ===" << std::endl;
	{
		Array<int>	a(3);
		a[0] = 10;
		a[1] = 20;
		a[2] = 30;

		Array<int>	b;       // 空配列
		b = a;               // 代入演算子（中で旧メモリ解放 + 深いコピー）
		std::cout << "b.size() after 'b = a' = " << b.size() << std::endl; // 3
		std::cout << "b: " << b[0] << " " << b[1] << " " << b[2] << std::endl;

		// b を変更しても a に影響しないこと。
		b[1] = 555;
		std::cout << "after 'b[1] = 555':" << std::endl;
		std::cout << "  a[1] = " << a[1] << " (should stay 20)" << std::endl;
		std::cout << "  b[1] = " << b[1] << " (should be 555)" << std::endl;

		// 自己代入チェックの確認（クラッシュしないこと）。
		// 参照を経由して、コンパイラの self-assign 警告を避けつつ
		// 実行時に「同じオブジェクトへの代入」を行う。
		Array<int>&	aRef = a;
		a = aRef;
		std::cout << "self-assignment 'a = a' ok, a[0] = " << a[0] << std::endl;
	}

	std::cout << "\n=== 5. out-of-bounds access throws ===" << std::endl;
	{
		Array<int>	arr(2);
		try
		{
			std::cout << "accessing arr[5] (size 2)..." << std::endl;
			std::cout << arr[5] << std::endl;
		}
		catch (std::exception const& e)
		{
			std::cout << "caught: " << e.what() << std::endl;
		}

		// 負のインデックスは unsigned に変換され巨大な値になる → 範囲外。
		try
		{
			std::cout << "accessing arr[-1]..." << std::endl;
			std::cout << arr[-1] << std::endl;
		}
		catch (std::exception const& e)
		{
			std::cout << "caught: " << e.what() << std::endl;
		}
	}

	std::cout << "\n=== 6. works with other types (std::string) ===" << std::endl;
	{
		Array<std::string>	words(3);
		words[0] = "template";
		words[1] = "deep";
		words[2] = "copy";
		std::cout << "size() = " << words.size() << std::endl;
		for (unsigned int i = 0; i < words.size(); ++i)
			std::cout << "  words[" << i << "] = " << words[i] << std::endl;

		// const Array の const operator[] を使う。
		Array<std::string> const	cwords(words);
		std::cout << "const access cwords[1] = " << cwords[1] << std::endl;
	}

	std::cout << "\n=== 7. chained assignment ===" << std::endl;
	{
		Array<int>	a(1), b, c;
		a[0] = 7;
		c = b = a; // 連鎖代入。operator= が *this を返すので成立。
		std::cout << "a[0]=" << a[0] << " b[0]=" << b[0] << " c[0]=" << c[0] << std::endl;
	}

	std::cout << "\nAll tests done." << std::endl;
	return (0);
}
