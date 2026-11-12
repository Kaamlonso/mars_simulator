/*
 * MARS KOLONISI YASAM DESTEK SIMULATORU
 * Metin tabanli, tek karakterli komutlarla kontrol edilen hayatta kalma oyunu.
 * Derleme: gcc mars_simulator.c -o mars_simulator
 *
 * Not: Konsol kodlama sorunlari yasanmamasi icin ekran ciktilarinda
 * Turkce karakter kullanilmamistir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/* ---------------- DURUM DEGISKENLERI ---------------- */
int saglik, enerji, oksijen, su, parca, hasar;
int kilitMuhurlu = 0;
int gun = 1, tur = 0;
int firtinaSure;     /* firtinanin tur sayisi (zorluga gore) */
int firtinaTaban;    /* firtina taban hasari (zorluga gore)  */

/* ---------------- YARDIMCI FONKSIYONLAR ---------------- */

/* Tum degerleri 0-100 araliginda tutar (her komuttan sonra cagrilir) */
void sinirla(void)
{
    if (saglik < 0)   saglik = 0;
    if (saglik > 100) saglik = 100;
    if (enerji < 0)   enerji = 0;
    if (enerji > 100) enerji = 100;
    if (oksijen < 0)   oksijen = 0;
    if (oksijen > 100) oksijen = 100;
    if (hasar < 0)   hasar = 0;
    if (hasar > 100) hasar = 100;
    if (su < 0)    su = 0;
    if (parca < 0) parca = 0;
}

/* Zar ve esik degerini ekrana yazar */
void zarYaz(int zar, int esik, const char *olumlu, const char *olumsuz)
{
    printf("  Zar: %d / Esik: %d -> %s\n", zar, esik, (zar < esik) ? olumlu : olumsuz);
}

void durumSatiri(void)
{
    printf("[Gun %d | Tur %d] Saglik:%d Enerji:%d Oksijen:%d Su:%d Parca:%d Hasar:%d Kilit:%s\n",
           gun, tur, saglik, enerji, oksijen, su, parca, hasar,
           kilitMuhurlu ? "MUHURLU" : "ACIK");
}

/* ---------------- KOMUTLAR ---------------- */
/* Her komut fonksiyonu: tur harcadiysa 1, iptal/harcamadiysa 0 dondurur */

/* G: Kesif gorevi */
int kesif(void)
{
    int zar, esikSu, yaralanma, kayip, bulunan;

    if (enerji < 15 || oksijen <= 10) {
        printf("  Yetersiz kaynak! Kesif icin en az 15 enerji ve 10'dan fazla oksijen gerekir. Komut iptal.\n");
        return 0;
    }
    enerji -= 15;
    oksijen -= 10;
    printf("  Kesfe ciktin: enerji -15, oksijen -10.\n");

    /* Yaralanma kontrolu: (zar < 10 || modulHasari > 70) */
    zar = rand() % 100;
    yaralanma = (zar < 10 || hasar > 70);
    zarYaz(zar, 10, "YARALANMA", "GUVENLI");
    if (yaralanma) {
        kayip = 10;
        if (hasar > 70)
            kayip += 5;   /* agir hasarli modul riski artirir */
        saglik -= kayip;
        printf("  Yaralandin! Saglik -%d%s\n", kayip, (hasar > 70) ? " (modul hasari yuksek)" : "");
    }

    /* Su bulma esigi enerji ve oksijen durumuna baglidir */
    if (enerji > 40 && oksijen > 30)
        esikSu = 45;
    else
        esikSu = 25;

    zar = rand() % 100;
    printf("  Zar: %d / Esik(su): %d, Esik(parca): %d\n", zar, esikSu, esikSu + 25);
    if (zar < esikSu) {
        bulunan = 4 + rand() % 5;   /* 4-8 birim su */
        su += bulunan;
        printf("  Sonuc: SU BULUNDU! +%d su\n", bulunan);
    } else if (zar < esikSu + 25) {
        bulunan = 1 + rand() % 2;   /* 1-2 yedek parca */
        parca += bulunan;
        printf("  Sonuc: YEDEK PARCA BULUNDU! +%d parca\n", bulunan);
    } else {
        printf("  Sonuc: ELI BOS DONDUN.\n");
    }
    return 1;
}

