


Mai întâi am verificat dacă într-adevăr nu este semnalul SIGSEGV, cel care dă Segmentation Fault și dacă nu este acesta, se cheamă handler-ul default.

Am verificat dacă totuși și există o adresă fiabilă de eroare și dacă nu este se cheamă handler-ul default. Dacă au trecut aceste verificări, iau adresa erorii și dacă e nulă atunci se cheamă iar handler-ul default. Altfel, iau adresa de început a unui segment și pe cea finala și încerc să verific dacă adresa care a dat eroarea se află între aceste două adrese care descriu un segment/lungimea unui segment.

Dacă se întâmpla acest lucru, marchez variabila de verificare ok cu 1 și rețin poziția segmentului în variabila segm_x.

Dacă s-a putut încadra adresa erorii într-un segment, atunci încerc să aflu pagina în care există adresa aceasta. Dacă da, verific dacă pagina respectivă se află sau nu în segmentul bss și rețin numărul paginii in variabila pg_x.

Existenta in segmentul bss în marchez iar prin intermediul unei variabile bsss, inițial nulă.

Am făcut un fel de matrice, sau mai bine zis vector de vectori în care verific pentru fiecare pagină a unui segment dacă e mapată sau nu. 
Dacă pagina e nemapată, adică dacă pe linia și coloana din matricea corespunzătoare e 0, atunci pagina e nemapată. În fond, inițial, toate paginile sunt nemapate, motiv pentru care întreaga matrice este nulă.

Dacă e nemapată, atunci verific pentru cazurile în care există sau nu pagina în segmentul bss. Urmează să aflu dimensiunea pe care trebuie să mapez in memorie, care se ia pe mai multe cazuri, în funcție de locul paginii care a dat eroarea. Încep cu cazul în care pagina cu eroarea nu se află în acest segment, motiv pentru care verific dacă dimensiunea fișierului e mai mică decât dimensiunea unei pagini și, în funcție de acest lucru, iau dimensiunea pe care trebuie să mapez. Altfel, dacă face, totuși, parte din bss, am mai multe cazuri: când dimensiunea până la pagina curentă mai mică decât cea a fișierului, atunci e o mapare normală; dacă eu mai mare atunci se zeroizează.  

Daca sunt egale, se citește din fișierul executabil, când dimensiunea din segment până la pagina actuală e mai mică decât dimensiunea fișierului. Se face o diferență și se verifică dacă diferența aceasta e mai mică sau mai mare decât dimensiunea unei pagini propriu-zise. Pentru cazul în care este strict mai mare, știu că trebuie să mapez fix o pagină (nu mai mult) și, în schimb, pentru cazul în care sunt egale, am alte două cazuri în care verific dacă e mai mic sau nu decât dimensiunea până la următoarea pagină. În funcție de aceasta, reglez dimensiunea pe care eu o folosesc să mapez.

Tot în cazul de egalitate dintre dimensiunea până la pagina curentă și cea a fișierului, marchez faptul că trebuie să fac o citire din fișier prin intermediul variabilei reed. La final de tot, până să ies din cazul în care verific existența paginii cu eroarea în segmentul bss, marchez o variabilă ce indică dacă sau nu să pun zerouri memorie, variabila memm, mai ales că în zona bss sunt date neinițializate, nule.

După ce am aflat dimensiunea pe care să mapez, pun pagina in memorie (fac mmap). Dacă maparea eșuează, afișez numele funcției unde au loc eroarea și ies din program. In rest, marchez în matricea de pagini pentru fiecare segment faptul că pagina respectiva fost mapată.

Apoi încep celelalte verificări pentru care am luat atât de multe variabile de verificare: dacă este nevoie de zeroizare, adică variabila memm este unu, atunci folosesc memset pentru a pune zero-uri in memorie, în locul rămas dintre mem_size si file_size (mem_size e mai mare). Pentru cazul în care trebuie să citesc, adică atunci când eroarea pică în mijlocul unei pagini, citesc din fișierul de executabil punând cursorul de unde am mapat, la offsetul de la care începe segmentul respectiv + dimensiunea paginilor până la pagina care la care a avut loc eroarea și citesc până când se ajunge la adresa erorii care în acest caz pică în mijlocul paginii.

La final de tot, în cazul în care pagina nu a fost mapată, pun permisiunile cu ajutorul funcției mprotect în locul unde am mapat, de dimensiunea unei pagini.

La fel, dacă funcțiile mprotect, funcția de citire și funcția lseek dau erori, afișez numele funcției care a dat eroarea și ies din program.

Astfel, ies din if -ul care a verificat că pagina e nemapată. Dacă e, se cheamă handler-ul default.

La fel, când ies din verificarea faptului ca pagina e printre segmente, dacă aceasta nu aparține niciunuia, se cheamă hander-ul default.

Totuși, nu este unicul caz în care a trebuit să modific fișierul dat pentru a-l face complet funcțional. În funcția de inițializare so_init_loader a mai trebuit să pun un set gol de semnale la care mai târziu am adăugat semnalul SIGSEGV și am modificat tratarea semnalului acesta cu adăugarea la final a handler-ului default, pe care l-am inițializat eu înainte static global.

În funcția ce deservește drept main,  so_execute, am extras dimensiunea unei pagini pentru a putea afla numărul de pagini din fiecare segment din vectorul de segmente al structurii de executabil, asta pentru a aloca matricea de pagini mapate și nemapate, am deschis fișierul executabil cu drepturi de citire, iar la final, după ce s-a chemat funcția de pornire a executabilului, am închis fișierul.

Această temă a fost realizată cu ajutorul cursurilor și laboratoarelor vechi dispuse în secțiunea resurse utile din cerința temei de pe ocw, fiind de foarte mult ajutor laboratoarele patru și șase. Altă contribuție majoră la temă am avut și paginile de manual ale funcțiilor mmap, lseek, read, mprotect, memset.



