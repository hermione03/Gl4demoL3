#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief opened window width */
/*!\brief opened window height */
static GLfloat _dim[] = {1280, 900};
// extern GLfloat _dim[];
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
static GLuint _quad = 0;

// Music
/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
static GLint _amplitude_sonore = -1;
static int currentSampleIndex = 0;

/*!\brief initialise les paramètres OpenGL et les données. */
static void init(void)
{
    /* Création du programme shader (voir le dossier shader) */
    _pId = gl4duCreateProgram("<vs>shaders/julia.vs", "<fs>shaders/julia.fs", NULL);
    /* Set de la couleur (RGBA) d'effacement OpenGL */
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    _quad = gl4dgGenQuadf();

    /* Création des matrices GL4Dummies, une pour la projection, une pour la modélisation et une pour la vue */
    gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
    gl4duGenMatrix(GL_FLOAT, "modelMatrix");
    gl4duGenMatrix(GL_FLOAT, "viewMatrix");
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void)
{

    /** effacement du buffer de couleur, nous rajoutons aussi le buffer
     * de profondeur afin de bien rendre les fragments qui sont devant
     * au dessus de ceux qui sont derrière. Commenter le
     * "|GL_DEPTH_BUFFER_BIT" pour voir la différence. Nous avons ajouté
     * l'activation du test de profondeur dans la fonction init via
     * l'appel glEnable(GL_DEPTH_TEST). */
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* activation du programme _pId */
    glUseProgram(_pId);
    gl4duBindMatrix("projectionMatrix");
    gl4duLoadIdentityf();
    gl4duOrthof(-1, 1, -_dim[1] / _dim[0], _dim[1] / _dim[0], 0, 100);

    gl4duBindMatrix("viewMatrix");
    gl4duLoadIdentityf();
    /** Composer la matrice vue courante en simulant une "caméra" à
     * l'aide de la fonction LookAt(xyz_position_cam,
     * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
    gl4duLookAtf(0, 0, 1, 0, 0, 0, 0, 1, 0);

    /* lier (mettre en avant ou "courante") la matrice modèle créée dans init */
    gl4duBindMatrix("modelMatrix");
    gl4duLoadIdentityf();

    /** on incrémente angle d'un 1/60 de 1/4 de tour soit (360° x 1/60). Tester
     * l'application en activant/désactivant la synchronisation
     * verticale de votre carte graphique. Que se passe-t-il ? Trouver
     * une solution pour que résultat soit toujours le même. */
    // angle = 0.25f * (1.0f / 60.0f) * 360.0f;

    /* Envoyer, au shader courant, toutes les matrices connues dans GL4Dummies */
    gl4duSendMatrices();

    // Envoi des données d'amplitude au shader
    // float currentSample = ( GLfloat ) _hauteurs[currentSampleIndex];
    float currentSample = 0;
    for (int i = 0; i < ECHANTILLONS; i++)
    {
        currentSample += _hauteurs[i];
    }
    currentSample /= ECHANTILLONS;
    _amplitude_sonore = glGetUniformLocation(_pId, "amplitudeSon");
    printf("currentSample: %f\n", currentSample);
    glUniform1f(_amplitude_sonore, currentSample);
    currentSampleIndex = (currentSampleIndex + 1) % ECHANTILLONS;

    float et = gl4dGetElapsedTime();
    glUniform1f(glGetUniformLocation(_pId, "u_time"), (et / 1000));
    gl4dgDraw(_quad);

    /* désactiver le programme shader */
    glUseProgram(0);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void)
{
    if (_quad)
        gl4dgDelete(_quad);
    /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
    gl4duClean(GL4DU_ALL);
}

void julia(int state)
{
    static int h = 1;
    Sint16 *audio_samples;
    switch (state)
    {
    case GL4DH_INIT:
        h = _dim[1];
        init();
        return;
    case GL4DH_FREE:
        quit();
        return;
    case GL4DH_UPDATE_WITH_AUDIO:
        audio_samples = (Sint16 *)ahGetAudioStream();
        if (ahGetAudioStreamLength() >= 2 * ECHANTILLONS)
        {
            for (int i = 0; i < ECHANTILLONS; i++)
            {
                _hauteurs[i] = h / 2 + (h / 2) * audio_samples[i] / ((1 << 15) - 1.0);
            }
        }
        return;
    default: /* GL4DH_DRAW */
        draw();
        return;
    }
}