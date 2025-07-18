# Трансивер "Волк"
Проект DDC-DUC SDR трансивера https://ua3reo.ru/tag/transiver-ua3reo/

* Телеграм канал сообщества: https://t.me/TRX_Wolf
* English version https://github.com/XGudron/UA3REO-DDC-Transceiver/blob/master/README.md

## Принцип работы

ВЧ сигнал оцифровывается высокоскоростной микросхемой АЦП, и подаётся на FPGA процессор.<br>
В нём происходит DDC/DUC преобразование (цифровое смещение частоты вниз или вверх по спектру) - по аналогии с приёмником прямого преобразования.<br>
I и Q квадратурные сигналы, полученные в ходе преобразований, поступают на микропроцессор STM32.<br>
В нём происходит фильтрация, (де)модуляция и вывод звука на аудио-кодек/USB. Также он обрабатывает весь пользовательский интерфейс.<br>
При передаче процесс происходит в обратном порядке, только в конце цепочки стоит ЦАП, преобразующий цифровой сигнал обратно в аналоговый ВЧ.<br>

## Технические характеристики TRX Wolf-2

* Частоты приёма: 0 Mhz - 2700 Mhz, входные фильтры на любительские диапазоны (до 23см включительно)
* Частоты передачи: 0 MHz - 2700 Mhz, усилители на любительские диапазоны (до 23см включительно)
* Мощность TX: 100W (HF), 50W+ (VHF/UHF), 15W+ (SHF)
* Два Антенных входа КВ, 4 антенных входа VHF/UHF/SHF/Wideband
* Автоматический антенный тюнер
* Виды модуляции (TX/RX): CW, LSB, USB, AM, FM, WFM, DIGI
* МШУ(LNA) и Предусилитель
* Регулируемый аттенюатор на 0-31дБ
* Полосовые фильтры
* Динамический диапазон АЦП (16 бит) ~100дБ
* Напряжение питания: 13.8в (защита от перенапряжения и смены полярности)

## Технические характеристики TRX Wolf-1

* Частоты приёма: 0 MHz - 750 MHz с затуханиями каждые 61.44 MHz (выше 145 MHz - без входных фильтров)
* Частоты передачи: 0 MHz - 150 MHz с затуханием на 110 MHz
* Мощность TX (QRP версия): 7W+ (HF), 5W (VHF)
* Мощность TX (QRP++ DB5AT версия): 20W (HF), 7W (VHF)
* Мощность TX (RU4PN/WF-100D версия): 100W (HF), 50W+ (VHF)
* Два Антенных входа
* Виды модуляции (TX/RX): CW, LSB, USB, AM, FM, WFM, DIGI
* МШУ(LNA) и Предусилитель
* Регулируемый аттенюатор на 0-31дБ
* Полосовые фильтры
* Динамический диапазон АЦП (16 бит) ~100дБ
* Напряжение питания: 13.8в (защита от перенапряжения и смены полярности)
* Потребляемый ток при приёме: ~0.7А (3'2 QRP), 0.9A (7' BIG)
* Потребляемый ток при передаче: ~2.5А+ (QRP), 15A+ (BIG)
* Поддержка разных дисплеев: RA8875/HX8357B/HX8357C/ST7789/ST7796S/ST7735S/ILI9341/ILI9481/ILI9486/ILI9488

## Функции трансивера

* Панорама (спектр+водопад) шириной до 384 кГц
* Несколько видов оформления спектра
* Двойной приёмник (смешивание звука A+B или A&B в стерео)
* Регулируемая полоса пропускания: ФВЧ от 0 Hz до 2700 Hz, ФНЧ от 100 Hz до 20 KHz
* Встроенный КСВ/Power метр (КВ)
* Автоматический и ручной Notch фильтр
* Отключаемое АРУ (AGC) с регулируемой скоростью атаки
* Карта диапазонов, с возможностью автоматического переключения моды
* Цифровое уменьшение шумов (DNR), подавитель импульсных помех (NB)
* CAT виртуальный COM-порт (эмуляция TS-2000 / FT-450, RTS - PTT, DTR - CW)
* Работа по USB (передача звука, IQ, CAT, KEY, PTT)
* RDS/CW декодер, самоконтроль, гаусс-фильтр
* Построение графиков КСВ по диапазонам
* Анализатор спектра
* FT8/FT4 декодер/энкодер
* RTTY декодер/энкодер
* SSTV декодер 
* Отправка лога связей FT8/FT4 в лог https://allqso.ru/
* Эквалайзер TX/RX, ревербератор
* SSB/FM Режим сканера
* Режим WSPR маяка
* VOX
* Поддержка SDHC/SDSC/SDXC карт памяти до 16 Гб
* Цифровой магнитофон, проигрывание быстрого сообщения в эфир
* AGC учитывает особенности человеческого слуха (K-Weighting)
* TCXO стабилизация частоты (возможно использование внешнего источника тактового сигнала, например GPS)
* Работа по WiFi: Синхронизация времени, WiFi сервисы
* Поддержка тангент Yaesu MH-36 и MH-48
* Режим аппаратной самодиагностики
* И другое (см. работу с меню)

