// Milan Stajcic NRT-150/19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define MIN 10
#define MAX 20
#define MAXI 80


// zaglavlje loga
struct log_header{
    char start_of_log[MAXI];
    char location[MAXI];
    char contest[MAXI];
    char callsign[MAXI];
    char my_call[MAX];
    char mode[MAX];
    char ctg_op[MAXI];
    char ctg_tr[MAXI];
    char ctg_ass[MAXI];
    char ctg_pow[MAXI];
    char ctg_st[MAXI];
    char ctg_band[MAXI];
    char ctg_mode[MAXI];
    char clm_score[MAXI];
    char club[MAXI];
    char name[MAXI];
    char score[MAXI];
};

// Lista struktura
// podaci za svaku odrzanu vezu koji se na kraju upisuju u fajl
struct qso{
    char qso[MIN+1];
    char mhz[MIN+1];
    char mode[MIN+1];
    char date[MIN+1];
    char utc[MIN+1];
    char my_call[MAX+1];
    char rprt_s[MIN+1];
    char exch_s[MIN+1];
    char call[MAX+1];
    char rprt_r[MIN+1];
    char exch_r[MIN+1];
    struct qso *next_qso;
};


// upisuje u log ostatak podataka i time kompletira jedan ciklus
// poziva se iz glavne petlje u glavnoj funkciji
void upisi_qso(struct qso *veza, struct log_header *l_h, char *call){
    char datum[MAX];
    char utc[MIN];
    char pom[MIN];
    
    // u ovoj funkciji se koriste objekti iz biblioteke time.h, jer je potrebna precizna evidencija vremena za odrzanu vezu (+- 5 min)
    // po pravilima svih takmicelja ovog tipa, mora se raditi po UTC-u (ili GMT-u) a ne po lokalnom vremenu
    // ova biblioteka sadrzi i tu funkcionalnost
	struct tm *myTime, *gmt;
	time_t vreme;
	time(&vreme);
	myTime = localtime(&vreme);
	gmt = gmtime(&vreme);
	
	// sprintf() funkcija mi je ovde omogucila da datum i vreme formatiram u tacno odredjen oblik kako se to trazi po Cabrillo standardu
	sprintf(datum, "%d-%02d-%02d", myTime->tm_year+1900, myTime->tm_mon+1, myTime->tm_mday);
	sprintf(utc, "%02d%02d", gmt->tm_hour, gmt->tm_min);
	
	strcpy(veza->qso, "QSO: ");
    strcpy(veza->mode, l_h->mode);
    strcpy(veza->date, datum);
    strcpy(veza->utc, utc);
    strcpy(veza->my_call, l_h->my_call);
    
}


// postavlja sva slova u velika
void uvecaj(char *niz){
    int i, l;
    char temp;
    l = strlen(niz);
    for(i=0; i<l; i++){
        niz[i] = toupper(niz[i]);
    }
}


// Proverava da li trenutni znak vec postoji u logu
// Duple veze nisu dozvoljene i ako se i unese dupla veza u log takmicarska komisija za to dodeljuje negativne poene
int proveri(struct qso *baza, char *call, char *mhz, int i){
    int j, prov;
    for(j=0; j<i; j++){
        if(strcmp(baza->call, call)==0 && strcmp(baza->mhz, mhz)==0){
            prov = 0;
            break;
        }
        else{
            baza = baza->next_qso;
            prov = 1;
        }
    }
    return prov;
}


// Ova funkcija dodeluje ostvarene poene za svaku odrzanu vezu
// Uslov je nesto drugaciji od stvarnog pravilnika radi jednostavnosti programa
// Svaka odrzana veza sa domacom stanicom nosi 1 poen, dok svaka odrzana veza sa stanicom iz inostranstva nosi 2 poena
// Sve domace stanice imaju prefikf YU ili YT
void poentiraj(unsigned long *score, char *call){
    if(strncmp(call, "YU", 2)==0 || strncmp(call, "YT", 2)==0)
        *score += 1;
    else
        *score += 2;
}


