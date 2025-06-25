// Symulacja Windy
//Piotr Wilk 203469
//Szymon Zawiślak 203531

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
using namespace Gdiplus;
using namespace std;
#pragma comment(lib, "Gdiplus.lib")

HWND guziki[5][5];
HINSTANCE globalne;

LRESULT CALLBACK ProceduraOkna(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Osoba {
public:
    int PietroWsiadania;
    int PietroWysiadania;
    char KierunekJazdy;
    bool JedzieWinda;
    static constexpr int Waga = 70;

    Osoba(int Poczatek, int Koniec)
        : PietroWsiadania(Poczatek), PietroWysiadania(Koniec), JedzieWinda(false)
    {
        if (Poczatek < Koniec)
            KierunekJazdy = 'G';
        else
            KierunekJazdy = 'D';
    }

    int WagaOsoby() const { return Waga; }
    int SkadJedzie() const { return PietroWsiadania; }
    int DokadJedzie() const { return PietroWysiadania; }
    bool CzyJedzie() const { return JedzieWinda; }
    char PodajKierunek() const { return KierunekJazdy; }

    void Wsiadz() { JedzieWinda = true; }
    void Wysiadz() { JedzieWinda = false; }
};

vector<Osoba> osobyNaPietrze[5];

int PozycjeWindy(int pietro, int gornaKrawedzSzybu, int wysokoscPietra, int margines) {
    return gornaKrawedzSzybu + (4 - pietro) * wysokoscPietra + margines;
}

class Winda {
public:
    int AktualnePietro = 0;
    vector<int> KolejkaJazdy;
    vector<int> KolejkaCzekajacychGora;
    vector<int> KolejkaCzekajacychDol;
    int WagaWindy = 0;
    char KierunekJazdy = 'S'; // 'G' - góra, 'D' - dół, 'S' - stop
    bool CzyPostoj = false;
    bool PowrotNaParter = false;
    time_t CzasPostoju;
    vector<Osoba> osobyWinda;
    float PozycjaY_Windy = 0.0f;
    float CelY_Windy = 0.0f;
    bool WindaWTrasie = false;

    int NajblizszePietroWkolejce() {
        if (KolejkaJazdy.empty()) return -1;


        int najblizsze = -1;
        int minOdleglosc = 1000;

        for (int pietro : KolejkaJazdy) {
            if ((KierunekJazdy == 'G' && pietro > AktualnePietro) ||
                (KierunekJazdy == 'D' && pietro < AktualnePietro) ||
                KierunekJazdy == 'S') {
                int odleglosc = abs(pietro - AktualnePietro);
                if (odleglosc < minOdleglosc) {
                    minOdleglosc = odleglosc;
                    najblizsze = pietro;
                }
            }
        }
        return najblizsze;
    }

    void GdzieJechac(int pietro, char kierunek) {

        if (KierunekJazdy == 'S') {
            if (find(KolejkaJazdy.begin(), KolejkaJazdy.end(), pietro) == KolejkaJazdy.end())
                KolejkaJazdy.push_back(pietro);
            if (AktualnePietro < pietro) KierunekJazdy = 'G';
            else if (AktualnePietro > pietro) KierunekJazdy = 'D';
            else KierunekJazdy = 'S';
            return;
        }


        bool kierunekZgodny = (KierunekJazdy == kierunek);
        bool pietroPrzed = (KierunekJazdy == 'G' && pietro > AktualnePietro) || (KierunekJazdy == 'D' && pietro < AktualnePietro);

        if (kierunekZgodny && pietroPrzed) {

            if (find(KolejkaJazdy.begin(), KolejkaJazdy.end(), pietro) == KolejkaJazdy.end()) {
                KolejkaJazdy.push_back(pietro);
                if (KierunekJazdy == 'G')
                    sort(KolejkaJazdy.begin(), KolejkaJazdy.end());
                else
                    sort(KolejkaJazdy.begin(), KolejkaJazdy.end(), greater<int>());
            }
        }
        else {

            if (kierunek == 'G') {
                if (find(KolejkaCzekajacychGora.begin(), KolejkaCzekajacychGora.end(), pietro) == KolejkaCzekajacychGora.end()) {
                    KolejkaCzekajacychGora.push_back(pietro);
                }
            }
            else if (kierunek == 'D') {
                if (find(KolejkaCzekajacychDol.begin(), KolejkaCzekajacychDol.end(), pietro) == KolejkaCzekajacychDol.end()) {
                    KolejkaCzekajacychDol.push_back(pietro);
                }
            }
        }
    }

    void RuchWindy(int gornaKrawedzSzybu, int wysokoscPietra, int margines) {
        time_t teraz = time(NULL);

        // Postój windy na pietrze
        if (CzyPostoj && difftime(teraz, CzasPostoju) < 3) // Czeka 3 sekundy
            return;
        CzyPostoj = false;

        // Powrót na parter
        if (KierunekJazdy == 'S' && osobyWinda.empty() && KolejkaJazdy.empty() &&
            KolejkaCzekajacychGora.empty() && KolejkaCzekajacychDol.empty()) {
            if (difftime(teraz, CzasPostoju) >= 5 && AktualnePietro != 0 && !PowrotNaParter) {
                PowrotNaParter = true;
                KolejkaJazdy.push_back(0);
                KierunekJazdy = 'D';
            }
        }

        // Postój windy na pietrze
        if (CzyPostoj && difftime(teraz, CzasPostoju) < 3)
            return;
        CzyPostoj = false;

        // Przekazywanie nowej kolejki
        if (KolejkaJazdy.empty()) {
            if (KierunekJazdy == 'S') {
                if (!KolejkaCzekajacychGora.empty()) {
                    KierunekJazdy = 'G';
                    KolejkaJazdy = KolejkaCzekajacychGora;
                    KolejkaCzekajacychGora.clear();
                }
                else if (!KolejkaCzekajacychDol.empty()) {
                    KierunekJazdy = 'D';
                    KolejkaJazdy = KolejkaCzekajacychDol;
                    KolejkaCzekajacychDol.clear();
                }
                else {

                    return;
                }
            }
            else {
                // Nie ma zleceń to stój
                KierunekJazdy = 'S';
                return;
            }
        }

        int cel = NajblizszePietroWkolejce();

        if (cel == -1) {
            KierunekJazdy = 'S';
            return;
        }

        if (cel > AktualnePietro) KierunekJazdy = 'G';
        else if (cel < AktualnePietro) KierunekJazdy = 'D';
        else KierunekJazdy = 'S';

        // Ruch windy
        float predkosc = 10.0f;  // px na tik timeraa

        if (!WindaWTrasie) {
            int cel = NajblizszePietroWkolejce();
            if (cel == -1) {
                KierunekJazdy = 'S';
                return;
            }

            //na którym piętrze się mniej więcej znajduje winda
            CelY_Windy = PozycjeWindy(cel, gornaKrawedzSzybu, wysokoscPietra, margines);
            WindaWTrasie = true;

            if (CelY_Windy < PozycjaY_Windy) KierunekJazdy = 'G';
            else if (CelY_Windy > PozycjaY_Windy) KierunekJazdy = 'D';
            else KierunekJazdy = 'S';
        }

        if (KierunekJazdy == 'G') {
            PozycjaY_Windy -= predkosc;
            if (PozycjaY_Windy <= CelY_Windy) {
                PozycjaY_Windy = CelY_Windy;
                WindaWTrasie = false;
                AktualnePietro = NajblizszePietroWkolejce();
                KolejkaJazdy.erase(remove(KolejkaJazdy.begin(), KolejkaJazdy.end(), AktualnePietro), KolejkaJazdy.end());
                CzyPostoj = true;
                CzasPostoju = time(NULL);
            }
        }
        else if (KierunekJazdy == 'D') {
            PozycjaY_Windy += predkosc;
            if (PozycjaY_Windy >= CelY_Windy) {
                PozycjaY_Windy = CelY_Windy;
                WindaWTrasie = false;
                AktualnePietro = NajblizszePietroWkolejce();
                KolejkaJazdy.erase(remove(KolejkaJazdy.begin(), KolejkaJazdy.end(), AktualnePietro), KolejkaJazdy.end());
                CzyPostoj = true;
                CzasPostoju = time(NULL);
            }
        }

        // Winda dojechała na piętro
        if (AktualnePietro == cel) {
            KolejkaJazdy.erase(remove(KolejkaJazdy.begin(), KolejkaJazdy.end(), cel), KolejkaJazdy.end());
            CzyPostoj = true;
            CzasPostoju = teraz;

            // Czy ma jechać dalej?
            bool dalejWtymKierunku = false;
            for (int pietro : KolejkaJazdy) {
                if ((KierunekJazdy == 'G' && pietro > AktualnePietro) ||
                    (KierunekJazdy == 'D' && pietro < AktualnePietro)) {
                    dalejWtymKierunku = true;
                    break;
                }
            }

            // Jeśli nie zatrzymaj na chwile winde
            if (!dalejWtymKierunku) {
                KierunekJazdy = 'S';
            }
        }
    }

    void ZmianaSkladuOsob(vector<Osoba>& osobyNaTymPietrze) {
        // Wysiadanie
        for (int i = 0; i < osobyWinda.size();) {
            if (osobyWinda[i].DokadJedzie() == AktualnePietro) {
                WagaWindy -= osobyWinda[i].WagaOsoby();
                osobyWinda.erase(osobyWinda.begin() + i);
            }
            else {
                i++;
            }
        }

        // Wsiadanie
        bool ktosWsiadl = false;

        for (int i = 0; i < osobyNaTymPietrze.size();) {
            bool kierunekPasuje = (KierunekJazdy == 'S') || (osobyNaTymPietrze[i].PodajKierunek() == KierunekJazdy);

            if (osobyNaTymPietrze[i].SkadJedzie() == AktualnePietro &&
                kierunekPasuje &&
                WagaWindy + osobyNaTymPietrze[i].WagaOsoby() <= 600) {

                if (KierunekJazdy == 'S') {
                    KierunekJazdy = osobyNaTymPietrze[i].PodajKierunek();
                }

                osobyNaTymPietrze[i].Wsiadz();
                WagaWindy += osobyNaTymPietrze[i].WagaOsoby();
                osobyWinda.push_back(osobyNaTymPietrze[i]);
                GdzieJechac(osobyNaTymPietrze[i].DokadJedzie(), osobyNaTymPietrze[i].PodajKierunek());
                osobyNaTymPietrze.erase(osobyNaTymPietrze.begin() + i);
                ktosWsiadl = true;

                // winda czeka 3 sekundy zanim ruszy
                CzyPostoj = true;
                CzasPostoju = time(NULL);
            }
            else {
                i++;
            }
        }

        //  Jeśli wsiadli, usuń pietro
        if (ktosWsiadl) {
            if (KierunekJazdy == 'G') {
                KolejkaCzekajacychGora.erase(
                    remove(KolejkaCzekajacychGora.begin(), KolejkaCzekajacychGora.end(), AktualnePietro),
                    KolejkaCzekajacychGora.end());
            }
            else if (KierunekJazdy == 'D') {
                KolejkaCzekajacychDol.erase(
                    remove(KolejkaCzekajacychDol.begin(), KolejkaCzekajacychDol.end(), AktualnePietro),
                    KolejkaCzekajacychDol.end());
            }
        }

        // Jeśli nie wszyscy mogli wsiąść dodaj pietro spowrotem
        for (const auto& osoba : osobyNaTymPietrze) {
            if (osoba.SkadJedzie() == AktualnePietro && osoba.PodajKierunek() == KierunekJazdy) {
                if (KierunekJazdy == 'G') {
                    if (find(KolejkaCzekajacychGora.begin(), KolejkaCzekajacychGora.end(), AktualnePietro) == KolejkaCzekajacychGora.end()) {
                        KolejkaCzekajacychGora.push_back(AktualnePietro);
                    }
                }
                else if (KierunekJazdy == 'D') {
                    if (find(KolejkaCzekajacychDol.begin(), KolejkaCzekajacychDol.end(), AktualnePietro) == KolejkaCzekajacychDol.end()) {
                        KolejkaCzekajacychDol.push_back(AktualnePietro);
                    }
                }
                break;
            }
        }
    }
};


Winda winda;

int WINAPI WinMain(HINSTANCE hInstancja, HINSTANCE, LPSTR, int nCmdShow) {
    globalne = hInstancja;
    const wchar_t NAZWA_KLASY[] = L"KlasaOknaWindy";

    WNDCLASS wc = {};
    wc.lpfnWndProc = ProceduraOkna;
    wc.hInstance = hInstancja;
    wc.lpszClassName = NAZWA_KLASY;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, NAZWA_KLASY, L"Symulator Windy", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1000,
        NULL, NULL, hInstancja, NULL);

    SetTimer(hwnd, 1, 10, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    // Tworzenie guzików
    for (int pietro = 4; pietro >= 0; pietro--) {
        int pozycjaX;
        RECT obszarKlienta;
        GetClientRect(hwnd, &obszarKlienta);

        pozycjaX = (pietro % 2 == 1) ? obszarKlienta.right - 40 : 20;

        int pozycjaGornaLiniaPietra = 15 + (5 - pietro) * (obszarKlienta.bottom - 30) / 6 - 120;
        int pozycjaGuzikaY = pozycjaGornaLiniaPietra - 40;

        for (int cel = 0; cel < 5; cel++) {
            if (pietro == cel) continue;

            int guzikaY = pozycjaGuzikaY + cel * 30;
            guziki[pietro][cel] = CreateWindow(
                L"BUTTON", to_wstring(cel).c_str(),
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                pozycjaX, guzikaY, 25, 25,
                hwnd, (HMENU)(pietro * 10 + cel),
                globalne, NULL);
        }
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK ProceduraOkna(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static ULONG_PTR gdiToken;
    static bool gdiodpalone = false;

    switch (uMsg)
    {
    case WM_CREATE:
    {

        // GDI+
        if (!gdiodpalone)
        {
            GdiplusStartupInput gdiplusStartupInput;
            GdiplusStartup(&gdiToken, &gdiplusStartupInput, NULL);
            gdiodpalone = true;
        }
        return 0;
    }

    case WM_TIMER:
    {
        //Zadania windy
        //Animacja windy
        RECT obszarKlienta;
        GetClientRect(hwnd, &obszarKlienta);

        const int liczbaPieter = 5;
        const int MARGINES = 5;

        int szerokoscSzybu = 300;
        int wysokoscSzybu = obszarKlienta.bottom - 30;
        int gornaKrawedzSzybu = 15;
        int wysokoscPietra = wysokoscSzybu / (liczbaPieter + 1);
        //-----------
        winda.ZmianaSkladuOsob(osobyNaPietrze[winda.AktualnePietro]);
        winda.RuchWindy(gornaKrawedzSzybu, wysokoscPietra, MARGINES);;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_COMMAND:
    {
        // co robi kiedy klikniety guzik

        int id = LOWORD(wParam);
        int pietro = id / 10;
        int cel = id % 10;

        Osoba nowaOsoba(pietro, cel);
        osobyNaPietrze[pietro].push_back(nowaOsoba);

        winda.GdzieJechac(pietro, nowaOsoba.PodajKierunek());

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdcWin = BeginPaint(hwnd, &ps);

        RECT obszarKlientaRect;
        GetClientRect(hwnd, &obszarKlientaRect);
        int width = obszarKlientaRect.right - obszarKlientaRect.left;
        int height = obszarKlientaRect.bottom - obszarKlientaRect.top;

        Graphics graphicsHdc(hdcWin);
        Bitmap buforBitmap(width, height, &graphicsHdc);
        Graphics graphics(&buforBitmap);

        RECT obszarKlienta;
        GetClientRect(hwnd, &obszarKlienta);

        const int liczbaPieter = 5;
        const int margines = 5;

        int szerokoscSzybu = 300;
        int wysokoscSzybu = obszarKlienta.bottom - 30;
        int lewaKrawedzSzybu = (obszarKlienta.right - szerokoscSzybu) / 2;
        int gornaKrawedzSzybu = 15;

        int wysokoscPietra = wysokoscSzybu / (liczbaPieter + 1);

        int pozycjaWindyY = (int)winda.PozycjaY_Windy;

        if (winda.PozycjaY_Windy == 0.0f && winda.CelY_Windy == 0.0f) {
            winda.PozycjaY_Windy = PozycjeWindy(winda.AktualnePietro, gornaKrawedzSzybu, wysokoscPietra, margines);
            winda.CelY_Windy = winda.PozycjaY_Windy;
        }


        SolidBrush tloBrush(Color(255, 255, 255, 255));
        graphics.FillRectangle(&tloBrush,
            static_cast<int>(obszarKlienta.left),
            static_cast<int>(obszarKlienta.top),
            static_cast<int>(obszarKlienta.right - obszarKlienta.left),
            static_cast<int>(obszarKlienta.bottom - obszarKlienta.top));

        // Rysowanie pięter
        Pen blackPen(Color(255, 0, 0, 0), 1);


        for (int i = 1; i <= liczbaPieter; i++) {
            int pozycjaY = gornaKrawedzSzybu + i * wysokoscPietra;
            if (i % 2 == 1) {
                graphics.DrawLine(&blackPen, 0, pozycjaY, lewaKrawedzSzybu, pozycjaY);
            }
            else {
                graphics.DrawLine(&blackPen, lewaKrawedzSzybu + szerokoscSzybu, pozycjaY, obszarKlienta.right, pozycjaY);
            }
        }

        // Rysowanie osób stojących na piętrach
        SolidBrush redBrush(Color(255, 255, 0, 0));
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, 12, FontStyleBold, UnitPixel);
        StringFormat sf;
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);

        for (int pietro = 0; pietro < liczbaPieter; pietro++) {
            int wysokoscOsoby = 40;
            int pozycjaPodlogi = gornaKrawedzSzybu + (liczbaPieter - pietro) * wysokoscPietra;
            int pozycjaY = pozycjaPodlogi - wysokoscOsoby;

            if (pietro % 2 == 0) {
                int szerokoscOsoby = 15;
                int odstep = 20;
                int X = lewaKrawedzSzybu - 10;

                for (int i = 0; i < (int)osobyNaPietrze[pietro].size(); i++) {
                    int right = X - i * odstep;
                    int left = right - szerokoscOsoby;

                    Rect rect(left, pozycjaY, szerokoscOsoby, wysokoscOsoby);
                    graphics.FillRectangle(&redBrush, rect);

                    RectF rectF((REAL)rect.X, (REAL)rect.Y, (REAL)rect.Width, (REAL)rect.Height);
                    wstring tekst = to_wstring(osobyNaPietrze[pietro][i].DokadJedzie());
                    SolidBrush bialyText(Color(255, 255, 255, 255));
                    graphics.DrawString(tekst.c_str(), -1, &font, rectF, &sf, &bialyText);
                }
            }
            else {
                int szerokoscOsoby = 15;
                int odstep = 20;
                int X = lewaKrawedzSzybu + szerokoscSzybu + 10;

                for (int i = 0; i < (int)osobyNaPietrze[pietro].size(); i++) {
                    int left = X + i * odstep;
                    Rect rect(left, pozycjaY, szerokoscOsoby, wysokoscOsoby);
                    graphics.FillRectangle(&redBrush, rect);

                    RectF rectF((REAL)rect.X, (REAL)rect.Y, (REAL)rect.Width, (REAL)rect.Height);
                    wstring tekst = to_wstring(osobyNaPietrze[pietro][i].DokadJedzie());
                    SolidBrush bialyText(Color(255, 255, 255, 255));
                    graphics.DrawString(tekst.c_str(), -1, &font, rectF, &sf, &bialyText);
                }
            }
        }

        // Rysowanie szybu windy
        Pen szybPen(Color(255, 0, 0, 0), 1);
        Rect szybRect(lewaKrawedzSzybu, gornaKrawedzSzybu, szerokoscSzybu, liczbaPieter * wysokoscPietra + 10);
        graphics.DrawRectangle(&szybPen, szybRect);

        int wysokoscWindy = (wysokoscSzybu - 2 * margines) / (liczbaPieter + 1);
        int szerokoscWindy = szerokoscSzybu - 2 * margines;
        int lewaKrawedzWindy = lewaKrawedzSzybu + margines;

        Pen windaPen(Color(255, 0, 255, 0), 2);
        Rect windaRect(lewaKrawedzWindy, pozycjaWindyY, szerokoscWindy, wysokoscWindy);
        graphics.DrawRectangle(&windaPen, windaRect);

        SolidBrush redBrush2(Color(255, 255, 0, 0));
        int szerokoscOsoby = 15;
        int wysokoscOsoby = 40;
        int odstep = 20;
        int poczatekX = lewaKrawedzWindy + 10;
        int poczatekY = pozycjaWindyY + wysokoscWindy - wysokoscOsoby - 5;

        for (int i = 0; i < (int)winda.osobyWinda.size(); i++) {
            int left = poczatekX + i * odstep;
            Rect osobaRect(left, poczatekY, szerokoscOsoby, wysokoscOsoby);

            graphics.FillRectangle(&redBrush2, osobaRect);

            wstring tekst = to_wstring(winda.osobyWinda[i].DokadJedzie());
            SolidBrush whiteText(Color(255, 255, 255, 255));
            RectF rectF((REAL)osobaRect.X, (REAL)osobaRect.Y, (REAL)osobaRect.Width, (REAL)osobaRect.Height);

            graphics.DrawString(tekst.c_str(), -1, &font, rectF, &sf, &whiteText);
        }

        // Rysowanie wagi windy

        int szerokoscProstokata = 120;
        int wysokoscProstokata = 40;
        int x = obszarKlienta.right - szerokoscProstokata - 20;
        int y = 20;
        Rect wagaRect(x, y, szerokoscProstokata, wysokoscProstokata);

        Pen ramaPen(Color(255, 0, 0, 0), 2);
        graphics.DrawRectangle(&ramaPen, wagaRect);

        SolidBrush blackBrush(Color(255, 0, 0, 0));
        Font wagaFont(&fontFamily, 15, FontStyleBold, UnitPixel);

        wstring wagaText = L"Waga(kg): " + to_wstring(winda.WagaWindy);
        graphics.DrawString(wagaText.c_str(), -1, &wagaFont, PointF((float)(x + 10), (float)(y + 10)), &blackBrush);

        graphicsHdc.DrawImage(&buforBitmap, 0, 0);



        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
    {
        if (gdiodpalone)
        {
            GdiplusShutdown(gdiToken);
            gdiodpalone = false;
        }
        PostQuitMessage(0);
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
};
