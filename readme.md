# Grand Line 3D Viewer

> Un moteur de rendu 3D développé en **C++** et **OpenGL 3.3**. Cette application permet d'explorer les navires et personnages iconiques de l'univers de *One Piece* à travers une interface interactive complète, intégrant un éclairage dynamique, un pipeline de post-processing et une interface graphique fluide.

---

### Auteurs
* **Justine** (E4FI)
* **Aman** (E4FI) 
* **E4FI- Groupe 1I**

## Fonctionnalités Principales (Features)

L'application intègre plusieurs techniques avancées combinant la gestion de la géométrie 3D, des calculs matriciels de caméra, et des effets de rendu programmés via des shaders GLSL :

* **Chargement de Modèles 3D complexes**
    * **Gestion du format `.obj` et `.mtl` :** Décodage et chargement des géométries texturées (trouvé sur https://sketchfab.com/feed).
    * **Gestion des matériaux :** Prise en charge complète des textures diffuses, des couleurs d'ambiance, des composantes diffuses/spécualires (`u_material.specularColor`) et du coefficient de brillance (Shininess) propre à chaque maillage.
    * **Architecture multi-mesh :** Rendu optimisé via l'utilisation de structures de tampons OpenGL standard (VAO, VBO) adaptées aux sous-parties de chaque modèle.

* **Système de Caméra Orbitale & Cadrage Automatique**
    * **Contrôle Sphérique Intuitif :** Déplacement orbital fluide autour de l'origine en modifiant les angles de rotation $\theta$ et $\phi$ via la souris, avec un zoom géré à la molette.
    * **Calcul de Bounding Box (Boîte Englobante) :** Analyse automatique de la géométrie de chaque fichier à son chargement pour recentrer l'objet à l'origine et calculer son diamètre maximal.
    * **Zoom Adaptatif Automatique :** Ajustement de la distance de recul initiale (`radius`) et de la distance (`zFar`) de la matrice de projection en fonction de la taille réelle de l'objet (permettant de passer d'un petit modèle à un géant comme Laboon sans aucun problème de cadrage).

* **Interface Graphique Interactive (GUI) via Dear ImGui**
    * **Grand Line Dashboard :** Panneau de contrôle complet permettant de basculer d'un objet à un autre en un clic, d'ajuster dynamiquement l'intensité de la source lumineuse globale ou d'appliquer les effets sur l'objet 3D.
    * **HUD de Navigation :** Fenêtre personnalisée en haut de l'écran affichant l'intitulé exact de l'entité 3D visionnée ainsi qu'un système de pagination fluide.

* **Gestion de l'Éclairage (Blinn-Phong)**
    * Calcul des réflections en temps réel dans les Shaders de Fragment à l'aide de la position de la caméra et d'une source de lumière positionnelle.
    * **3 Modes d'Ambiance programmés :**
        * *Soleil de l'Aube :* Éclairage standard d'un blanc neutre et équilibré, idéal pour inspecter les détails des modèles.
        * *Nuit Profonde :* Ambiance nocturne tamisée aux teintes bleutées sombres.
        * *Disco Punk-Hazard :* Éclairage festif et dynamique dont les composantes chromatiques (Rouge, Vert, Bleu) oscillent continuellement en fonction du temps (`glfwGetTime()`). *Une ambiance digne du laboratoire de Caesar Clown.*

* **Pipeline de Post-Processing sur Mesure**
    * **Architecture FBO :** Dérivation du rendu de la scène principale vers une texture de couleur 2D et un tampon de profondeur (`GL_DEPTH_COMPONENT24`) au lieu du tampon d'affichage par défaut.
    * **Rendu sur Écran (Quad Rendering) :** Projection de la texture générée sur un rectangle couvrant l'intégralité du viewport à l'aide d'un shader de post-process dédié.
    * **Activation Ciblée :** Effet applicable manuellement depuis le Dashboard pour n'importe quel élément de la scène.

## Architecture du Code (Structure)

```text
COMPUTER-GRAPHICS/
├── 3Dobjects/            # Modèles 3D complets (.obj, .mtl et textures associées)
├── bin/                  # Dossier des binaires et exécutables générés
├── common/               # Utilitaires partagés (ex: GLShader.h pour compiler les shaders)
├── imgui/                # Bibliothèque Dear ImGui pour l'interface graphique
├── shaders/              # Codes sources GLSL (Vertex, Fragment et Post-process)
│
├── main.cpp              # Point d'entrée, boucle de rendu principale, FBO et éclairage
├── Makefile              # Script de compilation automatisé pour le projet
│
├── Maths.h / .cpp        # Algèbre linéaire maison (Vecteurs Vec3, Matrices Mat4, Transformations)
├── Navigation.h / .cpp   # Calculs de caméra orbitale, callbacks souris/clavier et HUD
├── Structures.h          # Déclarations des types (Mesh, Material, ViewProj, FramebufferData)
├── Utils.h / .cpp        # Parseur d'objets (LoadOBJ), chargeur de textures et Bounding Box
│
├── imgui.ini             # Sauvegarde de la disposition des fenêtres ImGui
└── readme.md             # Documentation du projet
```

