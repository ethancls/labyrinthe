def lire_et_ecrire_matrice(nom_fichier):
    # Ouvrir le fichier en mode lecture
    with open(nom_fichier, 'r') as fichier:
        # Lire la matrice depuis le fichier
        matrice = [ligne.strip().split() for ligne in fichier]

    # Ouvrir le fichier en mode écriture
    with open(nom_fichier, 'w') as fichier:
        # Réécrire la matrice dans le fichier avec des éléments séparés par des ;
        for ligne in matrice:
            nouvelle_ligne = ';'.join(ligne) + ';' # Ajouter le ; de la fin
            fichier.write(nouvelle_ligne + '\n')

nom_fichier = './labyrinthe_hard.txt'
lire_et_ecrire_matrice(nom_fichier)
