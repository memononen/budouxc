BodouX implementation in C
===

This is a simple implementation of [BudouX](https://google.github.io/budoux/) word boundary breaking for east asian languages.
The code tries to be a faitful implementation of the original algorithm, and uses the models from the orignal project.

Usage
--

```C
char const sentence[] = "私はその人を常に先生と呼んでいた。\n"
                             "だからここでもただ先生と書くだけで本名は打ち明けない。\n"
                             "これは世間を憚かる遠慮というよりも、その方が私にとって自然だからである。";

int len = strlen(sentence);

char* boundaries = malloc(len);
memset(boundaries, 0, len);

parse_boundaries_ja(sentence, boundaries);

for (int i = 0; i < len; i++) {
    if (boundaries[i]) printf("|");
    printf("%c", sentence[i]);
}

free(boundaries);
```
Outputs:
```
私は|その|人を|常に|先生と|呼んで|いた。|
だから|ここでも|ただ先生と|書くだけで|本名は|打ち明けない。|
これは|世間を|憚かる|遠慮と|いうよりも、|その方が|私に|とって|自然だからである。
```

Models
--
The `models` folder contains json files from BodouX project. They have been converted to C headers using the `covert.py` script:
```python
python .\convert.py .\models\zh-hant.json zh_hant model_zh_hant.h
```