### ВЧ Параметры

Чувствительность при соотношении сигнал-шум 10 dB, LNA включен:

* <150 MHz	| -131 dBm	| 63.0 nV
* 435 MHz	| -121 dBm	| 0.2 uV

Внеполосные излучения:

* 14 MHz -69 dBc
* 28 MHz -63 dBc
* 145 MHz -48 dBc -55 dBc

## Сборка и прошивка

Платы заказывал в китайском сервисе JLCPCB, они и их схемы находятся в папке Scheme.<br>
После сборки необходимо прошить FPGA и STM32 микросхемы.<br>
При необходимости, откалибровать трансивер через соответствующее меню<br>
WiFi модуль ESP-01 должен иметь свежую прошивку с SDK 3.0.4 и выше, и AT командами 1.7.4 и новее<br>

## Настройки

### TRX Settings

* **ANT type** - Выбор типа антенны (TX/RX или исключительно RX)
* **Band Map** - Карта диапазонов, автоматически переключает моду в зависимости от частоты
* **Beeper** - Звук нажатия клавиш
* **Callsign** - Позывной пользователя (оператора)
* **Operator Name** - Имя оператора
* **Locator** - QTH локатор пользователя
* **Channel Mode** - Канальный режим работы (для LPD/PMR и подобных)
* **Custom Transverter** - Включить управление внешним трансвертером для пользовательского смещения на любые частоты (просто сдвигает отображаемую частоту)
* **Debug Type** - Вывод отладочной и служебной информации в USB/UART порты
* **Debug Console** - Отображение консоли с системной отладочной информацией (копия того, что отправляется в отладочный COM-порт)
* **Encoder Accelerate** - Ускорение энкодера на больших оборотах
* **Fine RIT Tune** - Грубая или точная настройка для RIT/XIT (энкодером или резистором)
* **Freq Step xxx** - Шаг перестройки основным энкодером для каждой из моды, Гц
* **FAST Step Multiplier** - Множитель для перестройки в режиме FAST
* **ENC2 Step Mult** - Множитель для перестройки вторичным энкодером (для CW/SSB)
* **Notch Step** - Шаг перестройки частоты ручного Notch-фильтра
* **Full Duplex** - Режим одновременного приёма и передачи
* **Second COM Mode** - Выбор режима работы вторичного COM-порта (отладка, CAT, DATA)
* **SPLIT type** - В режиме DX - идёт управление частотой TX, в режиме CrossBand синхронно для двух VFO, для Free меняется только частота RX
* **RIT Interval** - Диапазон расстройки RIT (+-)
* **XIT Interval** - Диапазон расстройки XIT (+-)
* **Lock in IQ Mode** - Блокировка трансивера в IQ моде для работы с внешними программами
* **Transverter XXcm** - Поддержка внешнего трансвертера для радиолюбительских бэндов
* **URSI Code** - URSI код для вывода ионограмм https://digisonde.com/index.html#stationmap-section
* **Tropo Region** - Регион для прогноза тропо https://www.dxinfocentre.com/tropo.html 
* **Wolf Interface I2C** - Подключение трансивера к внешнему I2C интерфейсному устройству, см папку STUFF для получения примера

### FILTERS Settings

* **Adaptive FM Filter** - Автоматическая регулировка ширины NFM фильтра относительно силы принимаемого сигнала (УКВ)
* **AM/FM/CW/SSB LPF Stages** - Регулировка крутизны скатов LPF-фильтров на разных модах
* **CW Gauss filter** - Использовать фильтр с распределением Гаусса (для CW), принимает только пик в центре полосы
* **CW Gauss Q** - Добротность фильтра Гаусса
* **CW LPF Pass** - Частота среза ФНЧ при работе в CW
* **DIGI LPF Pass** - Частота среза ФНЧ при работе в DIGI
* **FM LPF Pass** - Частота среза ФНЧ при работе в FM
* **SSB HPF Pass** - Частота среза ФВЧ при работе в SSB
* **SSB LPF Pass** - Частота среза ФНЧ при работе в SSB
* **NOTCH Filter width** - Ширина среза ручного (Manual) Notch-фильтра

