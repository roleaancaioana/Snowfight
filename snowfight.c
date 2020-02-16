//Copyright Rolea Anca Ioana 313CA
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define NMAX 200

typedef struct {
    char nume[NMAX];
    int x;
    int y;
    int hp;
    int stamina;
    int dmg;
} spiridus_t;

void elibereaza_2d(int **a, int n) {
    for (int j = 0; j < n; ++j) {
        free(a[j]);
    }
    free(a);
}

int **aloca_2d(int n, int m) {
    int i;
    int **a = malloc(n * sizeof(int*));
    if (a != NULL) {
        for (i = 0; i < n; ++i) {
            a[i] = malloc(m * sizeof(int));
            if (a[i] == NULL) {
                elibereaza_2d(a, i);
                return NULL;
            }
        }
    } else {
        elibereaza_2d(a, 0);
        return NULL;
    }
    return a;
}

int castigator(int spiridus_plecat[NMAX], int nr_jucatori) {
    int i, c = 0, id;
    for (i = 0; i < nr_jucatori; ++i) {
        if (spiridus_plecat[i] == 0) {
            c++;
            id = i;
        }
    }
    if (c == 1) {
        return id;
    }
    return -1;
}

/*cream o functie care imi intoarce id-ul spiridusului din celula 
in care a intrat spiridusul "bataus"*/
int id_spiridus(spiridus_t *spiridus, int x, int y, 
    int nr_jucatori) {
    int i;
    for (i = 0; i < nr_jucatori; ++i) {
        if (spiridus[i].x == x && spiridus[i].y == y && spiridus[i].hp > 0)  {
            return i;
        }
    }
    //daca id-ul e -1 inseamna ca nu exista un spiridus in celula respectiva 
    //sau spiridusul aflat initial in celula s-a udat leoarca si a plecat
    return -1; 
}

int spiridus_in_ghetar(spiridus_t spiridus, int raza, int x, int y) {
    if (sqrt((spiridus.x - x)*(spiridus.x - x) + 
        (spiridus.y - y)*(spiridus.y - y)) <= raza) {
        return 1;
    }
    return 0;
}

//spiridusul[a] e cel care intra in celula
void atac(FILE *out, spiridus_t *spiridus, int a, int c, int spiridus_plecat[NMAX], int nr_eliminari[NMAX]) {
    if (spiridus[a].stamina > spiridus[c].stamina && spiridus_plecat[a] != 1 && spiridus_plecat[c] != 1) {
        spiridus[c].hp -= spiridus[a].dmg;
        while (spiridus[a].hp > 0 && spiridus[c].hp > 0) {
            spiridus[a].hp -= spiridus[c].dmg;
            if (spiridus[a].hp > 0 && spiridus[c].hp > 0) {
            	spiridus[c].hp -= spiridus[a].dmg;
            }
        }
    } 
    if (spiridus[a].stamina < spiridus[c].stamina && spiridus_plecat[a] != 1 && spiridus_plecat[c] != 1) {
        spiridus[a].hp -= spiridus[c].dmg;
        while (spiridus[a].hp > 0 && spiridus[c].hp > 0) {
            spiridus[c].hp -= spiridus[a].dmg;
            if (spiridus[a].hp > 0 && spiridus[c].hp > 0) {
                spiridus[a].hp -= spiridus[c].dmg;
            }
        }       
    }
    if (spiridus[a].stamina == spiridus[c].stamina && spiridus_plecat[a] != 1 && spiridus_plecat[c] != 1) {
        spiridus[c].hp -= spiridus[a].dmg;
        while (spiridus[a].hp > 0 && spiridus[c].hp > 0) {
            spiridus[a].hp -= spiridus[c].dmg;
            if (spiridus[a].hp > 0 && spiridus[c].hp > 0) {
                spiridus[c].hp -= spiridus[a].dmg;
            }
        }   
    }
    if (spiridus[a].hp <= 0) {
        spiridus[a].hp = 0; //spiridusul s-a udat leoarca
        spiridus[c].stamina += spiridus[a].stamina;
        spiridus[a].stamina = 0;
        nr_eliminari[c]++;
        fprintf(out, "%s sent %s back home.\n", spiridus[c].nume, spiridus[a].nume);
        spiridus_plecat[a] = 1;
    }
    if (spiridus[c].hp <= 0) {
        spiridus[c].hp = 0;
        spiridus[a].stamina += spiridus[c].stamina;
        spiridus[c].stamina = 0;
        nr_eliminari[a]++;
        fprintf(out, "%s sent %s back home.\n", spiridus[a].nume, spiridus[c].nume);
        spiridus_plecat[c] = 1;
    }
}

