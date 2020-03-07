#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <map>


using namespace std;


/*

Struktura uredjenaTrojka je samo pomocna struktura koja nam sluzi da kada budemo citali podatke iz fajla, da mozemo svaki red u fajlu smjestiti
u jednu prirodnu cjelinu te takvu ubacujemo u vektor iz kojeg cemo citati sve te vrijednosti i na osnovu njih formirati disjunktne skupove. Taj
vektor ce biti proslijedjen kao parametar konstruktoru klase konvertorValuta. Dakle, ova struktura sadrzi 2 stringa i jedan racionalni broj kao
atribute te klasicne konstruktore bez i sa parametrima, kao i getere i setere.

*/

class uredjenaTrojka {

    string s1, s2;
    double d;

public:

    uredjenaTrojka() : s1(""), s2(""), d(0)  {}
    uredjenaTrojka(string _s1, string _s2, double _d) : s1(_s1), s2(_s2), d(_d) {}

    void setuT(string _s1, string _s2, double _d) {
        s1 = _s1;
        s2 = _s2;
        d = _d;
    }

    string getS1() {
        return s1;
    }

    string getS2() {
        return s2;
    }

    double getD() {
        return d;
    }

};

/*

Struktura cvor ce sadrzavati sve atribute neke valute (svaki cvor cemo mapirati stringom - tj. nazivom neke valute kao sto cemo uskoro pokazati).
Cvor se sastoji od naziva (atribut ce kasnije biti koristan kod ispitivanja da li neke dvije valute pripadaju istom skupu - tj. da li neka dva
cvora imaju istog predstavnika). Predstavnik je pokazivac na cvor koji je predstavnik nekog disjunktnog skupa. U odnosu na predstavnika skupa
su definisane sve relacije medju valutama tog skupa i preko njega racunamo trazene relacije u konstantnom vremenu. Disjunktni skup od n elemenata
mozemo posmatrati kao stablo visine 1 gdje je prvi cvor predstavnik sa kojim je povezano ostalih n-1 elemenata. Predstavnik predstavnika je
sam predstavnik i sam sa sobom logicno ima odnos 1. odnosSaPredstavnikom je atribut koji oznacava koliko 1 jedinica valute predstavnik vrijedi
jedinica trenutne valute, tj. this cvora. nivo nam govori prilikom spajanja neka dva disjunktna skupa koji ce skup se pridruziti kojem (koji
ce predstavnik postat kome predstavnik). Pamtimo samo predstavniku nekog disjunktnog skupa nivo, ostalim clanovima nije potrebno. Na kraju standardan
konstruktor bez parametara koji postavlja vrijednosti na neke pocetne.

*/

struct cvor {

    string naziv;
    cvor *predstavnik;
    double odnosSaPredstavnikom;
    int nivo;

    cvor() : naziv(""), predstavnik(this), odnosSaPredstavnikom(1.0), nivo(0) {}

};

