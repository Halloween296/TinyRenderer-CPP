#pragma once
#include "Maths.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
struct Vertex {
	Vec3 pos;
	Vec2 uv;
};
struct Tri {
	Vertex v0, v1, v2;

};
class Mesh {
public:
	std::vector<Tri> tri;

	//obj
	bool LoadObj(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Error open" << filename << std::endl;
			return false;
		}
		std::vector<Vec2> tempUV;
		std::vector<Vec3> tempPos;
		
		std::string line;

		while (std::getline(file, line)) {
			std::stringstream ss(line);
			std::string pre;
			ss >> pre;
			//顶点
			if(pre == "v") {
				Vec3 v;
				ss >> v.x >> v.y >> v.z;
				tempPos.push_back(v);
			}
			else if (pre == "vt") {
				Vec2 uv;
				ss >> uv.x >> uv.y;
				tempUV.push_back(uv);
			}
			//面
			else if(pre == "f") {
				int vid[3];
				int tid[3];
				for (int i = 0; i < 3; i++) {
					std::string se;
					ss >> se;
					size_t sPos = se.find('/');
					size_t s2 = se.find('/',sPos + 1);

					vid[i] = std::stoi(se.substr(0,sPos));
					if (sPos != std::string::npos && s2 != std::string::npos) {
						if (s2 > sPos + 1) {
							std::string uvStr = se.substr(sPos + 1, s2 - sPos - 1);
							tid[i] = std::stoi(uvStr);
						}
						else {
							tid[i] = 0;
						}
					}
					else {
						tid[i] = 0;
					}					
				}
			
				////三角形index-1
				Tri t;
				t.v0.pos = tempPos[vid[0] - 1];
				t.v1.pos = tempPos[vid[1] - 1];
				t.v2.pos = tempPos[vid[2] - 1];
				
				for (int i = 0; i < 3; i++) {
					Vec2 uv = Vec2(0, 0);

					if (tid[i] != 0 && tid[i] <= tempUV.size()) {
						uv = tempUV[tid[i] - 1];
						uv.y = 1.0f - uv.y;
					}
					else {
						uv = Vec2(0, 0);
					}
					if (i == 0) t.v0.uv = uv;
					else if (i == 1) t.v1.uv = uv;
					else if (i == 2) t.v2.uv = uv;
			     }

				tri.push_back(t);
			}
		}
		std::cout << "mesh:" << tri.size() << "个三角形" << std::endl;
		return true;
	}
};