//cream o functie care imi modifica dmg-ul spiridusului 
//la intrarea intr-o celula
void atribuire_dmg(spiridus_t *spiridus, int **dimensiune_manusi, int a) {
    spiridus[a].dmg = dimensiune_manusi[spiridus[a].x][spiridus[a].y];
    dimensiune_manusi[spiridus[a].x][spiridus[a].y] = 0;
}

void schimbare_dmg(spiridus_t *spiridus, int **dimensiune_manusi, int a) {
    int aux = 0;
    if (spiridus[a].dmg < 
        dimensiune_manusi[spiridus[a].x][spiridus[a].y]) {
    	aux = spiridus[a].dmg;
    	//spiridusul ia manusile din celula in care a ajuns
    	atribuire_dmg(spiridus, dimensiune_manusi, a);
    	//spiridusul lasa manusile sale vechi in celula respectiva
    	dimensiune_manusi[spiridus[a].x][spiridus[a].y] = aux;
    }
}

void spiridus_in_afara_ghetarului(FILE *out, spiridus_t *spiridus, int raza, 
    int nr_jucatori, int spiridus_plecat[NMAX], int **dimensiune_manusi) {
    int i;
    for (i = 0; i < nr_jucatori; ++i) {
        if ((sqrt(pow((spiridus[i].x - raza), 2) + pow((spiridus[i].y -raza), 2)) > raza) && spiridus_plecat[i] != 1) {
            fprintf(out, "%s has missed the glacier.\n", spiridus[i].nume);
            spiridus_plecat[i] = 1;
        } else {
            atribuire_dmg(spiridus, dimensiune_manusi, i);
        }
    }
}

void consum_stamina(int inaltime1, int inaltime2, spiridus_t *spiridus, int a) {
    spiridus[a].stamina -= fabs(inaltime2 - inaltime1);
}

void mutare_D(FILE *out, spiridus_t *spiridus, int **dimensiune_manusi, int **inaltime, 
    int nr_jucatori, int spiridus_plecat[NMAX], int raza, int a, int nr_eliminari[NMAX]) {
    int copie_stamina = spiridus[a].stamina;
    int id;
    /*spiridusul consuma o parte din stamina pentru 
    a se deplasa*/
    consum_stamina(inaltime[spiridus[a].x][spiridus[a].y], 
        inaltime[spiridus[a].x + 1][spiridus[a].y], 
        spiridus, a);
    if (spiridus[a].stamina >= 0) {
        id = id_spiridus(spiridus, spiridus[a].x + 1,
        	spiridus[a].y, nr_jucatori);
        spiridus[a].x++;
        if (spiridus_in_ghetar(spiridus[a], raza, raza, raza) == 0) {
            fprintf(out, "%s fell off the glacier.\n", spiridus[a].nume);
            spiridus_plecat[a] = 1;
        }
        schimbare_dmg(spiridus, dimensiune_manusi, a);
        if (id != -1) {
            atac(out, spiridus, a, id, spiridus_plecat, nr_eliminari);
        }
    } else {
        spiridus[a].stamina = copie_stamina;
    }
}

void mutare_U(FILE *out, spiridus_t *spiridus, int **dimensiune_manusi, int **inaltime, 
    int nr_jucatori, int spiridus_plecat[NMAX], int raza, int a, int nr_eliminari[NMAX]) {
    int copie_stamina = spiridus[a].stamina;
    int id;
    consum_stamina(inaltime[spiridus[a].x][spiridus[a].y], 
        inaltime[spiridus[a].x - 1][spiridus[a].y], 
        spiridus, a);
    if (spiridus[a].stamina >= 0) {
        id = id_spiridus(spiridus, spiridus[a].x - 1,
        	spiridus[a].y, nr_jucatori);
        spiridus[a].x--;
        if (spiridus_in_ghetar(spiridus[a], raza, raza, raza) == 0) {
            fprintf(out, "%s fell off the glacier.\n", spiridus[a].nume);
            spiridus_plecat[a] = 1;
        }
        schimbare_dmg(spiridus, dimensiune_manusi, a);
        if (id != -1) {
            atac(out, spiridus, a, id, spiridus_plecat, nr_eliminari);
        }
    } else {
        spiridus[a].stamina = copie_stamina;
    }
}

