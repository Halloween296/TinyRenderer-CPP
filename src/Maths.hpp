#pragma once
#include <cmath>
#include <iostream>
//2
struct Vec2 {
	float x, y;
	Vec2() : x(0), y(0) {}
	Vec2(float x, float y) :x(x), y(y) {}

	Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
	Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
	Vec2 operator*(float f)const { return Vec2(x * f, y * f); }

};
//3
struct Vec3 {
	float x, y, z;
	Vec3() :x(0), y(0), z(0) {}
	Vec3(float x,float y,float z):x(x),y(y),z(z){}

	Vec3 operator+(const Vec3& v) const{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	Vec3 operator-(const Vec3& v)const {
		return Vec3(x - v.x, y - v.y, z - v.z);
	}

	Vec3 operator/(const Vec3& v)const {
		return Vec3(x / v.x, y / v.y, z / v.z);
    }
	Vec3 operator/(float f)const {
		return Vec3(x / f, y / f, z / f);
	}

	Vec3 operator*(const Vec3& v)const { 
		return Vec3(x * v.x, y * v.y, z * v.z); 
	}
	Vec3 operator*(float f)const {
		return Vec3(x * f, y * f, z * f);
	}


	float Dot(const Vec3& v)const {
		return x * v.x + y * v.y + z * v.z;
	}
	Vec3 Cross(const Vec3& v)const {
		return Vec3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}
	static Vec3 Lerp(Vec3 a, Vec3 b, float t) {
		return a + (b - a) * t;
	}
	//归一
	Vec3 Normalized() const {
		float len = std::sqrt(x * x + y * y + z * z);
		return Vec3(x / len, y / len, z / len);
	}
	static Vec3 Clamp(Vec3 v,float minVal,float maxVal) {
		Vec3 res = Vec3(
			std::max(minVal, std::min(maxVal, v.x)),
			std::max(minVal, std::min(maxVal, v.y)),
			std::max(minVal, std::min(maxVal, v.z))
			);
		return res;
	}
	static Vec3 Pow(Vec3 v,float p) {
		return Vec3(std::pow(v.x, p), std::pow(v.y, p), std::pow(v.z, p));
	}
};

struct Vec4 {
	float x, y, z, w;
	Vec4() : x(0), y(0), z(0), w(0) {} // 默认构造
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vec3 xyz() const { return Vec3(x, y, z); }
};


//旋转矩阵qici
struct Mat4 {
	float m[4][4];
	Mat4() {
		//初始化
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = (i == j ? 1.0f : 0.0f);
			}
		}
	}
	Mat4 operator*(const Mat4& b)const {
		Mat4 res;
		for (int i = 0; i < 4;i++) {
			for (int j = 0; j < 4;j++) {
				res.m[i][j] = 0;
				for (int k = 0; k <4 ;k++) {
					res.m[i][j] += m[i][k] * b.m[k][j];
				}
			}
		}
		return res;
	}
	Vec4 operator*(const Vec4& v) const {
		float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
		float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
		float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
		float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
		return { x, y, z, w };
	}

	Vec3 operator*(const Vec3& v) const {
		Vec4 v4(v.x, v.y, v.z, 1.0f);
		Vec4 res = (*this) * v4;
		return Vec3(res.x, res.y, res.z);
	}

	static Mat4 Scale(float s) {
		Mat4 res;
		res.m[0][0] = s;
		res.m[1][1] = s;
		res.m[2][2] = s;
		return res;
	}
	static Mat4 Translate(float tx, float ty, float tz) {
		Mat4 res;
		res.m[0][3] = tx;
		res.m[1][3] = ty;
		res.m[2][3] = tz;
		return res;
	}
	//z
	static Mat4 RotateZ(float a) {
		Mat4 res;
		float c = std::cos(a);
		float s = std::sin(a);
		res.m[0][0] = c;
		res.m[1][0] = s;
		res.m[0][1] = -s;
		res.m[1][1] = c;
		return res;
	}
	//y
	static Mat4 RotateY(float a) {
		Mat4 res;
		float c = std::cos(a);
		float s = std::sin(a);
		res.m[0][0] = c;
		res.m[2][0] = -s;
		res.m[0][2] = s;
		res.m[2][2] = c;
		return res;
	}
	//透视
	static Mat4 Perspective(float fov,float aspect,float zn,float zf) {
		Mat4 res;
		//csh
		for (int i = 0; i < 4; i++)for (int j = 0; j < 4; j++)res.m[i][j] = 0;
		//opengl透视矩阵
		float tn = std::tan(fov / 2.0f);
		res.m[0][0] = 1.0f / (aspect * tn);
		res.m[1][1] = 1.0f / tn;
		res.m[2][2] = -(zf + zn) / (zf - zn);
		res.m[2][3] = -(2.0f * zf * zn) / (zf - zn);
		res.m[3][2] = -1.0f;
		res.m[3][3] = 0.0f;
		return res;
	}
	static Mat4 LookAt(Vec3 eye, Vec3 target, Vec3 up) {
		Vec3 zaxis = (eye - target).Normalized();
		Vec3 xaxis = up.Cross(zaxis).Normalized();
		Vec3 yaxis = zaxis.Cross(xaxis).Normalized();

		Mat4 res;
		//逆矩阵/旋转
		res.m[0][0] = xaxis.x;
		res.m[0][1] = xaxis.y;
		res.m[0][2] = xaxis.z;

		res.m[1][0] = yaxis.x;
		res.m[1][1] = yaxis.y;
		res.m[1][2] = yaxis.z;

		res.m[2][0] = zaxis.x;
		res.m[2][1] = zaxis.y;
		res.m[2][2] = zaxis.z;

		//平移
		res.m[0][3] = -xaxis.Dot(eye);
		res.m[1][3] = -yaxis.Dot(eye);
		res.m[2][3] = -zaxis.Dot(eye);
		res.m[3][3] = 1.0f;
		return res;
	}
};