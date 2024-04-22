#include "MyTools.h"

using namespace std;

/// 
/// ツール関数 ここから
/// 

/// 範囲内の値を返す関数
float MyTools::Clamp(const float& num, const float& min, const float& max)
{
	if (num < min)
	{
		return min;
	}
	else if (num > max)
	{
		return max;
	}
	else
	{
		return num;
	}
}

/// 
/// ツール関数 ここまで
/// 

///
/// 2次元ベクトル ここから
///

/// 2次元ベクトルの内積を返す関数
float MyTools::Dot(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

/// 2次元ベクトルのクロス積(外積)を返す関数
float MyTools::Cross(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

/// 2次元ベクトルの長さ(ノルム)を返す関数
float MyTools::Length(float x, float y)
{
	return sqrtf(x * x + y * y);
}

/// 2次元ベクトルの正規化した値を返す関数
Vector2 MyTools::Normalize(float x, float y)
{
	float length = Length(x, y);
	if (length != 0) {
		x = x / length;
		y = y / length;
	}
	return Vector2{ x,y };
}

/// 2次元ベクトルの方向を求める関数
Vector2 MyTools::Direction(float x, float y)
{
	return Normalize(x, y);
}

///
/// 2次元ベクトル ここまで
/// 

///
/// 3次元ベクトル ここから
/// 

/// 3次元ベクトルの加算を返す関数
Vector3 MyTools::Add(const Vector3& v1, const Vector3& v2)
{
	return Vector3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

/// 3次元ベクトルの減算を返す関数
Vector3 MyTools::Subtract(const Vector3& v1, const Vector3& v2)
{
	return Vector3{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

/// 3次元ベクトルのスカラー倍を返す関数
Vector3 MyTools::Multiply(float scalar, const Vector3& v)
{
	return Vector3{ scalar * v.x, scalar * v.y, scalar * v.z };
}

/// 3次元ベクトルの内積を返す関数
float MyTools::Dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/// 3次元ベクトルのクロス積(外積)を返す関数
Vector3 MyTools::Cross(const Vector3& v1, const Vector3& v2)
{
	return Vector3{ v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

/// 3次元ベクトル長さ(ノルム)を返す関数
float MyTools::Length(const Vector3& v)
{
	return sqrtf(Dot(v, v));
}

/// 3次元ベクトルの正規化した値を返す関数
Vector3 MyTools::Normalize(const Vector3& v)
{
	float length = Length(v);
	float x = v.x, y = v.y, z = v.z;
	if (length != 0)
	{
		x /= length;
		y /= length;
		z /= length;
	}

	return Vector3{ x, y, z };
}

/// 
/// 3次元ベクトル ここまで
/// 
