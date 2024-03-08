# Hypercube Process System (HPS)

![](hypercube.png)

## À propos du projet

Ce projet implémente un système de processus structurés en hypercube, conçu pour illustrer les concepts de calcul parallèle et de communication inter-processus dans un environnement Unix/Linux. Chaque processus dans l'hypercube peut communiquer avec ses voisins directs à travers des tubes (pipes), permettant ainsi la circulation d'un jeton (token) à travers l'hypercube.

## Structure du code

Le projet est organisé en plusieurs fichiers principaux pour séparer les préoccupations et faciliter la maintenance et l'extension :

- **HypercubeProcessSystem.c** : Le point d'entrée du programme qui orchestre la création de l'hypercube et le démarrage des processus.
- **ProcessCommunication.c/h** : Gère la création des tubes, la communication entre les processus, et la logique spécifique à chaque processus.
- **SignalHandlers.c/h** : Contient les gestionnaires de signaux pour suspendre, reprendre et terminer proprement les processus.
- **Utilities.c/h** : Fournit des fonctions utilitaires, comme la vérification de la différence d'un bit entre deux étiquettes de sommets.

## Compilation

Un `Makefile` est fourni pour faciliter la compilation du projet. Pour compiler le projet, utilisez la commande suivante à la racine du projet :

```bash
make all
```

Cela générera l'exécutable `hypercube`.

## Exécution

Pour exécuter le programme, utilisez la commande suivante, où `n` est la dimension de l'hypercube :

```bash
./hypercube n
```

## Sujet du projet

Le sujet du projet est disponible dans le fichier `TP.pdf`.

## Auteurs

- Despoullains Romain
- Vautard Cloé
