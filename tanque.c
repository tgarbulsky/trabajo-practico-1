#include "tanque.h"

tanque_t* crear_tanque(float x, float y, float angz){
    tanque_t* tanque=malloc(sizeof(tanque_t));
    if(tanque==NULL){
        return NULL;
    }    
    tanque->pos[0]=x;
    tanque->pos[1]=y;
    tanque->pos[2]=0;
    tanque->angz=angz;
    tanque->ang_radar=0;
    tanque->ang_torreta=0;
    return tanque;
}

tanque_t* crear_tanque_enemigo(float x_fp, float y_fp, lista_t* obstaculos){
    float aux_ang=random_float(0, 2*M_PI);
    float x=x_fp+50*cos(aux_ang);
    float y=y_fp+50*sin(aux_ang);
    float pos[3]={x, y, 0};
    while(colisiones(obstaculos, NULL, pos, 5)){
        float aux_ang=random_float(0, 2*M_PI);
        float x=x_fp+50*cos(aux_ang);
        float y=y_fp+50*sin(aux_ang);
        pos[0]=x;
        pos[1]=y;
    }
    float angz=random_float(-M_PI, M_PI);
    return crear_tanque(x, y, angz);
}

void tanque_mover(tanque_t* tanque, enum tras tras, lista_t* obstaculos, tanque_t* otro_tanque){
    int signo_mov=0;
    if(tras == TRAS_FWD){
        signo_mov = 1;
    }else if(tras == TRAS_BWD){
        signo_mov=-1;
    }
    float new_x=tanque->pos[0]+signo_mov*cos(tanque->angz)*V_TANQUE*1.0/JUEGO_FPS;
    float new_y=tanque->pos[1]+signo_mov*sin(tanque->angz)*V_TANQUE*1.0/JUEGO_FPS;
    float new_pos[3]={new_x, new_y, 0};
    if(!colisiones(obstaculos, otro_tanque->pos, new_pos, 5)){
        tanque->pos[0] = new_x;
        tanque->pos[1] = new_y;
    }
}

void tanque_rotar(tanque_t* tanque, enum rot rot){
    int signo_rot=0;
    if(rot==ROT_CW){
        signo_rot=-1;
    }else if(rot==ROT_CCW){
        signo_rot=1;
    }
    tanque->angz+=signo_rot*WZ_TANQUE*1.0/JUEGO_FPS;
    if(tanque->angz<-M_PI){
        tanque->angz+=2*M_PI;
    }
    else if(tanque->angz>M_PI){
        tanque->angz-=2*M_PI;
    }
}

void tanque_radar(tanque_t* tanque){
    tanque->ang_radar+=WZ_RADAR*1.0/JUEGO_FPS;
    if(tanque->ang_radar>M_PI){
        (tanque->ang_radar)-=2*M_PI;
    }
    else if(tanque->ang_radar<-M_PI){
        (tanque->ang_radar)+=2*M_PI;
    }
}

void tanque_rotar_torreta(tanque_t* tanque, enum turr turr){
    float w=0;
    int signo_mov=0;
    if(turr==TURR_ON_CW || turr==TURR_ON_CCW){
        w=WZ_TORRETA_ON;
        if(turr==TURR_ON_CW){
            signo_mov=-1;
        }else if(turr==TURR_ON_CCW){
            signo_mov=1;
        }
    }else if(turr==TURR_OFF){
        w=WZ_TORRETA_OFF;
        if(tanque->ang_torreta<0){
            signo_mov=1;
        }else if(tanque->ang_torreta>0){
            signo_mov=-1;
        }
    }
    tanque->ang_torreta+=signo_mov*w*1.0/JUEGO_FPS;
    if(tanque->ang_torreta<-M_PI){
        tanque->ang_torreta+=2*M_PI;
    }else if(tanque->ang_torreta>M_PI){
        tanque->ang_torreta-=2*M_PI;
    }
}

bool _en_rango_vision(tanque_t* pov, tanque_t* obj, float low, float high){
    if(obj==NULL){
        return false;
    }
    float dy=obj->pos[1]-pov->pos[1];
    float dx=obj->pos[0]-pov->pos[0];
    double ang=atan2(dy, dx);
    float pov_ang=pov->angz;
    if(ang<0){
        ang+=2*M_PI;
    }
    if(pov_ang<0){
        pov_ang+=2*M_PI;
    }
    float dang=ang-pov_ang;
    while(dang<-M_PI){
        dang+=2*M_PI;
    }
    while(dang>M_PI){
        dang-=2*M_PI;
    }
    if(low<dang && high>dang){
        return true;
    }
    return false;
}

bool en_rango_vision_sim(tanque_t* pov, tanque_t* obj, float dang){
    return _en_rango_vision(pov, obj, -dang, dang);
}

bool en_rango_vision_ia(tanque_t* pov, tanque_t* obj, float dang, enum turr* turr){
    if(en_rango_vision_sim(pov, obj, dang)){
        if(en_rango_vision_turr(pov, obj, 0, M_PI)){
            *turr=TURR_ON_CCW;
        }else if(en_rango_vision_turr(pov, obj, -M_PI, 0)){
            *turr=TURR_ON_CW;
        }
        return true;
    }
    *turr=TURR_OFF;
    return false;
}

bool en_rango_vision_turr(tanque_t* pov, tanque_t* obj, float low, float high){
    return _en_rango_vision(pov, obj, low+pov->ang_torreta, high+pov->ang_torreta);
}

void decidir_accion_ia(enum acciones_ia* accion, int* t_accion_ia, tanque_t* ia, tanque_t* fp){
    if(*accion!=IA_NONE){ 
        return;
    }
    int p=random_int(0, JUEGO_FPS);
    if(p==1){ 
        float p2=random_entre01();
        if(p2>0.5){ 
            if(_en_rango_vision(ia, fp, 0, M_PI)){
                *accion=IA_CCW;
            }else{
                *accion=IA_CW;
            }
            *t_accion_ia=random_int(0, 3*JUEGO_FPS);
        }else{ 
            *t_accion_ia=random_int(-1*JUEGO_FPS, 3*JUEGO_FPS);
            if(*t_accion_ia<0){
                *accion=IA_BWD;
                *t_accion_ia*=-1;
            }else{
                *accion=IA_FWD;
            }
        }
    }
}

void ia_acciones(tanque_t* ia, enum acciones_ia accion, lista_t* obstaculos, tanque_t* otro_tanque){
    if(accion==IA_NONE){ 
        return;
    }
    if(accion==IA_FWD){
        tanque_mover(ia, TRAS_FWD, obstaculos, otro_tanque);
    }else if(accion==IA_BWD){
        tanque_mover(ia, TRAS_BWD, obstaculos, otro_tanque);
    }else if(accion==IA_CW){
        tanque_rotar(ia, ROT_CW);
    }else if(accion==IA_CCW){
        tanque_rotar(ia, ROT_CCW);
    }
}