void mutare_L(FILE *out, spiridus_t *spiridus, int **dimensiune_manusi, int **inaltime, 
    int nr_jucatori, int spiridus_plecat[NMAX], int raza, int a, int nr_eliminari[NMAX]) {
    int copie_stamina = spiridus[a].stamina;
    int id;
    consum_stamina(inaltime[spiridus[a].x][spiridus[a].y], 
        inaltime[spiridus[a].x][spiridus[a].y - 1], 
        spiridus, a);
    if (spiridus[a].stamina >= 0) {
        id = id_spiridus(spiridus, spiridus[a].x,
        	spiridus[a].y - 1, nr_jucatori);
        spiridus[a].y--;
        if (spiridus_in_ghetar(spiridus[a], raza, raza, raza) == 0) {
            fprintf(out, "%s fell off the glacier.\n", spiridus[a].nume);
            spiridus_plecat[a] = 1;
        }
        schimbare_dmg(spiridus, dimensiune_manusi, a);
        if (id != -1) {
            atac(out, spiridus, a, id, spiridus_plecat, nr_eliminari);
        }
    } else {
        spiridus[a].stamina = copie_stamina;
    }
}

void mutare_R(FILE *out, spiridus_t *spiridus, int **dimensiune_manusi, int **inaltime, 
    int nr_jucatori, int spiridus_plecat[NMAX], int raza, int a, int nr_eliminari[NMAX]) {
    int copie_stamina = spiridus[a].stamina;
    int id;
    consum_stamina(inaltime[spiridus[a].x][spiridus[a].y], 
        inaltime[spiridus[a].x][spiridus[a].y + 1], 
        spiridus, a);
    if (spiridus[a].stamina >= 0) {
        id = id_spiridus(spiridus, spiridus[a].x,
        	spiridus[a].y + 1, nr_jucatori);
        spiridus[a].y++;
        if (spiridus_in_ghetar(spiridus[a], raza, raza, raza) == 0) {
            fprintf(out, "%s fell off the glacier.\n", spiridus[a].nume);
            spiridus_plecat[a] = 1;
        }
        schimbare_dmg(spiridus, dimensiune_manusi, a);
        if (id != -1) {
            atac(out, spiridus, a, id, spiridus_plecat, nr_eliminari);
        }
    } else {
        spiridus[a].stamina = copie_stamina;
    }
}

void mutare(FILE *out, char s[NMAX], spiridus_t *spiridus, int **dimensiune_manusi, int **inaltime, 
    int nr_jucatori, int spiridus_plecat[NMAX], int raza, int nr_eliminari[NMAX]) {
    int c = 0, a;
    char v[5], u[20];
    sscanf(s,"%s %d %s", v, &a, u);
    while (u[c] != '\0') {
        if (u[c] == 'D' && spiridus_plecat[a] != 1) {
            mutare_D(out, spiridus, dimensiune_manusi, inaltime, nr_jucatori, spiridus_plecat, raza, a, nr_eliminari);
        }
        if (u[c] == 'U' && spiridus_plecat[a] != 1) {
            mutare_U(out, spiridus, dimensiune_manusi, inaltime, nr_jucatori, spiridus_plecat, raza, a, nr_eliminari);
        }
        if (u[c] == 'L' && spiridus_plecat[a] != 1) {
            mutare_L(out, spiridus, dimensiune_manusi, inaltime, nr_jucatori, spiridus_plecat, raza, a, nr_eliminari);
        }
        if (u[c] == 'R' && spiridus_plecat[a] != 1) {
            mutare_R(out, spiridus, dimensiune_manusi, inaltime, nr_jucatori, spiridus_plecat, raza, a, nr_eliminari);
        }
        c++;
    }
}

void furtuna_de_zapada(FILE *out, spiridus_t *spiridus,
    char s[NMAX], int nr_jucatori, int spiridus_plecat[NMAX], int *c) {
    int masca = 255, v[5], x_epicentru, y_epicentru, raza, dmg, k, i;
    char b[10];
    sscanf(s,"%s %d", b, &k);
    for (i = 0; i < 4; ++i) {
        int c = 8 * i;
        masca <<= c;
        v[i] = (masca & k)>>c;
        if (v[i] < 0) {
            v[i] = 256 + v[i];
        }
        masca = 255;
    }
    x_epicentru = v[0];
    y_epicentru = v[1];
    raza = v[2];
    dmg = v[3];
    for (i = 0; i < nr_jucatori; ++i) {
        if (spiridus_plecat[i] != 1) {
            if (sqrt(pow((spiridus[i].x + *c - x_epicentru), 2) +
            pow((spiridus[i].y + *c - y_epicentru), 2)) < raza) {
                spiridus[i].hp -= dmg;
            }
            if (spiridus[i].hp <= 0) {
                fprintf(out, "%s was hit by snowstorm.\n", spiridus[i].nume);
                spiridus_plecat[i] = 1;
            }
            if (spiridus[i].x == x_epicentru &&
            	spiridus[i].y == y_epicentru && raza == 0) {
                spiridus[i].hp -= dmg;
                if (spiridus[i].hp <= 0) {
                    fprintf(out, "%s was hit by snowstorm.\n", spiridus[i].nume);
                    spiridus_plecat[i] = 1;
                }
            }
        }
    }
}

