/**
* @file Statki.c
* @author Adam Steciuk
* @brief Napisac program realizujacy gre Okrety w wersji dla jednego gracza, oponentem ma byc komputer-program.
*/

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

//! przechowuje informacje o tym gdzie w danym momencie znajduje sie kursor gracza
typedef struct Kursor
{
    int xPos;	/**< iksowa wspolrzedna kursora */
    int yPos;	/**< igrekowa wspolrzedna kursora */
} Kursor;

//! przechowuje informacje o poszczegolnych statkach gracza i przeciwnika
typedef struct Statek
{
    int dlugosc;		/**< ilumasztowy statek */
    int orient;			/**< 0 - statek poziomo, 1 - pionowo */
    int czyZatopiony;	/**< 0 - niezatopiony, 1 - zatopiony */
    int xPos;			/**< iksowa wspolrzedna pierwszego pola statku */
    int yPos;			/**< igrekowa wspolrzedna pierwszego pola statku */
    int ileTrafien;		/**< ile masztow statku zostalo trafionych */
}Statek;

//! rozpoczyna gre wlasciwa
/*!
\param kursor wskaznik na kursor
\param pPlanszaGracza tablica 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param pPlanszaPrzeciw tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param xPosPlanszaPrzeciw przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param poziom poziom trudnosci przeciwnika, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param tryb czy gra przeciwko kumputerowi czy na dwoch graczy, opis w main()
\param pPlanszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\param ktoryGracz przekazuje informacje ktorego gracz jako ostatni wykonal ruch przed zapisem gry, opis w main()
\param czyNowaGra czy gra zostala wczytana czy rozpoczeta na nowo
\return 0 - gra zakonczona, 4 - gracz 1 zapisal gre, 5 - gracz 2 zapisal gre
*/
int graj(Kursor *kursor, int *pPlanszaGracza, int liczbaKolumn, int liczbaWierszy, Statek *statkiGracza, int *pPlanszaPrzeciw, int xPosPlanszaPrzeciw, Statek *statkiPrzeciw, int poziom, int czyTorus, int tryb, int *pPlanszaPomocnicza, int ktoryGracz, int czyNowaGra);
//! obsluguje menu glowne
/*!
\param kursor wskaznik na kursor
\param poziom poziom trudnosci przeciwnika, opis w main()
\param wielkosc wybrana w menu wieloksc planszy
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param tryb czy gra przeciwko kumputerowi czy na dwoch graczy, opis w main()
\return 1 - start gry, 2 - wczytanie gry, 4 - wyjscie z gry
*/
int menu(Kursor *kursor, int *poziom, int* wielkosc, int *czyTorus, int *tryb);
//! inicjalizuje i ustawia ncurses, wylacza wyswietlanie wciskanych klawiszy w termianlu, wylacza kursor terminala
void ustawEkran();
//! przesowa kursor na podane wspolrzedne planszy, NIE RZECZYWISTE WSPOLRZEDNE OKNA TERMINALA, blkuje mozliwosc wyjechania kursorem poza plansze
/*!
\param yPos igrekowa wspolrzedna
\param xPos iksowa wspolrzedna
\param kursor wskaznik na kursor
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param xPosPrzesun przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
*/
void przesunKursor(int yPos, int xPos, Kursor *kursor, int xPosPrzesun, int liczbaKolumn, int liczbaWierszy);
//! obsluguje wejscie uzytkownika i wywoluje przesunKursor()
/*!
\param wejscie klawisz wcisniety przez uzytkownika
\param kursor wskaznik na kursor
\param xPosPrzesun przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
*/
void ruchKursora(int wejscie, Kursor *kursor, int xPosPrzesun, int liczbaKolumn, int liczbaWierszy, int czyTorus);
//! rysuje kursor uzywany podczas budowania statkow < >
/*!
\param kursor wskaznik na kursor
\param statek wskaznik na obecnie budowany statek
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param xPos przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
*/
void rysujObecnaPozycjeBudowanie(Kursor *kursor, Statek *statek, int czyTorus, int liczbaKolumn, int liczbaWierszy, int xPos);
//! rysuje kursor uzywany podczas strzelania > <
/*!
\param kursor wskaznik na kursor
*/
void rysujObecnaPozycjeAtak(Kursor *kursor);
//! przypisuje parametrom statkow wartosci domyslne
/*!
\param statki 10-cio elementowa tablica zawierajaca statki
*/
void stworzStatki(Statek *statki);
//! obsluguje budowanie statkow gracza
/*!
\param kursor wskaznik na kursor
\param pPlanszaGracza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param xPos przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\return 0 - wybudowano wszystkie statki, 1 - gracz wyszedl do menu
*/
int budujStatkiGracza(Kursor *kursor, int *pPlanszaGracza, int liczbaKolumn, int liczbaWierszy, Statek *statkiGracza, int czyTorus, int xPos);
//! buduje konkretny statek, w przypadku nieprawidlowej proby budowania wyswietla komunikat
/*!
\param kursor wskaznik na kursor
\param pPlansza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param statek wskaznik na obecnie budowany statek
\param xPos przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\return 0 - udalo sie wybudowac statek, 1 - nie mozna danym miejscu wybudowac statku
*/
int budujStatekGracza(Statek *statek, int *pPlansza, Kursor *kursor, int liczbaKolumn, int liczbaWierszy, int xPos);
//! obraca statek
/*!
\param statek wskaznik na obecnie budowany statek
*/
void obrocStatek(Statek *statek);
//! obsluguje budowanie statkow przeciwnika, losuje ich pozaycje
/*!
\param pPlanszaPrzeciw tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param xPosPlanszy przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
*/
void budujStatkiPrzeciw(int *pPlanszaPrzeciw, int liczbaKolumn, int liczbaWierszy, int xPosPlanszy, Statek *statkiPrzeciw, int czyTorus);
//! buduje konkretny statek i wyswietla go na planszy gracza
/*!
\param statek wskaznik na obecnie budowany statek
\param pPlansza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param xPosPlanszy przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\return 0 - udalo sie wybudowac statek, 1 - nie mozna danym miejscu wybudowac statku
*/
int budujStatekPrzeciw(Statek *statek, int *pPlansza, int liczbaKolumn, int liczbaWierszy, int xPosPlanszy);
//! umieszcza 0 w kazdym polu planszy gracza lub przeciwnika, plansze - patrz: main()
/*!
\param plansza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza lub przeciwnika, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
*/
void wyczyscPlansze(int *plansza, int liczbaKolumn, int liczbaWierszy);
//! rysuje zawartosc pol planszy, plansze - patrz: main()
/*!
\param plansza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza lub przeciwnika, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param kursor wskaznik na kursor
\param xPos przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param czyWyswStatki 0 - nie wyswietla statkow na planszy, 1 - wyswietla
*/
void odswiezPlansze(int *plansza, int liczbaKolumn, int liczbaWierszy, Kursor *kursor, int xPos, int czyWyswStatki);
//! rozpoczyna faze atakow
/*!
\param planszaGracza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param PlanszaPrzeciw tablice 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param kursor wskaznik na kursor
\param xPosPlanszaPrzeciw przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param poziom poziom trudnosci przeciwnika, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param pPlanszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\return 1 - gracz wyszedl do glównego menu, 2 - gracz wygral, 3 - gracz przegral, 4 - gracz 1 zapisal gre, 5 - gracz 2 zapisal gre
*/
int atakiSolo(int *planszaGracza, int *planszaPrzeciw, Kursor *kursor, int xPosPlanszaPrzeciw, Statek *statkiGracza, Statek *statkiPrzeciw, int licbaKolumn, int liczbaWierszy, int poziom, int czyTorus, int *pPlanszaPomocnicza);
//! obsluguje strzal gracza, informauje o spudlowaniu
/*!
\param planszaPrzeciw tablice 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param kursor wskaznik na kursor
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param xPosPlanszaPrzeciw przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\return 0 - gracz spudlowal, 1 - gracz wyszedl do glownego menu, 2 - gracz zatopil wszystkie statki
*/
int strzalGracza(int *planszaPrzeciw, Kursor *kursor, Statek *statkiPrzeciw, int xPosPlanszaPrzeciw, int licbaKolumn, int liczbaWierszy, int czyTorus);
//! obsluguje trafienia w statki przeciwnika i sprawdza czy trafiony statek zostal zatopiony - wyswietla odpowieni komunikat
/*!
\param plansza tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza lub przeciwnika, opis w main()
\param statki 10-cio elementowa tablica zawierajaca statki przeciwnika
\param kursor wskaznik na kursor
\param xPosPlansza przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
*/
void trafienie(int *plansza, Statek *statki, Kursor *kursor, int xPosPlansza, int liczbaKolumn, int liczbaWierszy);
//! losowy strzal przeciwnika - poziom trudnosci 1
/*!
\param planszaGracza tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param kursor wskaznik na kursor
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\return 0 - przeciwnik spudlowal, 3 - przeciwnik zatopil wszystkie statki
*/
int strzalLosowy(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int liczbaWierszy, Kursor *kursor, int czyTorus);
//! logiczny strzal przeciwnika - poziom trudnosci 2, 3, 4
/*!
\param planszaGracza tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param kursor wskaznik na kursor
\param planszaPomocnicza tablica 2D 14x14 zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\param poziom poziom trudnosci przeciwnika, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param pPlanszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\return 0 - przeciwnik spudlowal, 3 - przeciwnik zatopil wszystkie statki
*/
int strzalSensowny(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int liczbaWierszy, Kursor *kursor, int *planszaPomocnicza, int poziom, int czyTorus, int *pPlanszaPomocnicza);
//! w polach planszy pomocniczej sasiadujacyh naroznikami z wybranym polem wpisuje 5 a w sasiadujacyh horyzontalnie i wertykalnie wpisuje 6, plansza pomocnicza patrz: main()
/*!
\param planszaGracza tablice 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param nrWiersza wspolrzedna igrekowa trafionego przez przeciwnika pola
\param nrKolumny wspolrzedna iksowa trafionego przez przeciwnika pola
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param planszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
*/
void obrysujRogi(int *planszaGracza, int nrWiersza, int nrKolumny, int liczbaKolumn, int *planszaPomocnicza);
//! w polach planszy pomocniczej otaczajacych trafiony statek wpisuje 5, plansza pomocnicza patrz: ataki()
/*!
\param planszaGracza tablice 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param planszaPomocnicza tablica 2D 14x14 zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
*/
void obrysujStatek(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int *planszaPomocnicza);
//! sprawdza czy sa pola o zwiekszonym prawdopodobienstwie zawierania statku jezeli tak losuje jedno z tych pol
/*!
\param planszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param nrWiersza wspolrzedna igrekowa trafionego przez przeciwnika pola
\param nrKolumny wspolrzedna iksowa trafionego przez przeciwnika pola
*/
void czySaTypy(int *planszaPomocnicza, int liczbaKolumn, int liczbaWierszy, int *nrWiersza, int *nrKolumny);
//! wyswietla ascii art glownego menu
void menuGrafika();
//! rysuje plansze przesunieta o xPos od punktu (0,0)
/*!
\param xPos przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
*/
void rysujPlansze(int xPos, int liczbaKolumn, int liczbaWierszy);
//! wyswietla ascii art wygranej
void wygrana();
//! wyswietla ascii art przegranej
void przegrana();
//! konwertuje igrekowa wspolrzedna planszy na rzeczywsita wspolrzedna okna terminala
/*!
\param yPos igrekowa wspolrzedna planszy
*/
int yKon(int yPos);
//! konwertuje iksowa wspolrzedna planszy na rzeczywsita wspolrzedna okna terminala
/*!
\param xPos iksowa wspolrzedna planszy
*/
int xKon(int xPos);
//! konwertuje igrekowa rzeczywsita wspolrzedna okna terminala na wspolrzedna planszy
/*!
\param yPos igrekowa rzeczywsita wspolrzedna okna terminala
*/
int yKonP(int yPos);
//! konwertuje iksowa rzeczywsita wspolrzedna okna terminala na wspolrzedna planszy
/*!
\param xPos iksowa rzeczywsita wspolrzedna okna terminala
*/
int xKonP(int xPos);
//! odpowiada za ustawienia planszy w menu glownym
/*!
\param kursor wskaznik na kursor
\param wielkosc wybrana w menu wieloksc planszy
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
*/
void ustawieniaPlanszy(Kursor *kursor, int *wielkosc, int *czyTorus);
//! odpowiada za ustawienia trudnosci w menu glownym
/*!
\param kursor wskaznik na kursor
\param poziom poziom trudnosci przeciwnika, opis w main()
*/
void ustawieniaTrudnosci(Kursor *kursor, int *poziom);
//! odpowiada za wybor trybu SP/MP w menu glownym
/*!
\param kursor wskaznik na kursor
\param poziom poziom trudnosci przeciwnika, opis w main()
*/
void ustawieniaTrybu(Kursor *kursor, int *tryb, int *poziom);
//! buduje konkretny statek na planszy torusowej, w przypadku nieprawidlowej proby budowania wyswietla komunikat
/*!
\param kursor wskaznik na kursor
\param pPlansza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param statek wskaznik na obecnie budowany statek
\param xPos przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\return 0 - udalo sie wybudowac statek, 1 - nie mozna danym miejscu wybudowac statku
*/
int budujStatekGraczaTorus(Statek *statek, int *pPlansza, Kursor *kursor, int liczbaKolumn, int liczbaWierszy, int xPos);
//! buduje konkretny statek przeciwnika na planszy torusowej i wyswietla go na planszy gracza
/*!
\param statek wskaznik na obecnie budowany statek
\param pPlansza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param xPosPlanszy przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\return 0 - udalo sie wybudowac statek, 1 - nie mozna danym miejscu wybudowac statku
*/
int budujStatekPrzeciwTorus(Statek *statek, int *pPlansza, int liczbaKolumn, int liczbaWierszy, int xPosPlanszy);
//! w polach torusowej planszy pomocniczej otaczajacych trafiony statek wpisuje 5, plansza pomocnicza patrz: ataki()
/*!
\param planszaGracza tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param planszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
*/
void obrysujStatekTorus(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int liczbaWierszy, int *planszaPomocnicza);
//! w polach torusoej planszy pomocniczej otaczajacych trafiony statek wpisuje 5, plansza pomocnicza patrz: ataki()
/*!
\param planszaGracza tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param planszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
*/
void obrysujRogiTorus(int *planszaGracza, int nrWiersza, int nrKolumny, int liczbaKolumn, int liczbaWierszy, int *planszaPomocnicza);
//! rozpoczyna faze atakow miedzy dwoma graczami
/*!
\param planszaGracza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param PlanszaPrzeciw tablice 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param kursor wskaznik na kursor
\param xPosPlanszaPrzeciw przesuniecie w osi X planszy przeciwnika wzgledem planszy gracza
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param poziom poziom trudnosci przeciwnika, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param ktoryGracz przekazuje informacje ktorego gracz jako ostatni wykonal ruch przed zapisem gry, opis w main()
\return 1 - gracz wyszedl do glównego menu, 2 - gracz wygral, 3 - gracz 2 wygral, 4 - gracz 1 zapisal gre, 5 - gracz 2 zapisal gre
*/
int atakiMulti(int *planszaGracza, int *planszaPrzeciw, Kursor *kursor, int xPosPlanszaPrzeciw, Statek *statkiGracza, Statek *statkiPrzeciw, int licbaKolumn, int liczbaWierszy, int poziom, int czyTorus, int ktoryGracz);
//! wyswietla ascii art wygranej gracza 1
void wygrana1();
//! wyswietla ascii art wygranej gracza 2
void wygrana2();
//! alokuje pamiec na tablice podanych rozmiarow
/*!
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\return wskaznik na zaalokowany obszar pamieci
*/
int * zalokuj(int liczbaWierszy, int liczbaKolumn);
//! zapisuje obecny stan rozgrywki w pliku tekstowym
/*!
\param planszaGracza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param pPlanszaPrzeciw tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param poziom poziom trudnosci przeciwnika, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param tryb czy gra przeciwko kumputerowi czy na dwoch graczy, opis w main()
\param pPlanszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\param gracz przekazuje informacje ktorego gracz jako ostatni wykonal ruch przed zapisem gry, opis w main()
*/
void zapiszStan(int tryb, int czyTorus, int poziom, int liczbaKolumn, int liczbaWierszy, int gracz, Statek *statkiGracza, Statek *statkiPrzeciw, int *planszaGracza, int *planszaPrzeciw, int *planszaPomoc);
//! wczytuje obecny stan rozgrywki z pliku tekstowego
/*!
\param planszaGracza tablica 2D zawierajaca informacje o tym co znajduje sie na danym polu planszy gracza, opis w main()
\param liczbaKolumn liczba kolumn planszy powiekszona o 4, opis w main()
\param liczbaWierszy liczba wierszy planszy powiekszona o 4, opis w main()
\param statkiGracza 10-cio elementowa tablica zawierajaca statki gracza
\param pPlanszaPrzeciw tablice 2D 14x14 zawierajaca informacje o tym co znajduje sie na danym polu planszy przeciwnika, opis w main()
\param statkiPrzeciw 10-cio elementowa tablica zawierajaca statki przeciwnika
\param poziom poziom trudnosci przeciwnika, opis w main()
\param czyTorus czy plansza jest kwadratem czy torusem, opis w main()
\param tryb czy gra przeciwko kumputerowi czy na dwoch graczy, opis w main()
\param pPlanszaPomocnicza tablica 2D zawierajaca infromacje o prawdopodobienstwie obecnosci statku w danym polu, opis w main()
\param gracz przekazuje informacje ktorego gracz jako ostatni wykonal ruch przed zapisem gry, opis w main()
\return 0 - niepowodzenie, 1 - powodzenie
*/
int wczytajStan(int *tryb, int *czyTorus, int *poziom, int *liczbaKolumn, int *liczbaWierszy, int *gracz, Statek *statkiGracza, Statek *statkiPrzeciw, int **planszaGracza, int **planszaPrzeciw, int **planszaPomoc);


