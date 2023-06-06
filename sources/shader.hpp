#pragma once

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// получить все строки из файла
auto get_file_contents(const std::string& file) -> std::string;

class Shader
{
public:
	GLuint ID;
	// строим шейдер из вершинного и фрагментных шейдеров
	Shader(const char* vertexShader, const char* fragmentShader);

	// используем
	void use();
	// удаление
	void remove();
};