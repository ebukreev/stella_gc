# Сборщик мусора для языка программирования Stella

Копирующий сборщик мусора с 2мя поколениями.

## Использование

Для начала необходимо транслировать программу на языке stella на язык С:

```bash
docker run -i fizruk/stella compile < test.stella > out.c
```

После этого скомпилировать полученную программу вместе со средой времени исполнения и сборщиком мусора:

```bash
gcc -std=c11 out.c stella/runtime.c stella/gc.c -o test
```

Затем при запуске `test 4` в процессе работы будет выводиться следующая информация:

<details>
  <summary>GC State</summary>

```
After garbage collecting:
G0 state
Cycles count 1
Objects:
From: 0x5729fd396770  | To: 0x5729fd397770  | Size: 4096 bytes
G1 state
Cycles count 0
Objects:
        Address: 0x5729fd397780  | Fields: 0x5729fd3977a0 
        Address: 0x5729fd397798  | Fields: 0x5729fd3977b8 
        Address: 0x5729fd3977b0  | Fields: 0x5729fd3977d0 
        Address: 0x5729fd3977c8  | Fields: 0x5729f9ecb110 
        Address: 0x5729fd3977e0  | Fields: 0x5729fd397800 
        Address: 0x5729fd3977f8  | Fields: 0x5729fd397818 
        Address: 0x5729fd397810  | Fields: 0x5729fd397830 
        Address: 0x5729fd397828  | Fields: 0x5729fd397848 
        Address: 0x5729fd397840  | Fields: 0x5729fd397860 
        Address: 0x5729fd397858  | Fields: 0x5729fd397878 
        Address: 0x5729fd397870  | Fields: 0x5729fd397890 
        Address: 0x5729fd397888  | Fields: 0x5729fd3978a8 
        Address: 0x5729fd3978a0  | Fields: 0x5729fd3978c0 
        Address: 0x5729fd3978b8  | Fields: 0x5729fd3978d8 
        Address: 0x5729fd3978d0  | Fields: 0x5729fd3978f0 
        Address: 0x5729fd3978e8  | Fields: 0x5729fd397908 
        Address: 0x5729fd397900  | Fields: 0x5729fd397920 
        Address: 0x5729fd397918  | Fields: 0x5729fd397938 
        Address: 0x5729fd397930  | Fields: 0x5729fd397950 
        Address: 0x5729fd397948  | Fields: 0x5729fd397968 
        Address: 0x5729fd397960  | Fields: 0x5729fd397980 
        Address: 0x5729fd397978  | Fields: 0x5729fd397998 
        Address: 0x5729fd397990  | Fields: 0x5729fd3979b0 
        Address: 0x5729fd3979a8  | Fields: 0x5729fd3979c8 
        Address: 0x5729fd3979c0  | Fields: 0x5729fd3979e0 
        Address: 0x5729fd3979d8  | Fields: 0x5729fd3979f8 
        Address: 0x5729fd3979f0  | Fields: 0x5729fd397a10 
        Address: 0x5729fd397a08  | Fields: 0x5729f9ecb110 
        Address: 0x5729fd397a20  | Fields: 0x5729f9ec7832 
        Address: 0x5729fd397a38  | Fields: 0x5729f9ec74cd  0x5729fd3977e8 
        Address: 0x5729fd397a58  | Fields: 0x5729f9ecb110 
        Address: 0x5729fd397a70  | Fields: 0x5729f9ec71a9  0x5729fd397a98 
        Address: 0x5729fd397a90  | Fields: 0x5729f9ec7392  0x5729fd397a60 
        Address: 0x5729fd397ab0  | Fields: 0x5729fd397ad0 
        Address: 0x5729fd397ac8  | Fields: 0x5729fd397ae8 
        Address: 0x5729fd397ae0  | Fields: 0x5729fd397b00 
        Address: 0x5729fd397af8  | Fields: 0x5729fd397b18 
        Address: 0x5729fd397b10  | Fields: 0x5729fd397b30 
        Address: 0x5729fd397b28  | Fields: 0x5729fd397b48 
        Address: 0x5729fd397b40  | Fields: 0x5729fd397b60 
        Address: 0x5729fd397b58  | Fields: 0x5729fd397b78 
        Address: 0x5729fd397b70  | Fields: 0x5729fd397b90 
        Address: 0x5729fd397b88  | Fields: 0x5729fd397ba8 
        Address: 0x5729fd397ba0  | Fields: 0x5729fd397bc0 
        Address: 0x5729fd397bb8  | Fields: 0x5729fd397bd8 
        Address: 0x5729fd397bd0  | Fields: 0x5729fd397bf0 
        Address: 0x5729fd397be8  | Fields: 0x5729f9ecb110 
From: 0x5729fd397780  | To: 0x5729fd39b780  | Size: 16384 bytes
To
Objects:
From: 0x5729fd39b790  | To: 0x5729fd39f790  | Size: 16384 bytes
Roots:
        Index: 0     | Address: 0x7ffe49de5c70  | 0x5729fd397788 
        Index: 1     | Address: 0x7ffe49de5c30  | 0x5729fd397788 
        Index: 2     | Address: 0x7ffe49de5c08  | 0x5729f9ecb110 
        Index: 3     | Address: 0x7ffe49de5c00  | 0x5729fd3977e8 
        Index: 4     | Address: 0x7ffe49de5bf8  | 0x5729fd397a28 
        Index: 5     | Address: 0x7ffe49de5bb8  | 0x5729fd397a40 
        Index: 6     | Address: 0x7ffe49de5bc0  | 0x5729fd397a60 
        Index: 7     | Address: 0x7ffe49de5bc8  | 0x5729f9ecb110 
        Index: 8     | Address: 0x7ffe49de5bd0  | 0x5729fd397a60 
        Index: 9     | Address: 0x7ffe49de5ba0  | 0x5729fd3977e8 
        Index: 10    | Address: 0x7ffe49de5bb0  | 0x5729f9ecb110 
        Index: 11    | Address: 0x7ffe49de5b50  | 0x5729fd3977e8 
        Index: 12    | Address: 0x7ffe49de5b58  | 0x5729f9ecb110 
        Index: 13    | Address: 0x7ffe49de5b60  | 0x5729fd397a78 
        Index: 14    | Address: 0x7ffe49de5b68  | 0x5729f9ecb080 
        Index: 15    | Address: 0x7ffe49de5b70  | 0x5729fd397a98 
        Index: 16    | Address: 0x7ffe49de5b78  | 0x5729f9ecb090 
        Index: 17    | Address: 0x7ffe49de5b80  | 0x5729fd397a60 
        Index: 18    | Address: 0x7ffe49de5b30  | 0x5729fd397a60 
        Index: 19    | Address: 0x7ffe49de5b48  | 0x5729fd3977e8 
        Index: 20    | Address: 0x7ffe49de5b08  | 0x5729fd397950 
        Index: 21    | Address: 0x7ffe49de5b00  | 0x5729fd397ab8 
        Index: 22    | Address: 0x7ffe49de5af8  | 0x5729fd397a78 
        Index: 23    | Address: 0x7ffe49de5ab8  | 0x5729fd397a60 
        Index: 24    | Address: 0x7ffe49de5ac0  | 0x5729fd397ab8 
        Index: 25    | Address: 0x7ffe49de5ac8  | 0x5729fd397a98 
        Index: 26    | Address: 0x7ffe49de5ad0  | 0xfd397340     
        Index: 27    | Address: 0x7ffe49de5aa0  | 0x5729fd397ab8 
        Index: 28    | Address: 0x7ffe49de5ab0  | 0x5729fd397a60 
```

</details>

Состояние поколений и корней.

И статистика работы сборщика:

```
Garbage collector (GC) statistics:
Total memory allocation:  2768 bytes (165 objects)
Maximum residency:        2768 bytes (165 objects)
Total memory use:         359 reads and 0 writes
```