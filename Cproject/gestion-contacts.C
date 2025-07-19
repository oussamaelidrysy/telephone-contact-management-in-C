#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definition de la structure Contact
typedef struct Contact {
    char nom[100];
    char tel[20];
    char mail[100];
    struct Contact* suivant;
} Contact;

// Definition de la structure Lettre
typedef struct Lettre {
    char lettre;
    Contact* listeContacts;
    struct Lettre* lettreSuivante;
} Lettre;

// Fonction pour creer un nouveau contact
Contact* creerContact(const char* nom, const char* tel, const char* mail) {
    Contact* c = malloc(sizeof(Contact));
    if (c == NULL) {
        printf("Erreur d'allocation memoire.\n");
        exit(1);
    }
    strcpy(c->nom, nom);
    strcpy(c->tel, tel);
    strcpy(c->mail, mail);
    c->suivant = NULL;
    return c;
}

// Fonction utilitaire pour comparer deux chaînes sans tenir compte de la casse
int comparer_noms_sans_casse(const char* nom1, const char* nom2) {
    char buf1[100], buf2[100];
    int i;
    for (i = 0; nom1[i] && i < 99; i++) buf1[i] = tolower((unsigned char)nom1[i]);
    buf1[i] = '\0';
    for (i = 0; nom2[i] && i < 99; i++) buf2[i] = tolower((unsigned char)nom2[i]);
    buf2[i] = '\0';
    return strcmp(buf1, buf2);
}

// Fonction pour ajouter un contact (création de la lettre si besoin)
void ajouterContact(Lettre** listeLettres, const char* nom, const char* tel, const char* mail) {
    if (nom == NULL || nom[0] == '\0') {
        printf("Nom invalide.\n");
        return;
    }
    char premiereLettre = toupper(nom[0]);
    Lettre *precLettre = NULL, *courantLettre = *listeLettres;

    // Recherche de la lettre ou de la position d'insertion
    while (courantLettre && courantLettre->lettre < premiereLettre) {
        precLettre = courantLettre;
        courantLettre = courantLettre->lettreSuivante;
    }

    // Cas 1 : La lettre n'existe pas, il faut la créer et l'insérer
    if (!courantLettre || courantLettre->lettre != premiereLettre) {
        Lettre* nouvelleLettre = malloc(sizeof(Lettre));
        if (nouvelleLettre == NULL) {
            printf("Erreur d'allocation memoire.\n");
            exit(1);
        }
        nouvelleLettre->lettre = premiereLettre;
        nouvelleLettre->listeContacts = NULL;
        nouvelleLettre->lettreSuivante = courantLettre;

        if (precLettre == NULL) {
            // Insertion en tête
            *listeLettres = nouvelleLettre;
        } else {
            precLettre->lettreSuivante = nouvelleLettre;
        }
        courantLettre = nouvelleLettre;
    }

    // Ajout du contact dans la liste de la lettre (ordre alphabétique)
    Contact *prec = NULL, *curr = courantLettre->listeContacts;
    while (curr && comparer_noms_sans_casse(nom, curr->nom) > 0) {
        prec = curr;
        curr = curr->suivant;
    }
    if (curr && comparer_noms_sans_casse(nom, curr->nom) == 0) {
        printf("Un contact avec ce nom existe deja.\n");
        return;
    }
    Contact* nouveau = creerContact(nom, tel, mail);
    if (prec == NULL) {
        nouveau->suivant = courantLettre->listeContacts;
        courantLettre->listeContacts = nouveau;
    } else {
        prec->suivant = nouveau;
        nouveau->suivant = curr;
    }
    printf("Contact ajoute avec succes.\n");
}

// Fonction pour supprimer un contact par nom
void supprimerContact(Lettre** listeLettres, const char* nom) {
    if (nom == NULL || nom[0] == '\0') {
        printf("Nom invalide.\n");
        return;
    }

    char premiereLettre = toupper(nom[0]);
    Lettre *precLettre = NULL, *courantLettre = *listeLettres;

    // Recherche de la lettre correspondante
    while (courantLettre && courantLettre->lettre < premiereLettre) {
        precLettre = courantLettre;
        courantLettre = courantLettre->lettreSuivante;
    }
    if (!courantLettre || courantLettre->lettre != premiereLettre) {
        printf("Aucun contact trouve avec ce nom.\n");
        return;
    }

    // Recherche du contact à supprimer
    Contact *prec = NULL, *curr = courantLettre->listeContacts;
    while (curr && comparer_noms_sans_casse(nom, curr->nom) != 0) {
        prec = curr;
        curr = curr->suivant;
    }
    if (!curr) {
        printf("Aucun contact trouve avec ce nom.\n");
        return;
    }

    // Suppression du contact
    if (prec == NULL) {
        courantLettre->listeContacts = curr->suivant;
    } else {
        prec->suivant = curr->suivant;
    }
    free(curr);
    printf("Contact supprime avec succes.\n");

    // Si la lettre n'a plus de contacts, la supprimer aussi
    if (courantLettre->listeContacts == NULL) {
        if (precLettre == NULL) {
            *listeLettres = courantLettre->lettreSuivante;
        } else {
            precLettre->lettreSuivante = courantLettre->lettreSuivante;
        }
        free(courantLettre);
    }
}

