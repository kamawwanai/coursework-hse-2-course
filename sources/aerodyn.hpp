#pragma once

#include <fstream>
#include <map>
#include <set>
#include <vector>

class Aerodyn {
   public:
    Aerodyn();
    ~Aerodyn();
    // Метод для получения значения силы тяги Fx
    auto getFx(std::vector<double>) -> double;
    // Метод для получения значения боковой силы Fy
    auto getFy(std::vector<double>, double) -> double;
    // Метод для получения значения момента Mz
    auto getMz(std::vector<double>) -> double;

    void logging(const double& dt);

   private:
    std::ofstream output;
    // Площадь крыла
    double wingArea = 1.0;
    // Длина хорды
    double chordLength = 1.25;
    // Карта для хранения значений силы тяги Fx
    std::map<std::vector<int>, double> Fx;
    // Карта для хранения значений боковой силы Fy
    std::map<std::vector<int>, double> Fy;
    // Карта для хранения значений момента Mz
    std::map<std::vector<int>, double> Mz;
    // Вектор для хранения значений скорости
    std::vector<double> vec_vel;
    // Вектор для хранения значений высоты
    std::vector<double> vec_h;
    // Вектор для хранения значений угла атаки
    std::vector<double> vec_alpha;
    // Метод для поиска приближенного значения в карте
    auto findApproximateValue(const std::map<std::vector<int>, double>& data,
                              std::vector<double>& arg) -> double;
    // Метод для инициализации карты значениями аэродинамических нагрузок
    auto initAerodynLoad(const std::string& name, const std::vector<int>& vels)
        -> std::map<std::vector<int>, double>;
    // Базовое значение оборотов двигателя
    int nBase = 29000;
};