int main()
{
    ustawEkran();

    srand(time(0));
    Kursor kursor;

    int tryb = 1; /**- tryb - 1 - singleplayer, 2 - multiplayer */
    int wielkoscPlanszy = 10; 
    int czyTorus = -1; /**- tryb - -1 - kwadrat, 1 - torus */
    int poziom = 4;	/**- poziom - Poziom trudnosci domyslnie ustawiony na trudny, 1 - bardzo latwy, 2 - latwy, 3 - sredni, 4 - trudny */

    int ktoryGracz = 1; /**- ktory gracz wykonywal ruch w trakcie zapisu - 1 - gracz 1, 2 - gracz 2 */

    int xPosPlanszaPrzeciw = 60 + (wielkoscPlanszy - 10) * 4;
    int liczbaKolumn = wielkoscPlanszy + 4;  /**- liczbaKolumn, liczbaWierszy - wielkosc planszy powiekszona o 4 aby latwiej sprawdzac mozliwosc budowania statku na przy granicach planszy oraz by wymazywac kursor po wyjechaniu poza plansze przy budowaniu czteromasztowca uniwersalna funkcja */
    int liczbaWierszy = wielkoscPlanszy + 4;

    int *pPlanszaGracza = NULL;  /**- planszaGracza, planszaPrzeciw - tablice 2D zawierajace informacje o tym co znajduje sie na danym polu planszy, 0 - pole puste, 1 - statek, 2 - trafiony statek, 3 - chybiony strzal */
    int *pPlanszaPrzeciw = NULL;
    int *pPlanszaPomocnicza = NULL; /**- planszaPomocnicza - tablica 2D 14x14 zawierajaca infromacje o polach w ktorych NAPEWNO NIE MOZE byc statku lub PRAWDOPODOBNIE JEST statek, 5 - nie moze byc statku, 6 - prawdopodobnie jest */

    Statek *statkiGracza = NULL;
    Statek noweStatkiGracza[10];
    statkiGracza = noweStatkiGracza;

    Statek *statkiPrzeciw = NULL;
    Statek noweStatkiPrzeciw[10];
    statkiPrzeciw = noweStatkiPrzeciw;

    int menuWybor;
    int stan;

    do
    {
        menuWybor = menu(&kursor, &poziom, &wielkoscPlanszy, &czyTorus, &tryb);

        if(menuWybor == 2)
        {
            if(wczytajStan(&tryb, &czyTorus, &poziom, &liczbaKolumn, &liczbaWierszy, &ktoryGracz, statkiGracza, statkiPrzeciw, &pPlanszaGracza, &pPlanszaPrzeciw, &pPlanszaPomocnicza) == 1)
            {
                erase();
                mvprintw(14, 40, "GAME LOADED");
                mvprintw(15, 40, "press any key...");
                getch();
                erase();

                xPosPlanszaPrzeciw = 60 + (liczbaKolumn - 14) * 4;

                stan = graj(&kursor, pPlanszaGracza, liczbaKolumn, liczbaWierszy, statkiGracza, pPlanszaPrzeciw, xPosPlanszaPrzeciw, statkiPrzeciw, poziom, czyTorus, tryb, pPlanszaPomocnicza, ktoryGracz, 0);

                if(stan == 4)
                {
                    zapiszStan(tryb, czyTorus, poziom, liczbaKolumn, liczbaWierszy, 1, statkiGracza, statkiPrzeciw, pPlanszaGracza, pPlanszaPrzeciw, pPlanszaPomocnicza);
                }
                else if(stan == 5)
                {
                    zapiszStan(tryb, czyTorus, poziom, liczbaKolumn, liczbaWierszy, 2, statkiGracza, statkiPrzeciw, pPlanszaGracza, pPlanszaPrzeciw, pPlanszaPomocnicza);
                }
            }
            else
            {
                erase();
                mvprintw(14, 40, "ERROR - FILE MISSING, DAMAGED OR IN WRONG FORMAT");
                mvprintw(15, 40, "press any key...");
                getch();
                erase();
            }

            free(pPlanszaGracza);
            free(pPlanszaPrzeciw);
            free(pPlanszaPomocnicza);
        }
        else if(menuWybor == 1)
        {
            xPosPlanszaPrzeciw = 60 + (wielkoscPlanszy - 10) * 4;

            liczbaKolumn = wielkoscPlanszy + 4;  /**- liczbaKolumn, liczbaWierszy - wielkosc planszy powiekszona o 4 aby latwiej sprawdzac mozliwosc budowania statku na przy granicach planszy oraz by wymazywac kursor po wyjechaniu poza plansze przy budowaniu czteromasztowca uniwersalna funkcja */
            liczbaWierszy = wielkoscPlanszy + 4;

            pPlanszaGracza = zalokuj(liczbaWierszy, liczbaKolumn);
            pPlanszaPrzeciw = zalokuj(liczbaWierszy, liczbaKolumn);
            pPlanszaPomocnicza = zalokuj(liczbaWierszy, liczbaKolumn);

            wyczyscPlansze(pPlanszaPomocnicza, liczbaKolumn, liczbaWierszy);
            wyczyscPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy);
            wyczyscPlansze(pPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);

            stworzStatki(statkiGracza);
            stworzStatki(statkiPrzeciw);

            stan = graj(&kursor, pPlanszaGracza, liczbaKolumn, liczbaWierszy, statkiGracza, pPlanszaPrzeciw, xPosPlanszaPrzeciw, statkiPrzeciw, poziom, czyTorus, tryb, pPlanszaPomocnicza, ktoryGracz, 1);

            if(stan == 4)
            {
                zapiszStan(tryb, czyTorus, poziom, liczbaKolumn, liczbaWierszy, 1, statkiGracza, statkiPrzeciw, pPlanszaGracza, pPlanszaPrzeciw, pPlanszaPomocnicza);
            }
            else if(stan == 5)
            {
                zapiszStan(tryb, czyTorus, poziom, liczbaKolumn, liczbaWierszy, 2, statkiGracza, statkiPrzeciw, pPlanszaGracza, pPlanszaPrzeciw, pPlanszaPomocnicza);
            }

            free(pPlanszaGracza);
            free(pPlanszaPrzeciw);
            free(pPlanszaPomocnicza);
        }
    }while(menuWybor != 4);

    endwin();
    return 0;
}

int * zalokuj(int liczbaWierszy, int liczbaKolumn)
{
    int * plansza = (int *)malloc(liczbaWierszy * liczbaKolumn * sizeof(int));
    return plansza;
}

