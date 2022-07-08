#include<math.h>//errythang
#include<stdio.h>//printf,scanf
#include<stdlib.h>
#include<time.h>

#define DOT4D(a,b) a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3]; //macro dot product; unused

#define TAU 6.28318531 //2*pi
#define DIMENSIONS 4 //the length of spacial vectors
#define SCATTERD 1.5 //maximum distance the scattergun can fire
#define SCATTERQ 30 //number of shots fired by the scatergun
#define SCATTERR 0.3 //radius of scattergun explosions
#define RIFLER 0.2 //radius of the rifle beam
#define SENSORR 0.5 //radius of close range sensors
#define LOOKR 0.6 //radius of lookat hypersphere
#define LOOKD 7 //maximum range of lookat
#define WHITESPACE " \t\n\r," //used for parsing

double ZEROS[DIMENSIONS] = {0,0,0,0}; //your starting point
double I[DIMENSIONS] = {1,0,0,0};
double J[DIMENSIONS] = {0,1,0,0};
double K[DIMENSIONS] = {0,0,1,0};
double L[DIMENSIONS] = {0,0,0,1}; //four linearly independent unit vectors
double NINES[DIMENSIONS] = {9,9,9,9}; //opponent's starting point

char control = 0; //see function switchctrl

typedef struct
{
 char id; //ships position in global ship array
 double loc[DIMENSIONS]; //current location
 double vel[DIMENSIONS]; //current velocity
 double acc; //magnitude of acceleration
 double reli[DIMENSIONS];
 double relj[DIMENSIONS];
 double relk[DIMENSIONS];
 double rell[DIMENSIONS]; //ship's heading
 double playerv[DIMENSIONS]; //vector from player to this ship
 double playerd; //distance from player
 char destroyed; //1 = kablooie, 0 = doin' fine
} ship;

//playerv and playerd are now depricated; use sub4d and mag4d;

void printvec(double* a,char dims) //prints in the form a,,b,,c,,d,,
{
 char i;
 for(i=0;i<dims;i++)
  printf("%6.3f,,",a[i]);
 printf("\n");
}

void copy4d(double* a,double* b) //copy vector sets b = a
{
 b[0] = a[0];
 b[1] = a[1];
 b[2] = a[2];
 b[3] = a[3];
}

void add4d(double* a,double* b,double* c) //add a to b store in c
{
 c[0] = a[0] + b[0];
 c[1] = a[1] + b[1];
 c[2] = a[2] + b[2];
 c[3] = a[3] + b[3];
}

void sub4d(double* a,double* b,double* c) //subtract b from a store in c
{
 c[0] = a[0] - b[0];
 c[1] = a[1] - b[1];
 c[2] = a[2] - b[2];
 c[3] = a[3] - b[3];
}

void scalemult4d(double* a,double b,double* c) //multibly a by b store in c
{
 c[0] = a[0]*b;
 c[1] = a[1]*b;
 c[2] = a[2]*b;
 c[3] = a[3]*b;
}

void mag4d(double* a,double* b) //find the magnitude of a, store in b
{
 *b = sqrt(pow(a[0],2) + pow(a[1],2) + pow(a[2],2) + pow(a[3],2));
}

void normalize4d(double* a,double* b) //stores in b a unit vector in the direction of a
{
 double mag = sqrt(pow(a[0],2) + pow(a[1],2) + pow(a[2],2) + pow(a[3],2));
 b[0] = a[0]/mag;
 b[1] = a[1]/mag;
 b[2] = a[2]/mag;
 b[3] = a[3]/mag;
}

void dist4d(double* a,double* b,double* c) //soter the magnitude of a-b in c
{
 double alfa[4];

 alfa[0] = a[0]-b[0];
 alfa[1] = a[1]-b[1];
 alfa[2] = a[2]-b[2];
 alfa[3] = a[3]-b[3];

 *c = sqrt(pow(alfa[0],2)+pow(alfa[1],2)+pow(alfa[2],2)+pow(alfa[3],2));
}