### RX Settings

* **ADC Dither** - Включение дизеринга АЦП для приёма слабых сигналов
* **ADC Driver** - Включение предусилителя-драйвера АЦП
* **ADC Preamp** - Включение предусилителя, встроенного в АЦП
* **ADC Randomizer** - Включение шифрования цифровой линии АЦП
* **AGC Gain target, LKFS** - Максимальное усиление AGC (максимальная громкость при включенном АРУ)
* **AGC Threshold** - Включение порога открытия АРУ
* **Att step, dB** - Шаг перестройки аттенюатора
* **Attenuation, dB** - Выбранное затухание аттенюатора
* **Auto Snap** - Автоматически отслеживать и передвигаться к ближайшему сигналу станции (CW mode)
* **AutoGainer** - Автоматическое управление ATT/PREAMP в зависимости от уровня сигнала на АЦП
* **A/B Balance** - Баланс каналов в режимах двойного приёмника A+B и A&B
* **CODEC Gain** - Регулировка уровня НЧ выхода аудио-кодека
* **DNR xxx** - Подстройка цифрового шумоподавителя
* **Squelch** - SSB/AM/CW/FM Шумодав
* **FM Demodulator** - Алгоритм работы FM-демодулятора
* **FM Squelch level** - Уровень шумодава FM
* **IF Gain, dB** - Усиление ПЧ
* **NB Threshold** - Порог срабатывания Noise blanker
* **Free tune** - Позволяет перемещать полосу приёма по спектру не меняя центральную частоту самого спектра
* **Center After Idle** - Центровать спектр при активном режиме Free Tune после 20 секунд неактивности
* **Noise Blanker** - Активация подавителя импульсных помех
* **Pseudo stereo** - Режим псевдо-стереофонии RX тракта
* **RX AUDIO Mode** - Управление каналами аудио: Стерео\Левый\Правый
* **RX AGC Hold time** - Время перед отпусканием усиления АРУ в пиках сигнала, мс
* **RX AGC Hold limit** - Ограничение на резкие всплески для времени задержки АРУ
* **RX AGC Hold step up** - Шаг нарастания времени задержки АРУ на пиках
* **RX AGC Hold step down** - Шаг спада времени задержки АРУ на пиках
* **RX AGC Max gain** - Максимальный уровень усиления при работе AРУ, дБ
* **RX AGC Speed** - Скорость срабатывания АРУ (автоматического регулятора уровня сигнала) на приём (больше-быстрее)
* **TRX Samplerate** - Максимальная ширина панорамы в модах SSB/DIGI и др.
* **FM Samplerate** - Максимальная ширина панорамы в моде NFM/WFM
* **CW Samplerate** - Максимальная ширина панорамы в моде CW
* **VAD Threshold** - Выбор порога срабатывания VAD детектора голоса (шумоподавитель для SSB моды и SCAN режима)
* **Volume step** - Шаг изменения громкости второстепенным энкодером (X1, Lite)
* **Volume** - Усиление НЧ
* **WFM Stereo** - Выбор между моно и стерео декодером WFM
* **WFM Stereo Modulation** - Уровень поднесущей стерео-сигнала WFM

### TX Settings