int wczytajStan(int *tryb, int *czyTorus, int *poziom, int *liczbaKolumn, int *liczbaWierszy, int *gracz, Statek *statkiGracza, Statek *statkiPrzeciw, int **planszaGracza, int **planszaPrzeciw, int **planszaPomoc)
{
    FILE *save01;
    if((save01 = fopen("save01.txt", "r")) == NULL)
    {
        return 0;
    }

    if(fscanf(save01, "%d %d %d %d %d %d", tryb, czyTorus, poziom, liczbaKolumn, liczbaWierszy, gracz) != 6)
    {
        return 0;
    }

    for(int i = 0; i<10; i++)
    {
        if(fscanf(save01, "%d %d %d %d %d %d", &(statkiGracza->dlugosc), &(statkiGracza->orient), &(statkiGracza->czyZatopiony), &(statkiGracza->yPos), &(statkiGracza->xPos), &(statkiGracza->ileTrafien)) != 6)
        {
            return 0;
        }
        statkiGracza ++;
    }

    for(int i = 0; i<10; i++)
    {
        if(fscanf(save01, "%d %d %d %d %d %d", &(statkiPrzeciw->dlugosc), &(statkiPrzeciw->orient), &(statkiPrzeciw->czyZatopiony), &(statkiPrzeciw->yPos), &(statkiPrzeciw->xPos), &(statkiPrzeciw->ileTrafien)) != 6)
        {
            return 0;
        }
        statkiPrzeciw ++;
    }

    *planszaGracza = zalokuj(*liczbaWierszy, *liczbaKolumn);
    *planszaPrzeciw = zalokuj(*liczbaWierszy, *liczbaKolumn);
    *planszaPomoc = zalokuj(*liczbaWierszy, *liczbaKolumn);

    int temp;
    for(int i=0; i < *liczbaWierszy; i++)
    {
        for(int j = 0; j < *liczbaWierszy; j++)
        {
            if((fscanf(save01, "%d", &temp)) != 1)
            {
                return 0;
            }

           *((*planszaGracza) + i * (*liczbaKolumn) + j) = temp;
        }
    }

    for(int i=0; i < *liczbaWierszy; i++)
    {
        for(int j = 0; j < *liczbaWierszy; j++)
        {
            if((fscanf(save01, "%d", &temp)) != 1)
            {
                return 0;
            }

            *((*planszaPrzeciw) + i * (*liczbaKolumn) + j) = temp;
        }
    }

    for(int i=0; i < *liczbaWierszy; i++)
    {
        for(int j = 0; j < *liczbaWierszy; j++)
        {
            if((fscanf(save01, "%d", &temp)) != 1)
            {
                return 0;
            }

            *((*planszaPomoc) + i * (*liczbaKolumn) + j) = temp;
        }
    }


//    mvprintw(29, 50, "%d %d %d %d %d %d", *tryb, *czyTorus, *poziom, *liczbaKolumn, *liczbaWierszy, *gracz);
//    getch();

    fclose(save01);
    return 1;
}

void zapiszStan(int tryb, int czyTorus, int poziom, int liczbaKolumn, int liczbaWierszy, int gracz, Statek *statkiGracza, Statek *statkiPrzeciw, int *planszaGracza, int *planszaPrzeciw, int *planszaPomoc)
{
    erase();
    FILE *save01;
    if((save01 = fopen("save01.txt", "w")) == NULL)
    {
        mvprintw(14, 40, "ERROR - CANNOT OPEN FILE");
        mvprintw(15, 40, "press any key...");
        getch();
        erase();
        return;
    }

    fprintf(save01, "%d\n%d\n%d\n%d\n%d\n%d\n", tryb, czyTorus, poziom, liczbaKolumn, liczbaWierszy, gracz);

    for(int i = 0; i<10; i++)
    {
        fprintf(save01, "%d %d %d %d %d %d\n", statkiGracza->dlugosc, statkiGracza->orient, statkiGracza->czyZatopiony, statkiGracza->yPos, statkiGracza->xPos, statkiGracza->ileTrafien);
        statkiGracza ++;
    }

    for(int i = 0; i<10; i++)
    {
        fprintf(save01, "%d %d %d %d %d %d\n", statkiPrzeciw->dlugosc, statkiPrzeciw->orient, statkiPrzeciw->czyZatopiony, statkiPrzeciw->yPos, statkiPrzeciw->xPos, statkiPrzeciw->ileTrafien);
        statkiPrzeciw ++;
    }

    for(int i = 0; i<liczbaWierszy; i++)
    {
        for(int j = 0; j<liczbaKolumn; j++)
        {
              fprintf(save01, "%d ", *(planszaGracza + i * liczbaKolumn + j));
        }
        fprintf(save01, "\n");
    }

    for(int i = 0; i<liczbaWierszy; i++)
    {
        for(int j = 0; j<liczbaKolumn; j++)
        {
              fprintf(save01, "%d ", *(planszaPrzeciw + i * liczbaKolumn + j));
        }
        fprintf(save01, "\n");
    }

    for(int i = 0; i<liczbaWierszy; i++)
    {
        for(int j = 0; j<liczbaKolumn; j++)
        {
              fprintf(save01, "%d ", *(planszaPomoc + i * liczbaKolumn + j));
        }
        fprintf(save01, "\n");
    }

    fclose(save01);

    mvprintw(14, 40, "GAME SAVED");
    mvprintw(15, 40, "press any key...");
    getch();
    erase();

    return;
}

void ustawEkran()
{
    initscr();
    noecho();
    refresh();
    curs_set(0);
}

void przesunKursor(int yPos, int xPos, Kursor *kursor, int xPosPrzesun, int liczbaKolumn, int liczbaWierszy)
{
    if(yPos >= yKon(1) && yPos <= yKon(liczbaWierszy - 4))
    {
        kursor->yPos = yPos;
    }

    if(xPos >= xKon(1) + xPosPrzesun && xPos <= xKon(liczbaKolumn - 4) + xPosPrzesun)
    {
        kursor->xPos = xPos;
    }

    move(kursor->yPos, kursor->xPos);
}

