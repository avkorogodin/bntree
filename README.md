### Бинарное дерево, со взятием и удалением элементов по индексу.

Добавление элемента происходит за время O(log n).  
Взятие элемента по индексу происходит за время O(log n).  
Удаление элемента по индексу происходит за время O(log n).  

Для теста использубтся данные из файла test/write.txt, происходит считывание 1000000 строк и заполнение ими дерева.

Файл modify.txt содержит 1000000 записей в виде: индекс_удалениемого_элемента новая_строка_для_добавления  
Файл read.txt содержит 1000000 записей в виде: индекс_элемента строка_проверка  

### Как запускать:
```bash
make clean && make && ./objs/test 
```