/* O: Oksijen uretimi (2 su -> 3 oksijen) */
int oksijenUret(void)
{
    int miktar, kullanilan, artik, uretilen;

    if (enerji < 5) {
        printf("  Yetersiz enerji! Oksijen uretimi icin en az 5 enerji gerekir. Komut iptal.\n");
        return 0;
    }
    if (su < 2) {
        printf("  Yeterli su yok (en az 2 birim gerekir). Komut iptal.\n");
        return 0;
    }
    printf("  Mevcut su: %d. Kac birim su donusturulsun? ", su);
    if (scanf("%d", &miktar) != 1) {
        while (getchar() != '\n') ;   /* gecersiz girisi temizle */
        printf("  Gecersiz giris. Komut iptal.\n");
        return 0;
    }
    if (miktar < 1 || miktar > su) {
        printf("  Gecersiz miktar (1-%d arasi olmali). Komut iptal.\n", su);
        return 0;
    }

    artik = miktar % 2;                 /* tekse 1 birim artik kalir */
    kullanilan = miktar - artik;        /* donusturulen cift miktar  */
    if (kullanilan == 0) {
        printf("  1 birim su donusturulemez (2 su = 3 oksijen). Komut iptal.\n");
        return 0;
    }
    uretilen = (kullanilan / 2) * 3;

    if (oksijen + uretilen > 100) {
        uretilen = 100 - oksijen;       /* oksijen 100'u asmaz */
        printf("  Depo dolu, uretim sinirlandi.\n");
    }
    su -= kullanilan;
    oksijen += uretilen;
    enerji -= 5;
    printf("  %d su harcandi, +%d oksijen uretildi, enerji -5.", kullanilan, uretilen);
    if (artik == 1)
        printf(" (1 birim su artik olarak depoda kaldi)");
    printf("\n");
    return 1;
}

/* T: Tamir */
int tamir(void)
{
    int gereken, onarim, kullanilan;
    const char *seviye;

    if (parca <= 0) {
        printf("  Yedek parca yok, tamir yapilamaz. Komut iptal.\n");
        return 0;
    }
    if (enerji < 10) {
        printf("  Yetersiz enerji! Tamir icin en az 10 enerji gerekir. Komut iptal.\n");
        return 0;
    }
    if (hasar == 0) {
        printf("  Modulde hasar yok, tamire gerek yok. Komut iptal.\n");
        return 0;
    }

    /* Hasar seviyesine gore kademeli tamir */
    if (hasar <= 30) {
        seviye = "HAFIF";  gereken = 1;  onarim = 15;
    } else if (hasar <= 70) {
        seviye = "ORTA";   gereken = 2;  onarim = 25;
    } else {
        seviye = "AGIR";   gereken = 3;  onarim = 40;
    }

    kullanilan = (parca >= gereken) ? gereken : parca;
    onarim = onarim * kullanilan / gereken;   /* parca eksikse kismi onarim */

    parca -= kullanilan;
    hasar -= onarim;
    enerji -= 10;
    printf("  Hasar seviyesi: %s. %d parca harcandi, hasar -%d, enerji -10.\n",
           seviye, kullanilan, onarim);
    if (kullanilan < gereken)
        printf("  (Parca yetersiz oldugu icin kismi onarim yapildi.)\n");
    return 1;
}

/* D: Durum raporu (tur harcamaz) */
void durumRaporu(void)
{
    printf("  ===== DURUM RAPORU =====\n");
    printf("  Saglik: %d | Enerji: %d | Oksijen: %d\n", saglik, enerji, oksijen);
    printf("  Su: %d | Yedek parca: %d | Modul hasari: %d\n", su, parca, hasar);
    printf("  Hava kilidi: %s | Gun: %d | Tur: %d\n",
           kilitMuhurlu ? "MUHURLU" : "ACIK", gun, tur);

    if (oksijen < 25) printf("  UYARI: Oksijen kritik seviyede!\n");
    if (saglik < 30)  printf("  UYARI: Saglik kritik seviyede!\n");
    if (hasar > 70)   printf("  UYARI: Modul hasari kritik seviyede!\n");
    if (enerji < 20)  printf("  UYARI: Enerji kritik seviyede!\n");

    if (oksijen >= 25 && saglik >= 30 && hasar <= 70 && enerji >= 20)
        printf("  TUM SISTEMLER NORMAL\n");
}

/* U: Uyu / dinlen */
int dinlen(void)
{
    int enerjiArtis = 30, saglikArtis = 10;

    enerji += enerjiArtis;
    oksijen -= 3;   /* oksijen tuketimi devam eder */
    printf("  Dinlendin: enerji +%d, oksijen -3.\n", enerjiArtis);

    if (oksijen < 20) {
        /* ic ice IF: oksijen azsa saglik artmaz, azalir */
        if (oksijen < 10) {
            saglik -= 10;
            printf("  Oksijen cok dusuk! Nefes alamadin, saglik -10.\n");
        } else {
            saglik -= 5;
            printf("  Oksijen 20'nin altinda, uykuda saglik -5.\n");
        }
    } else {
        saglik += saglikArtis;
        printf("  Saglik +%d.\n", saglikArtis);
    }
    return 1;
}

