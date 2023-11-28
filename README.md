# Модем относительной фазовой модуляции
Модем представляет собой [статическую библиотеку](https://radioprog.ru/post/1084?ysclid=lmd3gg87qb497647051), в которой реализованы
[модулятор и демодулятор](https://zvondozvon.ru/radiosvyaz/modulyator-demodulyator?ysclid=lpgsgktzqn951514591) сигналов
с [относительной фазовой модуляцией](https://vunivere.ru/work63579/page2) (ОФМ) для позиционностей, являющихся степенью двойки.
Сигнальное созвездие заполняется в соответствии с [кодом Грея](https://zvondozvon.ru/radiosvyaz/kod-greya?ysclid=lpgoc3vcky544727201).

![Gray-4-8-PSK](https://raw.githubusercontent.com/Seredenko-V/cpp-DPSK-modem/main/images/Gray-4-8-PSK.jpg?token=GHSAT0AAAAAACG5T7PNIRJWWXH4EY5EUB4OZLFZHDQ "Gray-4-8-PSK")

* Решена проблема дублирования кода, связанная с реализацией каждой конкретной позиционности ОФМ.
* Решена проблема некратности несущей частоты ($`Fc`$) частоте дискретизации ($`Fs`$).

Последняя проблема заключалась в том, что в одном периоде элементарного сигнала (символа) укладывалось нецелое количество отсчетов,
определяемое как $`Fs/Fc`$. Для решения использованы:

1. на передающей стороне - сигнал, [сопряженный по Гильберту](https://leonidov.su/ru/hilbert-transform-lection-notes/) к информационному,
$`Fc`$ которого определяется автоматически как ближайшее к заданной несущей частоте значение, делящее без остатка $`Fs`$;
2. на приёмной стороне - [синфазно-квадратурная](https://radioprog.ru/post/415?ysclid=lpgrirfca323143642) демодуляция и матрица 
[декорреляции](https://en.wikipedia.org/wiki/Decorrelation).

Таким образом, модулированный символ может содержать нецелое число периодов гармонического колебания.
> [!IMPORTANT]
> Главным требованием к работе модема является кратность частоты дискретизации символьной скорости.

  В дальнейшем эта проблема также может быть решена путём "накопления"
символов на приёмной стороне до тех пор, пока имеющееся количество отсчетов не станет кратно символьной скорости. После чего демодуляция
будет осуществляться поблочно, работая с расширенным алфавитом.


## Параметры сигнала
Базовым классом к модулятору и демодулятору является `SignalParameters`, который содержит необходимые методы установления и получения значений
параметров, нужных пользователю.

### Поля класса
Все поля являются `protected` для взаимодействия с ними в классах-наследниках.

1. `uint16_t positionality_` - позиционность ОФМ;
2. `double amplitude_` - амплитуда колебания в Вольтах. По умолчанию устанавливается равной единице;
3. `double phase_` - текущее значение фазы в радианах;
4. `double phase_shift_` - значение дополннительного фазового сдвига в радианах;
5. `uint32_t symbol_speed_` - символьная скорость, определяется как количество, символов передаваемое за секунду [символов/с];
6. `uint32_t carrier_frequency_` - несущая частота в Герцах;
7. `double carrier_cyclic_frequency_` - циклическая несущая частота в радианах;
8. `uint32_t sampling_frequency_` - частота дискретизации в Герцах;
9. `double time_step_between_samples_` - шаг дискретизации во временной области в секундах.

### Методы класса
Все методы класса являются `public` и имеют константную сложность, т.е. выполняются за $`О(1)`$ по времени и по памяти.

1. `SignalParameters` - конструктор, принимающий значения частоты дискретизации и символьной скорости. Для этих двух параметров не существует
сеттеров, поэтому в случае необходимости их изменения требуется создание нового объекта;
2. `SetPositionality` - установить позиционность модуляции. Является виртуальным и переопределяется в модуляторе и демодуляторе в соответствии
со спецификой этих классов;
3. `GetPositionality` - получить текущее количество позиций фаз;
4. `SetAmplitude` - установить значение амплитуды сигнала;
5. `GetAmplitude` - получить значение амплитуды сигнала;
6. `SetPhase` - установить текущее значение фазы. Позволяет сдвинуть сигнальное созвездие на заданный в **радианах** угол. Значение со знаком
"+" - сдвиг по против часовой стрелки, со знаком "-" - по часовой стрелке;
7. `GetPhase` - получить текущее значение фазы;
8. `SetCarrierFrequency` - установить значение несущей частоты. Является виртуальным и переопределяется только в демодуляторе;
9. `GetCarrierFrequency` - получить значение несущей частоты;
10. `GetSamplingFrequency` - получить значение частоты дискретизации;
11. `GetSymbolSpeed`- получить значение символьной скорости;
12. `SetPhaseShift` - установить значение дополнительного фазового сдвига в **радианах**. Является виртуальным и переопределяется в модуляторе и
демодуляторе в соответствии со спецификой этих классов;
13. `GetPhaseShift` - получить значение дополнительного фазового сдвига в **радианах**.


## Модулятор
Публично наследуется от класса `SignalParameters`. Главный функционал реализован в методе `Modulation`, который принимает на вход последовательность
бит в формате `const std::vector<bool>&`, надлежащую модуляции и флаг `PresencePivotSymbol`, который позволяет учесть наличие/отсутствие опорного
символа в этой последовательности. Внутри данного метода автоматически определяется схема модуляции:
1. Классическая, когда $`Fs`$ кратна $`Fc`$;
2. С использованием промежуточной частоты, когда $`Fs`$ **не** кратна $`Fc`$.

  То есть реализован структурный паттерн проектирования "[Фасад](https://refactoring.guru/ru/design-patterns/facade)". Пользователю не нужно
принимать участия в выборе нужной схемы модуляции. Ему достаточно указать необходимые параметры сигнала до начала сеанса связи. Если количество
бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки.

  Сложность данного метода $`O(N)`$ - если опорный символ содержится в последовательности бит и $`O(2*N)`$ - если опорный символ **не** 
содержится в последовательности бит и его нужно добавить в начало.

  Метод возвращает последовательность отсчетов `std::vector<double>`. Также существует перегрузка данного метода с указанием требуемой
позиционности. Перегрузка содержит в себе обработку установления нового значения позиционности с последующим вызовом перегрузки, принимающей
последовательность бит и флаг о наличии/отсутствии опорного символа.

### Поля класса
1. `uint32_t intermediate_frequency_` - промежуточная частота в Герцах;
2. `double intermediate_cyclic_frequency_` - циклическая промежуточная частота в радианах;
3. `std::map<uint16_t, double>  phase_shifts_` - словарь, в котором ключи - символы, а значения - соответствующие им фазовые сдвиги в **радианах**;
4. `std::function<double(double)> mod_function_` - модулирующая функция. По умолчанию устанавливается $`sin`$;
5. `std::function<double(double)> ortogonal_mod_function_` - функция, ортогональная модулирующей. По умолчанию устанавливается $`cos`$.

### Остальные методы класса
#### public
1. `DPSKModulator` - конструктор, принимающий значения частоты дискретизации, символьной скорости и позиционность. Для первых двух параметров не
существует методов их установления, поэтому в случае необходимости их изменения требуется создание нового объекта. По умолчанию позиционность
задаётся равной 2. Сложность: $`O(2 * positionality * log2(positionality))`$;
2. `SetPositionality` - установить позиционность модуляции. Переопределяется в данном классе т.к. после изменения позиционности ОФМ необходимо
заполнить словарь `phase_shifts_` актуальными значениями. Сложность: $`O(2 * positionality * log2(positionality))`$;
3. `SetModulationFunction` - установить модулирующую и ортогональную ей функции: $`cos`$ или $`sin`$. Сложность: $`O(1)`$;
4. `GetPhaseShifts` - получить константную ссылку на словарь символов с соответствующими сдвигами по фазе `phase_shifts_`. Сложность: $`O(1)`$;
5. `SetPhaseShift` - установить значение дополнительного фазового сдвига в **радианах**. Переопределяется в данном классе т.к. после изменения
дополнительного сдвига по фазе требуется заполнить словарь `phase_shifts_` актуальными значениями. Сложность: $`O(positionality)`$;
6. `SetIntermediateFrequency` - установить значение промежуточной частоты в случае $`Fs`$ некратности $`Fc`$. Данный метод является опциональным,
если пользователю необходимо установить собственное значение промежуточной частоты, которое заведомо обеспечивает целочисленное деление частоты
дискретизации (без остатка). В противном случае будет брошено исключение `std::invalid_argument`. Сложность: $`O(1)`. По умолчанию выбор
промежуточной частоты  осуществляется автоматически, при помощи функции `math::FindNearestMultiple`;
7. `GetIntermediateFrequency` - получить значение промежуточной частоты. Сложность: $`O(1)`$;
8. `ModulationOneSymbol` - модуляция одного символа. На данный момент используется исключительно для отладки или прочих проверок.
Сложность: $`O(Fs/Fc)`$. Не используется в методе `Modulation` т.к. модуляция одного символа для описаных схем модуляции довольно различна.
Тем не менее, метод `ModulationOneSymbol` может быть универсиализирован путём внесения дополнительных проверок. Поскольку речь идет о модуляции
одного символа, данная проверка будет осуществляться для каждого модулируемого символа, что не является оптимальным решением с точки зрения
производительности.

#### private
1. `FillPhaseShifts` - заполнить словарь `phase_shifts_` со значениями сдвигов фаз. Используется генератор кодов Грея, реализованный в виде функции
`gray_code::MakeGrayCodes`. Сложность: $`O(positionality)`$;
2. `ClassicalModulation` - модуляция без изпользования промежуточной частоты. Частота дискретизации кратна несущей частоте. Сложность: $`O(N)`$;
3. `ModulationWithUseIntermediateFreq` - модуляция с изпользованием промежуточной частоты. Частота дискретизации **не** кратна несущей частоте.
Сложность: $`O(N)`$.


## Демодулятор
Публично наследуется от класса `SignalParameters`. Класс использует стороннюю библиотеку для работы с матрицами. Её
описание можно найти в файле `matrix.h` или [по ссылке](http://matrix.drque.net/).
> [!IMPORTANT]
> Класс не решает задачу установления тактовой и цикловой синхронизации. На вход демодулятора
должен поступать сигнал, для которого установлена тактовая синхронизация.

  Главный функционал реализован в методе `Demodulation`, который принимает на вход последовательность
отсчетов в формате `const std::vector<double>&` и возвращает последовательность символов в 10-й системе счисления в виде `std::vector<uint32_t>`.
Сложность метода - $`O(N)`$.

  Аналогично модулятору, здесь реализован структурный паттерн проектирования "[Фасад](https://refactoring.guru/ru/design-patterns/facade)", поскольку
в методе автоматически определяется схема демодуляции:
1. Классическая - путем извлечения синфазной и квадратурной составляющих, когда $`Fs`$ кратна $`Fc`$;
2. С применением процедуры декорреляции полученных значений синфазной и квадратурной составляющих, а также с синтезом "эталонных" значений этих
составляющих, когда $`Fs`$ **не** кратна $`Fc`$. Эти действия осуществляются для демодуляции каждого символа.

### Поля класса
1. `std::vector<double> cos_oscillation_` - синфазная составляющая. Гармоническое колебание $`cos(2*\pi*Fc*\Delta t)`$;
2. `std::vector<double> sin_oscillation_` - квадратурная составляющая. Гармоническое колебание $`sin(2*\pi*Fc*\Delta t)`$;

    Количество отсчетов (длительность) синфазной и квадратурной составляющей определяется в соответствии с частотой дискретизации и символьной
скоростью как $`Fs/\upsilon`$. В случае, когда $`Fs`$ **не** кратна $`Fc`$ укладывается не целое число периодов колебания.

3. `std::vector<double> bounds_symbols_` - границы диапазонов разностей фаз между символами;
4. `std::vector<uint32_t> symbols_sequence_on_circle_` - полследовательность символов на окружности в соответствии с кодом Грея;
5. `Matrix<double> decorrelation_matrix_` - матрица декорреляции.

### Остальные методы класса
#### public
1. `DPSKDemodulator` - конструктор, принимающий значения частоты дискретизации, символьной скорости и позиционность. Для первых двух параметров не
существует методов их установления, поэтому в случае необходимости их изменения требуется создание нового объекта. По умолчанию позиционность
задаётся равной 2. Сложность: $`O(2 * positionality * log2(positionality))`$;
2. `SetPositionality` - установить позиционность модуляции. Переопределяется в данном классе т.к. после изменения позиционности ОФМ необходимо
заполнить последовательности `bounds_symbols_` и `symbols_sequence_on_circle_` актуальными значениями.
Сложность: $`O(2 * positionality * log2(positionality))`$;
3. `SetCarrierFrequency` - установить значение несущей частоты. Переопределяется в данном классе поскольку необходимо сгенерировать актульные
значения синфазной `cos_oscillation_` и квадратурной `sin_oscillation_` составляющих. Также в случае некратности $`Fs`$ $`Fc`$ следует создать
актуальную матрицу декорреляции. Сложность: $`O(Fs/Fc)`$;
4. `ExtractInPhaseAndQuadratureComponentsSymbol` - извлечь синфазную ($`I`$) и квадратурную ($`Q`$) составляющие символа (элементарного сиганала).
Составляющие определяются как скалярное произведение отсчетов демодулируемого символа с отсчетами "эталонных" составляющих с последующей
нормировкой $`2/N`$. На вход подается набор отсчетов демодулируемого символа в формате `const std::vector<double>&`, на выходе - $`I`$ и $`Q`$
составляющие в формате `std::complex<double>`. Действительная часть - $`I`$, мнимая - $`Q`$. Сложность: $`O(N)`$;
5. `ExtractPhaseValue` - извлечь значение фазы (угла) из $`I`$ и $`Q`$ составляющих элементарного сигнала в Евклидовом пространстве при помощи
[std::atan2](https://en.cppreference.com/w/cpp/numeric/math/atan2) в формате `double`. Сложность: $`O(1)`$;
6. `GetBoundsSymbols` - получить значения границ диапазонов разностей фаз между символами в формате `const std::vector<double>&`. Сложность: $`O(1)`$;
7. `GetSymbolsSequenceOnCircle` - получить последовательность символов на окружности в соответствии с кодом Грея в формате `const std::vector<double>&`.
Сложность: $`O(1)`$;
8. `DefineSymbol` - определить символ на основании значения разности фаз между символами. Релизован линейный поиск, т.к. бинарный вернет итератор и
чтобы получить индекс для обращения к вектору символов `bounds_symbols_` нужно будет использовать [std::distance](https://en.cppreference.com/w/cpp/iterator/distance).
Это сделает сложность $`O(N*N*log2(N))`$, что является менее эффективным решением. Сложность: $`O(positionality)`$;
9. `SetPhaseShift` - установить значение дополнительного фазового сдвига в **радианах**. Переопределяется в данном классе т.к. после изменения
дополнительного сдвига по фазе требуется заполнить последовательности `bounds_symbols_` и `symbols_sequence_on_circle_` актуальными значениями.
Сложность: $`O(positionality)`$;
10. `GetDecorrelationMatrix` - получить матрицу декорреляции в формате `const Matrix<double>&`. Сложность: $`O(1)`$.

#### private
1. `FillCosAndSinOscillation` - синфазную `cos_oscillation_` и квадратурную `sin_oscillation_` составляющие при заданных параметрах. Если
$`Fs`$ **не** кратна $`Fc`$ - укладывается не целое число периодов колебания. Сложность: $`O(Fs/Fc)`$;
2. `FillSymbolsBounds` - заполнить границы (сектора) символов `bounds_symbols_` на огружности. Сложность: $`O(positionality)`$;
3. `FillSymbolsSequenceOnCircle` - заполнить последовательность символов на окружности `symbols_sequence_on_circle_` в диапазоне $`[0; 2*\pi)`$
в соответсвии с кодом Грея. Сложность: $`O(2 * positionality * log2(positionality))`$;
4. `CreateDecorrelationMatrix` - построить матрицу декореляции для минимизации паразитного отклика $`I`$ и $`Q`$ компонент. Сложность: $`O(Fs/Fc)`$.
Матрица является обратной к матрице корреляции. Матрица корреляции для одночастотного сигнала всегда имеет размерность 2x2:
```
  cos(2*M_PI*Fc*delta_t) * cos(2*M_PI*Fc*delta_t)       cos(2*M_PI*Fc*delta_t) * sin(2*M_PI*Fc*delta_t)

  sin(2*M_PI*Fc*delta_t) * cos(2*M_PI*Fc*delta_t)       sin(2*M_PI*Fc*delta_t) * sin(2*M_PI*Fc*delta_t)
```

5. `Decorrelation` - уменьшение влияния паразитного отклика $`I`$ и $`Q`$ компонент, принимаемых на вход в формате `std::complex<double>`. Возвращает
декоррелированные компоненты в формате `std::complex<double>`. Процедура декорреляции заключается в умножении матрицы декорреляции на вектор, состоящий
из $`I`$ и $`Q`$ компонент демодулируемого символа. Сложность: $`O(N^2)`$.

> [!IMPORTANT]
> В случае необходимости изменения значений частоты дискретизации или символьной скорости - необходимо создать новые объекты модулятора и демодулятора.

## Математические операции
Вспомогательные математические операции вынесены в пространство имён `math`. Имеются следующие функции:
1. `IsPowerOfTwo` - [является ли число степенью двойки](https://blushing-medicine-fbd.notion.site/2-235264c8a23044af89eeeb17abc12bc5?pvs=4).
Сложность: $`O(1)`$;
2. `ExtractNumBitsFormValue` - извлечь количество бит, занимаемых числом. Возвращает количество бит числа в 2-й системе счисления.
Сложность: $`O(1)`$;
3. `ConvertationBinToDec` - перевод из 2-ой в 10-ю систему счисления (СС). Принимает константную ссылку на последовательность бит `std::vector<bool>`
и возвращает число в 10-й системе счисления. Сложность: $`O(N)`$;
4. `ConvertationBitsToDecValues` - перевод последовательности бит в последовательность символов 10-й СС. В качестве параметров принимает костантную
ссылку на последовательность бит и количество бит в одном символе. Если размер последовательности бит не кратен количеству бит в одном символе, то
к последовательности дописываются нули слева. Сложность: $`O(N)`$;
5. `IsSameDouble` - проверка на равенство двух `double` с заданной точностью. По умолчанию точность 1e-6. Сложность: $`O(1)`$;
6. `IsSameContainersWithDouble` - проверка на равенство двух контейнеров с `double`. Сложность: $`O(N)`$;
7. `DegreesToRadians` - перевод углов в радианы. Сложность: $`O(1)`$;
8. `PhaseToRangeFrom0To2PI` - перенос значения фазы в пределы $`[0; 2*\pi)`$. Сложность: $`O(phase/(2*\pi))`$;
9. `FindNearestMultiple` - принимает 2 целых числа `value` и `multiple` и осуществляет поиск ближайшего к `value` числа, которое делит без остатка
`multiple`. Сложность: $`O(N)`$.

---

## Системные требования
* GCC 6.3.0
* C++ 14
* CMake 3.5

## Установка и подключение к проекту
Пример подключения библиотеки к проекту можно найти в `user-DPSK-modem/CMakeLists.txt`, где
* `add_subdirectory(../DPSK-modem build-DPSK-modem)` - подключение подкаталога с названием `DPSK-modem`, в котором имеется собственный скрипт
сборки (CMakeLists.txt). Файлы сборки этого подкаталога будут располагаться в директории `build-DPSK-modem`. Такое подключение позволяет взаимодействовать
с исходными файлами.
* `target_include_directories(UserDPSKModem PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../DPSK-modem")` - указание директории с заголовочными файлами библиотеки;
* `target_link_libraries(UserDPSKModem DPSKModem ${SYSTEM_LIBS})` - указание зависимости проекта `UserDPSKModem` от библиотеки `DPSKModem`.

Если возникает потребность в подключении библиотеки без возможности её редактирования, следует выполнить сборку статической библиотеки и её подключение к
проекту.

### Сборка статической библиотеки
1. Находясь в директории, в которой хранится каталог `DPSK-modem` нужно создать директорию с именем `DPSKModemLib` и перейти в неё
```
mkdir DPSKModemLib
cd DPSKModemLib/
```

2. Внутри `DPSKModemLib` необходимо создать 2 директории: `include` и `lib`
3. В директории `include` нужно разместить все заголовочные файлы из каталога `DPSK-modem`
4. В директории `lib` нужно создать две директории `Debug` и `Release`, в которых будут храниться собранные статические библиотеки для Debug и Release сборок.
5. Перейти в каталог `DPSK-modem`
6. Создать в этом каталоге директорию `build`, в которой появятся файлы сборки и перейти в неё
```
mkdir build
cd build/
```

7. Выполнить сборку `Debug` версии библиотеки
```
cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```
На Windows лучше использовать компилятор MinGW. При этом может потребоваться явное указание генератора `-G "MinGW Makefiles"`. Тогда комманды для
сборки будут следующими:
```
cmake ../ -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"
cmake --build .
```

8. Получившийся файл библиотеки `libDPSKModem.a` необходимо переместить в директорию `DPSKModemLib/lib/Debug`. Если в Windows был выбран компилятор MSVC,
файл должен иметь расширение `.lib`.
9. Повторить пункты 7 и 8, за тем исключением флага сборки `-DCMAKE_BUILD_TYPE=Release` и каталогом, в который будет перемещена статическая библиотека.
Каталог размещения - `DPSKModemLib/lib/Release`

Таким образом должна быть получена статическая библиотека со следующей структурой
```
DPSKModemLib
├── include
│   ├── dpsk_demodulator.h
│   ├── dpsk_modulator.h
│   ├── gray_code.h
│   ├── math_operations.h
│   ├── matrix.h
│   └── signal_parameters.h
└── lib
    ├── Debug
    │   └── libDPSKModem.a
    └── Release
        └── libDPSKModem.a
```

### Подключение статической библиотеки к проекту
Скрипт для подключения библиотеки будет несколько отличаться от того, что приведен в `user-DPSK-modem/CMakeLists.txt`. Ключевым отличием является
отсутствие комманды [add_subdirectory](https://cmake.org/cmake/help/latest/command/add_subdirectory.html)
```
cmake_minimum_required(VERSION 3.5)
project(user-DPSK-modem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

add_executable(UserDPSKModem main.cpp)
set(DPSK_MODEM_LIB_DIR = "${CMAKE_CURRENT_SOURCE_DIR}/../DPSKModemLib") # путь до библиотеки DPSKModemLib

target_include_directories(UserDPSKModem PUBLIC "${DPSK_MODEM_LIB_DIR}/include")
target_link_directories(UserDPSKModem PUBLIC "${DPSK_MODEM_LIB_DIR}/lib/$<IF:$<CONFIG:Debug>,Debug,Release>")
target_link_libraries(UserDPSKModem DPSKModem)

```
В данном скрипте проверяется тип сборки проекта: `Debug` или `Release`, и в зависимости от типа выбирается нужная сборка библиотеки.

> [!IMPORTANT]
> Для успешного прохождения тестов модулятора необходимо наличие каталога `files-with-sample-for-tests`, содержащего ожидаемые отсчеты модулированных
сигналов для разных позиционностей ОФМ.

## Примеры использования
Примеры использования можно найти в тестах библиотеки, в файле `DPSK-modem/tests.cpp`.

## UML-диаграмма
![solution-UML](https://raw.githubusercontent.com/Seredenko-V/cpp-DPSK-modem/1a63a023b7970e45daa3cd07324d297dcdf24564/images/solution-UML.svg?token=AWESMYYHRTP7ZAL66DF4Q5DFMXE24 "solution-UML")