void topire(FILE *out, int *raza, int **inaltime, int **dimensiune_manusi,
	int *c, char s[NMAX], int nr_jucatori, 
    spiridus_t *spiridus, int spiridus_plecat[NMAX]) {
    *c = *c + 1;
    int stamina, i, j;
    char topire[10];
    sscanf(s,"%s %d", topire, &stamina);
    int raza_initiala = *raza;
    *raza = *raza - 1;
    int raza_finala = *raza;
    for (i = 0; i < nr_jucatori; ++i) {
        if (spiridus_plecat[i] != 1) {
            if (spiridus_in_ghetar(spiridus[i],
                raza_finala, raza_initiala, raza_initiala) == 1) {
                spiridus[i].stamina += stamina;
            } else if (spiridus_in_ghetar(spiridus[i],
                raza_finala, raza_initiala, raza_initiala) == 0) {
                fprintf(out, "%s got wet because of global warming.\n", spiridus[i].nume);
                spiridus_plecat[i] = 1;
            }
        }
    }
    //schimbam coordonatele spiridusilor
    for (i = 0; i < nr_jucatori; ++i) {
    	spiridus[i].x--;
    	spiridus[i].y--;
    }
    //repozitionam matricea initiala
    for (i = 0; i < 2 * raza_finala + 1; ++i) {
        for (j = 0; j < 2 * raza_finala + 1; ++j) {
            inaltime[i][j] = inaltime[i+1][j+1];
            dimensiune_manusi[i][j] = dimensiune_manusi[i+1][j+1];
        }
    }
}

void afisare_scoreboard(FILE *out, int nr_eliminari[NMAX], 
	int nr_jucatori, int spiridus_plecat[NMAX], spiridus_t *spiridus) {
	int i, j, c = 0, d = 0, aux, uscat[NMAX], umed[NMAX];
	char *nume_umed[NMAX]; 
	char *nume_uscat[NMAX];
	char *aux1;
	for (i = 0; i < nr_jucatori; ++i) {
    	uscat[i] = -1;
    	umed[i] = -1;
	}
	for (i = 0; i < nr_jucatori; ++i) {
    	if (spiridus_plecat[i] != 1) {
    		uscat[c] = i;
        	c++;
    	} else {
        	umed[d] = i;
        	d++;
    	} 
	}  
	for (i = 0; i < c - 1; ++i) {
    	for (j = i + 1; j < c; ++j) {
        	if (nr_eliminari[uscat[i]] < nr_eliminari[uscat[j]]) {
        		aux = uscat[i];
            	uscat[i] = uscat[j];
            	uscat[j] = aux;
        	} 
    	}
	}
	for (i = 0; i < d - 1; ++i) {
    	for (j = i + 1; j < d; ++j) {
        	if (nr_eliminari[umed[i]] < nr_eliminari[umed[j]]) {
            	aux = umed[i];
            	umed[i] = umed[j];
            	umed[j] = aux;
        	}
    	}
	}
	for (i = 0; i < c; ++i) {
    	nume_uscat[i] = spiridus[uscat[i]].nume;
	}
	for (i = 0; i < d; ++i) {
    	nume_umed[i] = spiridus[umed[i]].nume;
    }
	for (i = 0; i < d - 1; ++i) {
    	for (j = i+1; j < d; ++j) {
        	if (nr_eliminari[umed[i]] == nr_eliminari[umed[j]]) {
            	if (strcmp(nume_umed[i], nume_umed[j]) > 0) {
            		aux1 = nume_umed[i];
            		nume_umed[i] = nume_umed[j];
            		nume_umed[j] = aux1;
        		}
        	}
    	}
	}
	for (i = 0; i < c-1; ++i) {
    	for (j = i + 1; j < c; ++j) {
        	if (nr_eliminari[uscat[i]] == nr_eliminari[uscat[j]]) {
            	if (strcmp(nume_uscat[i], nume_uscat[j]) > 0) {
            		aux1 = nume_uscat[i];
            		nume_uscat[i] = nume_uscat[j];
            		nume_uscat[j] = aux1;
        		}
        	}
    	}
	}
	for (i = 0; i < c; ++i) {
    	fprintf(out, "%s\tDRY\t%d\n", nume_uscat[i],
    		nr_eliminari[uscat[i]]);
	}
	for (i = 0; i < d; ++i) {
    	fprintf(out, "%s\tWET\t%d\n", nume_umed[i],
    		nr_eliminari[umed[i]]);
	}
}