/* K: Kum firtinasi (FOR dongusu) */
int kumFirtinasi(void)
{
    int i, zar, esikEkstra, carpan, kayip;
    int hasarliTur = 0, toplamSaglik = 0, toplamOksijen = 0;

    if (enerji < 10) {
        printf("  Yetersiz enerji! Firtina hazirligi icin en az 10 enerji gerekir. Komut iptal.\n");
        return 0;
    }
    enerji -= 10;
    printf("  KUM FIRTINASI BASLADI! Sure: %d tur, enerji -10.\n", firtinaSure);

    /* Modul agir hasarliysa olumsuz olay esikleri 15 puan yukselir */
    esikEkstra = (hasar > 70) ? 15 : 0;

    for (i = 1; i <= firtinaSure; i++) {
        zar = rand() % 100;
        carpan = (i % 2 == 0) ? 2 : 1;   /* cift turlarda ruzgar siddetlenir */
        kayip = firtinaTaban * carpan;

        printf("  Tur %d/%d%s - Zar: %d (esikler: %d/%d/%d)\n", i, firtinaSure,
               (carpan == 2) ? " [RUZGAR SIDDETLENDI x2]" : "",
               zar, 25 + esikEkstra, 50 + esikEkstra, 70 + esikEkstra);

        if (zar < 25 + esikEkstra) {
            saglik -= kayip;
            toplamSaglik += kayip;
            hasarliTur++;
            printf("    Savruldun! Saglik -%d\n", kayip);
        } else if (zar < 50 + esikEkstra) {
            if (kilitMuhurlu == 1) {
                printf("    Oksijen sizintisi olacakti ama hava kilidi muhurlu, atlandi.\n");
                continue;   /* bu tur icin sizinti olayi atlanir */
            }
            oksijen -= kayip;
            toplamOksijen += kayip;
            hasarliTur++;
            printf("    Oksijen sizintisi! Oksijen -%d\n", kayip);
        } else if (zar < 70 + esikEkstra) {
            hasar += kayip;
            hasarliTur++;
            printf("    Modul hasar aldi! Hasar +%d\n", kayip);
        } else {
            printf("    Firtinadan zararsiz korundun.\n");
        }

        sinirla();
        if (saglik <= 0 || oksijen <= 0) {
            printf("    KRITIK: Saglik veya oksijen tukendi, firtina erken sonlandi!\n");
            break;
        }
    }

    printf("  --- FIRTINA OZETI ---\n");
    printf("  Hasar alinan tur: %d | Kaybedilen saglik: %d | Kaybedilen oksijen: %d\n",
           hasarliTur, toplamSaglik, toplamOksijen);
    return 1;
}

/* H: Hava kilidi sifresi (DO-WHILE) */
int havaKilidi(void)
{
    char sifre, tahmin;
    int deneme = 0;

    if (kilitMuhurlu == 1) {
        printf("  Hava kilidi zaten muhurlu. Komut iptal.\n");
        return 0;
    }
    if (enerji < 5) {
        printf("  Yetersiz enerji! Sifre denemesi icin en az 5 enerji gerekir. Komut iptal.\n");
        return 0;
    }

    sifre = 'A' + (gun % 26);   /* sifre gun sayisindan turetilir */
    printf("  Ipucu: Sifre, alfabede A harfinden bugunun gun sayisi (%d) kadar ilerideki harftir.\n", gun);
    printf("  3 deneme hakkin var.\n");

    do {
        deneme++;
        printf("  Deneme %d/3 - Sifre harfi: ", deneme);
        if (scanf(" %c", &tahmin) != 1)
            break;
        tahmin = toupper((unsigned char)tahmin);

        if (tahmin != sifre) {
            enerji -= 5;
            if (enerji < 0) enerji = 0;
            if (tahmin < sifre)
                printf("    Yanlis! Ipucu: ileri git. (enerji -5)\n");
            else
                printf("    Yanlis! Ipucu: geri git. (enerji -5)\n");
        }
    } while (tahmin != sifre && deneme < 3);   /* tek mantiksal ifade */

    if (tahmin == sifre) {
        kilitMuhurlu = 1;
        printf("  DOGRU SIFRE! Hava kilidi muhurlendi.\n");
    } else {
        hasar += 20;
        printf("  3 deneme de basarisiz! Kilit zorlandi, modul hasari +20.\n");
    }
    return 1;
}