// Fonction pour afficher tous les contacts par ordre alphabétique
void afficherContacts(const Lettre* listeLettres) {
    int vide = 1;
    const Lettre* l = listeLettres;
    while (l) {
        if (l->listeContacts) {
            vide = 0;
            break;
        }
        l = l->lettreSuivante;
    }
    if (vide) {
        printf("Aucun contact a afficher.\n");
        return;
    }
    printf("\nNom | Tel | Mail\n");
    printf("----------------+-----------------+-------------------------------\n");
    l = listeLettres;
    while (l) {
        const Contact* c = l->listeContacts;
        while (c) {
            printf("%-13s | %-13s | %-30s\n", c->nom, c->tel, c->mail);
            c = c->suivant;
        }
        l = l->lettreSuivante;
    }
}

// Fonction pour chercher des contacts selon différents critères
void chercherContact(const Lettre* listeLettres, const char* critere) {
    if (critere == NULL || critere[0] == '\0') {
        printf("Critere de recherche invalide.\n");
        return;
    }
    int critereLen = strlen(critere);
    int trouve = 0;
    if (critereLen == 1) {
        // Recherche par premiere lettre
        char lettreRecherchee = toupper(critere[0]);
        const Lettre* l = listeLettres;
        while (l && l->lettre < lettreRecherchee) l = l->lettreSuivante;
        if (l && l->lettre == lettreRecherchee) {
            const Contact* c = l->listeContacts;
            while (c) {
                if (!trouve) {
                    printf("\nNom | Tel | Mail\n");
                    printf("----------------+-----------------+-------------------------------\n");
                }
                printf("%-13s | %-13s | %-30s\n", c->nom, c->tel, c->mail);
                trouve = 1;
                c = c->suivant;
            }
        }
    } else if (critereLen == 2) {
        // Recherche par premiere et deuxieme lettre
        char lettreRecherchee = toupper(critere[0]);
        char deuxiemeLettre = tolower(critere[1]);
        const Lettre* l = listeLettres;
        while (l && l->lettre < lettreRecherchee) l = l->lettreSuivante;
        if (l && l->lettre == lettreRecherchee) {
            const Contact* c = l->listeContacts;
            while (c) {
                if (tolower((unsigned char)c->nom[1]) == deuxiemeLettre) {
                    if (!trouve) {
                        printf("\nNom | Tel | Mail\n");
                        printf("----------------+-----------------+-------------------------------\n");
                    }
                    printf("%-13s | %-13s | %-30s\n", c->nom, c->tel, c->mail);
                    trouve = 1;
                }
                c = c->suivant;
            }
        }
    } else {
        // Recherche par nom complet (insensible a la casse)
        const Lettre* l = listeLettres;
        while (l) {
            const Contact* c = l->listeContacts;
            while (c) {
                if (comparer_noms_sans_casse(critere, c->nom) == 0) {
                    if (!trouve) {
                        printf("\nNom | Tel | Mail\n");
                        printf("----------------+-----------------+-------------------------------\n");
                    }
                    printf("%-13s | %-13s | %-30s\n", c->nom, c->tel, c->mail);
                    trouve = 1;
                }
                c = c->suivant;
            }
            l = l->lettreSuivante;
        }
    }

    if (!trouve) {
        printf("Aucun contact trouve pour le critere donne.\n");
    }
}

// Fonction pour modifier un contact (modifie tel et/ou mail)
void modifierContact(Lettre* listeLettres, const char* nom, const char* nouveauTel, const char* nouveauMail) {
    if (nom == NULL || nom[0] == '\0') {
        printf("Nom invalide.\n");
        return;
    }
    char premiereLettre = toupper(nom[0]);
    Lettre* l = listeLettres;
    // Recherche de la lettre correspondante
    while (l && l->lettre < premiereLettre) l = l->lettreSuivante;
    if (!l || l->lettre != premiereLettre) {
        printf("Aucun contact trouve avec ce nom.\n");
        return;
    }
    Contact* c = l->listeContacts;
    while (c) {
        if (comparer_noms_sans_casse(nom, c->nom) == 0) {
            if (nouveauTel && nouveauTel[0] != '\0') {
                strncpy(c->tel, nouveauTel, sizeof(c->tel)-1);
                c->tel[sizeof(c->tel)-1] = '\0';
            }
            if (nouveauMail && nouveauMail[0] != '\0') {
                strncpy(c->mail, nouveauMail, sizeof(c->mail)-1);
                c->mail[sizeof(c->mail)-1] = '\0';
            }
            printf("Contact modifie avec succes.\n");
            return;
        }
        c = c->suivant;
    }
    printf("Aucun contact trouve avec ce nom.\n");
}