void cross4d(double* a,double* b,double* c,double* d) //currently unused
{
 d[0] = a[1]*b[2]*c[3] - a[3]*b[2]*c[1];
 d[1] = a[2]*b[3]*c[0] - a[0]*b[3]*c[2];
 d[2] = a[3]*b[0]*c[1] - a[1]*b[0]*c[3];
 d[3] = a[0]*b[1]*c[2] - a[2]*b[1]*c[0];
}

void det3(double* a,double* b,double* c,double* d) //stores the determinate of a matrix defined by a, b, and c in d
{
 double alfa,beta,gama;
 alfa = a[0]*b[1]*c[2]-a[0]*b[2]*c[1];
 beta = b[0]*a[1]*c[2]-b[0]*a[2]*c[1];
 beta = -beta;
 gama = c[0]*a[1]*b[2]-c[0]*b[1]*a[2];
 *d = alfa + beta + gama;
}

void perp4d(double* a,double* b,double* c,double* d) //given 3 4-vectors, finds a vector perpendicular to all of them
{
 int count;
 double num,den[4];
 double alfa[3], beta[3], gama[3], delt[3], epsi[3];

 alfa[0] = a[0];
 alfa[1] = b[0];
 alfa[2] = c[0];
 beta[0] = a[1];
 beta[1] = b[1];
 beta[2] = c[1];
 gama[0] = a[2];
 gama[1] = b[2];
 gama[2] = c[2];
 delt[0] = a[3];
 delt[1] = b[3];
 delt[2] = c[3];

 det3(beta,gama,delt,&d[0]);
 det3(alfa,gama,delt,&d[1]);
 det3(alfa,beta,delt,&d[2]);
 det3(alfa,beta,gama,&d[3]);
 d[1]=-d[1];
 d[3]=-d[3];
}