/*

Imamo klasu disjunktniSkupovi u kojoj baratamo disjunktnim skupovima. Svi cvorovi su, kao sto je receno, sadrzani u mapi kao vrijednosti za neke
stringove (nazive valuta, odnosno njihove skracenice). Koristen je std::map jer, pored toga sto je jednostavno prirodno koristiti mapiranje valuta,
on ima ugradjene funkcije at/find i count koje su jako korisne za potrebe rjesavanja ovog problema. Podjimo od jednostavnijih do slozenijih funkcija
koje su definisane u ovoj klasi. Funkcija napraviSkup za proslijedjene vrijednosti valute i zadanog odnosa sa predstavnikom formira cvor kojeg
opisujemo upravo sa proslijedjenim nazivom valute i njenim odnosnom sa predstavnikom (na pocetku za sve jednoclane skupove, predstavnik tog
skupa je naravno sam cvor koji se nalazi u tom skupu). Takodjer stavljamo mu da mu je nivo 0, uzimamo iterator sa kraja mape i na to mjesto
smjestamo kljuc-valuta i cvor-vrijednost valute. Dalje imamo funkciju dajOdnosSaPredstavnikom cije ime samo govori sta radi i nije potrebno
neko objasnjenje posto je veoma jednostavna za shvatiti. Medjutim, konstantna funkcija nadjiPredstavnikaSkupa koja kao parametar prima cvor po
referenci obavlja niz vaznih stvari koje nisu ocigledne. Dakle, kao sto samo ime govori ona vraca predstavnika skupa u kojem se nalazi proslijedjeni
cvor c (tacnije pokazivac na njega). Ako je naziv predstavnika jednak nazivu samog cvora c, to znaci da smo pronasli trazeni cvor (predstavnika
pocetnog cvora - rjesenje je rekurzivno). Inace, mijenjamo odnos sa predstavnikom tako sto mnozimo trenutnu vrijednost odnosa sa predstavnikom
sa odnosa sa predstavnikom predstavnika (ovo sam uradio posto sam primijetio da to tako ide nakon sto sam raspisao na papir par slucajeva). Takodjer,
ono sto omogucava da se pretraga vrsi u gotovo konstantnom vremenu (ogranicenom inverznom Ackermannovom funkcijom) jeste kompresija puta
gdje u svakom rekurzivnom pozivu za predstavnika trenutnog postavljamo predstavnikovog predstavnika. Na ovaj nacin svi elementi jednog skupa
ce imati istog predstavnika te ce se odrzavati trazena dubina stabla. Na kraju vracamo tog predstavnika. Funkcija nadjiPredstavnikaSkupa koja
prima kao parametar string koristi ovu funkciju koja vraca pokazivac na cvor i mapu gdje je mapiran cvor sa trazenom valutom.
Na kraju, unija takodjer prima dvije valute kao parametre te jednostavno trazi predstavnike njihovih skupova te dodaje jedan skup drugome. Primjetimo
da u sustini ovdje ne baratamo nikakvim skupovima, nego to podrazumijevamo pod tim da neka grupa cvorova ima kao atribut pokazivac na neki cvor
koji predstavlja glavni cvor tog skupa. Uniranje se vrsi prema nivoima, tj. onaj predstavnik koji je viseg nivoa ce postati predstavnik
objedinjenog disjunktnog skupa. Ova funkcija vraca pokazivac na predstavnika koji je pobijedio (koji je bio viseg nivoa) sto ce nam biti korisno
kasnije prilikom racunanja novih odnosa valuta nakon objedinjavanja skupova.

*/

class disjunktniSkupovi {

public:

    map<string, cvor*> mapa; //svakom nazivu valute je pridruzen cvor, tj. svakoj valuti je pridruzen cvor

    cvor* nadjiPredstavnikaSkupa (cvor& c) const {
        cvor *predstavnik = c.predstavnik;

        if (predstavnik->naziv == c.naziv) {
            return predstavnik;
        }

        c.odnosSaPredstavnikom *= c.predstavnik->odnosSaPredstavnikom;

        c.predstavnik = nadjiPredstavnikaSkupa(*c.predstavnik);
        return c.predstavnik;
    }

    void napraviSkup (string valuta, double odnosSaPreds) {
        cvor *c = new cvor();
        c->naziv = valuta;
        c->predstavnik = c;
        c->nivo = 0;
        c->odnosSaPredstavnikom = odnosSaPreds;
        auto it = mapa.end();
        mapa.insert(it, pair<string,cvor*> (valuta, c));
    }

    cvor *unija (string valuta1, string valuta2) {

        cvor& c1 = *mapa.at(valuta1);
        cvor& c2 = *mapa.at(valuta2);

        cvor *predstavnik1 = nadjiPredstavnikaSkupa(c1);
        cvor *predstavnik2 = nadjiPredstavnikaSkupa(c2);

        if (predstavnik1->naziv == predstavnik2->naziv) return predstavnik1;

        if (predstavnik1->nivo >= predstavnik2->nivo) {
            if (predstavnik1->nivo == predstavnik2->nivo) {
                predstavnik1->nivo++;
            }
            predstavnik2->predstavnik = predstavnik1;

            return predstavnik1;
        }
        else {
            predstavnik1->predstavnik = predstavnik2;

            return predstavnik2;
        }

    }

    string nadjiPredstavnikaSkupa(string valuta) {
        return nadjiPredstavnikaSkupa(*mapa.at(valuta))->naziv;
    }


