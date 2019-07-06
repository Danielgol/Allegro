#include <stdio.h>
    #include <stdlib.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

//FAZER UM TIMER
//srand(time(NULL));

//TEM QUE PERGUNTAR SE TEM ALGUM COMETA VIVO APOS CADA A�AO DO PLAYER
//TEM QUE ATUALIZAR CADA COMETA A CADA A��O TBM
//StartCometa(cometa, NUM_COMETA);
//UpdateCometa(cometa, NUM_COMETA);
//-----------------------------------------------
//TESTE CAIO COMETAS
//const int NUM_COMETA = 10;
//InitCometa(cometa, NUM_COMETA);


//void InitCometa(Cometa cometas[],int size)
//{
//  for(int i = 0;i < size;i++)
//    {
//      cometa[i].ID = ENEMY;
//      cometa[i].live = false;
//      cometa[i].speed = 5; VELOCIDADE DO COMETA, AJUSTAR DE ACORDO COM O JOGO
//      cometa[i].bondx = 18; LEMBRAR DE EXPLICAR ESSES COMANDOS DEPOIS
//      cometa[i].bondy = 18;  ''
//    };
//};


//DrawCometa(cometas,NUM_COMETA);
//void DrawCometa(Cometa cometas[],int size)
//{
// for(int i = 0;i < size;i++)
//   {
//     if(cometa[i].live)
//      {
//        al_load_bitmap("images/asteroids/Asteroid.png");
//      }
//   }
//};

//LEMBRAR DE FAZER NUMERO MAXIMO DE COMETAS NA TELA
//void StartCometa(Cometa cometas[],int size)
//{
//    for(int i = 0; i <size; i++)
//    {
//      if(!cometa[i].live)
//      {
//
//         if(rand % 2000 == 0) FAZER DE OUTRA FORMA DEPOIS
//             {
//                 cometa[i].live = true;
//                 cometa[i].x = TEM QUE SER ALEATORIO mas n em cima da nave, entao algo do tipo nave x + rand ou outro if;
//                 cometa[i].y = TEM QUE SER ALEATORIO ;
//                 break;
//             }
//       }
//    }
//}
//void UpdateCometa(Cometa cometas[],int size)
//{
//   for(int i = 0; i <size; i++)
//     {
//        if(cometa[i].live)
//        {
//          FAZER O COMETA IR EM DIRE��O DA NAVEV
//          cometa[i].x -= cometa[i].speed;
//          cometa[i].y -= cometa[i].speed;
//          //DESTRUIR O COMETA SE ELE TOCAR NA BORDA DO MAPA
//
//          if(cometa[i].x < 0)
//           {
//             cometa[i].live = false;
//           }
//          if(cometa[i].y < 0)
//           {
//             cometa[i].live = false;
//           }
//        }
//     }
//}
//-----------------------------------------------

//ISSUES:
//*PENSAR MAIS SOBRE O JOGO
//*VIDA(WEI)
//*FAZER OS BONECOS ANDAREM NO CAMINHO
//*ATIRAR (TIRO COLIDIR)(GABRIEL)
//*GERAR ASTEROIDES ALEATORIAMENTE NO MAPA(CAIO)
//*MOVIMENTA��O DA NAVE (DANIEL)
//*COMETA AINDA SOME E VOLTA (IMPLANTAR PRECISAMENTE A COLIS�O NAVE-COMETA) (JO�O)

//AO APAGAR QUALQUER OBJETO LEMBRAR:
//*APAGAR OS DADOS QUE CRIAM O OBJETO
//*APAGAR OS COMANDOS DO TECLADO
//*APAGAR O COMANDO DE DESENHO
//*APAGAR OS DESTROYERS

const float FPS = 60;
const int WORLD_W = 3000;
const int WORLD_H = 1500;
const int VELOCITY = 2;
const int DELAY = 6;

typedef struct{
    int width,height;
    ALLEGRO_BITMAP *img;
}SIMP_OBJECT;

typedef struct{
    int cur_Frame;
    int width,height;
    ALLEGRO_BITMAP **imgs;
}DYNF_OBJECT;

typedef struct{
    float x,y;
    float dir;
    int width,height;
    int cur_Frame;
    ALLEGRO_BITMAP **imgs;
}PLAYER;

typedef struct{
    int vida;
    float x,y;
    float forceX, forceY;
    int width,height;
    ALLEGRO_BITMAP *img;
}ASTEROID;

typedef struct{
    float x, y;
    int rotation;
    int controle;
    int repondoOxi;
    float forceX, forceY;
    float width, height;
    ALLEGRO_BITMAP *img;
}SHIP;

typedef struct{
    float x,y;
    int carga;
    int controle;
    int angle;
}GUN;

typedef struct{
    int ativo;
    float x,y;
    float velX,velY;
    int width, height;
    ALLEGRO_BITMAP *img;
}SHOT;

//@OBSOLETE
DYNF_OBJECT load_dynamic_frame_object_bitmaps(int quant, DYNF_OBJECT object, char path[], char img[]){
    //VERIFICAR SE ESSE C�DIGO ALOCA 2 ESPA�OS, SE SIM, DEVOLVER ESSE C�DIGO PARA ANTES DA CHAMADA DESSE M�TODO.
    object.imgs = (ALLEGRO_BITMAP*) malloc(sizeof(ALLEGRO_BITMAP*)*quant);
    for(int i=0; i<quant; i++){
        char *imgi = (char*) malloc(strlen(img)+1);
        imgi[0] = '\0';
        strcat(imgi, img);
        for(int x=0; x<strlen(imgi); x++){
            if(imgi[x]=='#'){
                imgi[x] = (i/10)+'0';
                imgi[x+1] = i-((i/10)*10)+'0';
                break;
            }
        }
        char *result = (char*) malloc(strlen(path) + strlen(imgi) + 2);
        result[0] = '\0';
        strcat(result, path);
        strcat(result, imgi);
        object.imgs[i] = al_load_bitmap(result);
        free(result);
        free(imgi);
    }
    object.width = al_get_bitmap_width(object.imgs[0]);
    object.height = al_get_bitmap_height(object.imgs[0]);
    return object;
}

ALLEGRO_BITMAP** load_dynamic_bitmaps(int quant,char path[], char img[]){
    //VERIFICAR SE ESSE C�DIGO ALOCA 2 ESPA�OS, SE SIM, DEVOLVER ESSE C�DIGO PARA ANTES DA CHAMADA DESSE M�TODO.
    ALLEGRO_BITMAP** imagens = (ALLEGRO_BITMAP**) malloc(sizeof(ALLEGRO_BITMAP*)*quant);
    for(int i=0; i<quant; i++){
        char *imgi = (char*) malloc(strlen(img)+1);
        imgi[0] = '\0';
        strcat(imgi, img);
        for(int x=0; x<strlen(imgi); x++){
            if(imgi[x]=='#'){
                imgi[x] = (i/10)+'0';
                imgi[x+1] = i-((i/10)*10)+'0';
                break;
            }
        }
        char *result = (char*) malloc(strlen(path) + strlen(imgi) + 2);
        result[0] = '\0';
        strcat(result, path);
        strcat(result, imgi);
        imagens[i] = al_load_bitmap(result);
        free(result);
        free(imgi);
    }
    return imagens;
}

