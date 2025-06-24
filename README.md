# Projekt 4 Zadanie 3(Winda)
Szymon Zawiślka 203531\
Piotr Wilk 203469


Napisz program symulujący działanie windy. Symulator powinien uwzględniać możliwość zatrzymywania
się na poszczególnych piętrach oraz przywołania winda za pomocą przycisku na interfejsie użytkownika.
Program powinien kolejkować wywołania. Prezentacja poruszania się windy powinna zostać
przedstawiona na modelu osadzonym w przestrzeni dwuwymiarowej.

![image](https://github.com/user-attachments/assets/2a7168b5-9463-4c91-aee0-11e31f72ed3a)

Użytkownik tworzy nowe osoby klikając przycisk na odpowiednim piętrze. Liczby na przycziskąch oznaczają 
pietro docelowe stworzonej osoby.

Jeżeli winda stoi to po stworzeniu pierwszej osoby winda wybiera odpowiedni kierunek i dodaje te piętro do kolejki.
Gdy winda porusza się i pojawi się nowe wezwanie winda sprawdza czy może je dodać do aktualnej kolejki. Jeżeli tak opcja 
nie istnije piętro jest dodawane do odpowidniej kolejki zależności od kierunku jazdy. 
Jeżeli winda zatrzyma się na pietrze. z windy najpierw wychodzą osoby a następnie do windy wchodzą jedynie osoby których 
kierunek jazdy zgadza się z kierunkiem jazdy windy.W przypadku kiedy waga jest maksymalna i nikt nie moze już wjeść osoby 
te zostają na pietrze i nie są usuwane z kolejki. 


Winda po zatrzymaniu się zaczyna odliczać czas do 5 sekund po czym zaczyna wracać na parter.

Winda posiada mechanizm, który wyświetla aktualna wage wsyzstkich użytkowników windy w prawym górnym rogu.
