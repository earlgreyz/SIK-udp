# Sieci Komputerowe
## Zadanie 1

### Termin oddania
__8.05.2017, godzina 19.00__ _(liczy się czas na studentsie)_

### Programy
Należy przygotować parę programów – __klient__ i __serwer__ – służącą do komunikacji
pomiędzy komputerami na następujących zasadach:

1. __Klient__ to program, który po uruchomieniu __wysyła po UDP__ do serwera
  `znacznik czasu` oraz `literę`, a następnie w nieskończonej pętli __oczekuje
  na datagramy__ od serwera i __wypisuje ich treść__ na standardowe wyjście.

2. __Serwer__ zaś, gdy tylko otrzyma poprawny datagram, to __dopisuje__ do jego treści
  __zawartość wskazanego przy uruchomieniu pliku__ i tak uzupełniony datagram
  __rozsyła do pozostałych__ klientów, którzy przesłali coś do serwera
  w czasie ostatnich __2 minut__.


### Serwer
#### Uruchamianie

Serwer uruchamiamy poleceniem:

```
./server port nazwa_pliku
```

#### Parametry

* `port`        – numer portu, na którym serwer ma odbierać dane od klientów _(liczba dziesiętna)_
* `nazwa_pliku` – nazwa pliku, którego treść należy doklejać


### Klient
#### Uruchamianie

Klienta uruchamiamy poleceniem:

```
./client timestamp c host [port]
```

#### Parametry

* `timestamp` – znacznik czasu wysyłany w pakiecie
* `c`         – znak wysyłany w pakiecie
* `host`      – nazwa serwera, z którym należy się połączyć
* `port`      – opcjonalny numer portu serwera, z którym należy się połączyć _(liczba dziesiętna)_; 
  jeśli nie podano argumentu, jako numer portu powinna być przyjęta liczba `20160`


### Protokół

Komunikacja pomiędzy klientem i serwerem odbywa się po UDP. Klient po
uruchomieniu powinien wysłać datagram do serwera, a następnie w nieskończonej
pętli czekać na datagramy zwrotne.

__Komunikaty__ przesyłane pomiędzy serwerem i klientem powinny być następującej
postaci:

1. __Liczba 64-bitowa bez znaku__, podana w sieciowej kolejności bajtów, interpretowana 
  jako znacznik czasu, taki, jak zwracany przez time(), tzn. liczba sekund od godziny 0.00 1 stycznia 1970 UTC.

2. __Jeden znak__ (1 bajt).

3. W przypadku datagramów od serwera do klienta – __tekst wczytany z podanego pliku__. Tekst kończy się znakiem `NULL`. Można założyć, że w pliku nie znajduje się znak `NULL` i plik nie zmienia się w czasie wykonania programu.

Jeżeli rok zawarty w znaczniku czasu jest mniejszy niż `1717` lub większy niż
`4242`, to taki znacznik czasu uznajemy za niepoprawny.


### Wymagania szczegółowe

Programy powinny być odporne na podanie niepoprawnych parametrów lub złej liczby
parametrów. W takim przypadku należy wypisać stosowny komunikat o błędzie i
zakończyć program z kodem powrotu `1`. __Programy nie mogą wysyłać nieprawidłowych
danych.__

Programy powinny kompilować się i działać zarówno na maszynach 64-bitowych,
jak i 32-bitowych.

W przypadku otrzymania nieprawidłowych (złośliwych) danych przysłanych przez
sieć zarówno klient, jak i serwer powinny wypisać zrozumiały dla człowieka
komunikat na standardowe wyjście błędów, zawierający także adres nadawcy
błędnego datagramu, i działać dalej.

W serwerze można przyjąć __ograniczenie__ `42` __na liczbę jednocześnie obsługiwanych
klientów__.

W kliencie na standardowe wyjście należy wypisywać tylko komunikaty
otrzymane od serwera, bez żadnych dodatkowych informacji ani dodatkowych
znaków końca linii.

Rozwiązanie powinno:
* działać w środowisku Linux w laboratorium komputerowym
* być napisane w języku C lub C++ z wykorzystaniem interfejsu gniazd oraz,
  w serwerze, mechanizmu poll lub select (nie wolno tworzyć wielu wątków ani
  procesów, jak również korzystać z `libevent` ani `boost::asio`)
* kompilować się za pomocą GCC (polecenie gcc lub g++), wśród parametrów
  należy użyć -Wall -O2

Wysyłanie datagramów nie powinno blokować ich odbierania. Jeżeli datagramy
nadsyłane są szybciej, niż udaje się wysyłać odpowiedzi, to powinny być
umieszczane w buforze o rozmiarze 4096 datagramów, a jeżeli zabraknie miejsca
w buforze, to należy nadpisać najstarszy datagram. Datagramy do każdego
z klientów należy zawsze wysyłać w kolejności zgodnej z tą, w jakiej były
odebrane przez serwer.

Ponieważ rozwiązanie będzie automatycznie testowane, należy ściśle
przestrzegać podanej specyfikacji (nazwy plików, protokół komunikacji,
komunikaty na standardowym wyjściu, kody powrotu).


### Oddawanie rozwiązania

Jako rozwiązanie należy dostarczyć pliki źródłowe `client.c` i `server.c` (albo
`client.cc` i `server.cc`), które należy umieścić na studentsie w katalogu

```
/home/students/inf/PUBLIC/SIK/students/ab123456/zadanie1/
```

gdzie `ab123456` to standardowy login osoby oddającej rozwiązanie, używany
na maszynach wydziału, wg schematu: inicjały, nr indeksu.

W katalogu mogą też znajdować się dowolne pliki nagłówkowe o nazwach
*.h i katalog private z plikami, które nie będą sprawdzane. Nie należy
umieszczać plików innych niż wymienione w treści zadania poza katalogiem
private.


### Ocena

Ocena zadania będzie składała się z trzech składników:

* ocena wzrokowa działania programu (20%)
* testy automatyczne (60%)
* jakość kodu źródłowego (20%)

Każda rozpoczęta minuta spóźnienia będzie kosztowała 0,01 punktu.