## Dépendances & Prérequis

Avant de pouvoir compiler et lancer l'application, assurez-vous que votre environnement de développement dispose des outils et bibliothèques suivants :

### 1. Langage & Compilateur
* **C++11** (ou version supérieure).
* Un compilateur prenant en charge le C++.

### 2. Gestionnaire de Fenêtres & API Graphique
Le projet s'appuie sur les bibliothèques standards de OpenGL pour l'affichage et la gestion du contexte :
* **GLEW (OpenGL Extension Wrangler) :** Indispensable pour charger les extensions OpenGL modernes. 
* **GLFW3 :** Gère la création de la fenêtre, le contexte OpenGL, ainsi que la capture des événements clavier/souris.
* **OpenGL 3.3 :** L'API graphique matérielle native utilisée pour le pipeline programmable (Shaders).

### 3. Interface Graphique (GUI)
* **Dear ImGui :** Utilisé pour le Dashboard de contrôle et le HUD de navigation. 
  * *Note : Les fichiers sources de Dear ImGui sont directement intégrés au dépôt (dans le dossier `/imgui`), vous n'avez donc pas besoin de l'installer manuellement sur votre système.*

### 4. Outil de Compilation (Build System)
* **GNU Make (Makefile) :** Le projet utilise un fichier `Makefile` à la racine pour automatiser et lier toutes les dépendances lors de la compilation.

---

## Installation et Compilation

Grâce au `Makefile` configuré à la racine du projet, la compilation et le lancement de l'application sont entièrement automatisés. Le script se charge également de détecter automatiquement vos dossiers d'inclusion (notamment pour les utilisateurs de macOS via Homebrew).

Ouvrez votre terminal et exécutez les commandes suivantes :

```bash
# 1. Cloner le dépôt et entrer dans le répertoire du projet
git clone <url-du-repo>
cd <nom-du-projet>

# 2. Compiler et lancer l'application en une seule commande
make
```

###  Les commandes du Makefile à votre disposition

Le `Makefile` met à disposition plusieurs cibles utiles pour gérer votre build :

* **`make` (ou `make all`) :** Crée le dossier `bin/`, compile l'intégralité des sources (y compris ImGui et les utilitaires communs) et lance immédiatement l'exécutable.
* **`make compile` :** Compile uniquement le code source et génère l'exécutable sous `bin/projet`.
* **`make run` :** Exécute l'application sans relancer la compilation.
* **`make clean` :** Nettoie le dossier `bin/` en supprimant les fichiers générés.

---

### ⚠️ Note Importante : Contexte d'Exécution

> **IMPORTANT :** L'application utilise des chemins relatifs stricts (`./3Dobjects/...` et `./shaders/...`) pour charger les ressources 3D et les shaders GLSL.

> Vous devez impérativement lancer l'application **depuis la racine du projet** (là où se trouve le `Makefile`). Si vous tentez de vous déplacer dans le dossier `bin/` et de lancer `./projet`, l'application ne trouvera ni les navires (Merry, Sunny) ni les shaders, ce qui provoquera une erreur ou un plantage au démarrage. En utilisant simplement la commande `make` ou `make run` depuis la racine, vous êtes assuré que tout fonctionnera parfaitement !

## Commandes et Raccourcis

Pour interagir avec le visualiseur 3D et explorer les modèles sous tous les angles, vous pouvez utiliser les commandes clavier et souris détaillées ci-dessous :

| Périphérique | Action / Input | Description |
| :--- | :--- | :--- |
| ⌨️ **Clavier** | `Flèche Droite` | Passe instantanément à l'objet 3D suivant dans la liste. |
| | `Flèche Gauche` | Revient à l'objet 3D précédent dans la liste. |
| 🖱️ **Souris** | `Clic Gauche + Glisser` | Fait tourner la caméra orbitale (ajustement des angles $\theta$ et $\phi$) autour du modèle central. |
| | `Molette de défilement` | Ajuste la distance de la caméra (`radius`) pour zoomer ou dézoomer sur l'objet. |
| 🎛️ **Interface** | `Clic Gauche` | Permet d'interagir directement avec les boutons, curseurs (sliders) et menus déroulants du Dashboard. |

---

## Galerie de Captures d'Écran (Screenshots)

Voici un aperçu du rendu en temps réel de l'application avec ses différents modes et modèles :

![Vogue Merry - Rendu Standard]()
*Le Vogue Merry affiché*

![Thousand Sunny - Mode Disco]()
*L'ambiance chromatique dynamique "Disco Punk-Hazard" appliquée sur le Thousand Sunny.*

![Pipeline de Post-Processing]()
*Effet de rendu activé via le shader de Post-Processing et le Framebuffer (FBO).*