void ruchKursora(int wejscie, Kursor *kursor, int xPosPrzesun, int liczbaKolumn, int liczbaWierszy, int czyTorus)
{


    switch (wejscie)
    {
        case 'W':
            if(czyTorus == 1 && kursor->yPos == yKon(1))
            {
                przesunKursor(yKon(liczbaWierszy - 4), kursor->xPos, kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }
            else
            {
                przesunKursor(kursor->yPos - yKon(1), kursor->xPos, kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }

            break;

        case 'S':
            if(czyTorus == 1 && kursor->yPos == yKon(liczbaWierszy - 4))
            {
                przesunKursor(yKon(1), kursor->xPos, kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }
            else
            {
                przesunKursor(kursor->yPos + yKon(1), kursor->xPos, kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }

            break;

        case 'A':
            if(czyTorus == 1 && kursor->xPos <= (xKon(1) + xPosPrzesun))
            {
                przesunKursor(kursor->yPos, xKon(liczbaKolumn - 4) + xPosPrzesun, kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }
            else
            {
                przesunKursor(kursor->yPos, kursor->xPos - xKon(1), kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }

            break;

        case 'D':
            if(czyTorus == 1 && kursor->xPos >= (xKon(liczbaKolumn - 4) + xPosPrzesun))
            {
                przesunKursor(kursor->yPos, xKon(1) + xPosPrzesun, kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }
            else
            {
                przesunKursor(kursor->yPos, kursor->xPos + xKon(1), kursor, xPosPrzesun, liczbaKolumn, liczbaWierszy);
            }
            break;

        default:
            break;
    }
}

int yKon(int yPos)
{
    return 2*yPos;
}

int xKon(int xPos)
{
    return 4*xPos;
}

int yKonP(int yPos)
{
    return yPos/2;
}

int xKonP(int xPos)
{
    return xPos/4;
}

void stworzStatki(Statek *statki)
{
    for(int i=0; i<4; i++)
    {
        statki->dlugosc = 1;
        statki++;
    }

    for(int i=0; i<3; i++)
    {
        statki->dlugosc = 2;
        statki++;
    }

    for(int i=0; i<2; i++)
    {
        statki->dlugosc = 3;
        statki++;
    }

    for(int i=0; i<1; i++)
    {
        statki->dlugosc = 4;
        statki++;
    }

    statki -= 10;

    for(int i=0; i<10; i++)
    {
        statki->orient = 0;
        statki->czyZatopiony = 0;
        statki->xPos = 0;
        statki->yPos = 0;
        statki->ileTrafien = 0;
        statki++;
    }

}

void obrocStatek(Statek *statek)
{
    if(statek->orient == 0)
    {
        statek->orient = 1;
    }else
    {
        statek->orient = 0;
    }
}

void wyczyscPlansze(int *plansza, int liczbaKolumn, int liczbaWierszy)
{
    for(int i = 0; i<liczbaKolumn; i++)
    {
        for(int j = 0; j<liczbaWierszy; j++)
        {
            (*(plansza + (i * liczbaKolumn + j)) = 0);
        }
    }

}

void odswiezPlansze(int *plansza, int liczbaKolumn, int liczbaWierszy, Kursor *kursor, int xPos, int czyWyswStatki)
{
    for(int i=1; i < liczbaKolumn; i++)
    {
        for(int j=1; j < liczbaWierszy; j++)
        {
                if((*(plansza + (i * liczbaKolumn + j))) == 0)
                {
                    mvprintw(yKon(i), xKon(j)-1 + xPos, "   ");
                }
                else if((*(plansza + (i * liczbaKolumn + j))) == 1 && czyWyswStatki == 1)
                {
                    mvprintw(yKon(i), xKon(j)-1 + xPos, " @ ");
                }
                else if((*(plansza + (i * liczbaKolumn + j))) == 1 && czyWyswStatki == 0)
                {
                    mvprintw(yKon(i), xKon(j)-1 + xPos, "   ");
                }
                else if((*(plansza + (i * liczbaKolumn + j))) == 2)
                {
                    mvprintw(yKon(i), xKon(j)-1 + xPos, " X ");
                }
                else if((*(plansza + (i * liczbaKolumn + j))) == 3)
                {
                    mvprintw(yKon(i), xKon(j)-1 + xPos, " - ");
                }
        }
    }

    move(kursor->yPos, kursor->xPos);
}

void rysujObecnaPozycjeBudowanie(Kursor *kursor, Statek *statek, int czyTorus, int liczbaKolumn, int liczbaWierszy, int xPos)
{
    if(statek->orient == 0)
    {
        for(int i=0; i < statek->dlugosc; i++)
        {
            if(czyTorus == 1 && xKonP(kursor->xPos - xPos) + i > liczbaKolumn - 4)
            {
                mvprintw(kursor->yPos, kursor->xPos - 1 + xKon(i) - xKon(liczbaKolumn - 4), "<");
                mvprintw(kursor->yPos, kursor->xPos + 1 + xKon(i) - xKon(liczbaKolumn - 4), ">");
                move(kursor->yPos, kursor->xPos);
            }
            else
            {
                mvprintw(kursor->yPos, kursor->xPos - 1 + xKon(i), "<");
                mvprintw(kursor->yPos, kursor->xPos + 1 + xKon(i), ">");
                move(kursor->yPos, kursor->xPos);
            }
        }
    }
    else
    {
        for(int i=0; i < statek->dlugosc; i++)
        {
            if(czyTorus == 1 && yKonP(kursor->yPos) + i > liczbaWierszy - 4)
            {
                mvprintw(kursor->yPos + yKon(i) - yKon(liczbaWierszy - 4), kursor->xPos - 1, "<");
                mvprintw(kursor->yPos + yKon(i) - yKon(liczbaWierszy - 4), kursor->xPos + 1, ">");
                move(kursor->yPos, kursor->xPos);
            }
            else
            {
                mvprintw(kursor->yPos + yKon(i), kursor->xPos - 1, "<");
                mvprintw(kursor->yPos + yKon(i), kursor->xPos + 1, ">");
                move(kursor->yPos, kursor->xPos);
            }

        }
    }
}

void rysujObecnaPozycjeAtak(Kursor *kursor)
{
            mvprintw(kursor->yPos, kursor->xPos - 1, ">");
            mvprintw(kursor->yPos, kursor->xPos + 1, "<");
            move(kursor->yPos, kursor->xPos);
}

int budujStatekGraczaTorus(Statek *statek, int *pPlansza, Kursor *kursor, int liczbaKolumn, int liczbaWierszy, int xPos)
{
    int indexY = 0;
    int indexX = 0;

    if(statek->orient == 0)
    {
        for(int i=0; i < statek->dlugosc + 2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if(yKonP(kursor->yPos) - 1 + j > liczbaWierszy - 4)
                {
                    indexY = yKonP(kursor->yPos) - 1 + j - (liczbaWierszy - 4);
                }
                else if(yKonP(kursor->yPos) - 1 + j < 1)
                {
                    indexY = liczbaWierszy - 4 + yKonP(kursor->yPos) - 1 + j;
                }
                else
                {
                    indexY = yKonP(kursor->yPos) - 1 + j;
                }

                if(xKonP(kursor->xPos - xPos) - 1 + i > liczbaKolumn - 4)
                {
                    indexX = xKonP(kursor->xPos - xPos) - 1 + i - (liczbaKolumn - 4);
                }
                else if(xKonP(kursor->xPos - xPos) - 1 + i < 1)
                {
                    indexX = liczbaKolumn - 4 + xKonP(kursor->xPos - xPos) - 1 + i;
                }
                else
                {
                    indexX = xKonP(kursor->xPos - xPos) - 1 + i;
                }

                if((*(pPlansza + indexY * liczbaKolumn + indexX)) != 0 )
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            if(xKonP(kursor->xPos - xPos + xKon(i)) > liczbaKolumn - 4)
            {
                indexX = xKonP(kursor->xPos - xPos + xKon(i)) - (liczbaKolumn - 4);
            }
            else
            {
                indexX = xKonP(kursor->xPos - xPos + xKon(i));
            }

            (*(pPlansza + yKonP(kursor->yPos) * liczbaKolumn + indexX)) = 1;
            move(kursor->yPos, kursor->xPos);
        }

        statek->yPos = yKonP(kursor->yPos);
        statek->xPos = xKonP(kursor->xPos - xPos);
    }
    else
    {
        for(int i=0; i < statek->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if(yKonP(kursor->yPos) - 1 + i > liczbaWierszy - 4)
                {
                    indexY = yKonP(kursor->yPos) - 1 + i - (liczbaWierszy - 4);
                }
                else if(yKonP(kursor->yPos) - 1 + i < 1)
                {
                    indexY = liczbaWierszy - 4 + yKonP(kursor->yPos) - 1 + i;
                }
                else
                {
                    indexY = yKonP(kursor->yPos) - 1 + i;
                }

                if(xKonP(kursor->xPos - xPos) - 1 + j > liczbaKolumn - 4)
                {
                    indexX = xKonP(kursor->xPos - xPos) - 1 + j - (liczbaKolumn - 4);
                }
                else if(xKonP(kursor->xPos - xPos) - 1 + j < 1)
                {
                    indexX = liczbaKolumn - 4 + xKonP(kursor->xPos - xPos) - 1 + j;
                }
                else
                {
                    indexX = xKonP(kursor->xPos - xPos) - 1 + j;
                }

                if((*(pPlansza + indexY * liczbaKolumn + indexX)) != 0 )
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            if(yKonP(kursor->yPos + yKon(i)) > liczbaWierszy - 4)
            {
                indexY = yKonP(kursor->yPos + yKon(i)) - (liczbaWierszy - 4);
            }
            else
            {
                indexY = yKonP(kursor->yPos + yKon(i));
            }

            (*(pPlansza + indexY * liczbaKolumn + xKonP(kursor->xPos - xPos))) = 1;
            move(kursor->yPos, kursor->xPos);
        }

        statek->yPos = yKonP(kursor->yPos);
        statek->xPos = xKonP(kursor->xPos - xPos);
    }
    return 0;
}

int budujStatekGracza(Statek *statek, int *pPlansza, Kursor *kursor, int liczbaKolumn, int liczbaWierszy, int xPos)
{
    if(statek->orient == 0)
    {
        if(xKonP(kursor->xPos - xPos) + statek->dlugosc > liczbaKolumn-3)
        {
            return 1;
        }

        for(int i=0; i < statek->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if((*(pPlansza + ((yKonP(kursor->yPos) - 1 + j) * liczbaKolumn + xKonP(kursor->xPos - xPos) - 1 + i))) != 0)
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            mvprintw(kursor->yPos, kursor->xPos + xKon(i), "@");
            (*(pPlansza + (yKonP(kursor->yPos) * liczbaKolumn + xKonP(kursor->xPos - xPos + xKon(i))))) = 1;
            move(kursor->yPos, kursor->xPos);
        }

        statek->yPos = yKonP(kursor->yPos);
        statek->xPos = xKonP(kursor->xPos - xPos);
    }
    else
    {
        if(yKonP(kursor->yPos) + statek->dlugosc > liczbaWierszy-3)
        {
            return 1;
        }

        for(int i=0; i < statek->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if((*(pPlansza + ((yKonP(kursor->yPos) - 1 + i) * liczbaKolumn + xKonP(kursor->xPos - xPos) - 1 + j))) != 0)
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            mvprintw(kursor->yPos + yKon(i), kursor->xPos, "@");
            (*(pPlansza + (yKonP(kursor->yPos + yKon(i)) * liczbaKolumn + xKonP(kursor->xPos - xPos)))) = 1;
            move(kursor->yPos, kursor->xPos);
        }

        statek->yPos = yKonP(kursor->yPos);
        statek->xPos = xKonP(kursor->xPos - xPos);
    }
    return 0;
}

int budujStatkiGracza(Kursor *kursor, int *pPlanszaGracza, int liczbaKolumn, int liczbaWierszy, Statek *statkiGracza, int czyTorus, int xPos)
{
    erase();
    rysujPlansze(xPos, liczbaKolumn - 4, liczbaWierszy - 4);
    odswiezPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy, kursor, xPos, 1);

    int ileWybudowano = 0;
    wyczyscPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy);

    przesunKursor(yKon(1), xKon(1) + xPos, kursor, xPos, liczbaKolumn, liczbaWierszy);
    rysujObecnaPozycjeBudowanie(kursor, statkiGracza, czyTorus, liczbaKolumn, liczbaWierszy, xPos);

    int czyMoznaBudowac;

    int wejscie;
    while((wejscie = toupper(getch())) != 'Q')
    {
//
//
//                for(int u = 0; u < liczbaKolumn; u++)                                                                     /** HELP HELP */
//           {
//                for(int y = 0; y < liczbaWierszy; y++)
//                {
//                    mvprintw(30 + y, 60 + u, "%d", *(pPlanszaGracza + y * liczbaKolumn + u));
//                }
//            }



        mvprintw(liczbaWierszy * 2 - 1, (liczbaKolumn - 4)*2 - 15 + xPos, "                                                        ");
        ruchKursora(wejscie, kursor, xPos, liczbaKolumn, liczbaWierszy, czyTorus);

        if(wejscie == 'R')
        {
            obrocStatek(statkiGracza);
        }
        else if(wejscie == ' ')
        {
            if(czyTorus == 1)
            {
                czyMoznaBudowac = budujStatekGraczaTorus(statkiGracza, pPlanszaGracza, kursor, liczbaKolumn, liczbaWierszy, xPos);
                odswiezPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy, kursor, xPos, 1);
            }
            else if(czyTorus == -1)
            {
                czyMoznaBudowac = budujStatekGracza(statkiGracza, pPlanszaGracza, kursor, liczbaKolumn, liczbaWierszy, xPos);
                odswiezPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy, kursor, xPos, 1);
            }

            if( czyMoznaBudowac == 1)
            {
                mvprintw(liczbaWierszy * 2 - 1, (liczbaKolumn - 4)*2 - 15 + xPos, "Nie mozesz tu zbudowac statku!                       ");
                move(kursor->yPos, kursor->xPos);
            }
            else
            {
                statkiGracza++;
                ileWybudowano++;

                if(ileWybudowano >= 10)
                {
                    odswiezPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy, kursor, xPos, 1);
                    return 0;
                }
            }
        }

        odswiezPlansze(pPlanszaGracza, liczbaKolumn, liczbaWierszy, kursor, xPos, 1);
        rysujObecnaPozycjeBudowanie(kursor, statkiGracza, czyTorus, liczbaKolumn, liczbaWierszy, xPos);
    }

    return 1;
}

int budujStatekPrzeciw(Statek *statek, int *pPlansza, int liczbaKolumn, int liczbaWierszy, int xPosPlanszy)
{
    int nrWiersza;
    int nrKolumny;

    nrWiersza = (rand() % (liczbaWierszy - 4)) + 1;
    nrKolumny = (rand() % (liczbaKolumn - 4)) + 1;
    statek->orient = (rand() % 2);

    if(statek->orient == 0)
    {
        if(nrKolumny + statek->dlugosc > liczbaKolumn-3)
        {
            if(nrWiersza + statek->dlugosc > liczbaWierszy-3)
            {
                return 1;
            }
            else
            {
                statek->orient = 1;
            }
        }
    }
    else
    {
        if(nrWiersza + statek->dlugosc > liczbaWierszy-3)
        {
            if(nrKolumny + statek->dlugosc > liczbaKolumn-3)
            {
                return 1;
            }
            else
            {
                statek->orient = 0;
            }
        }
    }

    if(statek->orient == 0)
    {
        for(int i=0; i < statek->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if((*(pPlansza + ((nrWiersza - 1 + j) * liczbaKolumn + nrKolumny - 1 + i))) != 0)
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            //mvprintw(yKon(nrWiersza), xKon(nrKolumny) + xKon(i) + xPosPlanszy, "@");
            (*(pPlansza + (nrWiersza * liczbaKolumn + nrKolumny + i))) = 1;
        }

        statek->yPos = nrWiersza;
        statek->xPos = nrKolumny;
    }
    else
    {
        for(int i=0; i < statek->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if((*(pPlansza + ((nrWiersza - 1 + i) * liczbaKolumn + nrKolumny - 1 + j))) != 0)
                {
                    return 1;
                }
            }
        }

        for(int k=0; k < statek->dlugosc; k++)
        {
            //mvprintw(yKon(nrWiersza) + yKon(k), xKon(nrKolumny) + xPosPlanszy, "@");
            (*(pPlansza + (nrWiersza + k) * liczbaKolumn + nrKolumny)) = 1;
        }

        statek->yPos = nrWiersza;
        statek->xPos = nrKolumny;
    }

    return 0;
}

int budujStatekPrzeciwTorus(Statek *statek, int *pPlansza, int liczbaKolumn, int liczbaWierszy, int xPosPlanszy)
{
    int nrWiersza;
    int nrKolumny;

    nrWiersza = (rand() % (liczbaWierszy - 4)) + 1;
    nrKolumny = (rand() % (liczbaKolumn - 4)) + 1;
    statek->orient = (rand() % 2);

    int indexY = 0;
    int indexX = 0;

    if(statek->orient == 0)
    {
        for(int i=0; i < statek->dlugosc + 2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if(nrWiersza - 1 + j > liczbaWierszy - 4)
                {
                    indexY = nrWiersza - 1 + j - (liczbaWierszy - 4);
                }
                else
                {
                    indexY = nrWiersza - 1 + j;
                }

                if(nrKolumny - 1 + i > liczbaKolumn - 4)
                {
                    indexX = nrKolumny - 1 + i - (liczbaKolumn - 4);
                }
                else
                {
                    indexX = nrKolumny - 1 + i;
                }

                if((*(pPlansza + indexY * liczbaKolumn + indexX)) != 0 )
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            if(nrKolumny + i > liczbaKolumn - 4)
            {
                indexX = nrKolumny + i - (liczbaKolumn - 4);
            }
            else
            {
                indexX = nrKolumny + i;
            }

            (*(pPlansza + nrWiersza * liczbaKolumn + indexX)) = 1;
        }

        statek->yPos = nrWiersza;
        statek->xPos = nrKolumny;
    }
    else
    {
        for(int i=0; i < statek->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if(nrWiersza - 1 + i > liczbaWierszy - 4)
                {
                    indexY = nrWiersza - 1 + i - (liczbaWierszy - 4);
                }
                else
                {
                    indexY = nrWiersza - 1 + i;
                }

                if(nrKolumny - 1 + j > liczbaKolumn - 4)
                {
                    indexX = nrKolumny - 1 + j - (liczbaKolumn - 4);
                }
                else
                {
                    indexX = nrKolumny - 1 + j;
                }

                if((*(pPlansza + indexY * liczbaKolumn + indexX)) != 0 )
                {
                    return 1;
                }
            }
        }

        for(int i=0; i < statek->dlugosc; i++)
        {
            if(nrWiersza + i > liczbaWierszy - 4)
            {
                indexY = nrWiersza + i - (liczbaWierszy - 4);
            }
            else
            {
                indexY = nrWiersza + i;
            }

            (*(pPlansza + indexY * liczbaKolumn + nrKolumny)) = 1;
        }

        statek->yPos = nrWiersza;
        statek->xPos = nrKolumny;
    }
    return 0;

//    if(statek->orient == 0)
//    {
//        for(int i=0; i < statek->dlugosc+2; i++)
//        {
//            for(int j=0; j < 3; j++)
//            {
//                if((*(pPlansza + ((nrWiersza - 1 + j) * liczbaKolumn + nrKolumny - 1 + i))) != 0)
//                {
//                    return 1;
//                }
//            }
//        }

//        for(int i=0; i < statek->dlugosc; i++)
//        {
//            //mvprintw(yKon(nrWiersza), xKon(nrKolumny) + xKon(i) + xPosPlanszy, "@");
//            (*(pPlansza + (nrWiersza * liczbaKolumn + nrKolumny + i))) = 1;
//        }
//
//        statek->yPos = nrWiersza;
//        statek->xPos = nrKolumny;
//    }
//    else
//    {
//        for(int i=0; i < statek->dlugosc+2; i++)
//        {
//            for(int j=0; j < 3; j++)
//            {
//                if((*(pPlansza + ((nrWiersza - 1 + i) * liczbaKolumn + nrKolumny - 1 + j))) != 0)
//                {
//                    return 1;
//                }
//            }
//        }
//
//        for(int k=0; k < statek->dlugosc; k++)
//        {
//            //mvprintw(yKon(nrWiersza) + yKon(k), xKon(nrKolumny) + xPosPlanszy, "@");
//            (*(pPlansza + (nrWiersza + k) * liczbaKolumn + nrKolumny)) = 1;
//        }
//
//        statek->yPos = nrWiersza;
//        statek->xPos = nrKolumny;
//    }
//
//    return 0;
}