* **ADC Shutdown** - Отключение АЦП во время передачи
* **TX AGC MaxGain** - Максимальное усиление для АРУ
* **TX AGC Speed** - Скорость срабатывания АРУ/компрессора на передачу (больше-быстрее)
* **TX AGC Clipping** - Определяет, будет ли АРУ резко ограничивать усиление сигнала при больших всплесках амплитуды
* **ATU Cap** - Комбинация ёмкостей тюнера
* **ATU Enabled** - Включение автоматического антенного тюнера
* **ATU Ind** - Комбинация индуктивностей тюнера
* **ATU T** - Положение ёмкостного плеча тюнера
* **ATU Mem step, kHz** - Шаг сохранения значений тюнера для частот в память
* **Auto Input Switch** - Автоматический выбор входа (PTT - микрофон, CAT - USB)
* **CESSB** - Использование модуляции с контролем огибающей при формировании SSB сигнала
* **CESSB Compress, dB** - Уровень предварительного усиления сигнала перед компрессией в CESSB
* **DRV Shutdown** - Отключение драйвера ЦАП во время приёма
* **CTCSS Frequency** - Частота передачи CTCSS субтона для FM
* **Input Type** - Выбор аудио входа (микрофон, линейный вход, USB)
* **LINE Gain** - Регулировка усиления линейного входа
* **MIC Boost** - +20db аппаратного усиления микрофона
* **MIC DSP Type** - Выбор типа DSP обработки микрофонного тракта: None, Downward Expander, Upward/Downward Compressor, Limiter, Noise Gate
* **MIC DSP Threshold** - Порог срабатывания DSP обработки микрофона
* **MIC DSP Ratio** - Уровень ската DSP обработки микрофона
* **MIC Reverber** - Ревербератор микрофона
* **MIC Gain** - Усиление микрофона
* **Power for each band** - Сохранять настройки мощности для каждого бэнда по отдельности
* **Power for each mode** - Сохранять настройки мощности для каждой моды по отдельности
* **RF Power** - Мощность передачи, %
* **RF Power ENC3 step** - Шаг регулировки мощности энкодером (Wolf-2)
* **Repeater Mode"** -  Режим работы через репитер
* **Repeater offset, kHz** -  Отстройка по частоте передачи в режиме работы через репитер
* **Repeater tone** - Добавляет тон 1750Гц в начале передачи для открытия репитера (в FM моде)
* **SelfHear CW** - Громкость самоконтроля CW 
* **SelfHear on SSB** - Самоконтроль в моде SSB/AM, для DIGI/RTTY/LOOPBACK активен всегда
* **SelfHear VOICE** - Регулировка громкости самоконтроля голосовых мод относительно общей громкости трансивера
* **SelfHear DIGI** - Регулировка громкости самоконтроля DIGI относительно общей громкости трансивера
* **TUNER Enabled** - Включение антенного тюнера
* **TUNER Enbl per band** - Сохранять вкл-выкл тюнера для каждого бенда независимо
* **Tune Type** - Режим работы TUNE (несущая, двухсигнальный генератор, мульти-тон, белый шум)
* **TOT, min** - Защита передатчика от длительной работы, автоматическое отключение по истечению времени
* **VOX Threshold, dBFS** - Порог срабатывания VOX, dBFS
* **VOX Timeout, ms** - Задержка передачи VOX после молчания, миллисекунд
* **VOX** - Активация передачи голосом

### CW Settings

* **Auto CW Mode** - Автоматический переход на CW моду при нажатии ключа, либо только манипуляция на время передачи
* **DotToDash Rate** - Соотношение длины тире к точке
* **Edges smooth, ms** - Сглаживание фронтов сигнала
* **Iambic Keyer** - Режим ямбического ключа
* **Iambic Type** - Тип ямбического ключа (A/B)
* **Key Invert** - Инвертировать точку/тире на ключе
* **Key timeout** - Время до остановки режима передачи после отпускания ключа
* **Keyer** - Автоматический ключ
* **Keyer Memory** - Режим памяти на один символ
* **Keyer WPM** - Скорость ключа, WPM
* **Mode** - Тип боковой полосы CW-LSB / CW-USB
* **PTT Type** - Режим перехода на передачу CW: от ключа или от сигнала PTT
* **Pitch** - Отстройка генератора приёма от частоты передачи
* **Stereo** - Разделение полосы пропускания телеграфа по каналам наушников (аудио-панорама)
* **Self Hear** - Самоконтроль CW (слышно нажатие ключа)

### Macros

* **AutoCQ Macros 1** - Авто-вызов CQ макроса CW/SSB/RTTY с заданным интервалом
* **CW Macros x** - Настройка CW-макросов для передачи телеграфом
* **CW Macros x name** - Настройка имени CW-макросов для отображения в интерфейсе
* **SSB Macros x name** - Настройка имени SSB-макросов для отображения в интерфейсе

Допустимые вставки в макросах:

* {MYCALL} - Позывной оператора
* {DXCALL} - Позывной корреспондента
* {MYNAME} - Имя оператора
* {MYLOC} - Локатор оператора
* {CLRDX} - Очистка позывного корреспондента

### Equalizer Settings

* **RX EQ xxx** - Уровни эквалайзера приёмника
* **MIC EQ xxx** - Уровни эквалайзера микрофона
* **EQ Q xxx** - Добротность каждого из каналов эквалайзера

