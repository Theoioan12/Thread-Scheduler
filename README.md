Buliga Theodor Ioan
323 CA

Tema 2 - Planificator de threaduri

Descriere:

Tema implementeaza un planificator de threaduri. Acesta simuleaza
un scheduler pe procese preemptiv, intr-un sistem uniprocesor, care
utilizeaza algoritmul de planificare Round-Robin.

Structura de thread prezinta toate campurile de care e nevoie, iar
cele de tip intreg au fost alese de tip short pentru ca nu s-a lucrat
cu numere suficient de mari.

Pentru structura de scheduler am mers tot pe ideea de short pentru
tipurile intregi de date, iar pentru coada am ales un simplu array de
threaduri fiindca mi s-a parut mai usor de lucrat. In loc de mutex,
am ales sa folosesc semaforul.

Functiile "error_checking" si "compare_function" ar fi trebuit sa ma ajute
sa gestionez erorile intr-un mod mai lizibil, insa nu am reusit sa ma 
folosesc eficient de ele, dar le-am pastrat pentru eventuale actualizari
asupra codului.

O functie care consider ca necesita explicatii suplimentare este cea
de adaugare a unui thread, "a_thread".  In cazul in care coada de
prioritate nu este plina, se insereaza threadul pe pozitia
corespunzatoare in functie de campul in care se regaseste prioritatea.

Pentru cearea unui thread, am realizat o functie ajutatoare, unde
aloc memoria, initializez campurile, semaforul si apoi creez
threadul propriu-zis.

Functia "up_scheduler" actualizeaza schedulerul in functie de starile
threadurilor si prioritatea acestora. Tot aici e implementat si algoritmul
Round Robin. Functia va fi folosita mai departa in "s_routine".

Comentarii asupra temei:

Cred ca puteam realiza o tema mai buna, cel putin la nivel de modularizare
si gestionare a memoriei si vitezei de executie, as fi vrut sa reusesc si
sa impart in mai multe fisiere codul, dar a fost o perioada incarcata, 
cu mai multe deadline-uri in acelasi timp.
Per total, sunt multe lucruri de imbunatatit, dar am inteles mult mai bine
cum ar functiona un planificator de threaduri si ma simt mult mai stapan
la nivel de programare in C. Ma simt, de asemenea, mult mai pregatit
pentru examen si ma bucur ca am reusit sa aplic niste concepte destul
de dificile.

Bibliografie:
https://ocw.cs.pub.ro/courses/so/teme/tema-4
https://ocw.cs.pub.ro/courses/so/cursuri/curs-04
https://ocw.cs.pub.ro/courses/so/cursuri/curs-08
https://ocw.cs.pub.ro/courses/so/cursuri/curs-09
https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-08
https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-09