SIMP_OBJECT build_Simple_Object(char path[]){
    SIMP_OBJECT object;
    object.img = al_load_bitmap(path);
    object.width = al_get_bitmap_width(object.img);
    object.height = al_get_bitmap_height(object.img);
    return object;
}

DYNF_OBJECT build_Dynamic_Object(int quant, char path[], char img[], int cur_Frame){
    DYNF_OBJECT object;
    object.cur_Frame = cur_Frame;
    object.imgs = load_dynamic_bitmaps(quant, path, img);
    object.width = al_get_bitmap_width(object.imgs[0]);
    object.height = al_get_bitmap_height(object.imgs[0]);
    return object;
}

PLAYER build_player(float x, float y, int quant, char path[], char img[], int cur_Frame, int direction){
    PLAYER player;
    player.x = x;
    player.y = y;
    player.dir = direction;
    player.cur_Frame = cur_Frame;
    player.imgs = load_dynamic_bitmaps(quant, path, img);
    player.width = al_get_bitmap_width(player.imgs[0]);
    player.height = al_get_bitmap_height(player.imgs[0]);
    return player;
}

ASTEROID build_asteroid(float x, float y, float forceX, float forceY, char path[]){
    ASTEROID asteroid;
    asteroid.x = x;
    asteroid.y = y;
    asteroid.forceX = forceX;
    asteroid.forceY = forceY;
    asteroid.img = al_load_bitmap(path);
    asteroid.width = al_get_bitmap_width(asteroid.img);
    asteroid.height = al_get_bitmap_height(asteroid.img);
    return asteroid;
}

SHIP build_ship(float x, float y, float forceX, float forceY, int controle, int rotation, char path[]){
    SHIP ship;
    ship.x = x;
    ship.y = y;
    ship.forceX = forceX;
    ship.forceY = forceY;
    ship.rotation = rotation;
    ship.controle = controle;
    ship.img = al_load_bitmap(path);
    ship.width = al_get_bitmap_width(ship.img);
    ship.height = al_get_bitmap_height(ship.img);
    return ship;
}

GUN build_gun(float x, float y, int controle, int angle){
    GUN gun;
    gun.x = x;
    gun.y = y;
    gun.carga = 20;
    gun.controle = controle;
    gun.angle = angle;
    return gun;
}


void adaptarCamera(float *cameraX, float *cameraY, float x, float y, int width, int height, int SCREEN_W, int SCREEN_H){

    //ATUALIZA A C�MERA AO OBJETO EM MOVIMENTO
    *cameraX = x - SCREEN_W/2 +width/2;
    *cameraY = y - SCREEN_H/2 +height/2;

    //LIMITA A CAMERA NAS BORDAS DO MAPA (PENSAR UM LIMITE DE ASTEROIDS E N�O DA C�MERA)
    if(*cameraX < 0){
        *cameraX = 0;
    }
    if(*cameraY < 0){
        *cameraY = 0;
    }
    if(*cameraX > WORLD_W - SCREEN_W){
        *cameraX = WORLD_W - SCREEN_W;
    }
    if(*cameraY > WORLD_H - SCREEN_H){
        *cameraY = WORLD_H - SCREEN_H;
    }

}

enum MYKEYS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_L, KEY_E
};

/* tirar o comment qnd o tiro tiver implementado
//Verificar se o tiro acertou o asteroide, if(1) == ocorreu a coli�so, asteroid perde vida.
// Observa��o: COLOQUEI COMO NOME DA VARI�VEL DO TIRO "TIRO", quando for implantado, me retornar para eu mudar para o nome que vcs decidirem.
int VerifyColisionTiroAsteroid (TIRO *tiro, ASTEROID *asteroid)
{
    float dist = sqrt((pow((tiro->x + tiro->width/2)-(asteroid->x + asteroid->width/2), 2)) + (pow((tiro->y + tiro->height/2)-(asteroid->y + asteroid->height/2), 2)));
    if (dist <= (shot->width/2)+(asteroid->width/2))
    {
        return 1;
    }
    return 0;
}
*/
//Verificar se o Asteroide bateu na nave - A decidir o que vai acontecer - Retornar 1 significa que ocorreu a colis�o.
int VerifyColisionShipAsteroid (SHIP *ship, ASTEROID *asteroid)
{
    //distancia de dois pontos cartesianos = raiz de (x2-x1)^2 + (y2-y1)^2
    // ship.x+ship.width/2 � o ponto central da nave
    // asteroid.x+asteroid.width/2 � o ponto central do asteroide
    // Dist � calculada pela dist�ncia entre os centros dos dois objetos, lembrando que por serem c�rculos, os raios s�o os mesmos, portanto, height e widht s�o iguais.
    float dist = sqrt((pow((ship->x+ship->width/2)-(asteroid->x+asteroid->width/2), 2))+(pow((ship->y+ship->width/2)-(asteroid->y+asteroid->height/2), 2)));
    if(dist <= (ship->width/2) + (asteroid->width/2))
    {
        return 1;
    }
    return 0;
}

//void limitarAsteroideMundo(float *x, float *y, float *forceX, float *forceY, int width, int height){

    //REPELE O ASTEROIDE NAS BORDAS (PENSAR EM TIRAR)
   // if(*x <= 0){
   //     *forceX = -*forceX;
   // }
   // if(*x + width >= WORLD_W){
    //    *forceX = -*forceX;
   // }
   // if(*y <= 0){
    //    *forceY = -*forceY;
   // }
   // if(*y + height >= WORLD_H){
    //    *forceY = -*forceY;
   // }

void generateAsteroids(int quant, ASTEROID *asteroids){

    float Fx = (rand()%15)/10+0.5;
    float Fy = (rand()%15)/10+0.5;

    (asteroids[0]) = build_asteroid(0, 0, Fx, Fy,"images/asteroids/Asteroid.png");

    int xA = rand()%(WORLD_W-(asteroids->width));
    int yA = rand()%(WORLD_H-(asteroids->height));

    (asteroids)->x = xA;
    (asteroids)->y = yA;

    for(int i=1; i<quant; i++){
        Fx = (rand()%15)/10+0.5;
        Fy = (rand()%15)/10+0.5;

        xA = rand()%(WORLD_W-(asteroids->width));
        yA = rand()%(WORLD_H-(asteroids->width));

        (asteroids[i]) = build_asteroid(xA, yA, Fx, Fy,"images/asteroids/Asteroid.png");
    }
}

