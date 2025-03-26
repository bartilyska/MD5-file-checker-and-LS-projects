
# Porównywanie plików i katalogów na podstawie skrótu MD5

## Opcje:
- **Pomoc**: `-h`
- **Wersja**: `-v`

## Uruchomienie skryptu

### Bez parametrów
Uruchomienie skryptu bez parametrów i opcji uruchomi okno dialogowe w **Zenity**. Użytkownik powinien:
1. Wybrać interesującą go opcję.
2. Wskazać pliki lub katalogi do porównania.
3. Otrzymać informację o zgodności plików/katalogów.
4. Ponownie zobaczyć okno dialogowe z wyborem opcji.

> Skrypt działa w pętli do momentu naciśnięcia **"ZAKOŃCZ"**.

### Z parametrami
Aby uruchomić skrypt z parametrami, należy podać **dwie pełne ścieżki** do plików lub katalogów. W przeciwnym wypadku skrypt poinformuje o **niepoprawnych argumentach**.

> Ścieżki można również podać po opcjach `-h` lub `-v`.

Po wprowadzeniu odpowiednich danych użytkownik otrzyma informację o zgodności plików/katalogów.

Aby ponownie wykonać skrypt, należy go uruchomić na nowo.

## Działanie

- **Pliki**: Porównywany jest skrót **MD5** oraz ich nazwy.
- **Katalogi**:
  1. Tworzone są skróty MD5 dla nazw wszystkich plików w katalogu.
  2. Do listy dodawane są nazwy podfolderów.
  3. Lista jest sortowana, a następnie tworzony jest unikalny skrót MD5 na jej podstawie.
  4. Skrót jest porównywany z drugim katalogiem.


