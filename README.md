# Решение кейса 1 от команды echo("637_NrF3D")

https://disk.yandex.ru/i/vRceE684DP-BwA - демонстрация решения кейса.

Репозиторий содержит код (Arduino `.ino`), 3D-модели/чертежи и схемы для двух частей проекта: **CUBESAT** и **STATION**.

## Структура репозитория

```text
.
├─ code/                          # Исходники прошивок (Arduino)
│  ├─ CUBESAT/
│  │  └─ CUBESAT.ino              # Прошивка для модуля CUBESAT
│  └─ STATION/
│     └─ STATION.ino              # Прошивка для наземной станции
│
├─ models/                        # 3D-модели и документация к механике
│  ├─ drawings/                   # PDF-чертежи деталей/сборок
│  │  ├─ Сборка.pdf
│  │  ├─ balka_central - balka_central.pdf
│  │  ├─ bok002 (Solid) _ bok002 (Solid).pdf
│  │  ├─ bok005 _ bok005.pdf
│  │  ├─ dno001 _ dno001.pdf
│  │  ├─ krysha001 _ krysha001 .pdf
│  │  ├─ palka002 _ palka002.pdf
│  │  └─ platforma _ platforma.pdf
│  │
│  └─ stls/                       # STL для 3D-печати
│     ├─ balki.stl
│     ├─ bok1.stl
│     ├─ bok2.stl
│     ├─ bok3.stl
│     ├─ bok4.stl
│     ├─ dno.stl
│     ├─ krysha.stl
│     ├─ palka1.stl
│     ├─ platforma__platforma.stl
│     └─ stolb.stl
│
├─ photos/                        # Фотографии макета/сборки
│  ├─ image.png
│  ├─ image1.png
│  ├─ image2.png
│  └─ image3.png
│
├─ schemes/                       # Схемы (блок/электрика/кинематика)
│  ├─ cubesat/
│  │  ├─ blockscheme.png
│  │  ├─ electricalcircuit.png
│  │  └─ electricscheme.png
│  │
│  └─ station/
│     ├─ blockscheme.png
│     ├─ electricalcircuit.png
│     ├─ electricscheme.png
│     └─ cinematicscheme.jpg
│
└─ README.md                      # Описание проекта
       

```

> По вопросам обращайтесь по Pull request'ам.