void aplicarForcas(float *x, float *y, float forceX, float forceY){
    *x += forceX;
    *y += forceY;
}

void limitarForcas(float *forceX, float *forceY, float limite){

    if(*forceY < -limite){
        *forceY = -limite;
    }
    if(*forceY > limite){
        *forceY = limite;
    }
    if(*forceX < -limite){
        *forceX = -limite;
    }
    if(*forceX > limite){
        *forceX = limite;
    }

}

void frearObjetos(float *forceX, float *forceY, float freio){
    if(*forceY < 0){
        *forceY += freio;
        if(*forceY>0){
            *forceY=0;
        }
    }else if(*forceY > 0){
        *forceY -= freio;
        if(*forceY < 0){
            *forceY = 0;
        }
    }
    if(*forceX < 0){
        *forceX += freio;
        if(*forceX>0){
            *forceX=0;
        }
    }else if(*forceX > 0){
        *forceX -= freio;
        if(*forceX < 0){
            *forceX = 0;
        }
    }
}


int VerifyColisionInterAsteroids (ASTEROID *asteroid1, ASTEROID *asteroid2)
{
    float dist = sqrt((pow((asteroid1->x + asteroid1->width/2)-(asteroid2->x + asteroid2->width/2), 2)) + (pow((asteroid1->y + asteroid1->height/2)-(asteroid2->y + asteroid2->height/2), 2)));
    if (dist <= (asteroid1->width/2) + (asteroid2->width/2))
    {
        return 1;
    }

    return 0;
}

/*
void ColisaoShipAsteroid (SHIP *ship, ASTEROID *asteroid)
{
    float vx = (ship->x+(ship->width/2)) - (asteroid->x + (asteroid->width/2));
    float vy = (ship->y+(ship->height/2)) - (asteroid->y + (asteroid->height/2));

    float lenSq = (vx*vx + vy*vy);
    float len = sqrt(lenSq);

    vx /= len;
    vy /= len;

    asteroid->forceX += -vx;
    asteroid->forceY += -vy;

    limitarForcas(&asteroid->forceX, &asteroid->forceY, 1.5);

    ship->forceX -= asteroid->forceX/10;
    ship->forceY -= asteroid->forceY/10;



}
 tirar comment qnd o tiro tiver implementado
void ColisaoTiroAsteroid (TIRO *tiro, ASTEROID *asteroid)
{
    float dist = sqrt((pow((tiro->x + tiro->width/2)-(asteroid->x + asteroid->width/2), 2)) + (pow((tiro->y + tiro->height/2)-(asteroid->y + asteroid->height/2), 2)));
    if(dist <= (tiro->width/2)+(asteroid->width/2)){
        return 1;
    }
    return 0;


}

void ColisaoInterAsteroids (ASTEROID *asteroid1, ASTEROID *asteroid2)
{
    float vx = (asteroid1->x+(asteroid1->width/2)) - (asteroid2->x + (asteroid2->width/2));
    float vy = (asteroid1->y+(asteroid1->height/2)) - (asteroid2->y + (asteroid2->height/2));

    float lenSq = (vx*vx + vy*vy);
    float len = sqrt(lenSq);

    vx /= len;
    vy /= len;

    asteroid2->forceX += -vx;
    asteroid2->forceY += -vy;

    limitarForcas(&asteroid2->forceX, &asteroid2->forceY, 1.5);

    asteroid1->forceX -= asteroid2->forceX;
    asteroid1->forceY -= asteroid2->forceY;

    asteroid2->forceX *= 0.6;
    asteroid2->forceY *= 0.6;

    limitarForcas(&asteroid1->forceX, &asteroid1->forceY, 1.5);

    asteroid1->forceX *= 0.6;
    asteroid1->forceY *= 0.6;


}
*/

/*
void atirar(SHOT *shots, bool UP, int *carga, float x, float y, int angle, int QUANT_SHOTS){
    if(UP && *carga == 20){
        for(int i=0; i<QUANT_SHOTS; i++){
            if((shots+i)->ativo == 0 ){
                (shots+i)->ativo = 1;
                (shots+i)->x = x;
                (shots+i)->y = y;
                (shots+i)->velX = -sin(angle*3.14159/180)*38;
                (shots+i)->velY = cos(angle*3.14159/180)*38;
                break;
            }
        }
        *carga = 0;
    }
}
*/

/*
void controlarCanhao(bool LEFT, bool RIGHT, int *angle, int limite){
    if(LEFT){
        if(*angle - 2 > limite){
            *angle -= 2;
        }
    }
    if(RIGHT){
        if(*angle + 2 < limite+180){
            *angle += 2;
        }
    }
}
*/

/*
int verificar_Colisao_SHOT_ASTEROID(SHOT *shot, ASTEROID *asteroid){
    float dist = sqrt((pow((shot->x + shot->width/2)-(asteroid->x + asteroid->width/2), 2)) + (pow((shot->y + shot->height/2)-(asteroid->y + asteroid->height/2), 2)));
    if(dist <= (shot->width/2)+(asteroid->width/2)){
        return 1;
    }
    return 0;
}
*/

/*
void realizar_Colisao_SHOT_ASTEROID(SHOT *shot, ASTEROID *asteroid){

    float vx = (shot->x+(shot->width/2)) - (asteroid->x + (asteroid->width/2));
    float vy = (shot->y+(shot->height/2)) - (asteroid->y + (asteroid->height/2));

    float lenSq = (vx*vx + vy*vy);
    float len = sqrt(lenSq);

    vx /= len;
    vy /= len;

    asteroid->forceX += -vx/2;
    asteroid->forceY += -vy/2;

    limitarForcas(&asteroid->forceX, &asteroid->forceY, 1.5);
}
*/