void budujStatkiPrzeciw(int *pPlanszaPrzeciw, int liczbaKolumn, int liczbaWierszy, int xPosPlanszy, Statek *statkiPrzeciw, int czyTorus)
{
    rysujPlansze(xPosPlanszy, liczbaKolumn - 4, liczbaWierszy - 4);
    int ileWybudowano = 0;
    wyczyscPlansze(pPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);

    if(czyTorus == 1)
    {
        while (ileWybudowano < 10)
        {
            if(budujStatekPrzeciwTorus(statkiPrzeciw, pPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, xPosPlanszy) == 0)
            {
                ileWybudowano++;
                statkiPrzeciw++;
            }
        }
    }
    else
    {
        while (ileWybudowano < 10)
        {
            if(budujStatekPrzeciw(statkiPrzeciw, pPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, xPosPlanszy) == 0)
            {
                ileWybudowano++;
                statkiPrzeciw++;
            }
        }
    }


}

void ustawieniaPlanszy(Kursor *kursor, int *wielkosc, int *czyTorus)
{

    kursor->yPos = 12;
    kursor->xPos = 5;
    move(kursor->yPos, kursor->xPos);
    mvprintw(kursor->yPos, kursor->xPos,">");

    int wejscie;

    while(1 > 0)
    {
        mvprintw(11, 7, "(w, s, a, d, space)         ");
        mvprintw(12, 7, "BOARD SIZE: %d              ", *wielkosc);
        if(*czyTorus == -1)
        {
            mvprintw(13, 7, "BOARD SHAPE: SQUARE     ");
        }
        else
        {
            mvprintw(13, 7, "BOARD SHAPE: TORUS      ");
        }
        mvprintw(14, 7, "PLAY!                     ");
        mvprintw(15, 7, "                          ");
        mvprintw(16, 7, "                          ");

        wejscie = toupper(getch());

        mvprintw(kursor->yPos, kursor->xPos," ");
        move(kursor->yPos, kursor->xPos);

        if(kursor->yPos > 12 && wejscie == 'W')
        {
            kursor->yPos -= 1;
        }
        else if(kursor->yPos < 14 && wejscie == 'S')
        {
            kursor->yPos += 1;
        }
        else if(kursor->yPos == 14 && wejscie == ' ')
        {
            return;
        }
        else if(wejscie == 'A')
        {
            if(kursor->yPos == 12)
            {
                if(*wielkosc > 10)
                {
                    (*wielkosc)--;
                }
            }
            else if(kursor->yPos == 13)
            {
                (*czyTorus) *= -1;
            }
        }
        else if(wejscie == 'D')
        {
            if(kursor->yPos == 12)
            {
                if(*wielkosc < 20)
                    (*wielkosc)++;
            }
            else if(kursor->yPos == 13)
            {
                (*czyTorus) *= -1;
            }
        }

        mvprintw(kursor->yPos, kursor->xPos,">");
        move(kursor->yPos, kursor->xPos);
    }
}

void ustawieniaTrudnosci(Kursor *kursor, int *poziom)
{

    kursor->yPos = 12;
    kursor->xPos = 5;
    move(kursor->yPos, kursor->xPos);
    mvprintw(kursor->yPos, kursor->xPos,">");

    int wejscie;

    while(1 > 0)
    {
            mvprintw(11, 7, "(w, s, space)             ");
            mvprintw(12, 7, "BARDZO LATWY (0%%)        ");
            mvprintw(13, 7, "LATWY        (33%%)       ");
            mvprintw(14, 7, "SREDNI       (50%%)       ");
            mvprintw(15, 7, "TRUDNY       (100%%)      ");
            mvprintw(16, 7, "                          ");

        wejscie = toupper(getch());

        mvprintw(kursor->yPos, kursor->xPos," ");
        move(kursor->yPos, kursor->xPos);

        if(kursor->yPos > 12 && wejscie == 'W')
        {
            kursor->yPos -= 1;
        }
        else if(kursor->yPos < 15 && wejscie == 'S')
        {
            kursor->yPos += 1;
        }
        else if(wejscie == ' ')
        {
            if(kursor->yPos == 12)
            {
                *poziom = 1;
            }
            else if(kursor->yPos == 13)
            {
                *poziom = 2;
            }
            else if(kursor->yPos == 14)
            {
                *poziom = 3;
            }
            else if(kursor->yPos == 15)
            {
                *poziom = 4;
            }

            return;
        }

        mvprintw(kursor->yPos, kursor->xPos,">");
        move(kursor->yPos, kursor->xPos);
    }
}

void ustawieniaTrybu(Kursor *kursor, int *tryb, int *poziom)
{

    kursor->yPos = 12;
    kursor->xPos = 5;
    move(kursor->yPos, kursor->xPos);
    mvprintw(kursor->yPos, kursor->xPos,">");

    int wejscie;

    while(1 > 0)
    {
            mvprintw(11, 7, "(w, s, space)             ");
            mvprintw(12, 7, "SINGLE PLAYER             ");
            mvprintw(13, 7, "HOTSEAT MULTIPLAYER       ");
            mvprintw(14, 7, "                          ");
            mvprintw(15, 7, "                          ");
            mvprintw(16, 7, "                          ");

        wejscie = toupper(getch());

        mvprintw(kursor->yPos, kursor->xPos," ");
        move(kursor->yPos, kursor->xPos);

        if(kursor->yPos > 12 && wejscie == 'W')
        {
            kursor->yPos -= 1;
        }
        else if(kursor->yPos < 13 && wejscie == 'S')
        {
            kursor->yPos += 1;
        }
        else if(wejscie == ' ')
        {
            if(kursor->yPos == 12)
            {
                (*tryb) = 1;
                ustawieniaTrudnosci(kursor, poziom);
            }
            else if(kursor->yPos == 13)
            {
                (*tryb) = 2;
            }

            return;
        }

        mvprintw(kursor->yPos, kursor->xPos,">");
        move(kursor->yPos, kursor->xPos);
    }
}

int menu(Kursor *kursor, int *poziom, int *wielkosc, int *czyTorus, int *tryb)
{
    menuGrafika();

    kursor->yPos = 12;
    kursor->xPos = 5;
    move(kursor->yPos, kursor->xPos);
    mvprintw(kursor->yPos, kursor->xPos,">");


    int wejscie;
    while(1 > 0)
    {
            mvprintw(11, 7, "(w, s, space)             ");
            mvprintw(12, 7, "NEW GAME                  ");
            mvprintw(13, 7, "LOAD GAME                 ");
            mvprintw(14, 7, "CONTROLS                  ");
            mvprintw(15, 7, "ABOUT                     ");
            mvprintw(16, 7, "EXIT                      ");

        wejscie = toupper(getch());

        mvprintw(kursor->yPos, kursor->xPos," ");
        move(kursor->yPos, kursor->xPos);

        if(kursor->yPos > 12 && wejscie == 'W')
        {
            kursor->yPos -= 1;
        }
        else if(kursor->yPos < 16 && wejscie == 'S')
        {
            kursor->yPos += 1;
        }
        else if(wejscie == ' ')
        {
            if(kursor->yPos == 12)
            {
                ustawieniaTrybu(kursor, tryb, poziom);
                ustawieniaPlanszy(kursor, wielkosc, czyTorus);

                return 1;
            }
            else if(kursor->yPos == 13)
            {
                return 2;
            }
            else if(kursor->yPos == 14)
            {
                mvprintw(11, 7, "                          ");
                mvprintw(12, 7, "W, A, S, D - ruch kursorem");
                mvprintw(13, 7, "SPACJA - budowanie/strzal ");
                mvprintw(14, 7, "R - obracanie statku      ");
                mvprintw(15, 7, "T - zapisanie rozgrywki   ");
                mvprintw(16, 7, "Q - wyjscie do menu       ");
                getch();
            }
            else if(kursor->yPos == 15)
            {
                //about
            }
            else if(kursor->yPos == 16)
            {
                return 4;
            }

            kursor->yPos = 12;
            kursor->xPos = 5;
        }

        mvprintw(kursor->yPos, kursor->xPos,">");
        move(kursor->yPos, kursor->xPos);
    }
}

int graj(Kursor *kursor, int *pPlanszaGracza, int liczbaKolumn, int liczbaWierszy, Statek *statkiGracza, int *pPlanszaPrzeciw, int xPosPlanszaPrzeciw, Statek *statkiPrzeciw, int poziom, int czyTorus, int tryb, int *pPlanszaPomocnicza, int ktoryGracz, int czyNowaGra)
{
    if(tryb == 1)
    {
        if(czyNowaGra == 0)
        {
            switch (atakiSolo(pPlanszaGracza, pPlanszaPrzeciw, kursor, xPosPlanszaPrzeciw, statkiGracza, statkiPrzeciw, liczbaKolumn, liczbaWierszy, poziom, czyTorus, pPlanszaPomocnicza))
            {
                case 1:
                    break;

                case 2:
                    wygrana();
                    getch();
                    break;

                case 3:
                    przegrana();
                    getch();
                    break;

                case 4:         //zapis gracz 1
                    return 4;
                    break;

                case 5:         //zapis gracz 2
                    return 5;
                    break;
            }
        }
        else if(budujStatkiGracza(kursor, pPlanszaGracza, liczbaKolumn, liczbaWierszy, statkiGracza, czyTorus, 0) != 1)
        {
            budujStatkiPrzeciw(pPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, xPosPlanszaPrzeciw, statkiPrzeciw, czyTorus);

            switch (atakiSolo(pPlanszaGracza, pPlanszaPrzeciw, kursor, xPosPlanszaPrzeciw, statkiGracza, statkiPrzeciw, liczbaKolumn, liczbaWierszy, poziom, czyTorus, pPlanszaPomocnicza))
            {
                case 1:
                    break;

                case 2:
                    wygrana();
                    getch();
                    break;

                case 3:
                    przegrana();
                    getch();
                    break;

                case 4:         //zapis gracz 1
                    return 4;
                    break;

                case 5:         //zapis gracz 2
                    return 5;
                    break;
            }
        }
    }
    else
    {
        if(czyNowaGra == 0)
        {
            switch (atakiMulti(pPlanszaGracza, pPlanszaPrzeciw, kursor, xPosPlanszaPrzeciw, statkiGracza, statkiPrzeciw, liczbaKolumn, liczbaWierszy, poziom, czyTorus, ktoryGracz))
            {
                case 1:
                    break;

                case 2:
                    wygrana1();
                    getch();
                    break;

                case 3:
                    wygrana2();
                    getch();
                    break;

                case 4:         //zapis gracz 1
                    return 4;
                    break;

                case 5:         //zapis gracz 2
                    return 5;
                    break;
            }
        }
        else
        {
            erase();
            mvprintw(14, 40, "PLAYER 1 - BUILDING");
            mvprintw(15, 40, "press any key...");
            getch();

            if(budujStatkiGracza(kursor, pPlanszaGracza, liczbaKolumn, liczbaWierszy, statkiGracza, czyTorus, 0) != 1)
            {
                erase();
                mvprintw(14, 40, "PLAYER 2 - BUILDING");
                mvprintw(15, 40, "press any key...");
                getch();

                if(budujStatkiGracza(kursor, pPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, statkiPrzeciw, czyTorus, xPosPlanszaPrzeciw) != 1)
                {
                    switch (atakiMulti(pPlanszaGracza, pPlanszaPrzeciw, kursor, xPosPlanszaPrzeciw, statkiGracza, statkiPrzeciw, liczbaKolumn, liczbaWierszy, poziom, czyTorus, ktoryGracz))
                    {
                        case 1:
                            break;

                        case 2:
                            wygrana1();
                            getch();
                            break;

                        case 3:
                            wygrana2();
                            getch();
                            break;

                        case 4:         //zapis gracz 1
                            return 4;
                            break;

                        case 5:         //zapis gracz 2
                            return 5;
                            break;
                    }
                }
            }
        }
    }

    return 0;
}