### SCREEN Settings

* **Bottom navi buttons** - Отображать кнопок перелистывания в нижнем меню (для 7 дюймовых дисплеев)
* **Color Theme** - Выбор темы цветового оформления (0 - черная, 1 - белая, 2 - черная с цветными цифрами)
* **Layout Theme** - Выбор темы оформления интерфейса (0 - по умолчанию)
* **DX Cluster Type** - Выбор источника данных для DX Cluster
* **FFT 3D Mode** - Режим 3D FFT 3D (0 - выкл, 1 - линии, 2 - пиксели)
* **FFT Automatic** - Автоматическая подстройка шкалы FFT. Full - автоматический выбор верхней и нижней границ, Half - авто нижний и ручной верхний, No - ручное выставление диапазона уровней спектра. (Не забывайте выбрать нужный FFT Scale Type, чтобы диапазон уместился на экране)
* **FFT Averaging** - Уровень усреднения всплесков FFT
* **FFT BW Style** - Стиль оформления полосы приёма на FFT: полупрозрачный, почти прозрачный, линии
* **FFT BW Position** - Положение полосы приёма на FFT/WTF (везде \ на верху \ на верху + линии \ верх + низ с временной задержкой)
* **FFT Background** - Градиентный фон за спектром
* **FFT Color** - Цвета FFT: 1(синий -> желтый -> красный), 2(чёрный -> желтый -> красный), 3(чёрный -> желтый -> зеленый), 4(чёрный -> красный), 5(чёрный -> зеленый), 6(чёрный -> синий), 7(чёрный -> белый)
* **FFT Compressor** - Сжимать пики при перегрузке FFT
* **FFT DXCluster Azimuth** - Добавить данные об азимуте к DX-кластеру
* **FFT DXCluster Timeout** - Таймаут отображаемых спотов из DX-кластера в минутах
* **FFT DXCluster** - Отображать данные из кластера поверх спектра
* **FFT Enabled** - Включение водопада и FFT
* **FFT Freq Grid** - Сетка на FFT и водопаде: 1(нет сетки), 2(сетка на FFT), 3(сетка на FFT и водопаде), 4(сетка на водопаде)
* **FFT Height** - Пропорциональная высота отображения FFT и водопада
* **FFT Hold Peaks** - Отображать пики сигнала на спектре
* **FFT Lens** - Режим линзы (увеличения центра спектра)
* **FFT Manual Bottom, dBm** - Нижний порог чувствительности FFT при ручной настройке
* **FFT Manual Top, dBm** - Верхний порог чувствительности FFT при ручной настройке
* **FFT Scale Type** - Стиль оси Y на FFT между амплитудой сигнала и dBm, что позволяет уместить больший динамический диапазон на экране (Полезно при ручной настройке диапазона FFT).
* **FFT Sensitivity** - Верхний уровень чувствительности автоподстройки FFT (чем меньше - тем сигналы контрастней, 30 - выравнивание по сильнейшему сигналу)
* **FFT Speed** - Скорость отображения FFT и водопада
* **FFT Style** - Стиль FFT: 1(градиент), 2(заливка), 3(точки), 4(контур), 5(градиент + контур)
* **FFT Window** - Выбор окна FFT (1-Dolph–Chebyshev 2-Blackman-Harris 3-Nutall 4-Blackman-Nutall 5-Hann 6-Hamming 7-No window)
* **FFT dBm Grid** - Сетка мощности сигнала на FFT
* **LCD Brightness** - Управление яркостью экрана (не для всех дисплеев)
* **LCD Sleep Timeout** - Время бездействия, перед тем как снизится яркость экрана, сек (0 - функция отключена)
* **S-Meter style** - Выбор внешнего вида аналогового S-Метра
* **S-Meter smooth** - Коэффициент сглаживания значения S-метра
* **Show Sec VFO** - Отображать положение второго VFO на спектре
* **WTF Color** - Цвета водопада: 1(синий -> желтый -> красный), 2(чёрный -> желтый -> красный), 3(чёрный -> желтый -> зеленый), 4(чёрный -> красный), 5(чёрный -> зеленый), 6(чёрный -> синий), 7(чёрный -> белый)
* **WTF Moving** - Смещение водопада вместе с изменением частоты
* **Wolf Cluster** - Отображать других пользователей трансивера Волк на спектре и передавать свои данные

### Decoders

