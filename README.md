# Projekt 4 Zadanie 3(Winda)
Szymon Zawiślka 203531\
Piotr Wilk 203469


Napisz program symulujący działanie windy. Symulator powinien uwzględniać możliwość zatrzymywania
się na poszczególnych piętrach oraz przywołania winda za pomocą przycisku na interfejsie użytkownika.
Program powinien kolejkować wywołania. Prezentacja poruszania się windy powinna zostać
przedstawiona na modelu osadzonym w przestrzeni dwuwymiarowej.

![image](https://github.com/user-attachments/assets/2a7168b5-9463-4c91-aee0-11e31f72ed3a)

Użytkownik tworzy nowe osoby, klikając przycisk na odpowiednim piętrze. Liczby na przyciskach oznaczają piętro docelowe stworzonej osoby.

Jeżeli winda stoi (KierunekJazdy == 'S'), to po stworzeniu pierwszej osoby winda wybiera odpowiedni kierunek ('G' lub 'D') i dodaje to piętro do kolejki jazdy.

Podczas ruchu windy, gdy pojawi się nowe wezwanie, winda sprawdza, czy można dodać je do aktualnej kolejki w zależności od kierunku jazdy i położenia piętra. Jeżeli nie, piętro jest dodawane do odpowiedniej kolejki oczekujących (KolejkaCzekajacychGora lub KolejkaCzekajacychDol).

Nowo dodane piętra są sortowane rosnąco lub malejąco w kolejce, aby zoptymalizować trasę windy (w górę rosnąco, w dół malejąco).

Gdy winda zatrzyma się na piętrze, najpierw wysiadają osoby, których celem jest to piętro, a następnie wsiadają osoby z tego piętra, które chcą jechać w tym samym kierunku co winda (lub ustawiają kierunek, jeśli winda stoi). Jeśli winda jest pełna (waga osób >= 600), osoby które nie zmieszczą się, pozostają na piętrze i nie są usuwane z kolejki oczekujących. Winda posiada mechanizm wyświetlający aktualną sumaryczną wagę wszystkich pasażerów w prawym górnym rogu interfejsu.

Po zatrzymaniu się, winda odlicza 5 sekund postoju. Jeśli w tym czasie nie pojawią się nowe wezwania, winda automatycznie zaczyna wracać na parter (piętro 0).