    double dajOdnosSaPredstavnikom (const string& valuta) const {
        auto iterCvor = mapa.find(valuta);
        if (iterCvor == mapa.end()) {
            return 0.;
        } else {
            return iterCvor->second->odnosSaPredstavnikom;
        }
    }

};

/*

Klasa konvertovValuta sadrzi kao atribut mapu disjunktnih skupova na kojima se mogu izvrsavati operacije pravljenja novog skupa, uniranja,
pronalazenja predstavnika skupa i trazenja odnosa sa predstavnikom neke valute/cvora. Dakle, ovo je prirodno bilo za uraditi posto na konvertor
valuta mozemo gledati kao na specijalan slucaj disjunktnih skupova (koji su modifikovani u svojoj definiciji kako bi se mogli koristiti za
konvertiranje valuta). Dodavanje odnosa izmedju neke dvije valute vrsimo pomocu funkcije dodajOdnos (koja logicno prima dvije valute i njihov
odnos) kao parametre te ispituje vise stvari. Ako u mapi nemamo nijednu od te dvije valute, to znaci da su one novodefinisane i one ce formirati
same svoj disjunktni skup. Ako je prva valuta vec ranije definisana, a druga nije, onda trazimo predstavnika prve i uniramo predstavnika i drugu
, tj. dodajemo drugu valutu u disjunktni skup prve. Analogno vrijedi ako je prva nedefinisana prethodno, a druga jeste. Na kraju, imamo slucaj
ako su obje valute prethodno definisane u mapi valuta. Sada razlikujemo dva slucaja, ukoliko pripadaju istom skupu (imaju istog predstavnika)
i ako ne pripadaju. Ako pripadaju, tada ne treba raditi nista jer su vec unirane. Ako ne pripadaju istom skupu vrsimo uniju. Dakle, uniramo
po predstavnicima dvije zadane valute i pamtimo koji je predstavnik pobijedio (tj. koji ce biti predstavnik cjelokupnog skupa). Ako je pobijedio
prvi onda je ovom predstavniku gubitniku odnos sa predstavnikom jednak datom odnosu izmedju valuta 1 i 2 pomnozenom sa omjerom odnosa sa predstavnikom
predstavnika pobjednika i gubitnika (opet ovo se najlakse uvidja ako sebi graficki predstavimo situaciju nakon uniranja neka dva skupa).
Ostali odnosi ce biti podeseni pozivom funkcije nadjiPredstavnikaSkupa u funkciji dajOdnos. Ako je drugi predstavnik pobijedio, potrebno je samo
invertirati odnos (posto je dat odnos izmedju valute 1 i 2, a sada defacto imamo odnos izmedju valuta 2 i 1 pa je stoga potrebno gledati 1/odnos)
kao i obrnut omjer u odnosu na prethodni slucaj. Na kraju imamo funkciju koja ispituje da li dvije valute pripadaju istom skupu. Ako nisu, vraca 0
, a ako jesu vraca omjer odnosa sa predstavnikom (opet veoma lahko se procita sa grafickog prikaza situacije).

*/

class konvertorValuta {

    disjunktniSkupovi dS;

public:

    konvertorValuta (vector<uredjenaTrojka> v) {
        for (auto x : v) {
            dodajOdnos(x.getS1(), x.getS2(), x.getD());
        }
    }

