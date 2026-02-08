# Решение кейса 1 от команды echo("637_NrF3D")

Репозиторий содержит код (Arduino `.ino`), 3D-модели/чертежи и схемы для двух частей проекта: **CUBESAT** и **STATION**.

## Структура репозитория

```text
.
├─ code/                          # Исходники прошивок (Arduino)[1]
│  ├─ CUBESAT/
│  │  └─ CUBESAT.ino              # Прошивка для модуля CUBESAT[1]
│  └─ STATION/
│     └─ STATION.ino              # Прошивка для наземной станции[1]
│
├─ models/                        # 3D-модели и документация к механике[1]
│  ├─ drawings/                   # PDF-чертежи деталей/сборок[1]
│  │  ├─ Сборка.pdf
│  │  ├─ balka_central - balka_central.pdf
│  │  ├─ bok002 (Solid) - bok002 (Solid).pdf
│  │  ├─ bok005 _ bok005.pdf
│  │  ├─ dno001 _ dno001.pdf
│  │  ├─ krysha001 _ krysha001.pdf
│  │  ├─ palka002 _ palka002.pdf
│  │  └─ platforma _ platforma.pdf
│  │
│  └─ stls/                       # Экспортированные STL для печати[1]
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
├─ schemes/                       # Схемы (блок/электрика/кинематика)[1]
│  ├─ cubesat/
│  │  ├─ blockscheme.png          # Блок-схема CUBESAT[1]
│  │  ├─ electricalcircuit.png    # Электрическая схема/цепь CUBESAT[1]
│  │  └─ electricscheme.png       # Электросхема CUBESAT[1]
│  │
│  └─ station/
│     ├─ blockscheme.png          # Блок-схема STATION[1]
│     ├─ electricalcircuit.png    # Электрическая схема/цепь STATION[1]
│     ├─ electricscheme.png       # Электросхема STATION[1]
│     └─ cinematicscheme.jpg      # Кинематическая схема STATION[1]
│
└─ README.md       

```

> По вопросам обращайтесь по Pull request'ам.