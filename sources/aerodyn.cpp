#include "aerodyn.hpp"

#include <algorithm>
#include <cstdio>
#include <vector>
#include "constants.hpp"

auto Aerodyn::initAerodynLoad(const std::string& name,
                              const std::vector<int>& vels)
    -> std::map<std::vector<int>, double> {
    // Создание пустой карты для сохранения значений нагрузок
    std::map<std::vector<int>, double> res;
    // Определение, является ли это первым запуском метода
    bool isFirstTimeInizialization = vec_vel.empty();
    // Цикл по скоростям из вектора vels
    for (const int& vel : vels) {
        // Сборка имени файла с соответствующими значениями нагрузок
        char fName[100];  // NOLINT
        sprintf(fName, "aerodyn/%d/%s", vel, name.c_str());
        // Открытие файла
        std::ifstream file(fName);
        // Цикл для чтения значений нагрузок из файла
        while (file) {
            // Чтение высоты
            double h;
            file >> h;
            // Чтение числа углов атаки
            int n;
            file >> n;
            // Цикл для чтения значений нагрузок для каждого угла атаки
            for (int i = 0; i < n; ++i) {
                double alpha;
                double value;
                // Чтение значения угла атаки и соответствующего значения
                // нагрузки
                file >> alpha >> value;
                // Если это первый запуск метода, то заполнение вектора
                // vec_alpha
                if (isFirstTimeInizialization) {
                    vec_alpha.push_back(alpha);
                }
            }
            // Если это первый запуск метода, то заполнение вектора vec_h
            if (isFirstTimeInizialization) {
                vec_h.push_back(h);
            }
        }
        // Если это первый запуск метода, то заполнение вектора vec_vel
        if (isFirstTimeInizialization) {
            vec_vel.push_back(vel);
        }
    }
    // Сортировка и удаление дубликатов значений векторов vec_vel, vec_h и
    // vec_alpha
    std::sort(vec_vel.begin(), vec_vel.end());
    std::sort(vec_h.begin(), vec_h.end());
    std::sort(vec_alpha.begin(), vec_alpha.end());
    vec_alpha.erase(unique(vec_alpha.begin(), vec_alpha.end()),
                    vec_alpha.end());
    vec_h.erase(unique(vec_h.begin(), vec_h.end()), vec_h.end());
    vec_vel.erase(unique(vec_vel.begin(), vec_vel.end()), vec_vel.end());

    // Цикл по скоростям из вектора vels
    for (const int& vel : vels) {
        // Сборка имени файла с соответствующими значениями нагрузок
        char fName[100];  // NOLINT
        sprintf(fName, "aerodyn/%d/%s", vel, name.c_str());
        // Открытие файла
        std::ifstream file(fName);
        // Поиск индекса скорости в векторе vec_vel
        int idxV = 0;
        for (idxV = 0; vec_vel[idxV] < vel; ++idxV) {
            ;
        }
        // Цикл для чтения значений нагрузок из файла
        while (file) {
            // Чтение высоты
            double h;
            file >> h;
            // Чтение числа углов атаки
            int n;
            file >> n;
            // Поиск индекса высоты в векторе vec_h
            int idxH = 0;
            for (idxH = 0; vec_h[idxH] < h; ++idxH) {
                ;
            }
            // Цикл для чтения значений нагрузок для каждого угла атаки
            for (int i = 0; i < n; ++i) {
                double alpha;
                double value;
                // Чтение значения угла атаки и соответствующего значения
                // нагрузки
                file >> alpha >> value;
                // Нахождение индекса угла атаки в векторе vec_alpha
                int idxAlpha = 0;
                for (idxAlpha = 0; vec_alpha[idxAlpha] < alpha; ++idxAlpha) {
                    ;
                }
                // Добавление значения нагрузки в карту res с ключом, состоящим
                // из индексов скорости, высоты и угла атаки
                res[{idxV, idxH, idxAlpha}] = value;
            }
        }
    }
    return res;
}

Aerodyn::Aerodyn() {
    // открываем файл для записи результатов
    output.open("aerodyn.cvs");
    // записываем заголовок таблицы
    output << "t, Fx, Fy, Mz" << std::endl;

    // инициализируем данные для Fx, Fy, и Mz с помощью соответствующих файлов
    Fx = initAerodynLoad("Fx.data", {0, 40, 50, 60, 160});
    Fy = initAerodynLoad("Fy.data", {0, 40, 50, 60, 160});
    Mz = initAerodynLoad("Mz.data", {0, 40, 50, 60, 160});
}