// Fonction pour sauvegarder tous les contacts dans un fichier texte
void sauvegarderContacts(const Lettre* listeLettres, const char* nomFichier) {
    FILE* f = fopen(nomFichier, "w");
    if (!f) {
        printf("Erreur lors de l'ouverture du fichier pour la sauvegarde.\n");
        return;
    }

    const Lettre* l = listeLettres;
    while (l) {
        const Contact* c = l->listeContacts;
        while (c) {
            fprintf(f, "%s;%s;%s\n", c->nom, c->tel, c->mail);
            c = c->suivant;
        }
        l = l->lettreSuivante;
    }

    fclose(f);
    printf("Contacts sauvegardes dans le fichier '%s'\n", nomFichier);
}

// Fonction pour libérer toute la mémoire de la liste des contacts
void libererContacts(Lettre** listeLettres) {
    Lettre* l = *listeLettres;
    while (l) {
        Contact* c = l->listeContacts;
        while (c) {
            Contact* tmpC = c;
            c = c->suivant;
            free(tmpC);
        }
        Lettre* tmpL = l;
        l = l->lettreSuivante;
        free(tmpL);
    }
    *listeLettres = NULL;
}

// Fonction pour recharger les contacts depuis un fichier texte
void rechargerContacts(Lettre** listeLettres, const char* nomFichier) {
    libererContacts(listeLettres);
    FILE* f = fopen(nomFichier, "r");
    if (!f) {
        printf("Erreur lors de l'ouverture du fichier pour le rechargement.\n");
        return;
    }

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), f)) {
        char nom[100], tel[20], mail[100];
        // Suppression du saut de ligne
        ligne[strcspn(ligne, "\r\n")] = 0;
        if (sscanf(ligne, "%99[^;];%19[^;];%99[^\n]", nom, tel, mail) == 3) {
            ajouterContact(listeLettres, nom, tel, mail);
        }
    }

    fclose(f);
    printf("Contacts recharges depuis le fichier '%s'\n", nomFichier);
}

int main() {
    Lettre* listeLettres = NULL;
    int choix;
    char nom[100], tel[20], mail[100], critere[100], fichier[128];

    do {
        printf("\n--- Gestionnaire de contacts ---\n");
        printf("1. Ajouter un contact\n");
        printf("2. Supprimer un contact\n");
        printf("3. Modifier un contact\n");
        printf("4. Afficher tous les contacts\n");
        printf("5. Chercher un contact\n");
        printf("6. Sauvegarder les contacts dans un fichier\n");
        printf("7. Recharger les contacts depuis un fichier\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        if (scanf("%d", &choix) != 1) {
            while (getchar() != '\n'); // vider le buffer
            choix = -1;
        }

        getchar(); // consommer le \n

        switch (choix) {
            case 1:
                printf("\nNom : ");
                fgets(nom, sizeof(nom), stdin);
                nom[strcspn(nom, "\r\n")] = 0;
                printf("Telephone : ");
                fgets(tel, sizeof(tel), stdin);
                tel[strcspn(tel, "\r\n")] = 0;
                printf("Email : ");
                fgets(mail, sizeof(mail), stdin);
                mail[strcspn(mail, "\r\n")] = 0;
                ajouterContact(&listeLettres, nom, tel, mail);
                break;
            case 2:
                printf("\nNom du contact a supprimer : ");
                fgets(nom, sizeof(nom), stdin);
                nom[strcspn(nom, "\r\n")] = 0;
                supprimerContact(&listeLettres, nom);
                break;
            case 3:
                printf("\nNom du contact a modifier : ");
                fgets(nom, sizeof(nom), stdin);
                nom[strcspn(nom, "\r\n")] = 0;
                printf("\nNouveau telephone (laisser vide pour ne pas changer) : ");
                fgets(tel, sizeof(tel), stdin);
                tel[strcspn(tel, "\r\n")] = 0;
                printf("\nNouvel email (laisser vide pour ne pas changer) : ");
                fgets(mail, sizeof(mail), stdin);
                mail[strcspn(mail, "\r\n")] = 0;
                modifierContact(listeLettres, nom, tel, mail);
                break;
            case 4:
                afficherContacts(listeLettres);
                break;
            case 5:
                printf("Critere de recherche (nom complet, 1ere lettre ou 1ere+2eme lettre) : ");
                fgets(critere, sizeof(critere), stdin);
                critere[strcspn(critere, "\r\n")] = 0;
                chercherContact(listeLettres, critere);
                break;
            case 6:
                printf("\nNom du fichier pour sauvegarder : ");
                fgets(fichier, sizeof(fichier), stdin);
                fichier[strcspn(fichier, "\r\n")] = 0;
                sauvegarderContacts(listeLettres, fichier);
                break;
            case 7:
                printf("\nNom du fichier a recharger : ");
                fgets(fichier, sizeof(fichier), stdin);
                fichier[strcspn(fichier, "\r\n")] = 0;
                rechargerContacts(&listeLettres, fichier);
                break;
            case 0:
                printf("Au revoir !\n");
                break;
            default:
                printf("Choix invalide.\n");
        }

        if (choix != 0) {
            char rep[8];
            printf("Voulez-vous revenir au menu ? (o/n) : ");
            fgets(rep, sizeof(rep), stdin);
            if (tolower(rep[0]) != 'o') break;
        }

    } while (choix != 0);

    libererContacts(&listeLettres);
    return 0;
}