* **CW Decoder** - Программный декодер CW приёма
* **CW Decoder Threshold** - Порог срабатывания CW декодера
* **FT8 Auto CQ** - Автоматический переход в режим CQ после проведения связи FT8/FT4
* **RDS Decoder** - Включить RDS декодер для WFM моды
* **RTTY Freq** - Центральная частота декодирования RTTY
* **RTTY InvertBits** - Инверсия битов RTTY
* **RTTY Shift** - Разнос сигналов в полосе RTTY
* **RTTY Speed** - Скорость данных для RTTY декодера
* **RTTY StopBits** - RTTY стоп-биты
* **SSTV Decoder** - Запуск SSTV декодера (PD 50/90/120/180, Scottie 1/2, Martin 1/2)

### WIFI Settings

* **WIFI Enabled** - Включение WiFi модуля (нужен перезапуск трансивера после включения модуля)
* **WIFI Network** - Выбор точки доступа WiFi
* **WIFI Network Pass** - Установка пароля для точки доступа WiFi
* **WIFI Timezone** - Временная зона (для обновления времени через интернет)
* **WIFI Update ESP firmware** - Запуск автообновления прошивки ESP-01 через интернет (если доступно)
* **ALLQSO.RU Token/LogId** - Токен и ID лога на онлайн-журнале https://allqso.ru/

### SD Card

* **File Manager** - Запуск файлового менеджера по SD карте памяти, функции проигрывания и удаления файлов, прослушивания и передачи записей в эфир, а также обновления прошивки с карты памяти
* **Record CQ message** -  Запись короткого сообщения для быстрой передачи в эфир
* **USB SD Card Reader** - Режим SD картридера по USB
* **Export Settings** - Экспорт настроек и калибровок на SD карту
* **Import Settings** - Импорт настроек и калибровок с SD карты
* **Format SD card** - Форматирование SD карты

### Satellites

* **SAT Mode** - Включение режима работы со спутниками
* **Auto Doppler** - Автоматически смещает частоту трансивера исходя из расчитанного эффекта Доплера
* **Download TLE** - Скачивание актуальных данных о спутниках
* **Select SAT** - Выбор спутника из скачанного TLE файла
* **Calc Pass** - Расчёт будущих прохождений спутника над выбранным QTH (локальное время)
* **QTH xxx** - Установка Широты\Долготы\Высоты вашего расположения

### Calibration [появляется при долгом нажатии кнопки MENU в меню настроек]

### Calibration Bands
* **ENABLE 2200m/60m/6m/4m/FM/2m/AIR/Marine Band/70cm** - Включение скрытых бэндов
* **NOTX xxx** - Запрет передачи на выбранных бэндах

### Calibration Control
* **CAT Type** - Тип CAT подсистемы (FT-450 / TS-2000)
* **COM CAT DTR/RTS** - Настройка функций управляющих линий DTR/RTS порта CAT
* **SEC COM DTR/RTS** - Настройка функций управляющих линий DTR/RTS порта отладки
* **FAN Full start** - Температура оконечного каскада для запуска вентилятора на полной скорости
* **FAN Medium start** - Температура оконечного каскада для запуска вентилятора на средней скорости
* **FAN Medium stop** - Температура оконечного каскада для остановки вентилятора
* **FAN MB ---** - Настройки включения вентилятора для обдува STM32
* **FAN Medium PWM** - Регулировка скважности ШИМ вентилятора в Medium режиме
* **FAN Motherboard** - Запуск вентилятора не только по датчику, но и по температуре STM32 процессора
* **IF Gain MIN/MAX** - Пределы регулировки усиления ПЧ
* **Min Voltage protect** - Минимальный порог питающего напряжения, ниже сработает защита и трансивер отключится (INA226)
* **OTA Update** - Включение обновления прошивки через WiFi