auto Aerodyn::findApproximateValue(
    const std::map<std::vector<int>, double>& data, std::vector<double>& arg)
    -> double {
    // проверяем, что второй элемент вектора arg больше 5.2, и если это не так,
    // то устанавливаем его равным 5.2
    if (arg[1] < 5.2 && arg[1] != 0.0) {
        arg[1] = 5.2;
    }

    // инициализируем переменные для поиска ближайших значений в каждом
    // измерении
    int lowVel;
    int upperVel;
    int lowH;
    int upperH;
    int lowAlpha;
    int upperAlpha;

    // коэффициенты интерполяции для каждого измерения
    double coeffVel = 1;
    double coeffH = 1;
    double coeffAlpha = 1;

    // ищем ближайшее значение скорости в табличных данных
    for (upperVel = 0; arg[0] >= vec_vel[upperVel] &&
                       upperVel < static_cast<int>(vec_vel.size());
         ++upperVel) {
        ;
    }
    lowVel = upperVel - 1;
    if (upperVel >= static_cast<int>(vec_vel.size())) {
        upperVel = static_cast<int>(vec_vel.size()) - 1;
        coeffVel = 0.0;
    } else if (lowVel < 0) {
        lowVel = 0;
    } else {
        coeffVel = (vec_vel[upperVel] - arg[0]) /
                   (vec_vel[upperVel] - vec_vel[lowVel]);
    }

    // ищем ближайшее значение высоты в табличных данных
    for (upperH = 0;
         arg[1] >= vec_h[upperH] && upperH < static_cast<int>(vec_h.size());
         ++upperH) {
        ;
    }
    lowH = upperH - 1;
    if (upperH >= static_cast<int>(vec_h.size())) {
        upperH = static_cast<int>(vec_h.size()) - 1;
        coeffVel = 0.0;
    } else if (lowH < 0) {
        lowH = 0;
    } else {
        coeffH = (vec_h[upperH] - arg[1]) / (vec_h[upperH] - vec_h[lowH]);
    }

    // ищем ближайшее значение угла атаки в табличных данных
    for (upperAlpha = 0; arg[2] >= vec_alpha[upperAlpha] &&
                         upperAlpha < static_cast<int>(vec_alpha.size());
         ++upperAlpha) {
        ;
    }
    lowAlpha = upperAlpha - 1;
    if (upperAlpha >= static_cast<int>(vec_alpha.size())) {
        upperAlpha = static_cast<int>(vec_alpha.size()) - 1;
        coeffAlpha = 0.0;
    } else if (lowAlpha < 0) {
        lowAlpha = 0;
    } else {
        coeffAlpha = (vec_alpha[upperAlpha] - arg[2]) /
                     (vec_alpha[upperAlpha] - vec_alpha[lowAlpha]);
    }

    // интерполируем все значения в каждом измерении
    double LL = coeffAlpha * data.at({lowVel, lowH, lowAlpha}) +
                (1 - coeffAlpha) * data.at({lowVel, lowH, upperAlpha});
    double LU = coeffAlpha * data.at({lowVel, upperH, lowAlpha}) +
                (1 - coeffAlpha) * data.at({lowVel, upperH, upperAlpha});
    double L = coeffH * LL + (1 - coeffH) * LU;
    double UL = coeffAlpha * data.at({upperVel, lowH, lowAlpha}) +
                (1 - coeffAlpha) * data.at({upperVel, lowH, upperAlpha});
    double UU = coeffAlpha * data.at({upperVel, upperH, lowAlpha}) +
                (1 - coeffAlpha) * data.at({upperVel, upperH, upperAlpha});
    double U = coeffH * UL + (1 - coeffH) * UU;

    // интерполируем все значения вдоль третьего измерения и возвращаем
    // результат
    return coeffVel * L + (1 - coeffVel) * U;
}

Aerodyn::~Aerodyn() {
    // закрываем файл для записи результатов
    output.close();
}

auto Aerodyn::getFx(std::vector<double> args) -> double {
    // возвращаем результат для Fx
    return findApproximateValue(Fx, args);
}

auto Aerodyn::getFy(std::vector<double> args, double gap) -> double {
    // если зазор отрицательный, устанавливаем его в 0
    if (gap < 0) {
        // std::cout << "Negative gap " << gap << std::endl;
        gap = 0.0;
    }
    // если зазор меньше 1.5% от диаметра, интерполируем между нулевым зазором и
    // реальным зазором
    if (gap < 0.015 * 1.1) {
        double gapRatio = gap / (0.015 * 1.1);
        std::vector<double> zeroGap = {args[0], 0.0, args[2]};
        double garden = findApproximateValue(Fy, args);
        double zero = findApproximateValue(Fy, zeroGap);
        return garden * gapRatio + zero * (1 - gapRatio);
    }

    // возвращаем результат для Fy
    return findApproximateValue(Fy, args);
}

auto Aerodyn::getMz(std::vector<double> args) -> double {
    // возвращаем результат для Mz
    return findApproximateValue(Mz, args);
}

void Aerodyn::logging(const double& dt) { (void)dt; }