int atakiMulti(int *planszaGracza, int *planszaPrzeciw, Kursor *kursor, int xPosPlanszaPrzeciw, Statek *statkiGracza, Statek *statkiPrzeciw, int liczbaKolumn, int liczbaWierszy, int poziom, int czyTorus, int ktoryGracz)
{
    while(1 > 0)
   {
//            for(int u = 0; u < liczbaKolumn; u++)                                                                     /** HELP HELP */
//            {
//                for(int y = 0; y < liczbaWierszy; y++)
//                {
//                    mvprintw(30 + y, 60 + u, "%d", *(planszaPrzeciw + y * liczbaKolumn + u));
//                }
//            }
        if(ktoryGracz == 1)
        {
            erase();
            mvprintw(14, 40, "PLAYER 1 - ATTACK");
            mvprintw(15, 40, "press any key...");
            getch();
            erase();

            rysujPlansze(0, liczbaKolumn - 4, liczbaWierszy - 4);
            odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
            rysujPlansze(xPosPlanszaPrzeciw, liczbaKolumn - 4, liczbaWierszy - 4);
            odswiezPlansze(planszaPrzeciw, liczbaKolumn, liczbaWierszy, kursor, xPosPlanszaPrzeciw, 0);

            przesunKursor(yKon(1), xKon(1) + xPosPlanszaPrzeciw, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);
            rysujObecnaPozycjeAtak(kursor);

            switch (strzalGracza(planszaPrzeciw, kursor, statkiPrzeciw, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, czyTorus))
            {
                case 1:
                    return 1;
                    break;

                case 2:
                    return 2;
                    break;

                case 4:
                    return 4;
                    break;

                case 0:
                    break;
            }
        }

        ktoryGracz = 1;

        erase();
        mvprintw(14, 40, "PLAYER 2 - ATTACK");
        mvprintw(15, 40, "press any key...");
        getch();
        erase();

        rysujPlansze(0, liczbaKolumn - 4, liczbaWierszy - 4);
        odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 0);
        rysujPlansze(xPosPlanszaPrzeciw, liczbaKolumn - 4, liczbaWierszy - 4);
        odswiezPlansze(planszaPrzeciw, liczbaKolumn, liczbaWierszy, kursor, xPosPlanszaPrzeciw, 1);

        przesunKursor(yKon(1), xKon(1), kursor, 0, liczbaKolumn, liczbaWierszy);
        rysujObecnaPozycjeAtak(kursor);

        switch (strzalGracza(planszaGracza, kursor, statkiGracza, 0, liczbaKolumn, liczbaWierszy, czyTorus))
        {
            case 1:
                return 1;
                break;

            case 2:
                return 3;
                break;

            case 4:
                return 5;
                break;

            case 0:
                break;
        }

}


    return 1;
}

int atakiSolo(int *planszaGracza, int *planszaPrzeciw, Kursor *kursor, int xPosPlanszaPrzeciw, Statek *statkiGracza, Statek *statkiPrzeciw, int liczbaKolumn, int liczbaWierszy, int poziom, int czyTorus, int *pPlanszaPomocnicza)
{
    rysujPlansze(0, liczbaKolumn - 4, liczbaWierszy - 4);
    rysujPlansze(xPosPlanszaPrzeciw, liczbaKolumn - 4, liczbaWierszy - 4);
    odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
    odswiezPlansze(planszaPrzeciw, liczbaKolumn, liczbaWierszy, kursor, xPosPlanszaPrzeciw, 0);
    int rezultat;
    przesunKursor(yKon(1), xKon(1) + xPosPlanszaPrzeciw, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);
    rysujObecnaPozycjeAtak(kursor);



    while(1 > 0)
   {
//            for(int u = 0; u < liczbaKolumn; u++)                                                                     /** HELP HELP */
//            {
//                for(int y = 0; y < liczbaWierszy; y++)
//                {
//                    mvprintw(30 + y, 60 + u, "%d", *(planszaPrzeciw + y * liczbaKolumn + u));
//                }
//            }



        switch (strzalGracza(planszaPrzeciw, kursor, statkiPrzeciw, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, czyTorus))
        {
            case 1:
                return 1;
                break;

            case 2:
                return 2;
                break;

            case 4:
                return 4;
                break;

            case 0:
                break;
        }

        if(poziom == 1)
        {
            rezultat = strzalLosowy(planszaGracza, statkiGracza, liczbaKolumn, liczbaWierszy, kursor, czyTorus);
        }
        else if(poziom == 2 || poziom == 3 || poziom == 4)
        {
            rezultat = strzalSensowny(planszaGracza, statkiGracza, liczbaKolumn, liczbaWierszy, kursor, pPlanszaPomocnicza, poziom, czyTorus, pPlanszaPomocnicza);
        }

        if(rezultat == 3)
        {
            return 3;
        }
    }

    return 1;
}

int strzalGracza(int *planszaPrzeciw, Kursor *kursor, Statek *statkiPrzeciw, int xPosPlanszaPrzeciw, int liczbaKolumn, int liczbaWierszy, int czyTorus)
{
    int wejscie = 0;
    int ileTrafionych = 0;
    int indexX = 0;
    int indexY = 0;
    while((wejscie = toupper(getch())) != 'Q')
    {
        ruchKursora(wejscie, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy, czyTorus);

        if(wejscie == ' ')
        {
            if((*(planszaPrzeciw + (yKonP(kursor->yPos) * liczbaKolumn + xKonP(kursor->xPos - xPosPlanszaPrzeciw)))) == 1)
            {
                for(int i=0; i<10; i++)
                {
                    for(int j=0; j < statkiPrzeciw->dlugosc; j++)
                    {
                        if(statkiPrzeciw->orient == 0)
                        {
                            if(czyTorus == 1 && statkiPrzeciw->xPos + j > liczbaKolumn - 4)
                            {
                                indexX = statkiPrzeciw->xPos + j - (liczbaKolumn - 4);

                                if(yKonP(kursor->yPos) == statkiPrzeciw->yPos && xKonP(kursor->xPos - xPosPlanszaPrzeciw) == indexX)
                                {
                                    trafienie(planszaPrzeciw, statkiPrzeciw, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);
                                }
                            }
                            else if(yKonP(kursor->yPos) == statkiPrzeciw->yPos && xKonP(kursor->xPos - xPosPlanszaPrzeciw) == statkiPrzeciw->xPos + j)
                            {
                                trafienie(planszaPrzeciw, statkiPrzeciw, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);
                            }
                        }
                        else if(statkiPrzeciw->orient == 1)
                        {
                            if(czyTorus == 1 && statkiPrzeciw->yPos + j > liczbaWierszy - 4)
                            {
                                indexY = statkiPrzeciw->yPos + j - (liczbaWierszy - 4);

                                if(yKonP(kursor->yPos) == indexY && xKonP(kursor->xPos - xPosPlanszaPrzeciw) == statkiPrzeciw->xPos)
                                {
                                    trafienie(planszaPrzeciw, statkiPrzeciw, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);
                                }
                            }
                            else if(yKonP(kursor->yPos) == statkiPrzeciw->yPos + j && xKonP(kursor->xPos - xPosPlanszaPrzeciw) == statkiPrzeciw->xPos)
                            {
                                trafienie(planszaPrzeciw, statkiPrzeciw, kursor, xPosPlanszaPrzeciw, liczbaKolumn, liczbaWierszy);
                            }
                        }
                    }

                    statkiPrzeciw++;
                }

                statkiPrzeciw -= 10;
            }
            else if((*(planszaPrzeciw + (yKonP(kursor->yPos) * liczbaKolumn + xKonP(kursor->xPos - xPosPlanszaPrzeciw)))) == 0)
            {
                mvprintw(liczbaWierszy * 2 - 1, (liczbaKolumn - 4)*2 - 7 + xPosPlanszaPrzeciw, "%c%d - Pudlo!                                        ", yKonP(kursor->yPos) + 64, xKonP(kursor->xPos - xPosPlanszaPrzeciw));
                move(kursor->yPos, kursor->xPos);
                (*(planszaPrzeciw + (yKonP(kursor->yPos) * liczbaKolumn + xKonP(kursor->xPos - xPosPlanszaPrzeciw)))) = 3;
                odswiezPlansze(planszaPrzeciw, liczbaKolumn, liczbaWierszy, kursor, xPosPlanszaPrzeciw, 0);
                rysujObecnaPozycjeAtak(kursor);
                return 0;
            }
        }
        else if(wejscie == 'T')
        {
            return 4;
        }

        for(int i=0; i<10; i++)
        {
            if(statkiPrzeciw->czyZatopiony == 1)
            {
                ileTrafionych++;
            }

            statkiPrzeciw++;
        }

        if(ileTrafionych >= 10)
        {
            return 2;
        }

        ileTrafionych = 0;
        statkiPrzeciw -= 10;

        odswiezPlansze(planszaPrzeciw, liczbaKolumn, liczbaWierszy, kursor, xPosPlanszaPrzeciw, 0);
        rysujObecnaPozycjeAtak(kursor);
    }

    return 1;
}

void trafienie(int *plansza, Statek *statek, Kursor *kursor, int xPosPlansza, int liczbaKolumn, int liczbaWierszy)
{
    (*(plansza + (yKonP(kursor->yPos) * liczbaKolumn + xKonP(kursor->xPos - xPosPlansza)))) = 2;
    statek->ileTrafien += 1;

    if(statek->ileTrafien >= statek->dlugosc)
    {
        mvprintw(liczbaWierszy * 2 - 1, (liczbaKolumn - 4)*2 - 7 + xPosPlansza, "%c%d - Trafiony zatopiony!                           ", yKonP(kursor->yPos) + 64, xKonP(kursor->xPos - xPosPlansza));
        statek->czyZatopiony = 1;
    }
    else
    {
        mvprintw(liczbaWierszy * 2 - 1, (liczbaKolumn - 4)*2 - 7 + xPosPlansza, "%c%d - Trafiony!                                    ", yKonP(kursor->yPos) + 64, xKonP(kursor->xPos - xPosPlansza));
    }

    move(kursor->yPos, kursor->xPos);

    return;
}