// inicijalizuje, samo na pocetku programa,
// zaglavlje dnevnika svim neophodnim podacima
void init_log_header(struct log_header *zaglavlje, char *kHz){
    char pom[MAXI];
    char mode[MAX];
    int izbor;
   
    strcpy(zaglavlje->start_of_log, "START-OF-LOG: 1.0");
    
    strcpy(zaglavlje->callsign, "CALLSIGN: ");
    printf("Unesite vas pozivni znak: ");
    scanf("%s", pom); fflush(stdin);
    uvecaj(pom);
    strcat(zaglavlje->callsign, pom);
    strcpy(zaglavlje->my_call, pom);
   
    strcpy(zaglavlje->location, "LOCATION: ");
    printf("Unesite vasu DX lokaciju (po ARRL kategorizaciji): ");
    scanf("%s", pom); fflush(stdin);
    uvecaj(pom);
    strcat(zaglavlje->location, pom);

    do{
        printf("Odaberite takmicenje (1. CQ-WW-CW / 2. CQ-WW-SSB): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor == 1){
            strcpy(zaglavlje->contest, "CONTEST: CQ-WW-CW");
            strcpy(zaglavlje->ctg_mode, "CATEGORY-MODE: CW");
            strcpy(zaglavlje->mode, "CW");
            break;
        }
        else if(izbor == 2){
            strcpy(zaglavlje->contest, "CONTEST: CQ-WW-SSB");
            strcpy(zaglavlje->ctg_mode, "CATEGORY-MODE: SSB");
            strcpy(zaglavlje->mode, "SSB");
            break;
        }
        else
            printf("Uneli ste nepostojeci mod, pokusajte ponovo.\n");
    }while(1);
   
   
    do{   
        printf("Odaberite operatorsku kategoriju (1. SINGLE-OP / 2. MULTI-OP): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor==1){
            strcpy(zaglavlje->ctg_op, "CATEGORY-OPERATOR: SINGLE-OP");
            break;
        }
        else if(izbor==2){
            strcpy(zaglavlje->ctg_op, "CATEGORY-OPERATOR: MULTI-OP");
            break;
        }
        else
            printf("Odabrali ste nepostojecu operatorsku kategoriju, pokusajte ponovo.\n");
    }while(1);
   
   
    do{   
        printf("Odaberite Transceiver kategoriju (1. SINGLE / 2. MULTI-SINGLE / 3. MULTI-TWO / 4. MULTI-MULTI): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor==1){
            strcpy(zaglavlje->ctg_tr, "CATEGORY-TRANSMITTER: SINGLE");
            break;
        }
        else if(izbor==2){
            strcpy(zaglavlje->ctg_tr, "CATEGORY-TRANSMITTER: MULTI-SINGLE");
            break;
        }
        else if(izbor==3){
            strcpy(zaglavlje->ctg_tr, "CATEGORY-TRANSMITTER: MULTI-TWO");
            break;
        }
        else if(izbor==4){
            strcpy(zaglavlje->ctg_tr, "CATEGORY-TRANSMITTER: MULTI-MULTI");
            break;
		}
        else
            printf("Odabrali ste nepostojecu Transceiver kategoriju, pokusajte ponovo\n");
    }while(1);
       
       
    do{   
        printf("Odaberite kategoriju Asistencija (1. ASSISTED / 2. NON-ASSISTED): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor==1){
            strcpy(zaglavlje->ctg_ass, "CATEGORY-ASSISTED: ASSISTED");
            break;
        }
        else if(izbor==2){
            strcpy(zaglavlje->ctg_ass, "CATEGORY-ASSISTED: NON-ASSISTED");
            break;
        }
        else
            printf("Odabrali ste nepostojecu kategoriju Asistencije, pokusajte ponovo.\n");
    }while(1);
   
   
    do{   
        printf("Odaberite kategoriju Snaga predajnika (1. HIGH / 2. LOW / 3. QRP): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor==1){
            strcpy(zaglavlje->ctg_pow, "CATEGORY-POWER: HIGH");
            break;
        }
        else if(izbor==2){
            strcpy(zaglavlje->ctg_pow, "CATEGORY-POWER: LOW");
            break;
        }
        else if(izbor==3){
            strcpy(zaglavlje->ctg_pow, "CATEGORY-POWER: QRP");
            break;
        }
        else
            printf("Odabrali ste nepostojecu kategoriju Snaga predajnika, pokusajte ponovo.\n");
    }while(1);
   
   
    do{   
        printf("Odaberite kategoriju Lokacija stanice (1. FIXED / 2. PORTABLE / 3. MOBILE): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor==1){
            strcpy(zaglavlje->ctg_st, "CATEGORY-STATION: FIXED");
            break;
        }
        else if(izbor==2){
            strcpy(zaglavlje->ctg_st, "CATEGORY-STATION: PORTABLE");
            break;
        }
        else if(izbor==3){
            strcpy(zaglavlje->ctg_st, "CATEGORY-STATION: MOBILE");
            break;
        }
        else
            printf("Odabrali ste nepostojecu kategoriju za Lokaciju stanice, pokusajte ponovo.\n");
    }while(1);
   
   
    do{   
        printf("Odaberite kategoriju Opsega (1. ALL / 2. 80M / 3. 40M / 4. 20M / 5. 15M / 6. 10M): ");
        scanf("%d", &izbor); fflush(stdin);
        if(izbor==1){
            strcpy(zaglavlje->ctg_band, "CATEGORY-BAND: ALL");
            break;
        }
        else if(izbor==2){
            strcpy(zaglavlje->ctg_band, "CATEGORY-BAND: 80M");
            strcpy(kHz, "3500");
            break;
        }
        else if(izbor==3){
            strcpy(zaglavlje->ctg_band, "CATEGORY-BAND: 40M");
            strcpy(kHz, "7000");
            break;
        }
        else if(izbor==4){
            strcpy(zaglavlje->ctg_band, "CATEGORY-BAND: 20M");
            strcpy(kHz, "14000");
            break;
        }
        else if(izbor==5){
            strcpy(zaglavlje->ctg_band, "CATEGORY-BAND: 15M");
            strcpy(kHz, "21000");
            break;
        }
        else if(izbor==6){
            strcpy(zaglavlje->ctg_band, "CATEGORY-BAND: 10M");
            strcpy(kHz, "28000");
            break;
        }
        else
            printf("Odabrali ste nepostojecu kategoriju za Band, pokusajte ponovo.\n");
    }while(1);
   
   
    strcpy(zaglavlje->club, "CLUB: ");
    printf("Unesite ime kluba (ili ENTER za dalje): ");
    gets(pom); fflush(stdin);
    strcat(zaglavlje->club, pom);
   
   
    strcpy(zaglavlje->name, "NAME: ");
    printf("Unesite vase ime (ili ENTER za dalje): ");
    gets(pom); fflush(stdin);
    strcat(zaglavlje->name, pom);
   
    strcpy(zaglavlje->clm_score, "CLAIMED-SCORE: ");
}


