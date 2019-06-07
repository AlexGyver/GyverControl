![PROJECT_PHOTO](https://github.com/AlexGyver/gyverControl/blob/master/proj_img.jpg)
# Универсальный контроллер умной теплицы
* [Описание проекта](#chapter-0)
* [Папки проекта](#chapter-1)
* [Схемы подключения](#chapter-2)
* [Материалы и компоненты](#chapter-3)
* [Как скачать и прошить](#chapter-4)
* [FAQ](#chapter-5)
* [Полезная информация](#chapter-6)
[![AlexGyver YouTube](http://alexgyver.ru/git_banner.jpg)](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)

<a id="chapter-0"></a>
## Описание проекта
**GyverControl** - универсальный контроллер-таймер для теплицы и других мест, где нужна автоматизация по таймеру или показателям микроклимата/другим датчикам. Ссылки на все компоненты, схемы, инструкции и другая информация находятся на странице проекта на официальном сайте: https://alexgyver.ru/gyvercontrol/.  
   
#### Особенности:
- 7 каналов с логическим выходом 5V, к которым можно подключать обычное реле, твердотельное реле, силовые ключи (транзисторы, модули на основе транзисторов)
- 2 канала сервоприводов, подключаются обычные модельные серво больших и маленьких размеров
- 1 канал управления линейным электроприводом с концевиками ограничения движения и с работой по тайм-ауту
- Датчик температуры воздуха (BME280)
- Датчик влажности воздуха (BME280)
- 4 аналоговых датчика (влажности почвы или других)
- Модуль опорного (реального) времени RTC DS3231 с автономным питанием
- Большой LCD дисплей (LCD 2004, 20 столбцов, 4 строки)
- Орган управления - энкодер
- Периодичный полив (реле)
- Схема с индивидуальными помпами/клапанами
- Схема с одной помпой и несколькими клапанами
- Полив на основе показаний датчиков влажности почвы
- Управление освещением (реле) с привязкой ко времени суток
- Проветривание (привод открывает окно/серво открывает заслонку) по датчику температуры или влажности воздуха
- Увлажнение (включение увлажнителя) по датчику влажности воздуха
- Обогрев (включение обогревателя) по датчику температуры
- Выполнение действий сервоприводом (нажатие кнопок на устройствах, поворот рукояток, поворот заслонок, перемещение предметов) по датчику или таймеру

<a id="chapter-1"></a>
## Папки
**ВНИМАНИЕ! Если это твой первый опыт работы с Arduino, читай [инструкцию](#chapter-4)**
- **libraries** - библиотеки проекта. Заменить имеющиеся версии
- **firmware** - прошивки для Arduino
- **schemes** - схемы подключения компонентов
- **docs** - документация, картинки
- **PCB** - файлы печатной платы

<a id="chapter-2"></a>
## Схемы
![SCHEME](https://github.com/AlexGyver/gyverControl/blob/master/schemes/scheme1.jpg)
![SCHEME](https://github.com/AlexGyver/gyverControl/blob/master/PCB/PCBmap.jpg)

<a id="chapter-3"></a>
## Материалы и компоненты
### Ссылки оставлены на магазины, с которых я закупаюсь уже не один год
Железо контроллера
- Arduino NANO 328p http://ali.ski/vQENg  http://ali.ski/vCLD4m  http://ali.ski/PTT3G
- Энкодер http://ali.ski/6Vqf_  http://ali.ski/3ElKb  http://ali.ski/Y6ASf  http://ali.ski/9kD70b
- Дисплей 2004 http://ali.ski/ns30n  http://ali.ski/K70Qdm
- DS3231 мини http://ali.ski/iEOal  http://ali.ski/-4lZJ  http://ali.ski/3-lhX
- Датчик т/вл воздуха BME280 (5V) http://ali.ski/K_4yQ  http://ali.ski/HbIQUR
- Драйвер привода http://ali.ski/SUlJG  http://ali.ski/fyfou6

Датчики вл. почвы
- Обычный http://ali.ski/hfAPFR  http://ali.ski/-S4KK
- Золотой http://ali.ski/Rh23E  http://ali.ski/ioVGRT
- Ёмкостный http://ali.ski/OzKaU3  http://ali.ski/Sweg4  http://ali.ski/vw05eG

Аналоговые датчики
- Освещённости http://ali.ski/rP2XuQ  http://ali.ski/Kqoan8  http://ali.ski/jHUZN
- Термистор http://ali.ski/0URXo  http://ali.ski/fUkzn
- Звука http://ali.ski/er1AKe  http://ali.ski/W_Biw
- ИК излучения http://ali.ski/cuFSD3
- Дождя http://ali.ski/OhEYBI  http://ali.ski/6QHO_W
- Уровня воды http://ali.ski/7RU3F  http://ali.ski/5lW-7
- Газоанализатор http://ali.ski/PV7Si  http://ali.ski/wyKmOA

"Специальные" датчики
- DS18b20-пуля http://ali.ski/hkXyQ  http://ali.ski/TE2nRo
- DHT11 http://ali.ski/AOZYS  http://ali.ski/XLKWO
- DHT22 http://ali.ski/kjy4u  http://ali.ski/HX_AtY
- Термистор http://ali.ski/k39S2  http://ali.ski/m3V-8M
- Термистор-пуля http://ali.ski/DrAuMF  http://ali.ski/Xo98y5  http://ali.ski/NZDe7K

Плавный контроль
- Диммер 4 кВт http://ali.ski/TQihAE  http://ali.ski/bIOWyl
- MOSFET модуль 

Модули реле
- Реле 5V 1/2/4/8 каналов http://ali.ski/hl-ks http://ali.ski/TlKdRU
- Реле 5V 2 канала http://ali.ski/lAZpPj http://ali.ski/HN7rD
- Реле 5V 4 канала http://ali.ski/x9448y
- Твердотельные http://ali.ski/cH5I3  http://ali.ski/tc-Wvl  http://ali.ski/7mmv_  http://ali.ski/GKS1ni
- Одноканальное мощное SSR http://ali.ski/iG9jX8  http://ali.ski/Bl_Rye

Сервоприводы
- Малый (2 кг*см) http://ali.ski/rFG1e  http://ali.ski/lqDaZ
- Большой (13 кг*см) http://ali.ski/qsvQoE  http://ali.ski/-Wq58
- Большой (20 кг*см) http://ali.ski/FTJ3W-  http://ali.ski/eikGZ
- Очень большой (50 кг*см) http://ali.ski/n2GcZ

Линейные приводы
- Привод 100мм 12V (скорость я брал 45mm/s) http://ali.ski/nLdJOj
- Привод 20/30/50мм 12V http://ali.ski/09Lqys
- Привод 150/200мм 12V http://ali.ski/ipCsB

Железки для теплицы
- Средняя помпа http://ali.ski/lIVte  http://ali.ski/TP-uy  http://ali.ski/KvkuTQ
- Мощная помпа http://ali.ski/rfBhND  http://ali.ski/hpiO85  http://ali.ski/HKekA  http://ali.ski/DiYbgv
- Лампочки с полным спектром http://ali.ski/AXzAf  http://ali.ski/8Qi8a0  http://ali.ski/JZcVv7
- Матрицы с полным спектром http://ali.ski/pH-EGI  http://ali.ski/x2tVm
- Готовые полноспектровые светильники http://ali.ski/joEm8  http://ali.ski/JnoHA  http://ali.ski/6Rslgi

Разное
- БП 5V 2A питание схемы http://ali.ski/7iOfQn  http://ali.ski/ygpCU  http://ali.ski/qrJ41
- БП 12V 3A питание привода http://ali.ski/7iOfQn  http://ali.ski/ygpCU  http://ali.ski/qrJ41
- Кнопки NC http://ali.ski/GmyPb
- Кнопки NO http://ali.ski/vk-rb
- Концевик герметичный http://ali.ski/MTxcf  http://ali.ski/g655sn
- Колодки http://ali.ski/wA07ne  http://ali.ski/THSQ9
- Рейка контактная http://ali.ski/opd64
- Рейка угловая http://fas.st/opqeGu  http://ali.ski/u9Bz9

В наших магазинах
- Рейка контактная https://www.chipdip.ru/product/pls-40
- Рейка угловая тройная PLT-120R https://www.chipdip.ru/product/plt-120r-pitch-2-54-mm

## Вам скорее всего пригодится
* [Всё для пайки (паяльники и примочки)](http://alexgyver.ru/all-for-soldering/)
* [Недорогие инструменты](http://alexgyver.ru/my_instruments/)
* [Все существующие модули и сенсоры Arduino](http://alexgyver.ru/arduino_shop/)
* [Электронные компоненты](http://alexgyver.ru/electronics/)
* [Аккумуляторы и зарядные модули](http://alexgyver.ru/18650/)

<a id="chapter-4"></a>
## Как скачать и прошить
* [Первые шаги с Arduino](http://alexgyver.ru/arduino-first/) - ультра подробная статья по началу работы с Ардуино, ознакомиться первым делом!
* Скачать архив с проектом
> На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**
* Установить библиотеки в  
`C:\Program Files (x86)\Arduino\libraries\` (Windows x64)  
`C:\Program Files\Arduino\libraries\` (Windows x86)
* **Подключить внешнее питание 5 Вольт**
* Подключить Ардуино к компьютеру
* Запустить файл прошивки (который имеет расширение .ino)
* Настроить IDE (COM порт, модель Arduino, как в статье выше)
* Настроить что нужно по проекту
* Нажать загрузить
* Пользоваться  

## Настройки в коде
    #define ENCODER_TYPE 1      // тип энкодера (0 или 1). Если энкодер работает некорректно (пропуск шагов/2 шага), смените тип
    #define ENC_REVERSE 0       // 1 - инвертировать направление энкодера, 0 - нет
    #define DRIVER_LEVEL 1      // 1 или 0 - уровень сигнала на драйвер/реле для привода
    #define LCD_ADDR 0x3f       // адрес дисплея 0x27 или 0x3f . Смени если не работает!!
<a id="chapter-5"></a>
## FAQ
### Основные вопросы
В: Как скачать с этого грёбаного сайта?  
О: На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**

В: Скачался какой то файл .zip, куда его теперь?  
О: Это архив. Можно открыть стандартными средствами Windows, но думаю у всех на компьютере установлен WinRAR, архив нужно правой кнопкой и извлечь.

В: Я совсем новичок! Что мне делать с Ардуиной, где взять все программы?  
О: Читай и смотри видос http://alexgyver.ru/arduino-first/

В: Вылетает ошибка загрузки / компиляции!
О: Читай тут: https://alexgyver.ru/arduino-first/#step-5

В: Сколько стоит?  
О: Ничего не продаю.

### Вопросы по этому проекту

<a id="chapter-6"></a>
## Полезная информация
* [Мой сайт](http://alexgyver.ru/)
* [Основной YouTube канал](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)
* [YouTube канал про Arduino](https://www.youtube.com/channel/UC4axiS76D784-ofoTdo5zOA?sub_confirmation=1)
* [Мои видеоуроки по пайке](https://www.youtube.com/playlist?list=PLOT_HeyBraBuMIwfSYu7kCKXxQGsUKcqR)
* [Мои видеоуроки по Arduino](http://alexgyver.ru/arduino_lessons/)