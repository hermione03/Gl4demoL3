/*!\file wi ndow.c
 *
 * \brief utilisation de GL4Dummies et Lib Assimp pour chargement de
 * modèles 3D sous différents formats.
 *
 * \author Farès Belhadj amsi@up8.edu
 * \date May 21 2023
 */

#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>
#include "assimp.h"
#include <SDL_mixer.h>
#include "audioHelper.h"

/*!\brief opened window width */
static int _windowWidth = 1280;
/*!\brief opened window height */
static int _windowHeight = 900;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
static GLuint _quadId = 0;
static GLuint _quadId2 = 0;

static GLfloat _dim[] = {1280, 900};

/*!\brief identifiant de modèles générés à partir de fichiers 3D (3dsmax, obj, ...) */
static GLuint _id_modele[2] = {0};
static GLuint _texId[2] = {0};


#define ECHANTILLONS 1024
static Sint16 _hauteurs[ECHANTILLONS];

static void init(void);
static void sortie(void);
static void resize(int w, int h);
static void draw(void);


static void init(void)
{
  /* charger 3 modèles différents */
  _id_modele[0] = assimpGenScene("models/Snapshooter/scene.gltf");
  // _id_modele[1] = assimpGenScene("models/nixanz.3ds");
  _id_modele[1] = assimpGenScene("models/police_car/scene.gltf");
  SDL_Surface * s;


  _quadId = gl4dgGenQuadf();
  _quadId2 = gl4dgGenQuadf();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  _pId = gl4duCreateProgram("<vs>shaders/models.vs", "<fs>shaders/models.fs", NULL);

  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // glGenTextures(sizeof _texId / sizeof*_texId, _texId);
  //  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
  //  * possibles) */
  // glBindTexture(GL_TEXTURE_2D, _texId);
  // /* paramétrer la texture, voir la doc de la fonction glTexParameter
  //  * sur
  //  * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // /* chargement de l'image logo GL4D avec une fonction native SDL2, si
  //  * vous souhaitez charger d'autres type d'images, utilisez par
  //  * exemple IMG_Load de la sous-bibliothèque SDL2_image */
  // if((s = IMG_Load("models/p8.jpg"))) {
  //     /* envoi de la donnée texture depuis la RAM CPU vers la RAM GPU voir
  //      * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
  //      * sur
  //      * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0,
  //        s->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, s->pixels);
  //   SDL_FreeSurface(s);
  // }
  // /* dé-lier la texture 2D */
  // glBindTexture(GL_TEXTURE_2D, 0);
  resize(_windowWidth, _windowHeight);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
static void resize(int w, int h)
{
  _windowWidth = w;
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005, 0.005, -0.005 * _windowHeight / _windowWidth, 0.005 * _windowHeight / _windowWidth, 0.01, 1000.0);
  /* alternative : gl4duPerspectivef(60.0, (GLfloat)_windowWidth/(GLfloat)_windowHeight, 0.01, 1000.0); */
  gl4duBindMatrix("modelViewMatrix");
}

static void draw(void)
{
  static const GLfloat rouge[] = {0.6f, 0.0f, 0.0f, 1.0f}, bleu[] = {0.0f, 0.0f, 0.6f, 1.0f},
    lumpos[] = {-4.0f, 4.0f, 0.0f, 1.0f};
  int i;
  const int nb_modeles = sizeof _id_modele / sizeof *_id_modele, total = 3;
  static GLfloat angle = 0.0f;
  GLfloat lum[4] = {0.0, 0.0, 3.0, 1.0};
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005, 0.005, -0.005 * _windowHeight / _windowWidth, 0.005 * _windowHeight / _windowWidth, 0.01, 1000.0);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();

  gl4duLookAtf(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  /* on va en dessiner "total" objets en alternant entre les 3 modèles ... */
  assimpDrawScene(_id_modele[0]);
  for (i = 0; i < total; ++i)
  {
    gl4duPushMatrix(); /* sauver la matrice (modelViewMatrix) */
    gl4duRotatef(angle + i * (360.0f / total), 0.0f, 1.0f, 0.0f);
    gl4duTranslatef(5.0f, 0.0f, 0.0f);
    gl4duRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    gl4duScalef(6.0f, 6.0f, 6.0f);
    assimpDrawScene(_id_modele[1]);
    gl4duPopMatrix(); /* restaurer la matrice (modelViewMatrix) */
  } 
  
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 5.0f, -30.0f);
  gl4duRotatef(-180.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(12.0f, 5.0f, 10.0f);
  gl4duSendMatrices();
  // /* activer l'étage de textures 0, plusieurs étages sont disponibles,
  //  * nous pouvons lier une texture par type et par étage */
  // glActiveTexture(GL_TEXTURE0);
  // /* lier la texture _texId comme texture 2D */
  // glBindTexture(GL_TEXTURE_2D, _texId);
  // /* envoyer une info au program shader indiquant que tex est une
  //  * texture d'étage 0, voir le type (sampler2D) de la variable tex
  //  * dans le shader */
  // glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  // /* envoi d'un booléen pour inverser l'axe y des coordonnées de
  //  * textures (plus efficace à faire dans le vertex shader */
  // glUniform1i(glGetUniformLocation(_pId, "inv"), 1) ;
  glUniform4fv(glGetUniformLocation(_pId, "couleur") , 1, bleu);
  gl4dgDraw(_quadId2);

  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, -5.0f, -18.0f);
  gl4duRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(10.0f, 10.0f, 10.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur") , 1, rouge);
  gl4dgDraw(_quadId);
/* gestion de l'angle en fonction du temps.
   *
   * L'idéal est de le mettre dans une fonction idle (simulation) ...
   */
  {
    static double t0 = 0.0;
    double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
    angle += 45.0 * dt; /* quart de tour par seconde : 45° */
    t0 = t;
  }
}

static void sortie(void)
{
  gl4duClean(GL4DU_ALL);
}

void chargeModels(int state)
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
        sortie();
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