// Funkcija koja po zavrsetku takmicenja kreira CABRILLO fajl koji ima .LOG ekstenziju
void napravi_t_fajl(struct log_header *l_h, struct qso *Qso, char *ime_loga, int br, long *score){
	int i=0;
	FILE *t_fajl;
	
	strcpy(ime_loga, l_h->my_call);
    strcat(ime_loga, ".LOG");
	
	if((t_fajl = fopen(ime_loga, "w")) == 0){
    		fprintf(stderr, "Doslo je do greske u kreiranju tekstualnog fajla dnevnika!");
		}
	
	fprintf(t_fajl, "%s\n", l_h->start_of_log);
    fprintf(t_fajl, "%s\n", l_h->location);
    fprintf(t_fajl, "%s\n", l_h->contest);
    fprintf(t_fajl, "%s\n", l_h->callsign);
    fprintf(t_fajl, "%s\n", l_h->ctg_op);
    fprintf(t_fajl, "%s\n", l_h->ctg_tr);
    fprintf(t_fajl, "%s\n", l_h->ctg_ass);
    fprintf(t_fajl, "%s\n", l_h->ctg_pow);
    fprintf(t_fajl, "%s\n", l_h->ctg_st);
    fprintf(t_fajl, "%s\n", l_h->ctg_band);
    fprintf(t_fajl, "%s\n", l_h->ctg_mode);
    fprintf(t_fajl, "%s %ld\n", l_h->clm_score, *score);
    fprintf(t_fajl, "%s\n", l_h->club);
    fprintf(t_fajl, "%s\n", l_h->name);
	
	while(i<br){
		fprintf(t_fajl, "%s", Qso->qso);
		fprintf(t_fajl, "%s\t", Qso->mhz);
		fprintf(t_fajl, "%s ", Qso->mode);
	    fprintf(t_fajl, "%s ", Qso->date);
	    fprintf(t_fajl, "%s ", Qso->utc);
	    fprintf(t_fajl, "%s\t", Qso->my_call);
	    fprintf(t_fajl, "%s\t", Qso->rprt_s);
	    fprintf(t_fajl, "%s\t", Qso->exch_s);
	    fprintf(t_fajl, "%s\t\t", Qso->call);
	    fprintf(t_fajl, "%s\t", Qso->rprt_r);
	    fprintf(t_fajl, "%s\t", Qso->exch_r);
	    fprintf(t_fajl, "%s", "\n");
	    Qso = Qso->next_qso;
	    i++;
	}
	fprintf(t_fajl, "%s", "END-OF-LOG:");

	fclose(t_fajl);
}