### Calibration Hardware
* **RF-Unit Type** - Тип RF-Unit платы (QRP / BIG)
* **ADC Offset** - Добавление виртуального смещения к сигналу АЦП
* **ALC Port Enabled** - Активация управления мощностью трансивера внешним усилителем по линии ALC: до 1 вольта - ALC отключен и используется мощность выставленная в трансивере. с 1.0v до 3.0v - регулировка мощности от 0 до 100 %
* **ALC Inverted** - Инвертирование логики ALC (3.0v - 0%, 1.0v - 100%)
* **BPF x** - Параметры полосовых фильтров
* **HPF START** - Параметры ФВЧ фильтра
* **LPF END** - Параметры ФНЧ фильтра
* **DAC Driver Mode** - Режим работы драйвера OPA2673 (2 = 100% bias, 1 = 75% bias, 0 = 50% bias)
* **DAC Interpolation** - Использование интерполяции в первой зоне частоты ЦАП (КВ)
* **EXT xxx** - Управление комбинациями порта External в зависимости от диапазона (EXT3, EXT2, EXT1, EXT0) - открытый коллектор
* **Encoder acceleration** - Ускорение энкодера при быстром вращении
* **Encoder debounce** - Время устранения дребезга контактов основного энкодера
* **Encoder invert** - Инвертировать вращение основного энкодера
* **Encoder on falling** - Энкодер срабатывает только на падение уровня A
* **Encoder slow rate** - Коэффициент замедления основного энкодера
* **Encoder2 debounce** - Время устранения дребезга контактов дополнительного энкодера
* **Encoder2 invert** - Инвертировать вращение дополнительного энкодера
* **Encoder2 on fall** - Режим работы вторичного энкодера (некоторые пропускают шаг или делают лишний)
* **Flash GT911** - Запуск процесса обновления конфигурации тачпада в соответствии с разрешением LCD экрана
* **INA226** - Активация и калибровка INA226 датчика напряжения\тока на I2C шине
* **KTY81 Calibration** - Калибровка датчика KTY81 (установка сопротивления плеча делителя)
* **LCD Rotate** - Повернуть экран на 180 градусов
* **MAX ChargePump, kHz** - Максимальная частота ШИМ цепи ChargePump
* **RTC COARSE CALIBR** - Грубая калибровка часового кварца, при больших отклонениях
* **RTC FINE CALIBR** - Калибровка часового кварца, одно деление равняется 0.954 ppm
* **Sequencer support** - Поддержка внешнего секвенсора (выход через линию EXT_TUNE)
* **Swap AF & IF** - Замена регуляторов AF Gain и IF Gain местами
* **TCXO Frequency, kHz** - Выбор частоты опорного генератора
* **VCXO Correction** - Поправка на отклонение частоты основного генератора
* **Touchpad flip** - Отразить работу тачпада по горизонтали/вертикали
* **Touchpad timeout** - Минимальное время между нажатиями (защита от ложных групп срабатываний), мсек
* **Touchpad click threshold** - Минимальное время для срабатывания короткого клика, мсек
* **Touchpad click timeout** - Максимальное время для срабатывания короткого клика, мсек
* **Touchpad hold timeout** - Время до срабатывания долгого нажатия, мсек
* **Touchpad swipe threshold** - Минимальное расстояние для срабатывания свайпа, пикс
* **Tangent Type** - Выбор используемой тангенты
* **Transverter Offset, MHz** - Смещение частоты, отображаемой на дисплее (для кастомных трансвертеров)
* **Transverter xxx IF, MHz** - Установка ПЧ частоты трансвертера
* **Transverter xxx RF, MHz** - Установка ВЧ частоты трансвертера, соответствующей ПЧ
* **Transv only TX** - Использовать трансвертер только для TX
* **VHF Mixer Board** - Включение поддержки УКВ платы с смесителем (Волк-2)
* **VHF Mixer IF, MHz** - Выбор частоты ПЧ смесителя УКВ платы (Волк-2)
* **VHF Mixer Current** - Выбор уровня тока смесителя (Волк-2)
* **VHF TCXO, kHz** - Выбор частоты TCXO генератора УКВ платы (Волк-2)
* **VHF TCXO Correction** - Поправка на отклонение частоты TCXO генератора УКВ платы (Wolf-2)

### Calibration RX
* **ATT Compensation** - Компенсирует значение S-метра при включении аттенюатора
* **LNA Compensation** - Компенсирует значение S-метра при включении LNA, dBm
* **CICCOMP Shift** - Битовое смещение после CIC компенсатора
* **S METER** - Калибровка S-метра (LHF 160/80, MHF 40/30/20/17, HHF 15/12/10, 6M, VHF, SHF)
* **Swap USB IQ** - Инверсия выхода IQ по USB