void dot4d(double* a,double* b,double *c) //a (dot) b store in c
{
 *c = a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

char hit(double* unitv,double* target,double rad) //determines whether the rifle hits
{
 double dist, err, cosof, utar[4];
 mag4d(target,&dist); // = target->playerv
 normalize4d(target,utar);
 dot4d(utar,unitv,&cosof);
 err = acos(cosof)*dist;
 if(err<rad)
  return 1;
 else
  return 0;
}

void fourpto4c(double* a,double* b) //converts polar to cartesian
{
 b[3] = a[0]*sin(a[3]);
 b[2] = a[0]*cos(a[3])*sin(a[2]);
 b[1] = a[0]*cos(a[3])*cos(a[2])*sin(a[1]);
 b[0] = a[0]*cos(a[3])*cos(a[2])*cos(a[1]);
}

void printvagu(ship* ship) //prints incomplete info about ship
{
 double alfa;
 printf(" In the vecinity of %d,,%d,,%d,,%d, ",(int)floor(ship->loc[0]),(int)floor(ship->loc[1]),(int)floor(ship->loc[2]),(int)floor(ship->loc[3]));
 mag4d(ship->vel,&alfa);
 printf("and traveling slower than %d\n\n",(int)ceil(alfa));
}

void printshipinfo(ship* ship) //prints complete info about ship
{
 printf(" Located at: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ship->loc[0],ship->loc[1],ship->loc[2],ship->loc[3]);

 printf(" Traveling:  %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ship->vel[0],ship->vel[1],ship->vel[2],ship->vel[3]);

 printf(" Relative i: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ship->reli[0],ship->reli[1],ship->reli[2],ship->reli[3]);
 printf(" Relative j: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ship->relj[0],ship->relj[1],ship->relj[2],ship->relj[3]);
 printf(" Relative k: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ship->relk[0],ship->relk[1],ship->relk[2],ship->relk[3]);
 printf(" Relative l: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ship->rell[0],ship->rell[1],ship->rell[2],ship->rell[3]);
 printf(" Thrust:     %6.3f\n\n",ship->acc);
}

void getthrust(ship* ship) //gets user input stores in ship.acc
{
 float get;
 printf("Input floating point acceleration magnitude:\n");
 scanf("%f",&get);
 if(get > 1 || get < -1.5)
 {
  printf("The engines are made out of tin foil and krazy glue. You can't ask *THAT* outta them.\n\n");
  ship->acc = 0;
 }
 else
 {
  ship->acc = (double)get;
  printf("\n");
 }
}

void getangs(ship* ship) //used to reorient ship
{
 float thf,phf,psf;
 double th,ph,ps;
 double i[DIMENSIONS],j[DIMENSIONS],k[DIMENSIONS],l[DIMENSIONS],alfa[DIMENSIONS],beta[DIMENSIONS],gama[DIMENSIONS];

 printf("Input floating point angle adjustments in the form th,ph,ps\n");
 scanf("%f,%f,%f",&thf,&phf,&psf);
 printf("\n");
 if(!control)
 {th = (double)thf; ph = (double)phf; ps = (double)psf;}
 else if(control==1)
 {th = (double)thf*M_PI; ph = (double)phf*M_PI; ps = (double)psf*M_PI;}
 else if(control==2)
 {th = (double)thf*TAU; ph = (double)phf*TAU; ps = (double)psf*TAU;}

 copy4d(ship->reli,i);
 copy4d(ship->relj,j);
 copy4d(ship->relk,k);
 copy4d(ship->rell,l);


 scalemult4d(i,cos(th)-1,alfa);
 add4d(i,alfa,beta);
 scalemult4d(j,sin(th),alfa);
 add4d(beta,alfa,gama);
 normalize4d(gama,ship->reli);

 scalemult4d(j,cos(th)-1,alfa);
 add4d(j,alfa,beta);
 scalemult4d(i,-sin(th),alfa);
 add4d(beta,alfa,gama);
 normalize4d(gama,ship->relj);

 copy4d(ship->reli,i);

 scalemult4d(i,cos(ph)-1,alfa);
 add4d(i,alfa,beta);
 scalemult4d(k,sin(ph),alfa);
 add4d(beta,alfa,gama);
 normalize4d(gama,ship->reli);

 scalemult4d(k,cos(ph)-1,alfa);
 add4d(k,alfa,beta);
 scalemult4d(i,-sin(ph),alfa);
 add4d(beta,alfa,gama);
 normalize4d(gama,ship->relk);

 copy4d(ship->reli,i);


 scalemult4d(i,cos(ps)-1,alfa);
 add4d(i,alfa,beta);
 scalemult4d(l,sin(ps),alfa);
 add4d(beta,alfa,gama);
 normalize4d(gama,ship->reli);

 scalemult4d(l,cos(ps)-1,alfa);
 add4d(l,alfa,beta);
 scalemult4d(i,-sin(ps),alfa);
 add4d(beta,alfa,gama);
 normalize4d(gama,ship->rell);
}

void slambreaks(ship* ship)
{
 copy4d(ship->vel,ZEROS);
 ship->acc = 0;
 printf("Slammed the breaks at: %6.3f,,%6.3f,,%6.3f,,%6.3f\n\n",ship->loc[0],ship->loc[1],ship->loc[2],ship->loc[3]);
}

void nullor(ship* ship) //sets ship to its inital orientation
{
 copy4d(I,ship->reli);
 copy4d(J,ship->relj);
 copy4d(K,ship->relk);
 copy4d(L,ship->rell);
 printf("Null oriented the ship traveling: %6.3f,,%6.3f,,%6.3f,,%6.3f\n\n",ship->vel[0],ship->vel[1],ship->vel[2],ship->vel[3]);
}

void checksensors(ship* ships,char shipc) //displays info about other ships
{
 char i;
 for(i=1;i<shipc;i++)
 {
  if(ships[i].playerd < SENSORR)
  {
   printf("Ship %d is very close:\n",i);
   printshipinfo(&ships[i]);
  }
  else
  {
   printf("Ship %d:\n",i);
   printvagu(&ships[i]);
}}}

void lookat(ship* ships,char shipc) //possibly displays info about other ships
{
 double alfa[4], beta[4], delt[4], epsi[4], dist;
 float gama[4];
 char i, spotted=0;
 printf("Input examination point in the form r,th,ph,ps\n");
 scanf("%f,%f,%f,%f",gama,&gama[1],&gama[2],&gama[3]);
 while(gama[0] > LOOKD)
 {
  printf("You'd need some pretty epic glasses to see *THAT* far, eagle-eye\n");
  scanf("%f,%f,%f,%f",gama,&gama[1],&gama[2],&gama[3]);
 }
 
 alfa[0] = (double)gama[0];
 if(!control)
 {alfa[1] = (double)gama[1]; alfa[2] = (double)gama[2]; alfa[3] = (double)gama[3];}
 else if(control==1)
 {alfa[1] = (double)gama[1]*M_PI; alfa[2] = (double)gama[2]*M_PI; alfa[3] = (double)gama[3]*M_PI;}
 else if(control==2)
 {alfa[1] = (double)gama[1]*TAU; alfa[2] = (double)gama[2]*TAU; alfa[3] = (double)gama[3]*TAU;}

 fourpto4c(alfa,beta);
 scalemult4d(ships->reli,beta[0],delt);
 copy4d(delt,epsi);
 scalemult4d(ships->relj,beta[1],delt);
 add4d(delt,epsi,epsi);
 scalemult4d(ships->relk,beta[2],delt);
 add4d(delt,epsi,epsi);
 scalemult4d(ships->rell,beta[3],delt);
 add4d(delt,epsi,epsi);
 add4d(epsi,ships->loc,epsi);

 printf("You looked towards: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",epsi[0],epsi[1],epsi[2],epsi[3]);

 normalize4d(beta,beta);
 
 for(i=0;i<shipc;i++)
 {
  dist4d(epsi,ships[i].loc,&dist);
  if((hit(beta,ships[i].playerv,LOOKR) && (ships[i].playerd < alfa[0])) || (dist < LOOKR))
  {
   spotted = 1;
   printf("Ship %d spotted!\n",i);
   printshipinfo(&ships[i]);
   if(i==0)
    printf("Man, it's unnerving how you can see yourself like that\n\n");
  }
 }
 if(!spotted)
  printf("\n");
}

void scattergun(ship* ships,char shipc) //fires the scattergun
{
 char i,s;
 double angs[4], shot[4], adjshot[4], dist;
 
 printf("Scattergun hits at:\n");
 for(i=0;i<SCATTERQ;i++)
 {
  angs[0] = (double)rand()/(double)RAND_MAX*SCATTERD;
  angs[1] = (double)rand()/(double)RAND_MAX*TAU/4;
  angs[2] = (double)rand()/(double)RAND_MAX*TAU/4;
  angs[3] = (double)rand()/(double)RAND_MAX*TAU/4;
  fourpto4c(angs,shot);
  add4d(shot,ships[0].loc,adjshot);
  for(s=1;s<shipc;s++)
  {
   dist4d(adjshot,ships[s].loc,&dist);
   if(dist<SCATTERR)
   {
    ships[s].destroyed = 1;
    printf("Ship %d blown away by your scattergun",s);
   }
  }
  printf("%6.3f,,%6.3f,,%6.3f,,%6.3f\n",adjshot[0],adjshot[1],adjshot[2],adjshot[3]);
 }
}

void rifle(ship* ships,char shipc) //fires the rifle
{
 char i;
 for(i=1;i<shipc;i++)
  if(hit(ships[0].reli,ships[i].playerv,RIFLER))
  {
   ships[i].destroyed = 1;
   printf("Ship %d lanced by your rifle",i);
  }
 printf("Rifle fired at: %6.3f,,%6.3f,,%6.3f,,%6.3f\n",ships[0].reli[0],ships[0].reli[2],ships[0].reli[2],ships[0].reli[3]);
}

void gunning(ship* ship,char shipc) //asks user which gun to fire
{
 char c = 0;
 printf("Choose gun");
 printf("\n s) Scattergun");
 printf("\n r) Rifle");
 printf("\n x) Don't shoot");
 printf("\nWHAT DO? ");
 while(!((c=='s')||
         (c=='r')||
         (c=='x')))
  scanf("%c",&c);
 printf("\n");
 switch(c)
 {
  case 's':
   scattergun(ship, shipc);
  break;
  case 'r':
   rifle(ship, shipc);
  break;
 }
}

void endturn(ship* ship) //ends the current turn, moves ships
{
 double alfa[DIMENSIONS];
 scalemult4d(ship->reli,ship->acc,alfa);
 add4d(ship->vel,alfa,ship->vel);
 add4d(ship->loc,ship->vel,ship->loc);
 ship->acc = 0;
 printf("Turn over\n\n");
}

void switchctrl() //determines how angles are interpreted
{
 char c = 0;
 printf("Choose input method");
 printf("\n r) Radians");
 printf("\n p) Radians/pi");
 printf("\n t) Radians/(tau=2*pi)");
 printf("\nWHAT DO? ");
 while(!((c=='r')||
         (c=='p')||
         (c=='t')))
  scanf("%c",&c);
 printf("\n");
 switch(c)
 {
  case 'r':
   control = 0;
  break;
  case 'p':
   control = 1;
  break;
  case 't':
   control = 2;
  break;
 }
}

void setships(ship* ships,char shipc) //initialize ships
{
 char i;
 double alfa[4];
 scalemult4d(ZEROS,(double)i,ships[i].loc);
 for(i=0;i<shipc;i++)
 {
  scalemult4d(NINES,(double)i,ships[i].loc);
  copy4d(ZEROS,ships[i].vel);
  copy4d(I,ships[i].reli);
  copy4d(J,ships[i].relj);
  copy4d(K,ships[i].relk);
  copy4d(L,ships[i].rell);
  ships[i].destroyed = 0;
 }
}

void orcheat(ship* ship) //set orientation using cartesian vectors
{
 float or[4];
 printf("h4x mode go!\n");
 
 scanf("%f,%f,%f,%f",&or[0],&or[1],&or[2],&or[3]);
 ship->reli[0] = (double)or[0];
 ship->reli[1] = (double)or[1];
 ship->reli[2] = (double)or[2];
 ship->reli[3] = (double)or[3];

 scanf("%f,%f,%f,%f",&or[0],&or[1],&or[2],&or[3]);
 ship->relj[0] = (double)or[0];
 ship->relj[1] = (double)or[1];
 ship->relj[2] = (double)or[2];
 ship->relj[3] = (double)or[3];

 scanf("%f,%f,%f,%f",&or[0],&or[1],&or[2],&or[3]);
 ship->relk[0] = (double)or[0];
 ship->relk[1] = (double)or[1];
 ship->relk[2] = (double)or[2];
 ship->relk[3] = (double)or[3];

 perp4d(ship->reli,ship->relj,ship->relk,ship->rell);
 scalemult4d(ship->rell,-1,ship->rell);
 perp4d(ship->rell,ship->reli,ship->relj,ship->relk);

 perp4d(ship->relk,ship->rell,ship->reli,ship->relj);
 scalemult4d(ship->relj,-1,ship->relj);

 normalize4d(ship->reli,ship->reli);
 normalize4d(ship->relj,ship->relj);
 normalize4d(ship->relk,ship->relk);
 normalize4d(ship->rell,ship->rell);

 printvec(ship->reli,DIMENSIONS);
 printvec(ship->relj,DIMENSIONS);
 printvec(ship->relk,DIMENSIONS);
 printvec(ship->rell,DIMENSIONS);

 printf("\n");
}

void jmpcheat(ship* ship) //jums ship to a specific location
{
 printf("hXc h4x mode go!\n");
 float jmpto[4];
 scanf("%f,%f,%f,%f",&jmpto[0],&jmpto[1],&jmpto[2],&jmpto[3]);
 ship->loc[0] = (double)jmpto[0];
 ship->loc[1] = (double)jmpto[1];
 ship->loc[2] = (double)jmpto[2];
 ship->loc[3] = (double)jmpto[3];
 printf("\n");
}

void killcheat(ship* shooter,ship* target) //orients ship at target for a rifle shot
{
 printf("Killshot aligned\n");
 copy4d(target->playerv,shooter->reli);

 perp4d(shooter->reli,shooter->relj,shooter->relk,shooter->rell);
 scalemult4d(shooter->rell,-1,shooter->rell);
 perp4d(shooter->rell,shooter->reli,shooter->relj,shooter->relk);
 perp4d(shooter->relk,shooter->rell,shooter->reli,shooter->relj);
 scalemult4d(shooter->relj,-1,shooter->relj);
 
 normalize4d(shooter->reli,shooter->reli);
 normalize4d(shooter->relj,shooter->relj);
 normalize4d(shooter->relk,shooter->relk);
 normalize4d(shooter->rell,shooter->rell);

 printvec(shooter->reli,DIMENSIONS);
 printvec(shooter->relj,DIMENSIONS);
 printvec(shooter->relk,DIMENSIONS);
 printvec(shooter->rell,DIMENSIONS);

 printf("\n");
}

int main(int argc, char** argv)
{
 char c;
 srand((unsigned int)time(NULL));
 ship ships[2];
 setships(ships,2);

 printf("*********************\n");
 printf("*Welcome to 4d space*\n");
 printf("*********************\n"); 

 while (c != 'x')
 {
  sub4d(ships[1].loc,ships[0].loc,ships[1].playerv);
  mag4d(ships[1].playerv,&ships[1].playerd);

  printf("Actions:");
  printf("\n t) Fire thrusters!");
  printf("\n r) Reorient ship");
  printf("\n b) BREAKS!");
  printf("\n n) Null orient ship");
  printf("\n s) Review sensor output");
  printf("\n l) Look at");
  printf("\n p) Print ship info");
  printf("\n w) Weapons");
  printf("\n g) Go");
  printf("\n c) Change controls");
  printf("\n x) Exit");
  printf("\nWHAT DO? ");
  c = 0;
  while(!((c=='t')||
          (c=='r')||
          (c=='b')||
          (c=='n')||
          (c=='s')||
          (c=='l')||
          (c=='p')||
          (c=='w')||
          (c=='g')||
          (c=='c')||
          (c=='o')||
          (c=='j')||
          (c=='k')||
          (c=='x')))
   scanf("%c", &c);
  switch(c)
  {
  case 't':
   getthrust(&ships[0]);
  break;
  case 'r':
   getangs(&ships[0]);
  break;
  case 'b':
   slambreaks(&ships[0]);
  break;
  case 'n':
   nullor(&ships[0]);
  break;
  case 's':
   checksensors(ships,2);
  break;
  case 'l':
   lookat(ships,2);
  break;
  case 'p':
   printf("Your Ship:\n");
   printshipinfo(&ships[0]);
  break;
  case 'w':
   gunning(ships,2);
   if(ships[1].destroyed)
   {
    printf("He so dead!\n\n");
    printf("*********\n");
    printf("*You win*\n");
    printf("*********\n");
    exit(0);
   }
   else
    printf("Whiff!\n\n");
  break;
  case 'g':
   endturn(&ships[0]);
  break;
  case 'c':
   switchctrl();
  break;
  case 'o':
   orcheat(&ships[0]);
  break;
  case 'j':
   jmpcheat(&ships[0]);
  break;
  case 'k':
   killcheat(&ships[0],&ships[1]);
  break;
  case 'x':
   printf("Adios!\n");
  break;
  }
 }
 return 0;
}