int main(int argc, char **argv){


    ALLEGRO_DISPLAY *display = NULL;
    //ALLEGRO_DISPLAY_MODE   disp_data; //FULLSCREEN
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    //FAZER A CAMERA SEGUIR A NAVE AT� NAS BORDAS DO MAPA, PARA EVITAR O "LAG" DA NAVE.
    float cameraX = 0, cameraY = 0;
    bool key[10] = { false, false, false, false, false, false, false, false, false, false};
    //###PENSAR EM CRIAR OUTRO KEY (UM PARA MEGAMEN E OUTRO PARA SONIC)
    //###PARA CRIAR UMA FUN��O DE MOVIMENTO, ONDE CADA KEY SERIA PASSADO
    bool redraw = true;

    al_init();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(20);
    //al_init_primitives_addon(); //FULLSCREEN
    al_install_keyboard();
    al_install_mouse();
    timer = al_create_timer(1.0 / FPS);

    //FULLSCREEN
    //al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    //display = al_create_display(disp_data.width, disp_data.height);
    //int SCREEN_W = al_get_display_width(display);
    //int SCREEN_H = al_get_display_height(display);
    int SCREEN_W = 1380;
    int SCREEN_H = 780;
    display = al_create_display(SCREEN_W, SCREEN_H);

    srand(time(NULL));



    SHIP ship;
    ship = build_ship(WORLD_W/2, WORLD_H/2, 0, 0, 0, 0,"images/ship/shipWhite.png");

    SIMP_OBJECT propulsor;
    propulsor = build_Simple_Object("images/ship/propulsorWhite.png");

    DYNF_OBJECT fire;
    fire = build_Dynamic_Object(12,"images/ship/fire/","f##.png",0);

    PLAYER player1;
    player1 = build_player(ship.width/2+7,ship.height/2+10,22,"images/players/","a##.png",0,1);

    PLAYER player2;
    player2 = build_player(ship.width/2-35,ship.height/2+10,22,"images/players/","b##.png",0,2);

    //ASTEROIDS------------------------------------------------------{
    int QUANT_ASTEROIDS = 30;
    ASTEROID asteroids[QUANT_ASTEROIDS];
    generateAsteroids(QUANT_ASTEROIDS , &asteroids);
    for(int i=0; i<QUANT_ASTEROIDS; i++){
        asteroids[i].vida = 4;
    }
    //ASTEROIDS


    //OS TIRO E CANHAO-------------------------------
    /*
    GUN gunUP;
    gunUP = build_gun(ship.x+ship.width/2+2, ship.y-6, 0, -90);

    GUN gunLEFT;
    gunLEFT = build_gun(ship.x-6, ship.y+ship.height/2-3, 0, 180);

    GUN gunRIGHT;
    gunRIGHT = build_gun(ship.x+ship.width+10, ship.y+ship.height/2-4, 0, 0);

    GUN gunDOWN;
    gunDOWN = build_gun(ship.x+ship.width/2+2, ship.y+ship.height+7, 0, 90);

    SIMP_OBJECT canhao;
    canhao = build_Simple_Object("images/ship/canhao.png");

    SIMP_OBJECT canhaoBase;
    canhaoBase = build_Simple_Object("images/ship/gunBase.png");
    */
    //--------------------------------------------------------






    SIMP_OBJECT background;
    background = build_Simple_Object("images/background.jpg");
    float BGScale = 1.7;

    SIMP_OBJECT barra;
    barra = build_Simple_Object("images/barra/barra.png");

    SIMP_OBJECT vida;
    vida = build_Simple_Object("images/barra/vida.png");
    float vidascale = 0;

    SIMP_OBJECT oxigenio;
    oxigenio = build_Simple_Object("images/barra/oxigenio.png");
    float oxigenioscale = 0;


    //SONS
    ALLEGRO_SAMPLE *theme;
    ALLEGRO_SAMPLE_INSTANCE *inst_theme;
    theme = al_load_sample("sounds/theme.ogg");
    inst_theme = al_create_sample_instance(theme);
    al_attach_sample_instance_to_mixer(inst_theme,al_get_default_mixer());
    al_set_sample_instance_playmode(inst_theme, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_theme,0.8);

    ALLEGRO_SAMPLE *tiro;
    ALLEGRO_SAMPLE_INSTANCE *inst_tiro;
    tiro = al_load_sample("sounds/tiro.wav");
    inst_tiro = al_create_sample_instance(tiro);
    al_attach_sample_instance_to_mixer(inst_tiro,al_get_default_mixer());
    al_set_sample_instance_gain(inst_tiro,0.9);

    ALLEGRO_SAMPLE *explosao1;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao1;
    explosao1 = al_load_sample("sounds/explosao1.wav");
    inst_explosao1 = al_create_sample_instance(explosao1);
    al_attach_sample_instance_to_mixer(inst_explosao1,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao1,1.0);

    ALLEGRO_SAMPLE *explosao2;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao2;
    explosao2 = al_load_sample("sounds/explosao2.wav");
    inst_explosao2 = al_create_sample_instance(explosao2);
    al_attach_sample_instance_to_mixer(inst_explosao2,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao2,1.0);

    ALLEGRO_SAMPLE *explosao3;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao3;
    explosao3 = al_load_sample("sounds/explosao3.wav");
    inst_explosao3 = al_create_sample_instance(explosao3);
    al_attach_sample_instance_to_mixer(inst_explosao3,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao3,1.0);

    ALLEGRO_SAMPLE *som_propulsor;
    ALLEGRO_SAMPLE_INSTANCE *inst_som_propulsor;
    som_propulsor = al_load_sample("sounds/propulsor.ogg");
    inst_som_propulsor = al_create_sample_instance(som_propulsor);
    al_attach_sample_instance_to_mixer(inst_som_propulsor,al_get_default_mixer());
    al_set_sample_instance_gain(inst_som_propulsor,1.0);

    ALLEGRO_SAMPLE *rotacao_a;
    ALLEGRO_SAMPLE_INSTANCE *inst_rotacao_a;
    rotacao_a = al_load_sample("sounds/rotacaoa.ogg");
    inst_rotacao_a = al_create_sample_instance(rotacao_a);
    al_attach_sample_instance_to_mixer(inst_rotacao_a,al_get_default_mixer());
    al_set_sample_instance_gain(inst_rotacao_a,1.4);

    ALLEGRO_SAMPLE *rotacao_h;
    ALLEGRO_SAMPLE_INSTANCE *inst_rotacao_h;
    rotacao_h = al_load_sample("sounds/rotacaoh.ogg");
    inst_rotacao_h = al_create_sample_instance(rotacao_h);
    al_attach_sample_instance_to_mixer(inst_rotacao_h,al_get_default_mixer());
    al_set_sample_instance_gain(inst_rotacao_h,0.8);

    ALLEGRO_SAMPLE *alerta;
    ALLEGRO_SAMPLE_INSTANCE *inst_alerta;
    alerta = al_load_sample("sounds/alerta.wav");
    inst_alerta = al_create_sample_instance(alerta);
    al_attach_sample_instance_to_mixer(inst_alerta,al_get_default_mixer());
    al_set_sample_instance_gain(inst_alerta,1.0);



    //CONFIGURA/INICIALIZA EVENTOS E LOCAL DE DESENHO
    al_set_target_bitmap(al_get_backbuffer(display));
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_flip_display();
    al_start_timer(timer);

    //INICIALIZA��O DA TRILHA SONORA
    al_play_sample_instance(inst_theme);
    //VARIAVEL PRA FECHAR O WHILE DO JOGO
    int fechar = 0;

     while(1){
        ALLEGRO_EVENT ev2;
        al_wait_for_event(event_queue, &ev2);

        //EVENTOS RELACIONADO AO MOUSE (MENU INICIAL E FINAL)
        if(ev2.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            break;
        }
        /*INTERA��O DO MOUSE NO MENU
        if(ev2.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            ----------------
        }
        if(ev2.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if(ev2.mouse.button & 1)
            {
                --------------------
            }
        }
        --------------------------------------------*/


        /*
        -------------------------
        -------------------------
        ---MENU INICIAL(DIEGO)---
        -------------------------
        -------------------------
        */

        while((vidascale != vida.width)&&(fechar==0)){

            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);


            //EVENTOS RELACIONADO A VIDA
            if(oxigenioscale == oxigenio.width){
                if(vidascale < vida.width){
                    vidascale += 0.9;
                }
                else{
                    vidascale = vida.width;
                }

            }

            //EVENTOS REAIS (TEMPO)
            if(ev.type == ALLEGRO_EVENT_TIMER) {

                //MOVIMENTOS NAVE
                if(ship.controle == 1){

                    if(key[KEY_UP]){

                        fire.cur_Frame+=1;
                        if(fire.cur_Frame == 12){
                            fire.cur_Frame = 0;
                        }

                        //IDEIA:
                        //https://www.efunda.com/math/fourier_series/display.cfm?name=triangle
                        //float angle = rotation*3.14159/180;
                        //float senof = 0;
                        //float cosef = 0;
                        //if(-3.14159/2 <= angle && angle <= 3

                        //    senof = 2*angle/3.14159;
                        //    cosef = 1-senof;
                        //}else{
                        //    senof = (2*(3.14159-angle))/3.14159;
                        //    cosef = 1-senof;
                        //}
                        //forceX -= senof*0.01;
                        //forceY += cosef*0.01;

                        al_play_sample_instance(inst_som_propulsor);

                        ship.forceX -= sin(ship.rotation*3.14159/180)*0.01;
                        ship.forceY += cos(ship.rotation*3.14159/180)*0.01;

                        if(ship.forceY < -2){
                            ship.forceY = -2;
                        }
                        if(ship.forceY > 2){
                            ship.forceY = 2;
                        }
                        if(ship.forceX < -2){
                            ship.forceX = -2;
                        }
                        if(ship.forceX > 2){
                            ship.forceX = 2;
                        }
                    }else{
                        al_stop_sample_instance(inst_som_propulsor);
                    }

                    if(key[KEY_LEFT]){
                        ship.rotation+=1;
                        al_play_sample_instance(inst_rotacao_h);
                    }else{
                        al_stop_sample_instance(inst_rotacao_h);
                    }
                    if(key[KEY_RIGHT]){
                        ship.rotation-=1;
                        al_play_sample_instance(inst_rotacao_a);
                    }else{
                        al_stop_sample_instance(inst_rotacao_a);
                    }

                }else{

                    //MOVIMENTOS PLAYER 1

                int flag = 0;
                if(key[KEY_RIGHT] && player1.x < ship.width - player1.width){
                    player1.dir = 1;
                    if(player1.x + VELOCITY <=255 && player1.y>=76 && player1.y <=244 || player1.y >= 140 && player1.y <= 182 || player1.x + VELOCITY >=155 && player1.x + VELOCITY <=195 ){
                        if((player1.x + VELOCITY < 141)||(player1.x + VELOCITY > 141 && player1.x + VELOCITY < 195 && player1.y < 182 && player1.y > 122)||(player1.y > 182)||(player1.y <= 122 && player1.y>90) || (player1.y <=90 && player1.x <= 200) || (player1.x + VELOCITY > 195 && player1.x + VELOCITY < 290 && player1.y >= 90))
                        {
                        if((player1.x + VELOCITY > 195 && player1.y>120 && player1.y < 142 && player1.x + VELOCITY <235)||(player1.x<=195)||(player1.x>195 && player1.y<120)||(player1.x>195 && player1.y>142)|| (player1.x>=255))
                        {
                            player1.x += VELOCITY;
                        }
                        }
                    }
                       flag = 1;
                }
                    if(key[KEY_LEFT] && player1.x > 0){
                        player1.dir = 2;
                        if(player1.x - VELOCITY >=99 && player1.y>=76 && player1.y <=244 || player1.y >= 140 && player1.y <= 182 || player1.x - VELOCITY >=155 && player1.x - VELOCITY <=195 ){
                            if((player1.x - VELOCITY > 55 && player1.x - VELOCITY < 159 && player1.y < 230)||(player1.x-VELOCITY>159 && player1.x-VELOCITY<205 && player1.y <182 && player1.y > 122)||(player1.y > 182 && player1.x > 130)||(player1.y <= 122) || (player1.x - VELOCITY > 209))
                            {
                                if((player1.x - VELOCITY < 135 && player1.y>120 && player1.y<142 && player1.x - VELOCITY > 119)||(player1.x >=135)||(player1.x <135 && player1.y<120)||(player1.x<135 && player1.y > 142)||(player1.x<=99))
                                {
                                    player1.x -= VELOCITY;
                                }
                            }
                        }
                        flag = 1;
                    }
                    if(key[KEY_UP] && player1.y > 0){
                        player1.dir = 3;
                        if(player1.y - VELOCITY >= 76 && player1.x>=99 && player1.x<=255 || player1.x >= 155 && player1.x <= 195 || player1.y - VELOCITY >= 140 && player1.y - VELOCITY <= 182 ){
                            if((player1.y - VELOCITY> 30 && player1.y - VELOCITY <= 76)||(player1.x >= 135 && player1.x <= 205 && player1.y - VELOCITY > 184)|| (player1.x > 205 && player1.y > 90 )||(player1.y<=90 && player1.y>=76 && player1.x<205)||(player1.x <= 140)||(player1.y-VELOCITY <= 155 && player1.y-VELOCITY>=76 && player1.x >=135 && player1.x<=205))
                            {
                                if((player1.x>50 && player1.y - VELOCITY>142)||(player1.y - VELOCITY <=142 && player1.x>110 && player1.x<242)||(player1.y-VELOCITY<=120))
                                {
                                    player1.y -= VELOCITY;
                                }
                            }
                        }
                        flag = 1;
                    }
                    if(key[KEY_DOWN] && player1.y < ship.height - player1.height){
                        player1.dir = 4;
                        if(player1.y + VELOCITY <= 244 && player1.x>=99 && player1.x<=255 || player1.x >= 155 && player1.x <= 195 || player1.y + VELOCITY >= 140 && player1.y + VELOCITY <= 182 ){
                            if((player1.y + VELOCITY>= 244 && player1.y + VELOCITY < 295)||(player1.x >= 135 && player1.x <= 205 && player1.y + VELOCITY < 150)|| (player1.x > 205 )||(player1.x <= 140 && player1.y + VELOCITY < 230)||(player1.y + VELOCITY>= 230 && player1.x>115 && player1.x <=135)||(player1.x >= 135 && player1.x<=205 && player1.y + VELOCITY < 295 && player1.y>180))
                            {
                                if((player1.x>50 && player1.y + VELOCITY< 120)||(player1.y + VELOCITY >= 120 && player1.x>110 && player1.x<140 )||(player1.y + VELOCITY >= 120 && player1.x>159 && player1.x<195)||(player1.y + VELOCITY >= 120 && player1.x>209 && player1.x<242)||(player1.y+VELOCITY >= 142))
                                    {
                                    player1.y += VELOCITY;
                                    }
                            }
                        }
                        flag = 1;
                    }

                    if(player1.dir == 1){
                        if(player1.cur_Frame < 0 || player1.cur_Frame > (7*DELAY)-2){
                            player1.cur_Frame = 0;
                        }else if(flag){
                            player1.cur_Frame++;
                        }else{
                            player1.cur_Frame = 0;
                        }
                    }else if(player1.dir == 2){
                        if(player1.cur_Frame < 7*DELAY || player1.cur_Frame > (14*DELAY)-2){
                            player1.cur_Frame = 7*DELAY;
                        }else if(flag){
                            player1.cur_Frame++;
                        }else{
                            player1.cur_Frame = 7*DELAY;
                        }
                    }else if(player1.dir == 3){
                        if(player1.cur_Frame < 14*DELAY || player1.cur_Frame > (18*DELAY)-2){
                            player1.cur_Frame = 14*DELAY;
                        }else if(flag){
                            player1.cur_Frame++;
                        }else{
                            player1.cur_Frame = 15*DELAY;
                        }
                    }else if(player1.dir == 4){
                        if(player1.cur_Frame < 18*DELAY || player1.cur_Frame > (22*DELAY)-2){
                            player1.cur_Frame = 18*DELAY;
                        }else if(flag){
                            player1.cur_Frame++;
                        }else{
                            player1.cur_Frame = 19*DELAY;
                        }
                    }
                    al_play_sample_instance(inst_theme);
                }

                            //COLIS�O ============================================================================================================================================================

                /*
                for(int i = 0; i < QUANT_ASTEROIDS; i++)
               {
                    int colisao = VerifyColisionShipAsteroid(&ship, &asteroids[i]);
                    if(colisao)
                    {
                        if(asteroids[i].vida > 0)
                        {
                            ColisaoShipAsteroid(&ship, &asteroids[i]);
                        }
                    }
                }

                for(int i = 0; i < QUANT_ASTEROIDS; i++)
                    {
                    for(int x = 0; x < QUANT_ASTEROIDS; x++)
                    {
                        if(asteroids[i].vida > 0)
                        {
                            if(i != x && asteroids[x].vida > 0)
                            {
                                int colidiu = VerifyColisionInterAsteroids(&asteroids[i], &asteroids[x]);
                                if(colidiu){
                                    realizar_Colisao_ASTEROIDS(&asteroids[i], &asteroids[x]);
                                }
                            }
                        }else{
                            break;
                        }
                    }
                }
    */

                // FIM COLIS�O =========================================================================================================================================================


                //EVENTOS RELACIONADO AO OXIGENIO

                if(oxigenioscale < oxigenio.width){
                    oxigenioscale += 0.9;
                }
                else{
                    oxigenioscale = oxigenio.width;
                }
                if(ship.repondoOxi == 1){
                    if (oxigenioscale <= oxigenio.width && oxigenioscale>0){
                        oxigenioscale -= 1;
                    }else{
                        oxigenioscale = 0;
                    }

                }else{

                    //MOVIMENTOS PLAYER2

                    int flag = 0;
                    if(key[KEY_D] && player2.x < ship.width - player2.width){
                        player2.dir = 1;
                        if(player2.x + VELOCITY <=255 && player2.y>=76 && player2.y <=244 || player2.y >= 140 && player2.y <= 182 || player2.x + VELOCITY >=155 && player2.x + VELOCITY <=195 ){
                          if((player2.x + VELOCITY < 141)||(player2.x + VELOCITY > 141 && player2.x + VELOCITY < 195 && player2.y < 182 && player2.y > 122)||(player2.y > 182)||(player2.y <= 122 && player2.y>90) || (player2.y <=90 && player2.x <= 200) || (player2.x + VELOCITY > 195 && player2.x + VELOCITY < 290 && player2.y >= 90))
                          {
                            if((player2.x + VELOCITY > 195 && player2.y>120 && player2.y < 142 && player2.x + VELOCITY <235)||(player2.x<=195)||(player2.x>195 && player2.y<120)||(player2.x>195 && player2.y>142)|| (player2.x>=255))
                            {
                               player2.x += VELOCITY;
                            }
                          }
                        }
                        flag = 1;
                    }
                    if(key[KEY_A] && player2.x > 0){
                        player2.dir = 2;
                        if(player2.x - VELOCITY >=99 && player2.y>=76 && player2.y <=244 || player2.y >= 140 && player2.y <= 182 || player2.x - VELOCITY >=155 && player2.x - VELOCITY <=195 ){
                            if((player2.x - VELOCITY > 55 && player2.x - VELOCITY < 159 && player2.y < 230)||(player2.x-VELOCITY>159 && player2.x-VELOCITY<205 && player2.y <182 && player2.y > 122)||(player2.y > 182 && player2.x > 130)||(player2.y <= 122) || (player2.x - VELOCITY > 209))
                            {
                                if((player2.x - VELOCITY < 135 && player2.y>120 && player2.y<142 && player2.x - VELOCITY > 119)||(player2.x >=135)||(player2.x <135 && player2.y<120)||(player2.x<135 && player2.y > 142)||(player2.x<=99))
                                {
                                    player2.x -= VELOCITY;
                                }
                            }
                        }
                        flag = 1;
                    }
                    if(key[KEY_W] && player2.y > 0){
                        player2.dir = 3;
                        if(player2.y - VELOCITY >= 76 && player2.x>=99 && player2.x<=255 || player2.x >= 155 && player2.x <= 195 || player2.y - VELOCITY >= 140 && player2.y - VELOCITY <= 182 ){
                            if((player2.y - VELOCITY> 30 && player2.y - VELOCITY <= 76)||(player2.x >= 135 && player2.x <= 205 && player2.y - VELOCITY > 184)|| (player2.x > 205 && player2.y > 90 )||(player2.y<=90 && player2.y>=76 && player2.x<205)||(player2.x <= 140)||(player2.y-VELOCITY <= 155 && player2.y-VELOCITY>=76 && player2.x >=135 && player2.x<=205))
                            {
                                if((player2.x>50 && player2.y - VELOCITY>142)||(player2.y - VELOCITY <=142 && player2.x>110 && player2.x<242)||(player2.y-VELOCITY<=120))
                                {
                                    player2.y -= VELOCITY;
                                }
                            }
                        }
                        flag = 1;
                    }
                    if(key[KEY_S] && player2.y < ship.height - player2.height){
                        player2.dir = 4;
                        if(player2.y + VELOCITY <= 244 && player2.x>=99 && player2.x<=255 || player2.x >= 155 && player2.x <= 195 || player2.y + VELOCITY >= 140 && player2.y + VELOCITY <= 182 ){
                            if((player2.y + VELOCITY>= 244 && player2.y + VELOCITY < 295)||(player2.x >= 135 && player2.x <= 205 && player2.y + VELOCITY < 150)|| (player2.x > 205 )||(player2.x <= 140 && player2.y + VELOCITY < 230)||(player2.y + VELOCITY>= 230 && player2.x>115 && player2.x <=135)||(player2.x >= 135 && player2.x<=205 && player2.y + VELOCITY < 295 && player2.y>180))
                            {
                                if((player2.x>50 && player2.y + VELOCITY< 120)||(player2.y + VELOCITY >= 120 && player2.x>110 && player2.x<140 )||(player2.y + VELOCITY >= 120 && player2.x>159 && player2.x<195)||(player2.y + VELOCITY >= 120 && player2.x>209 && player2.x<242)||(player2.y+VELOCITY >= 142))
                                    {
                                    player2.y += VELOCITY;
                                    }
                            }
                        }
                        flag = 1;
                    }

                    if(player2.dir == 1){
                        if(player2.cur_Frame < 0 || player2.cur_Frame > (7*DELAY)-2){
                            player2.cur_Frame = 0;
                        }else if(flag){
                            player2.cur_Frame++;
                        }else{
                            player2.cur_Frame = 0;
                        }
                    }else if(player2.dir == 2){
                        if(player2.cur_Frame < 7*DELAY || player2.cur_Frame > (14*DELAY)-2){
                            player2.cur_Frame = 7*DELAY;
                        }else if(flag){
                            player2.cur_Frame++;
                        }else{
                            player2.cur_Frame = 7*DELAY;
                        }
                    }else if(player2.dir == 3){
                        if(player2.cur_Frame < 14*DELAY || player2.cur_Frame > (18*DELAY)-2){
                            player2.cur_Frame = 14*DELAY;
                        }else if(flag){
                            player2.cur_Frame++;
                        }else{
                            player2.cur_Frame = 15*DELAY;
                        }
                    }else if(player2.dir == 4){
                        if(player2.cur_Frame < 18*DELAY || player2.cur_Frame > (22*DELAY)-2){
                            player2.cur_Frame = 18*DELAY;
                        }else if(flag){
                            player2.cur_Frame++;
                        }else{
                            player2.cur_Frame = 19*DELAY;
                        }
                    }
                }

                //FAZ COM QUE A NAVE FREIE AOS POUCOS (PENSAR EM TIRAR)
                if(ship.forceY < 0){
                        ship.forceY += 0.002;
                        if(ship.forceY>0){
                            ship.forceY=0;
                        }
                }else if(ship.forceY > 0){
                        ship.forceY -= 0.002;
                        if(ship.forceY < 0){
                            ship.forceY = 0;
                        }
                }
                if(ship.forceX < 0){
                        ship.forceX += 0.002;
                        if(ship.forceX>0){
                            ship.forceX=0;
                        }
                }else if(ship.forceX > 0){
                        ship.forceX -= 0.002;
                        if(ship.forceX < 0){
                            ship.forceX = 0;
                        }
                }

                //LIMITA O VALOR DA ROTA��O NO INTERVALO [0,359]
                if(ship.rotation >= 360){
                    ship.rotation = 0;
                }else if(ship.rotation < 0){
                    ship.rotation = 359;
                }



                //APLICA AS FOR�AS NA NAVE
                ship.y += ship.forceY;
                ship.x += ship.forceX;

                //IMPEDE QUE A NAVE SAIA DOS LIMITES DO MUNDO
                if(ship.y < 0){
                    ship.y = 0;
                    ship.forceY=0;
                }
                if(ship.y > WORLD_H - ship.height){
                    ship.y = WORLD_H - ship.height;
                    ship.forceY=0;
                }
                if(ship.x < 0){
                    ship.x = 0;
                    ship.forceX=0;
                }
                if(ship.x > WORLD_W - ship.width){
                    ship.x = WORLD_W - ship.width;
                    ship.forceX=0;
                }

                //ATUALIZA A C�MERA AO OBJETO EM MOVIMENTO
                cameraX = ship.x - SCREEN_W/2 +ship.width/2;
                cameraY = ship.y - SCREEN_H/2 +ship.height/2;

                //LIMITA A CAMERA NAS BORDAS DO MAPA (PENSAR UM LIMITE DE ASTEROIDS E N�O DA C�MERA)
                if(cameraX < 0){
                    cameraX = 0;
                }
                if (cameraY < 0){
                    cameraY = 0;
                }
                if (cameraX > WORLD_W - SCREEN_W){
                    cameraX = WORLD_W - SCREEN_W;
                }
                if (cameraY > WORLD_H - SCREEN_H){
                    cameraY = WORLD_H - SCREEN_H;
                }

                redraw = true;

            //ASTEROIDS---------------------------------------------------------------------------------------------------------{
            //for(int i=0; i<QUANT_ASTEROIDS; i++){
             //   if(asteroids[i].vida > 0){
             //       limitarAsteroideMundo(&asteroids[i].x, &asteroids[i].y, &asteroids[i].forceX, &asteroids[i].forceY, asteroids[i].width, asteroids[i].height);
              //  }
           // }
            //ASTEROIDS---------------------------------------------------------------------------------------------------------}

            aplicarForcas(&ship.x, &ship.y, ship.forceX, ship.forceY);
            //ASTEROIDS---------------------------------------------------------------------------------------------------------{
            for(int i=0; i<QUANT_ASTEROIDS; i++){
                 if(asteroids[i].vida > 0){
                    aplicarForcas(&asteroids[i].x, &asteroids[i].y, asteroids[i].forceX, asteroids[i].forceY);
                 }
            }
            //ASTEROIDS---------------------------------------------------------------------------------------------------------}

            adaptarCamera(&cameraX, &cameraY, ship.x, ship.y, ship.width, ship.height, SCREEN_W, SCREEN_H);
            redraw = true;

            //ATIVA AS TECLAS PRESSIONADAS
            }else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(ev.keyboard.keycode) {
                    //PLAYER1
                    case ALLEGRO_KEY_UP:
                        key[KEY_UP] = true;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        key[KEY_DOWN] = true;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        key[KEY_LEFT] = true;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        key[KEY_RIGHT] = true;
                        break;
                    case ALLEGRO_KEY_L:
                        if(ship.controle == 1){
                            ship.controle = 0;
                        }else{
                            ship.controle = 1;
                        }
                        break;
                    //PLAYER2
                    case ALLEGRO_KEY_W:
                        key[KEY_W] = true;
                        break;
                    case ALLEGRO_KEY_S:
                        key[KEY_S] = true;
                        break;
                    case ALLEGRO_KEY_A:
                        key[KEY_A] = true;
                        break;
                    case ALLEGRO_KEY_D:
                        key[KEY_D] = true;
                        break;
                    case ALLEGRO_KEY_E:
                        if(ship.repondoOxi == 1){
                            ship.repondoOxi = 0;
                        }else{
                            ship.repondoOxi = 1;
                        }
                        break;
                }

            //DESATIVA AS TECLAS PRESSIONADASs
            }else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch(ev.keyboard.keycode) {
                    //PLAYER1
                    case ALLEGRO_KEY_UP:
                        key[KEY_UP] = false;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        key[KEY_DOWN] = false;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        key[KEY_LEFT] = false;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        key[KEY_RIGHT] = false;
                        break;
                    //PLAYER2
                    case ALLEGRO_KEY_W:
                        key[KEY_W] = false;
                        break;
                    case ALLEGRO_KEY_S:
                        key[KEY_S] = false;
                        break;
                    case ALLEGRO_KEY_A:
                        key[KEY_A] = false;
                        break;
                    case ALLEGRO_KEY_D:
                        key[KEY_D] = false;
                        break;

                }
            }else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                fechar = 1;
                break;
            }

            //DESENHOS
        if(redraw && al_is_event_queue_empty(event_queue)) {

            redraw = false;
            al_draw_scaled_bitmap(background.img,0,0,background.width,background.height,0-(cameraX/2.5),0-(cameraY/2.5),background.width*BGScale, background.height*BGScale, 0);

            if((key[KEY_UP] && ship.controle==1) || (key[KEY_W] && ship.controle==2) ){
                al_draw_rotated_bitmap(fire.imgs[fire.cur_Frame],25, -200,ship.x-cameraX+ship.width/2, ship.y-cameraY+ship.height/2,((ship.rotation+180)*3.14159/180),0);
            }

            al_draw_rotated_bitmap(propulsor.img,propulsor.height/2, propulsor.height/2,ship.x-cameraX+ship.width/2, ship.y-cameraY+ship.height/2,((ship.rotation-90)*3.14159/180),0);
            al_draw_scaled_bitmap(ship.img,0,0,ship.width,ship.height,ship.x-cameraX,ship.y-cameraY,ship.width, ship.height, 0);

            //al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX+ship.width/2+2, ship.y-cameraY-6,(gunUP.angle*3.14159/180), 0);
            //al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX+ship.width+10, ship.y-cameraY+ship.height/2-4,(gunRIGHT.angle*3.14159/180), 0);
            //al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX-6, ship.y-cameraY+ship.height/2-3,(gunLEFT.angle*3.14159/180), 0);
            //al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX+ship.width/2+2, ship.y-cameraY+ship.height+7,(gunDOWN.angle*3.14159/180), 0);

            //al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX+ship.width/2-12, ship.y-cameraY-10,0, 0);
            //al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX+ship.width+15, ship.y-cameraY+ship.height/2-17,(90*3.14159/180), 0);
            //al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX-12, ship.y-cameraY+ship.height/2+12,(-90*3.14159/180), 0);
            //al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX+ship.width/2+16, ship.y-cameraY+ship.height+12,180*3.14159/180, 0);

            if(player1.y >= player2.y){
                al_draw_scaled_bitmap(player2.imgs[player2.cur_Frame/DELAY],0,0,player2.width,player2.height,player2.x+ship.x-cameraX,player2.y+ship.y-cameraY,player2.width, player2.height, 0);
                al_draw_scaled_bitmap(player1.imgs[player1.cur_Frame/DELAY],0,0,player1.width,player1.height,player1.x+ship.x-cameraX,player1.y+ship.y-cameraY,player1.width, player1.height, 0);
            }
            else{
                al_draw_scaled_bitmap(player1.imgs[player1.cur_Frame/DELAY],0,0,player1.width,player1.height,player1.x+ship.x-cameraX,player1.y+ship.y-cameraY,player1.width, player1.height, 0);
                al_draw_scaled_bitmap(player2.imgs[player2.cur_Frame/DELAY],0,0,player2.width,player2.height,player2.x+ship.x-cameraX,player2.y+ship.y-cameraY,player2.width, player2.height, 0);
            }

            for(int i=0; i<QUANT_ASTEROIDS; i++){
                if(asteroids[i].vida > 0){
                    al_draw_scaled_bitmap(asteroids[i].img,0,0,asteroids[i].width,asteroids[i].height,asteroids[i].x-cameraX,asteroids[i].y-cameraY,asteroids[i].width, asteroids[i].height, 0);
                }
            }


        }
     }
        /*
        ------------------------------------
        ---------MENU ENCECRRAMENTO---------
        ---------------WEI------------------
        ------------------------------------
        */

        al_stop_sample_instance(inst_theme);
        al_stop_sample_instance(inst_tiro);
        al_stop_sample_instance(inst_explosao1);
        al_stop_sample_instance(inst_explosao2);
        al_stop_sample_instance(inst_explosao3);
        al_stop_sample_instance(inst_som_propulsor);
        al_stop_sample_instance(inst_rotacao_a);
        al_stop_sample_instance(inst_rotacao_h);
        al_stop_sample_instance(inst_alerta);
        al_flip_display();
    }

    //DESTROI TUDO DA MEMORIA
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_bitmap(background.img);
    al_destroy_bitmap(propulsor.img);
    al_destroy_bitmap(ship.img);
    al_destroy_bitmap(barra.img);
    al_destroy_bitmap(vida.img);
    al_destroy_bitmap(oxigenio.img);
    //al_destroy_bitmap(canhao.img);
    //al_destroy_bitmap(canhaoBase.img);

    al_destroy_sample(theme);
    al_destroy_sample(tiro);
    al_destroy_sample(explosao1);
    al_destroy_sample(explosao2);
    al_destroy_sample(explosao3);
    al_destroy_sample(som_propulsor);
    al_destroy_sample(rotacao_a);
    al_destroy_sample(rotacao_h);
    al_destroy_sample(alerta);
    al_destroy_sample_instance(inst_theme);
    al_destroy_sample_instance(inst_tiro);
    al_destroy_sample_instance(inst_explosao1);
    al_destroy_sample_instance(inst_explosao2);
    al_destroy_sample_instance(inst_explosao3);
    al_destroy_sample_instance(inst_som_propulsor);
    al_destroy_sample_instance(inst_rotacao_a);
    al_destroy_sample_instance(inst_rotacao_h);
    al_destroy_sample_instance(inst_alerta);

    for(int i=0; i<12; i++){
        al_destroy_bitmap(fire.imgs[i]);
    }
    for(int i=0; i<22; i++){
        al_destroy_bitmap(player1.imgs[i]);
    }
    for(int i=0; i<22; i++){
        al_destroy_bitmap(player2.imgs[i]);
    }
    al_destroy_event_queue(event_queue);
    for(int i=0; i<QUANT_ASTEROIDS; i++){
        al_destroy_bitmap(asteroids[i].img);
    }


    return 0;
}