// glavna funkcija
int main(){
    int i=0, j, izbor;
    char c;
    unsigned long score=0;
    unsigned long *score_ptr;
    score_ptr = &score;
    char status[MAX];
    char qsob4[] = "!!! QSO B4 !!!";
    char ime_loga[MAX+1];
    char kHz[MIN];
    char *p_kHz;
    char mode[MIN];
    char call[MAX];
    char *call_ptr;
    struct log_header *l_h;
    struct qso *Qso, *Qso_start;
    FILE *file;
    
    call_ptr = call;
    p_kHz = kHz;
    
	printf("***************************************\n");
	printf("*\tDobrodosli u                  *");
	printf("\n*\tCQ WW Contest logger!!!       *\n");
	printf("***************************************\n");
	printf("\nOvaj logger je namenjen iskljucivo za vodjenje dnevnika CQ World Wide\n");
	printf("takmicenja u organizaciji CQ magazina koji se odrzavaju, svake godine,\n");
	printf("poslednjeg punog vikenda oktobra ( SSB - Telefonijski deo ) i\nposlednjeg punog vikenda novembra ( CW - Telegrafski deo ).\n");
	printf("\nZa detaljan pravilnik takmicenja posetite zvanicnu web stranicu\n");
	printf("-->     www.cqww.com/rules.html     <--\n");

	printf("\nPazljivo pratite i postujte uputstva tokom koriscenja ovog programa\nda bi svi podaci bili ispravno uneti u dnevnik i sacuvani\n");
	printf("to jest, adekvatno pripremljeni za dalju obradu od strane Contest robota.");
	printf("\n\nU svakom trenutku mozete promeniti Band ili prekinuti ucesce u takmicenju i kreirati Cabrillo fajl\n");
	printf("tako sto ce te na mesto Call: uneti \"END\" ili \"end\".");
	printf("\n\nSva slova koja unosite u program se automatski konvertuju u velika\n");
	printf("pa o tome ne morate posebno brinuti.");


    if((l_h = malloc(sizeof(struct log_header))) == 0){
        printf("\nGreska pri postavljanju zaglavlja loga!\n");
        exit(1);
    }

    if((Qso_start = malloc(sizeof(struct qso))) == 0){
        printf("Greska pri postavci loga!");
        exit(1);
    }
    Qso = Qso_start;
	
	printf("\n\nDa li zelite da zapocnete novo takmicenje (D / N)? ");
	gets(status); fflush(stdin);
	if(strcmp(status, "D")==0 || strcmp(status, "d")==0){
	    init_log_header(l_h, p_kHz);
    }
	else if(strcmp(status, "N")==0 || strcmp(status, "n")==0){
		printf("\nMozete pregledati postojeci dnevnik.\n");
    	printf("Za pregled postojeceg dnevnika unesite njegovo tacno ime sa ekstenzijom (npr. YU5CW.LOG): ");
		scanf("%s", status); fflush(stdin);
		printf("\n\n");
		
    	if((file=fopen(status, "r")) == 0){
    		printf("\nDoslo je do greske prilikom otvaranje postojeceg dnevnika!\n");
    		exit(1);
		}
		
		while((c=fgetc(file)) != EOF)
			printf("%c", c);
		printf("\n\n");
		fclose(file);
		system("pause");
		exit(1);
	}
	else{
		printf("\n\nIzabrali ste ne postojecu opciju, vise srece drugi put.\n");
		printf("\n\t\t\tGL 73\n");
		exit(1);
	}
	
	
	if(strcmp(l_h->ctg_band, "CATEGORY-BAND: ALL") == 0){
		printf("\nUnesite jednu od ponudjenih frekvencija: 3500, 7000, 14000, 21000, 28000:\n");
	    scanf("%s", kHz); fflush(stdin);
	    printf("\n");
	}
		
	// Glavna petlja
    while(1){
        printf("Call: ");
        scanf("%s", call); fflush(stdin);
        uvecaj(call);
        if(strcmp(call, "END") != 0){
            if(proveri(Qso_start, call, kHz, i)){
                strcpy(Qso->call, call);
                printf("Raport Send: ");
                scanf("%s", Qso->rprt_s); fflush(stdin);
                printf("Exchange Send: ");
                scanf("%s", Qso->exch_s); fflush(stdin);
                printf("Raport Receive: ");
                scanf("%s", Qso->rprt_r); fflush(stdin);
                printf("Exchange Receive: ");
                scanf("%s", Qso->exch_r); fflush(stdin);
                strcpy(Qso->mhz, kHz);
                upisi_qso(Qso, l_h, call);
                Qso->next_qso = malloc(sizeof(struct qso));
                Qso = Qso->next_qso;
                poentiraj(score_ptr, call_ptr);
                printf("\n");
                i++;
            }
            else{
                printf("%s\n", qsob4);
            } 
        }
        else{
        	do{
        		if(strcmp(l_h->ctg_band, "CATEGORY-BAND: ALL") == 0){
		        	printf("\nOdaberite ociju: 1. Promenite opseg / 2. Zavrsite takmicenje\n");
		        	scanf("%d", &izbor); fflush(stdin);
		        	if(izbor == 1){
		        		printf("\nUnesite jednu od ponudjenih frekvencija: 3500, 7000, 14000, 21000, 28000:\n");
		        		scanf("%s", kHz); fflush(stdin);
		        		printf("\n");
		        		break;
					}
		        	else if(izbor == 2){
						napravi_t_fajl(l_h, Qso_start, ime_loga, i, score_ptr);
		    	        printf("\nOsvojili ste ukupno %lu poena\n", score);
						printf("\nKRAJ\n");
						free(l_h);
					    free(Qso);
					    free(Qso_start);
		        	    exit(1);
		        	}
		        	else
		        		printf("\nNepoznata opcija.\nPokusajte ponovo: ");
		        }
		        else{
		        	napravi_t_fajl(l_h, Qso_start, ime_loga, i, score_ptr);
		    	    printf("\nOsvojili ste ukupno %lu poena\n", score);
					printf("\nKRAJ\n");
					free(l_h);
					free(Qso);
					free(Qso_start);
					system("pause");
		        	exit(1);
				}
	        }while(izbor != 1 || izbor != 2);
        }
    }
	
	return 0;
}