void citire_date(FILE *in, int raza, int **inaltime, int **dimensiune_manusi, spiridus_t *spiridus, int nr_jucatori) {
    int i, j;
    for (i = 0; i < 2 * raza + 1; ++i) {
        for (j = 0; j < 2 * raza + 1; ++j) {
            fscanf(in, "%d %d", &inaltime[i][j], 
            &dimensiune_manusi[i][j]);
        }
    }
    for (i = 0; i < nr_jucatori; ++i) {
        fscanf(in, "%s", spiridus[i].nume);
        fscanf(in, "%d", &spiridus[i].x);
        fscanf(in, "%d", &spiridus[i].y);
        fscanf(in, "%d", &spiridus[i].hp);
        fscanf(in, "%d", &spiridus[i].stamina);
    }
}

void afisare(FILE *in, FILE *out, spiridus_t *spiridus, 
	int spiridus_plecat[NMAX], int nr_jucatori, 
	int **dimensiune_manusi, int **inaltime, 
	int raza, int nr_eliminari[NMAX], char s[NMAX], int c) {
    while (fgets(s, NMAX, in) != NULL) {
        int k = castigator(spiridus_plecat, nr_jucatori);
        //eliminam linia noua pe care o citeste fgets
        s[strlen(s) - 1] = '\0';
        if (strstr(s, "MOVE") != NULL) {
            if (k != -1) {
                fprintf(out, "%s has won.\n", spiridus[k].nume);
                break;
            } else {
                mutare(out, s, spiridus, dimensiune_manusi, inaltime,
                	nr_jucatori, spiridus_plecat, raza, nr_eliminari);
            }
        }
        if (strstr(s, "SNOWSTORM") != NULL) {
            if (k != -1) {
                fprintf(out, "%s has won.\n", spiridus[k].nume);
                break;
            } else {
                furtuna_de_zapada(out, spiridus, s, nr_jucatori,
                	spiridus_plecat, &c);
            }
        }
        if (strstr(s, "PRINT_SCOREBOARD") != NULL) {
            if (k != -1) {
                fprintf(out, "%s has won.\n", spiridus[k].nume);
                break;
            } else {
                fprintf(out, "SCOREBOARD:\n");
                afisare_scoreboard(out, nr_eliminari, nr_jucatori,
                	spiridus_plecat, spiridus);
            }
        }
        if (strstr(s, "MELTDOWN") != NULL) {
            if (k != -1) {
                fprintf(out, "%s has won.\n", spiridus[k].nume);
                break;
            } else {
                topire(out, &raza, inaltime, dimensiune_manusi,
                	&c, s, nr_jucatori, spiridus, spiridus_plecat);
            }
        }
    }
}

int main() {
    int raza, nr_jucatori, c = 0; 
    //initial presupunem ca toti spiridusii sunt pe ghetar;
    int spiridus_plecat[NMAX] = {0}; 
    char linie_noua, s[NMAX];
    int nr_eliminari[NMAX] = {0};
    FILE *in = fopen("snowfight.in", "r");
    FILE *out = fopen("snowfight.out", "w");
    if (in == NULL) {
        fprintf(stderr, "Nu s-a putut deschide fisierul");
        return -1;
    }
    if (out == NULL) {
        fprintf(stderr, "Nu s-a putut deschide fisierul");
        return -1;
    }
    fscanf(in, "%d %d", &raza, &nr_jucatori);
    fscanf(in, "%c", &linie_noua);
    spiridus_t *spiridus = malloc(nr_jucatori * sizeof(spiridus_t));
    if (spiridus == NULL) {
        printf("Nu se poate aloca");
        return -1;
    }
    int raza_initiala = raza;
    int **inaltime = aloca_2d(2 * raza + 1, 2 * raza + 1);
    int **dimensiune_manusi = aloca_2d(2 * raza + 1, 2 * raza + 1);
    citire_date(in, raza, inaltime, dimensiune_manusi, spiridus, nr_jucatori);
    spiridus_in_afara_ghetarului(out, spiridus, raza, nr_jucatori, spiridus_plecat, dimensiune_manusi);
    afisare(in, out, spiridus, spiridus_plecat, nr_jucatori,
        dimensiune_manusi, inaltime, raza, nr_eliminari, s, c);
    free(spiridus);
    elibereaza_2d(inaltime, 2 * raza_initiala + 1);
    elibereaza_2d(dimensiune_manusi, 2 * raza_initiala + 1);
    fclose(in);
    fclose(out);
    return 0;
}