### Calibration TX
* **AM Modulation Index** - Глубина модуляции TX AM
* **ATU Averaging** - Количество шагов усреднений значений КСВ при замерах в работе автоматического тюнера
* **FM Deviation Scale** - Усиление девиации в TX FM режиме
* **Linear Pwr Control** - Задаёт линейный способ изменения амплитуды сигнала при регулировке мощности (если отключен - логарифмический)
* **MAX PWR on Meter** - Максимальная выходная мощность (для индикации)
* **MAX Power in TUNE** - Максимальная мощность в режиме настройки
* **MAX RF Temp** - Максимальная температура оконечного каскада перед срабатыванием защиты
* **MAX SWR** - Максимальное КСВ перед срабатыванием защиты
* **Max Current, Amp** - Защита по току, отключает передачу по достижению порога
* **RF GAIN xxx** - Калибровка максимальной выходной мощности TX на каждый диапазон (раскачка с ЦАП)
* **SD Macros power** - Коэффициент усиления сигнала при проигрывании SSB макросов
* **SD Recording power** - Коэффициент усиления сигнала при проигрывании записей эфира
* **SSB Power addition** - Прибавка к выходной мощности в режиме SSB, %
* **SWR FWD/BWD RATE** - Подстройка коэффициента трансформации SWR-метра для падающей и отраженной волны
* **SWR Protector** - Выбор режима защиты по КСВ: Выключена, сброс мощности, отключение передачи
* **TSignal Balance** - Задаёт баланс мощности между сигналами в режиме Two signal tune
* **TX Start Delay** - Задержка при переходе на TX для срабатывания реле
* **TX End Delay** - Задержка при переходе с TX на RX (остановка сигнала до выключения реле)

### Calibration Reset
* **Settings reset** - Сброс настроек по умолчанию
* **Calibrate reset** - Сброс калибровок по умолчанию
* **Personal reset** - Сброс настроек персональных настроек и Wifi по умолчанию

### Memory Channels

* Смена названий для каналов памяти трансивера

### Set Clock Time

* Установка часов

### DFU Mode

* Запуск DFU режима для обновления прошивки STM32 по USB

### OTA Update

* Запуск обновления прошивка FPGA/STM32 через интернет

### System info

* Вывод системной информации

## Сервисы

### Spectrum Analyzer

* **Spectrum START** - Запуск спектрального анализатора
* **Begin, kHz** - Стартовая частота анализатора с шагом в 1 kHz
* **End, kHz** - Конечная частота анализатора с шагом в 1 kHz
* **Top, dBm** - Верхний порог графика
* **Bottom, dBm** - Нижний порог графика

### WSPR Beacon

* **WSPR Beacon START** - Запуск маячка WSPR
* **Freq offset** - Смещение относительно центра диапазона WSPR
* **WSPR Power, dBm** - Установка показателя мощности, отправляемого в сообщении маяка
* **WSPR Band xxx** - Выбор диапазонов для работы маячка

### Efficiency

* **Cable Type** - Выбор используемого кабеля в антенно-фидерном устройстве
* **Cable Length** - Длина кабеля от трансивера до антенны
* **Calc Efficiency** - Запуск расчёта эффективности. Для расчёта необходимо отключить тюнер (чтобы знать КСВ антенно-фидерного устройства) и подать несущую

### DX Cluster 

* Отображение данных по текущему диапазону из DX кластера (требуется подключение к интернету)

### WOLF Cluster 

* Отображение данных о находящихся сейчас в текущем диапазоне радиолюбителях, использующих трансиверы "Wolf" (требуется подключение к интернету)

### Propagation 

* Отображение статистики по прохождению радиоволн (требуется подключение к интернету)

### Tropo 

* Отображение прогноза тропосферного прохождения

### DayNight Map 

* Отображение светораздела на карте (требуется подключение к интернету)

### Ionogram 

* Отображение ионограммы для выбранного URSI Code (требуется подключение к интернету)

### foF2 

* Отображение карты критической частоты foF2 (требуется подключение к интернету)

### SWR Analyzer

* Запуск анализатора КСВ (Band SWR - по диапазону, HF SWR - по всем КВ диапазонам, Custom SWR - в выбранных границах)

### RDA Statistics 

* Отображение статистики по диплому RDA (требуется подключение к интернету)

### FT4

* FT4 Приёмник / передатчик

### FT8

* FT8 Приёмник / передатчик

### Locator Info

* Расчёт расстояния и азимута до корреспондента по его QTH-локатору

### Callsign Info

* Вывод данных по позывному

### Time Beacons

* Список маяков точного времени (для подстройки частоты приёма по несущим)

### Self Test

* Запуск системы аппаратной самодиагностики


### Auto calibration

* **Calibrate SWR** - Помощник в калибровке КСВ и измерителя мощности
* **Calibrate SWR** - Помощник в настройке выходной мощности