/* ---------------- SONUC RAPORU ---------------- */
void sonucRaporu(const char *neden)
{
    int puan = (gun * 100) + saglik + oksijen - hasar;
    printf("\n=========== SONUC RAPORU ===========\n");
    printf("Oyun sonu nedeni : %s\n", neden);
    printf("Hayatta kalinan gun: %d\n", gun);
    printf("Toplam tur         : %d\n", tur);
    printf("Saglik: %d | Enerji: %d | Oksijen: %d\n", saglik, enerji, oksijen);
    printf("Su: %d | Yedek parca: %d | Modul hasari: %d\n", su, parca, hasar);
    printf("PUAN = (gun x 100) + saglik + oksijen - hasar = %d\n", puan);
    printf("====================================\n");
}

/* ---------------- ANA PROGRAM ---------------- */
int main(void)
{
    char zorlukSecim, komut;
    int gecerli, turHarcandi, cikis = 0, bitti = 0, kazandi = 0;

    srand((unsigned int)time(NULL));

    printf("=== MARS KOLONISI YASAM DESTEK SIMULATORU ===\n");
    printf("Kurtarma ekibi gelene kadar 15 gun hayatta kal!\n\n");

    /* Zorluk secimi (SWITCH-CASE), gecersizse tekrar istenir */
    saglik = 100; enerji = 100; oksijen = 100; hasar = 0; parca = 3; su = 20;
    do {
        gecerli = 1;
        printf("Zorluk seviyesi secin (K: Kolay, N: Normal, Z: Zor): ");
        if (scanf(" %c", &zorlukSecim) != 1)
            return 0;
        switch (zorlukSecim) {
            case 'k': case 'K':
                su = 30; parca = 5; oksijen = 100;
                firtinaSure = 3; firtinaTaban = 5;
                printf("Kolay seviye secildi.\n");
                break;
            case 'n': case 'N':
                su = 20; parca = 3; oksijen = 100;
                firtinaSure = 5; firtinaTaban = 8;
                printf("Normal seviye secildi.\n");
                break;
            case 'z': case 'Z':
                su = 10; parca = 2; oksijen = 90;
                firtinaSure = 7; firtinaTaban = 12;
                printf("Zor seviye secildi.\n");
                break;
            default:
                printf("Gecersiz seviye! Lutfen tekrar deneyin.\n");
                gecerli = 0;
        }
    } while (gecerli == 0);

    printf("\nKomutlar: G:Kesif O:Oksijen uret T:Tamir D:Durum U:Uyu K:Kum firtinasi H:Hava kilidi Q:Cikis\n\n");

    /* Komut dinleme dongusu (DO-WHILE) */
    do {
        turHarcandi = 0;
        durumSatiri();
        printf("Komut> ");
        if (scanf(" %c", &komut) != 1)
            break;

        switch (komut) {
            case 'g': case 'G':
                turHarcandi = kesif();
                break;
            case 'o': case 'O':
                turHarcandi = oksijenUret();
                break;
            case 't': case 'T':
                turHarcandi = tamir();
                break;
            case 'd': case 'D':
                durumRaporu();          /* tur harcamaz */
                break;
            case 'u': case 'U':
                turHarcandi = dinlen();
                break;
            case 'k': case 'K':
                turHarcandi = kumFirtinasi();
                break;
            case 'h': case 'H':
                turHarcandi = havaKilidi();
                break;
            case 'q': case 'Q':
                cikis = 1;
                break;
            default:
                printf("  Tanimsiz komut '%c'! Tur harcanmadi.\n", komut);
        }

        sinirla();

        /* Zaman akisi: gecerli her komut turu 1 artirir */
        if (turHarcandi == 1) {
            tur++;
            if (tur % 3 == 0) {
                gun++;
                oksijen -= 8;
                printf("  --- YENI GUN BASLADI: Gun %d (oksijen -8) ---\n", gun);
                if (hasar > 70) {
                    saglik -= 5;
                    printf("  Modul hasari 70'in uzerinde, saglik -5.\n");
                }
                sinirla();
            }

            /* Oyun sonu kontrolleri (tur sonunda) */
            if (saglik <= 0 || oksijen <= 0) {
                bitti = 1;
            } else if (gun > 15) {
                gun = 15;
                bitti = 1;
                kazandi = 1;
            }
        }
    } while (cikis == 0 && bitti == 0);

    if (kazandi)
        printf("\nKURTARMA EKIBI GELDI! 15 gunu hayatta tamamladin. KAZANDIN!\n");
    else if (bitti)
        printf("\nHayatta kalamadin... OYUN KAYBEDILDI.\n");
    else
        printf("\nSimulasyondan cikildi.\n");

    sonucRaporu(kazandi ? "Kazanildi (kurtarildi)" : (bitti ? "Kaybedildi" : "Oyuncu cikti (Q)"));
    return 0;
}