int strzalLosowy(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int liczbaWierszy, Kursor *kursor, int czyTorus)
{
    int nrWiersza;
    int nrKolumny;
    int ileTrafionych = 0;
    int indexX = 0;
    int indexY = 0;

    while(1 > 0)
    {
        nrWiersza = (rand() % (liczbaKolumn - 4)) + 1;
        nrKolumny = (rand() % (liczbaWierszy - 4)) + 1;

        if((*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) == 1)
        {
            for(int i=0; i<10; i++)
            {
                for(int j=0; j < statkiGracza->dlugosc; j++)
                {
                    if(statkiGracza->orient == 0)
                    {
                        if(czyTorus == 1 && statkiGracza->xPos + j > liczbaKolumn - 4)
                        {
                            indexX = statkiGracza->xPos + j - (liczbaKolumn - 4);

                            if(nrWiersza == statkiGracza->yPos && nrKolumny == indexX)
                            {
                                (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                                statkiGracza->ileTrafien += 1;

                                if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                                {
                                    statkiGracza->czyZatopiony = 1;
                                }
                            }
                        }
                        else if(nrWiersza == statkiGracza->yPos && nrKolumny == statkiGracza->xPos + j)
                        {
                            (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                            statkiGracza->ileTrafien += 1;

                            if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                            {
                                statkiGracza->czyZatopiony = 1;
                            }
                        }
                    }
                    else if(statkiGracza->orient == 1)
                    {
                        if(czyTorus == 1 && statkiGracza->yPos + j > liczbaWierszy - 4)
                        {
                            indexY = statkiGracza->yPos + j - (liczbaWierszy - 4);

                            if(nrKolumny == statkiGracza->xPos && nrWiersza == indexY)
                            {
                                (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                                statkiGracza->ileTrafien += 1;

                                if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                                {
                                    statkiGracza->czyZatopiony = 1;
                                }
                            }
                        }
                        else if(nrWiersza == statkiGracza->yPos + j && nrKolumny == statkiGracza->xPos)
                        {
                            (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                            statkiGracza->ileTrafien += 1;

                            if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                            {
                                statkiGracza->czyZatopiony = 1;
                            }
                        }
                    }
                }

                statkiGracza++;
            }

            statkiGracza -= 10;
        }
        else if((*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) == 0)
        {
            (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 3;
            odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
            return 0;
        }

        for(int i=0; i<10; i++)
        {
            if(statkiGracza->czyZatopiony == 1)
            {
                ileTrafionych++;
            }

            statkiGracza++;
        }

        if(ileTrafionych >= 10)
        {
            odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
            return 3;
        }

        ileTrafionych = 0;
        statkiGracza -= 10;
        odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
    }

    return 0;
}

int strzalSensowny(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int liczbaWierszy, Kursor *kursor, int *planszaPomocnicza, int poziom, int czyTorus, int *pPlanszaPomocnicza)
{
    int nrWiersza;
    int nrKolumny;
    int ileTrafionych = 0;

    int indexX = 0;
    int indexY = 0;

    while(1 > 0)
    {
        if(poziom == 4)
        {
            czySaTypy(planszaPomocnicza, liczbaKolumn, liczbaWierszy, &nrWiersza, &nrKolumny);
        }
        else if(poziom == 2 && (rand() % 3) == 0)
        {
            czySaTypy(planszaPomocnicza, liczbaKolumn, liczbaWierszy, &nrWiersza, &nrKolumny);
        }
        else if(poziom == 3 && (rand() % 2) == 0)
        {
            czySaTypy(planszaPomocnicza, liczbaKolumn, liczbaWierszy, &nrWiersza, &nrKolumny);
        }
        else
        {
            nrWiersza = (rand() % (liczbaWierszy - 4)) + 1;
            nrKolumny = (rand() % (liczbaKolumn - 4)) + 1;
        }

        if((*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny)) != 5)
        {
            if((*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) == 1)
            {
                for(int i=0; i<10; i++)
                {
                    for(int j=0; j < statkiGracza->dlugosc; j++)
                    {
                        if(statkiGracza->orient == 0)
                        {
                            if(czyTorus == 1)
                            {
                                if(statkiGracza->xPos + j > liczbaKolumn - 4)
                                {
                                    indexX = statkiGracza->xPos + j - (liczbaKolumn - 4);
                                }
                                else
                                {
                                    indexX = statkiGracza->xPos + j;
                                }

                                if(nrWiersza == statkiGracza->yPos && nrKolumny == indexX)
                                {
                                    (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                                    statkiGracza->ileTrafien += 1;
                                    obrysujRogiTorus(planszaGracza, nrWiersza, nrKolumny, liczbaKolumn, liczbaWierszy, planszaPomocnicza);

                                    if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                                    {
                                        statkiGracza->czyZatopiony = 1;
                                        obrysujStatekTorus(planszaGracza, statkiGracza, liczbaKolumn, liczbaWierszy, planszaPomocnicza);
                                    }
                                }
                            }
                            else if(nrWiersza == statkiGracza->yPos && nrKolumny == statkiGracza->xPos + j)
                            {
                                (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                                statkiGracza->ileTrafien += 1;
                                obrysujRogi(planszaGracza, nrWiersza, nrKolumny, liczbaKolumn, planszaPomocnicza);

                                if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                                {
                                    statkiGracza->czyZatopiony = 1;
                                    obrysujStatek(planszaGracza, statkiGracza, liczbaKolumn, planszaPomocnicza);
                                }
                            }
                        }
                        else if(statkiGracza->orient == 1)
                        {
                            if(czyTorus == 1)
                            {
                                if(statkiGracza->yPos + j > liczbaWierszy - 4)
                                {
                                    indexY = statkiGracza->yPos + j - (liczbaWierszy - 4);
                                }
                                else
                                {
                                    indexY = statkiGracza->yPos + j;
                                }

                                if(nrKolumny == statkiGracza->xPos && nrWiersza == indexY)
                                {
                                    (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                                    statkiGracza->ileTrafien += 1;
                                    obrysujRogiTorus(planszaGracza, nrWiersza, nrKolumny, liczbaKolumn, liczbaWierszy, planszaPomocnicza);

                                    if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                                    {
                                        statkiGracza->czyZatopiony = 1;
                                        obrysujStatekTorus(planszaGracza, statkiGracza, liczbaKolumn, liczbaWierszy, planszaPomocnicza);
                                    }
                                }
                            }
                            else if(nrWiersza == statkiGracza->yPos + j && nrKolumny == statkiGracza->xPos)
                            {
                                (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 2;
                                statkiGracza->ileTrafien += 1;
                                obrysujRogi(planszaGracza, nrWiersza, nrKolumny, liczbaKolumn, planszaPomocnicza);

                                if(statkiGracza->ileTrafien >= statkiGracza->dlugosc)
                                {
                                    statkiGracza->czyZatopiony = 1;
                                    obrysujStatek(planszaGracza, statkiGracza, liczbaKolumn, planszaPomocnicza);
                                }
                            }
                        }
                    }

                    statkiGracza++;
                }

                statkiGracza -= 10;





//                for(int u = 0; u < liczbaKolumn; u++)                                                                     /** HELP HELP */
//                {
//                for(int y = 0; y < liczbaWierszy; y++)
//                {
//                    mvprintw(30 + y, 5 + u, "%d", *(planszaPomocnicza + y * liczbaKolumn + u));
//                }
//                }






            }
            else if((*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) == 0)
            {
                (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny)) = 3;
                odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
                (*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny)) = 5;





//                for(int u = 0; u < liczbaKolumn; u++)                                                                     /** HELP HELP */
//                {
//                for(int y = 0; y < liczbaWierszy; y++)
//                {
//                    mvprintw(30 + y, 5 + u, "%d", *(planszaPomocnicza + y * liczbaKolumn + u));
//                }
//                }





                return 0;
            }

            for(int i=0; i<10; i++)
            {
                if(statkiGracza->czyZatopiony == 1)
                {
                    ileTrafionych++;
                }

                statkiGracza++;
            }

            if(ileTrafionych >= 10)
            {
                odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
                return 3;
            }

            ileTrafionych = 0;
            statkiGracza -= 10;
            odswiezPlansze(planszaGracza, liczbaKolumn, liczbaWierszy, kursor, 0, 1);
        }

    }

    return 0;
}

void obrysujStatek(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int *planszaPomocnicza)
{
    if(statkiGracza->orient == 0)
    {
        for(int i=0; i < statkiGracza->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                (*(planszaPomocnicza + ((statkiGracza->yPos - 1 + j) * liczbaKolumn + statkiGracza->xPos - 1 + i))) = 5;
            }
        }
    }
    else
    {
        for(int i=0; i < statkiGracza->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                (*(planszaPomocnicza + ((statkiGracza->yPos - 1 + i) * liczbaKolumn + statkiGracza->xPos - 1 + j))) = 5;

            }
        }
    }

    return;
}

void obrysujStatekTorus(int *planszaGracza, Statek *statkiGracza, int liczbaKolumn, int liczbaWierszy, int *planszaPomocnicza)
{
    int indexX = 0;
    int indexY = 0;

    if(statkiGracza->orient == 0)       //OPT: Wcisnac ifa pod fory
    {
        for(int i=0; i < statkiGracza->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if(statkiGracza->yPos - 1 + j > liczbaWierszy - 4)
                {
                    indexY = statkiGracza->yPos - 1 + j - (liczbaWierszy - 4);
                }
                else
                {
                    indexY = statkiGracza->yPos - 1 + j;
                }

                if(statkiGracza->xPos - 1 + i > liczbaKolumn - 4)
                {
                    indexX = statkiGracza->xPos - 1 + i - (liczbaKolumn - 4);
                }
                else
                {
                    indexX = statkiGracza->xPos - 1 + i;
                }

                (*(planszaPomocnicza + indexY * liczbaKolumn + indexX)) = 5;
            }
        }
    }
    else
    {
        for(int i=0; i < statkiGracza->dlugosc+2; i++)
        {
            for(int j=0; j < 3; j++)
            {
                if(statkiGracza->yPos - 1 + i > liczbaWierszy - 4)
                {
                    indexY = statkiGracza->yPos - 1 + i - (liczbaWierszy - 4);
                }
                else
                {
                    indexY = statkiGracza->yPos - 1 + i;
                }

                if(statkiGracza->xPos - 1 + j > liczbaKolumn - 4)
                {
                    indexX = statkiGracza->xPos - 1 + j - (liczbaKolumn - 4);
                }
                else
                {
                    indexX = statkiGracza->xPos - 1 + j;
                }

                (*(planszaPomocnicza + indexY * liczbaKolumn + indexX)) = 5;
            }
        }
    }

    return;
}

void obrysujRogi(int *planszaGracza, int nrWiersza, int nrKolumny, int liczbaKolumn, int *planszaPomocnicza)
{
    (*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny)) = 5;

    (*(planszaPomocnicza + (nrWiersza - 1) * liczbaKolumn + nrKolumny - 1)) = 5;
    (*(planszaPomocnicza + (nrWiersza - 1) * liczbaKolumn + nrKolumny + 1)) = 5;
    (*(planszaPomocnicza + (nrWiersza + 1) * liczbaKolumn + nrKolumny - 1)) = 5;
    (*(planszaPomocnicza + (nrWiersza + 1) * liczbaKolumn + nrKolumny + 1)) = 5;

    if(((*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny - 1)) == 0 || (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny - 1)) == 1) && ((*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny - 1)) != 5))
    {
        (*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny - 1)) = 6;
    }

    if(((*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny + 1)) == 0 || (*(planszaGracza + nrWiersza * liczbaKolumn + nrKolumny + 1)) == 1) && ((*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny + 1)) != 5))
    {
        (*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny + 1)) = 6;
    }

    if(((*(planszaGracza + (nrWiersza - 1) * liczbaKolumn + nrKolumny)) == 0 || (*(planszaGracza + (nrWiersza - 1) * liczbaKolumn + nrKolumny)) == 1) && ((*(planszaPomocnicza + (nrWiersza - 1) * liczbaKolumn + nrKolumny)) != 5))
    {
        (*(planszaPomocnicza + (nrWiersza - 1) * liczbaKolumn + nrKolumny)) = 6;
    }

    if(((*(planszaGracza + (nrWiersza + 1) * liczbaKolumn + nrKolumny)) == 0 || (*(planszaGracza + (nrWiersza + 1) * liczbaKolumn + nrKolumny)) == 1) && ((*(planszaPomocnicza + (nrWiersza + 1) * liczbaKolumn + nrKolumny)) != 5))
    {
        (*(planszaPomocnicza + (nrWiersza + 1) * liczbaKolumn + nrKolumny)) = 6;
    }

    return;
}