    void dodajOdnos (string valuta1, string valuta2, double odnos) {

        if (dS.mapa.count(valuta1) == 0 && dS.mapa.count(valuta2) == 0) {
                dS.napraviSkup(valuta1, 1.0);
                dS.napraviSkup(valuta2, odnos);
                dS.unija(valuta1, valuta2);
            }
            else if (dS.mapa.count(valuta1) > 0 && dS.mapa.count(valuta2) == 0) {
                string predstavnik1 = dS.nadjiPredstavnikaSkupa(valuta1);
                dS.napraviSkup(valuta2, odnos * dS.dajOdnosSaPredstavnikom(valuta1));
                dS.unija(predstavnik1, valuta2);
            }
            else if (dS.mapa.count(valuta1) == 0 && dS.mapa.count(valuta2) > 0) {
                string predstavnik2 = dS.nadjiPredstavnikaSkupa(valuta2);
                dS.napraviSkup(valuta1, 1/odnos * dS.dajOdnosSaPredstavnikom(valuta2));
                dS.unija(valuta1, predstavnik2);
            }
            else {
                string predstavnik1 = dS.nadjiPredstavnikaSkupa(valuta1);
                string predstavnik2 = dS.nadjiPredstavnikaSkupa(valuta2);
                if (predstavnik1 != predstavnik2) {
                    cvor *pr = dS.unija(predstavnik1, predstavnik2);
                    cvor &c1 = *dS.mapa.at(valuta1);
                    cvor &c2 = *dS.mapa.at(valuta2);
                    if (c1.predstavnik == pr) {
                        dS.mapa.at(predstavnik2)->odnosSaPredstavnikom = odnos * c1.odnosSaPredstavnikom / c2.odnosSaPredstavnikom;
                    } else {
                        dS.mapa.at(predstavnik1)->odnosSaPredstavnikom = 1.0 / odnos * c2.odnosSaPredstavnikom / c1.odnosSaPredstavnikom;
                    }
                }
            }

    }

    double dajOdnos (string valuta1, string valuta2) {

        cvor& c1 = *dS.mapa.at(valuta1);
        cvor& c2 = *dS.mapa.at(valuta2);

        cvor *predstavnik1 = dS.nadjiPredstavnikaSkupa(c1);
        cvor *predstavnik2 = dS.nadjiPredstavnikaSkupa(c2);

        if (predstavnik1->naziv != predstavnik2->naziv) return 0;
        else {
            return c2.odnosSaPredstavnikom / c1.odnosSaPredstavnikom;
        }
    }
};

int main()
{

    string line;
    ifstream myfile ("odnosiValuta.txt");

    vector<uredjenaTrojka> valuteUOdnosu;

    if (myfile.is_open()) {

        while ( getline (myfile,line) ) {
            std::string s = line;
            std::stringstream ss(s);
            std::istream_iterator<std::string> begin(ss);
            std::istream_iterator<std::string> end;
            std::vector<std::string> vstrings(begin, end);
            string prvaValuta = vstrings[0], drugaValuta = vstrings[1];
            double odnosValuta = stod(vstrings[2]);
            uredjenaTrojka uT(prvaValuta, drugaValuta, odnosValuta);
            valuteUOdnosu.push_back(uT);
          }

            myfile.close();
        }

    else cout << "Nece file da otvori.";

    konvertorValuta kV(valuteUOdnosu);

    cout << "EUR" << " - " << "BAM" << " : " << kV.dajOdnos("EUR", "BAM") << endl;
    cout << "BAM" << " - " << "EUR" << " : " << kV.dajOdnos("BAM", "EUR") << endl;
    cout << "EUR" << " - " << "RSD" << " : " << kV.dajOdnos("EUR", "RSD") << endl;
    cout << "RSD" << " - " << "EUR" << " : " << kV.dajOdnos("RSD", "EUR") << endl;
    cout << "BAM" << " - " << "RSD" << " : " << kV.dajOdnos("BAM", "RSD") << endl;
    cout << "RSD" << " - " << "BAM" << " : " << kV.dajOdnos("RSD", "BAM") << endl;
    cout << endl;
    cout << "USD" << " - " << "HRK" << " : " << kV.dajOdnos("USD", "HRK") << endl;
    cout << "HRK" << " - " << "USD" << " : " << kV.dajOdnos("HRK", "USD") << endl;
    cout << "USD" << " - " << "GBP" << " : " << kV.dajOdnos("USD", "GBP") << endl;
    cout << "GBP" << " - " << "USD" << " : " << kV.dajOdnos("GBP", "USD") << endl;
    cout << "GBP" << " - " << "HRK" << " : " << kV.dajOdnos("GBP", "HRK") << endl;
    cout << "HRK" << " - " << "GBP" << " : " << kV.dajOdnos("HRK", "GBP") << endl;
    cout << endl;
    cout << "BAM" << " - " << "HRK" << " : " << kV.dajOdnos("BAM", "HRK") << endl;
    cout << "USD" << " - " << "RSD" << " : " << kV.dajOdnos("USD", "RSD") << endl;

    return 0;
}
