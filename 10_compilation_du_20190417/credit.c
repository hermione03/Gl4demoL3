/*!\file window.c
 * \brief rendu de texte avec bibliothèque SDL2_ttf
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date June 04 2018 */
#include <stdio.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_ttf.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
// static void resize(int w, int h);
static void draw(void);
static void quit(void);
static void initText(GLuint *ptId, const char *text);

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief identifiant de la géométrie d'un plan */
static GLuint _quad = 0;
/*!\brief identifiant de la texture contenant le texte */
static GLuint _textTexId = 0;

/*!\brief initialise les paramètres OpenGL et les données */
static void init(void)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 0.7f, 0.7f, 1.0f);
    _pId = gl4duCreateProgram("<vs>shaders/credits.vs", "<fs>shaders/credits.fs", NULL);
    gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
    gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
    //   resize(_wW, _wH);
    _quad = gl4dgGenQuadf();
    initText(&_textTexId,
             "Credits \n\n"
             "Library :\n"
             "- GL4Dummies : Fares Belhadj\n"
             "- OpenGL\n"
             "- SDL2\n\n"
             "Developpement : Maria Messaoud-Nacer\n\n"
             "Projet : Demo vigipirate 64k\n\n"
             "Musique :\n"
             "- final_fantasy_versus_XIII1\n\n"
             "Shaders :\n"
             "- mfa julia : Rodolphe Peccatte\n"
             "- lights fractal : Ribbon Assault  - Dave_Hoskins\n"

             "Merci ! ");
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void)
{
    const GLfloat inclinaison = 0.0;
    static GLfloat t0 = -1;
    GLfloat t, d;
    if (t0 < 0.0f)
        t0 = SDL_GetTicks();
    t = (SDL_GetTicks() - t0) / 1000.0f, d = -2.4f /* du retard pour commencer en bas */ + 0.25f /* vitesse */ * t;
    glClearColor(0, 0.1, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(_pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textTexId);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
    glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
    gl4duBindMatrix("modelViewMatrix");
    gl4duLoadIdentityf();
    gl4duScalef(1, 2, 1);
    gl4duTranslatef(0, d * cos(inclinaison * M_PI / 180.0f), -2 + d * sin(inclinaison * M_PI / 180.0f));
    gl4duRotatef(inclinaison, 1, 0, 0);
    gl4duSendMatrices();
    gl4dgDraw(_quad);
    glUseProgram(0);
}

static void quit(void)
{
    if (_textTexId)
    {
        glDeleteTextures(1, &_textTexId);
        _textTexId = 0;
    }
    gl4duClean(GL4DU_ALL);
}

static void initText(GLuint *ptId, const char *text)
{
    static int firstTime = 1;
    SDL_Color c = {255, 190, 0, 255};
    SDL_Surface *d, *s;
    TTF_Font *font = NULL;
    if (firstTime)
    {
        /* initialisation de la bibliothèque SDL2 ttf */
        if (TTF_Init() == -1)
        {
            fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
            exit(2);
        }
        firstTime = 0;
    }
    if (*ptId == 0)
    {
        /* initialisation de la texture côté OpenGL */
        glGenTextures(1, ptId);
        glBindTexture(GL_TEXTURE_2D, *ptId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    /* chargement de la font */
    if (!(font = TTF_OpenFont("SchrodingerSans-Bold.ttf", 64)))
    {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }
    /* création d'une surface SDL avec le texte */
    d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 2048);
    if (d == NULL)
    {
        TTF_CloseFont(font);
        fprintf(stderr, "Erreur lors du TTF_RenderText\n");
        return;
    }
    /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
    s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
    assert(s);
    SDL_BlitSurface(d, NULL, s, NULL);
    SDL_FreeSurface(d);
    /* transfert vers la texture OpenGL */
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);
    SDL_FreeSurface(s);
    TTF_CloseFont(font);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void credit(int state)
{
    switch (state)
    {
    case GL4DH_INIT:
        init();
        return;
    case GL4DH_FREE:
        quit();
        return;
    case GL4DH_UPDATE_WITH_AUDIO:
        // audio();
        return;
    default: /* GL4DH_DRAW */
        draw();
        return;
    }
}