void obrysujRogiTorus(int *planszaGracza, int nrWiersza, int nrKolumny, int liczbaKolumn, int liczbaWierszy, int *planszaPomocnicza)
{
    (*(planszaPomocnicza + nrWiersza * liczbaKolumn + nrKolumny)) = 5;

    int indexXp = 0;
    int indexYp = 0;
    int indexXm = 0;
    int indexYm = 0;

    if(nrKolumny == liczbaKolumn - 4)
    {
        indexXp = 1;
        indexXm = nrKolumny - 1;
    }
    else if(nrKolumny == 1)
    {
        indexXp = nrKolumny + 1;
        indexXm = liczbaKolumn - 4;
    }
    else
    {
        indexXp = nrKolumny + 1;
        indexXm = nrKolumny - 1;
    }

    if(nrWiersza == liczbaWierszy - 4)
    {
        indexYp = 1;
        indexYm = nrWiersza - 1;
    }
    else if(nrWiersza == 1)
    {
        indexYp = nrWiersza + 1;
        indexYm = liczbaWierszy - 4;
    }
    else
    {
        indexYp = nrWiersza + 1;
        indexYm = nrWiersza - 1;
    }


    (*(planszaPomocnicza + indexYm * liczbaKolumn + indexXm)) = 5;
    (*(planszaPomocnicza + indexYm * liczbaKolumn + indexXp)) = 5;
    (*(planszaPomocnicza + indexYp * liczbaKolumn + indexXm)) = 5;
    (*(planszaPomocnicza + indexYp * liczbaKolumn + indexXp)) = 5;

    if(((*(planszaGracza + nrWiersza * liczbaKolumn + indexXm)) == 0 || (*(planszaGracza + nrWiersza * liczbaKolumn + indexXm)) == 1) && ((*(planszaPomocnicza + nrWiersza * liczbaKolumn + indexXm)) != 5))
    {
        (*(planszaPomocnicza + nrWiersza * liczbaKolumn + indexXm)) = 6;
    }

    if(((*(planszaGracza + nrWiersza * liczbaKolumn + indexXp)) == 0 || (*(planszaGracza + nrWiersza * liczbaKolumn + indexXp)) == 1) && ((*(planszaPomocnicza + nrWiersza * liczbaKolumn + indexXp)) != 5))
    {
        (*(planszaPomocnicza + nrWiersza * liczbaKolumn + indexXp)) = 6;
    }

    if(((*(planszaGracza + indexYm * liczbaKolumn + nrKolumny)) == 0 || (*(planszaGracza + indexYm * liczbaKolumn + nrKolumny)) == 1) && ((*(planszaPomocnicza + indexYm * liczbaKolumn + nrKolumny)) != 5))
    {
        (*(planszaPomocnicza + indexYm * liczbaKolumn + nrKolumny)) = 6;
    }

    if(((*(planszaGracza + indexYp * liczbaKolumn + nrKolumny)) == 0 || (*(planszaGracza + indexYp * liczbaKolumn + nrKolumny)) == 1) && ((*(planszaPomocnicza + indexYp * liczbaKolumn + nrKolumny)) != 5))
    {
        (*(planszaPomocnicza + indexYp * liczbaKolumn + nrKolumny)) = 6;
    }

    return;
}

void czySaTypy(int *planszaPomocnicza, int liczbaKolumn, int liczbaWierszy, int *nrWiersza, int *nrKolumny)
{
    for (int i=1; i < liczbaWierszy - 3; i++)
    {
        for (int j=1; j < liczbaKolumn - 3; j++)
        {
            if((*(planszaPomocnicza + i * liczbaKolumn + j)) == 6)
            {
                do
                {
                    *nrWiersza = (rand() % (liczbaWierszy - 4)) + 1;
                    *nrKolumny = (rand() % (liczbaKolumn - 4)) + 1;

                }while((*(planszaPomocnicza + (*nrWiersza) * liczbaKolumn + (*nrKolumny))) != 6);

                return;
            }
        }
    }

    *nrWiersza = (rand() % (liczbaWierszy - 4)) + 1;
    *nrKolumny = (rand() % (liczbaKolumn - 4)) + 1;

    return;
}

void menuGrafika()
{
    erase();
     mvprintw(0, 5, " .d8888b. 88888888888     d8888 88888888888 888    d8P  8888888 ");
     mvprintw(1, 5, "d88P  Y88b    888        d88888     888     888   d8P     888   ");
     mvprintw(2, 5, "Y88b.         888       d88P888     888     888  d8P      888   ");
     mvprintw(3, 5, " 'Y888b.      888      d88P 888     888     888d88K       888   ");
     mvprintw(4, 5, "    'Y88b.    888     d88P  888     888     8888888b      888   ");
     mvprintw(5, 5, "      '888    888    d88P   888     888     888  Y88b     888   ");
     mvprintw(6, 5, "Y88b  d88P    888   d8888888888     888     888   Y88b    888   ");
     mvprintw(7, 5, " 'Y8888P'     888  d88P     888     888     888    Y88b 8888888 ");
     mvprintw(8, 5, "----------------------------------------------------------------");
     mvprintw(9, 5, "                                                 by Adam Steciuk");

    return;
}

void rysujPlansze(int xPos, int liczbaKolumn, int liczbaWierszy)
{
    for(int i = 1; i <= liczbaKolumn; i++)
    {
        mvprintw(0, xPos + i * 4, "%d", i);
    }

    for(int i = 0; i < liczbaWierszy; i++)
    {
        for(int j = 0; j < liczbaKolumn * 4 - 1; j++)
        {
            mvprintw(2 * i + 1, xPos + 3 + j, "-");
        }
        for(int j = 0; j <= liczbaKolumn; j++)
        {
            mvprintw(2 * i + 2, xPos + 2 + j * 4, "|");
        }
    }

    for(int i = 0; i < liczbaKolumn * 4 - 1; i++)
    {
        mvprintw(2 * liczbaWierszy + 1, xPos + 3 + i, "-");
    }

    for(int i = 0; i < liczbaWierszy; i++)
    {
        mvprintw(2 * i + 2, xPos, "%c", i + 65);
    }




//	 mvprintw(0, xPos, "    1   2   3   4   5   6   7   8   9   10 ");
//	 mvprintw(1, xPos, "   --------------------------------------- ");
//	 mvprintw(2, xPos, "A |   |   |   |   |   |   |   |   |   |   |");
//	 mvprintw(3, xPos, "  |---------------------------------------|");
//	 mvprintw(4, xPos, "B |   |   |   |   |   |   |   |   |   |   |");
//	 mvprintw(5, xPos, "  |---------------------------------------|");
//	 mvprintw(6, xPos, "C |   |   |   |   |   |   |   |   |   |   |");
//	 mvprintw(7, xPos, "  |---------------------------------------|");
//	 mvprintw(8, xPos, "D |   |   |   |   |   |   |   |   |   |   |");
//	 mvprintw(9, xPos, "  |---------------------------------------|");
//	mvprintw(10, xPos, "E |   |   |   |   |   |   |   |   |   |   |");
//	mvprintw(11, xPos, "  |---------------------------------------|");
//	mvprintw(12, xPos, "F |   |   |   |   |   |   |   |   |   |   |");
//	mvprintw(13, xPos, "  |---------------------------------------|");
//	mvprintw(14, xPos, "G |   |   |   |   |   |   |   |   |   |   |");
//	mvprintw(15, xPos, "  |---------------------------------------|");
//	mvprintw(16, xPos, "H |   |   |   |   |   |   |   |   |   |   |");
//	mvprintw(17, xPos, "  |---------------------------------------|");
//	mvprintw(18, xPos, "I |   |   |   |   |   |   |   |   |   |   |");
//	mvprintw(19, xPos, "  |---------------------------------------|");
//	mvprintw(20, xPos, "J |   |   |   |   |   |   |   |   |   |   |");
//	mvprintw(21, xPos, "   --------------------------------------- ");

	return;
}

void wygrana()
{
     erase();
     mvprintw(0, 5, "888       888 Y88b   d88P  .d8888b.  8888888b.         d8888 888b    888        d8888 ");
     mvprintw(1, 5, "888   o   888  Y88b d88P  d88P  Y88b 888   Y88b       d88888 8888b   888       d88888 ");
     mvprintw(2, 5, "888  d8b  888   Y88o88P   888    888 888    888      d88P888 88888b  888      d88P888 ");
     mvprintw(3, 5, "888 d888b 888    Y888P    888        888   d88P     d88P 888 888Y88b 888     d88P 888 ");
     mvprintw(4, 5, "888d88888b888     888     888  88888 8888888P'     d88P  888 888 Y88b888    d88P  888 ");
     mvprintw(5, 5, "88888P Y88888     888     888    888 888 T88b     d88P   888 888  Y88888   d88P   888 ");
     mvprintw(6, 5, "8888P   Y8888     888     Y88b  d88P 888  T88b   d8888888888 888   Y8888  d8888888888 ");
     mvprintw(7, 5, "888P     Y888     888      'Y8888P88 888   T88b d88P     888 888    Y888 d88P     888 ");

     mvprintw(9, 5, "press any buton...");

    return;
}

void przegrana()
{
     erase();
     mvprintw(0, 5, "8888888b.  8888888b.  8888888888P 8888888888 .d8888b.  8888888b.         d8888 888b    888        d8888 ");
     mvprintw(1, 5, "888   Y88b 888   Y88b       d88P  888       d88P  Y88b 888   Y88b       d88888 8888b   888       d88888 ");
     mvprintw(2, 5, "888    888 888    888      d88P   888       888    888 888    888      d88P888 88888b  888      d88P888 ");
     mvprintw(3, 5, "888   d88P 888   d88P     d88P    8888888   888        888   d88P     d88P 888 888Y88b 888     d88P 888 ");
     mvprintw(4, 5, "8888888P'  8888888P'     d88P     888       888  88888 8888888P'     d88P  888 888 Y88b888    d88P  888 ");
     mvprintw(5, 5, "888        888 T88b     d88P      888       888    888 888 T88b     d88P   888 888  Y88888   d88P   888 ");
     mvprintw(6, 5, "888        888  T88b   d88P       888       Y88b  d88P 888  T88b   d8888888888 888   Y8888  d8888888888 ");
     mvprintw(7, 5, "888        888   T88b d8888888888 8888888888 'Y8888P88 888   T88b d88P     888 888    Y888 d88P     888 ");

     mvprintw(9, 5, "press any buton...");

    return;
}

void wygrana1()
{
     erase();
     mvprintw(0, 5, "8888888b.  888             d8888 Y88b   d88P 8888888888 8888888b.        d888       888       888 8888888 888b    888  .d8888b.  ");
     mvprintw(1, 5, "888   Y88b 888            d88888  Y88b d88P  888        888   Y88b      d8888       888   o   888   888   8888b   888 d88P  Y88b ");
     mvprintw(2, 5, "888    888 888           d88P888   Y88o88P   888        888    888        888       888  d8b  888   888   88888b  888 Y88b.      ");
     mvprintw(3, 5, "888   d88P 888          d88P 888    Y888P    8888888    888   d88P        888       888 d888b 888   888   888Y88b 888  'Y888b.   ");
     mvprintw(4, 5, "8888888P'  888         d88P  888     888     888        8888888P'         888       888d88888b888   888   888 Y88b888     'Y88b. ");
     mvprintw(5, 5, "888        888        d88P   888     888     888        888 T88b          888       88888P Y88888   888   888  Y88888       '888 ");
     mvprintw(6, 5, "888        888       d8888888888     888     888        888  T88b         888       8888P   Y8888   888   888   Y8888 Y88b  d88P ");
     mvprintw(7, 5, "888        88888888 d88P     888     888     8888888888 888   T88b      8888888     888P     Y888 8888888 888    Y888  'Y8888P'  ");

     mvprintw(9, 5, "press any buton...");

    return;
}

void wygrana2()
{
     erase();
     mvprintw(0, 5, "8888888b.  888             d8888 Y88b   d88P 8888888888 8888888b.        .d8888b.       888       888 8888888 888b    888  .d8888b.  ");
     mvprintw(1, 5, "888   Y88b 888            d88888  Y88b d88P  888        888   Y88b      d88P  Y88b      888   o   888   888   8888b   888 d88P  Y88b ");
     mvprintw(2, 5, "888    888 888           d88P888   Y88o88P   888        888    888             888      888  d8b  888   888   88888b  888 Y88b.      ");
     mvprintw(3, 5, "888   d88P 888          d88P 888    Y888P    8888888    888   d88P           .d88P      888 d888b 888   888   888Y88b 888  'Y888b.   ");
     mvprintw(4, 5, "8888888P'  888         d88P  888     888     888        8888888P'        .od888P'       888d88888b888   888   888 Y88b888     'Y88b. ");
     mvprintw(5, 5, "888        888        d88P   888     888     888        888 T88b        d88P'           88888P Y88888   888   888  Y88888       '888 ");
     mvprintw(6, 5, "888        888       d8888888888     888     888        888  T88b       888'            8888P   Y8888   888   888   Y8888 Y88b  d88P ");
     mvprintw(7, 5, "888        88888888 d88P     888     888     8888888888 888   T88b      888888888       888P     Y888 8888888 888    Y888  'Y8888P'  ");

     mvprintw(9, 5, "press any buton...");

    return;
}
