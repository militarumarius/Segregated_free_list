### Nume: Militaru Ionut-Marius

### Segregated Free Lists 


### Descriere:

* In cadrul temei am implementat un alocator de memorie folosind structura de 
date Segregated Free Lists.Aceasta structura am implementat-o folosind un
vector de liste dublu inlantuite, la care fiecare nod reprezinta o zona de 
memorie din alocatorul virtual.Acesta contine o adresa si o dimensiune necesare
pentru a functiona in parametrii optimi alocatorul.

### Comenzile implementate sunt:
* INIT_HEAP: la aceasta comanda intiez heapul , creand vectorul de liste 
            in functie de adresa data
* MALLOC:   la aceasta comanda caut in vectorul de liste o adresa care sa aiba
            dimensiunea mai mica sau egala cu dimensiunea data si mut blocul
            in lista de blocuri alocate din alocator. In functie de numarul
            de bytes cititi blocurile se pot fragmenta
* DUMP_MEMORY: la aceasta comanda afisez detalii despre memoria alocatorului
* WRITE:    la aceasta comanda scriu la o adresa data o fraza, in functie de
            dimnesiunea pe care trebuie sa o scriu verific daca exista o zona
            consecutiva de memorie alocata. In caz ca nu exista programul
            afiseaza Segmentation fault.
* READ:     citesc de la o adresa data ceea ce se afla retinut in memorie
            In caz ca zona in care trebuie sa citesc nu este alocata total,
            programul afiseaza Segmentation fault
* FREE:     In cazul acestei comenzi fiecare bloc de memorie din lista de
            memorie alocata se muta in functie de tipul heapului intial 
            inapoi in vectorul de liste. Daca type=0 se muta direct in lista
            corespunzatoare dimensiunii actuale, iar daca type=1 se lipeste
            cu blocurile corespunzatoare pentru a forma structura initiala
            a heapului.
* DESTROY_HEAP: distrug tot heapul , si dau free la toata memoria necesara

### Comentarii asupra proiectului:

* As fi putut la functia add_node_by_address sa nu mai calculez pe ce
pozitie ar trebuii sa adaug nodul si dupa sa apelez functia add_nth_node
si sa-l adaug direct in lista in acea functie, dar am zis sa folosesc functia
pe care am implementat-o la laborator
* Din realizarea acestei teme am invatat sa lucrez cu liste dublu inlantuite ,
cum functioneaza un alocator de memorie si cum sa implementez
Segregated Free Lists

