#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI acos(-1.0)
#endif
#define PI M_PI

/* smooth/rough は自動的に場合わけされる*/

/* レンダリングプログラムの方は観測点までの距離が有限
   オブジェクト表面の各点ごとに，角度が異なる */

double hapke(double i, double e, double g){

    /* arguments in degree !!! */
	double	i_rad, e_rad, g_rad;
	double	w, B0, h, xi, theta;
	double	B, p, r0, H1, H2, S, r;
	double	phi, f_phi;
	double	theta_rad, x_theta;
	double	E1_i, E1_e, E2_i, E2_e;
	double	mu_0e, mu_e, mu_0e0, mu_e0;
	double tmp;

  /* SETTING parameters ,あとでポリゴンごとに変えられるようにする*/

	/*  w=0.37,h=0.050,B0=1.64,g=-0.26 (Pv=0.32,G=0.21に相当）
	    w=0.54,h=0.047,B0=1.20,g=-0.25 (Pv0.41,G=0.31に相当）
	    w=0.22,h=0.051,B0=2.24,g=-0.27 (Pv=0.23,G=0.11に相当）


   Phobos: w=0.07; h=0.055; g=-0.08; B0=4.0,theta=22.0*M_PI/180.0 ;
   APOLLO:  a=0.318; h=0.034; g=-0.32; B0=0.90,theta=15.0*M_PI/180.0 ;


	    #ジオメトリックアルベド：Vバンドの値として0.32±0.09
	    #スロープパラメータ：0.21±0.10

	    安部さんに教えてもらった論文参照
	    G 再現してみる


         */


	

	w=0.37; h=0.05; xi=-0.26; B0=1.64; theta=20.0 ;
	// 	w=0.1; h=0.05; xi=0; B0=0; theta=0.0 ;
	//	w=0.25; h=100; xi=0.0; B0=0.0; theta=25.0 ;  
	

	i_rad  = i*M_PI/180.0 ;
	e_rad  = e*M_PI/180.0 ;	
	g_rad  = g*M_PI/180.0;

	theta_rad = theta * M_PI / 180.0;


	/***** opposition effect function *****/
	B=B0/(1.0+(1.0/h)*tan(g_rad/2.0));     
	/***** 1-term Henyey-Greenstein phase function *****/
	p= (1.0-(xi*xi))/sqrt((1.0+(2.0*xi*cos(g_rad))
           +(xi*xi))*(1.0+(2.0*xi*cos(g_rad))
	   +(xi*xi))*(1.0+(2.0*xi*cos(g_rad))+(xi*xi)));     
	    
	/******************** smooth surface model ********************/

	if(fabs(theta_rad) < DBL_EPSILON){
	    
	    r0=2.0/(1.0+sqrt(1.0-w))-1.0;
	    H1=1.0/(1.0-(1.0-sqrt(1.0-w))*cos(i_rad)*(r0+(1.0-r0/2.0-r0*cos(i_rad))
						      *log((1.0+cos(i_rad))/cos(i_rad))));
	    H2=1.0/(1.0-(1.0-sqrt(1.0-w))*cos(e_rad)*(r0+(1.0-r0/2.0-r0*cos(e_rad))
						      *log((1.0+cos(e_rad))/cos(e_rad))));
	    
	    r=(w/(4.0*PI))*(cos(i_rad)/(cos(i_rad)+cos(e_rad)))*(((1.0+B)*p)+(H1*H2)-1.0);

	    
	}else{
	    
	/******************** rough surface model ********************/
	    
	    if(fabs(i_rad) < DBL_EPSILON && fabs(e_rad) < DBL_EPSILON){
		
			phi = M_PI;
			f_phi=exp(-2.0*tan(phi/2.0));
			theta_rad=theta*PI/180.0;
			x_theta=1.0/sqrt(1.0+PI*tan(theta_rad)*tan(theta_rad));
			
			
			mu_0e=x_theta;
			mu_e=x_theta;
			mu_0e0=x_theta;
			mu_e0=x_theta;
			S=(mu_e/mu_e0)*(1.0/mu_0e0)*(x_theta/(1.0-f_phi+f_phi*x_theta*(1.0/mu_0e0)));
			
	    }else if(fabs(i_rad) < DBL_EPSILON){
		
			phi = PI;
			f_phi=exp(-2.0*tan(phi/2.0));
			theta_rad=theta*PI/180.0;
			x_theta=1.0/sqrt(1.0+PI*tan(theta_rad)*tan(theta_rad));
			
			
			E1_e=exp((-2.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(e_rad)));
			E2_e=exp((-1.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(theta_rad))*(1.0/tan(e_rad))*(1.0/tan(e_rad)));
			mu_0e=x_theta;
			mu_e=x_theta*(cos(e_rad)+sin(e_rad)*tan(theta_rad)*(E2_e/(2.0-E1_e)));
			mu_0e0=x_theta;
			mu_e0=x_theta*(cos(e_rad)+sin(e_rad)*tan(theta_rad)*(E2_e/(2.0-E1_e)));
			S=(mu_e/mu_e0)*(1.0/mu_0e0)*(x_theta/(1.0-f_phi+f_phi*x_theta*(1.0/mu_0e0)));
			
	    }
	    
	    else if(fabs(e_rad) < DBL_EPSILON){
		
			phi = PI;
			f_phi=exp(-2.0*tan(phi/2.0));
			theta_rad=theta*PI/180.0;
			x_theta=1.0/sqrt(1.0+PI*tan(theta_rad)*tan(theta_rad));
			
			
			E1_i=exp((-2.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(i_rad)));
			E2_i=exp((-1.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(theta_rad))*(1.0/tan(i_rad))*(1.0/tan(i_rad)));
			mu_0e=x_theta*(cos(i_rad)+sin(i_rad)*tan(theta_rad)*(E2_i/(2.0-E1_i)));
			mu_e=x_theta;
			mu_0e0=x_theta*(cos(i_rad)+sin(i_rad)*tan(theta_rad)*(E2_i/(2.0-E1_i)));
			mu_e0=x_theta;
			S=(mu_e/mu_e0)*(cos(i_rad)/mu_0e0)*(x_theta/(1.0-f_phi+f_phi*x_theta*(1.0/mu_e0)));
			
		
	    }else if(i_rad<=e_rad){
		
			tmp=(cos(g_rad)-cos(i_rad)*cos(e_rad))/(sin(i_rad)*sin(e_rad));
				if (  tmp > 1 ){
				  phi = 0;
				}else if (tmp < -1){
					phi = M_PI;
				
				} else{
					phi=acos((cos(g_rad)-cos(i_rad)*cos(e_rad))/(sin(i_rad)*sin(e_rad)));
				}
				
			f_phi=exp(-2.0*tan(phi/2.0));
			theta_rad=theta*PI/180.0;
			x_theta=1.0/sqrt(1.0+PI*tan(theta_rad)*tan(theta_rad));
			
			E1_i=exp((-2.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(i_rad)));
			E1_e=exp((-2.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(e_rad)));
			E2_i=exp((-1.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(theta_rad))*(1.0/tan(i_rad))*(1.0/tan(i_rad)));
			E2_e=exp((-1.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(theta_rad))*(1.0/tan(e_rad))*(1.0/tan(e_rad)));
			mu_0e=x_theta*(cos(i_rad)+sin(i_rad)*tan(theta_rad)*((cos(phi)*E2_e+sin(phi/2.0)*sin(phi/2.0)*E2_i)/(2.0-E1_e-(phi/PI)*E1_i)));
			mu_e=x_theta*(cos(e_rad)+sin(e_rad)*tan(theta_rad)*((E2_e-sin(phi/2.0)*sin(phi/2.0)*E2_i)/(2.0-E1_e-(phi/PI)*E1_i)));
			mu_0e0=x_theta*(cos(i_rad)+sin(i_rad)*tan(theta_rad)*(E2_i/(2.0-E1_i)));
			mu_e0=x_theta*(cos(e_rad)+sin(e_rad)*tan(theta_rad)*(E2_e/(2.0-E1_e)));
			S=(mu_e/mu_e0)*(cos(i_rad)/mu_0e0)*(x_theta/(1.0-f_phi+f_phi*x_theta*(cos(i_rad)/mu_0e0)));
	    }
	    
	    else {
		
			tmp=(cos(g_rad)-cos(i_rad)*cos(e_rad))/(sin(i_rad)*sin(e_rad));
			if (  tmp > 1 ){
				phi = 0;
			}else if (tmp < -1){
				phi = M_PI;
				
			} else{
				phi=acos((cos(g_rad)-cos(i_rad)*cos(e_rad))/(sin(i_rad)*sin(e_rad)));
			}
	    			
			f_phi=exp(-2.0*tan(phi/2.0));
			theta_rad=theta*PI/180.0;
			x_theta=1.0/sqrt(1.0+PI*tan(theta_rad)*tan(theta_rad));
			

			E1_i=exp((-2.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(i_rad)));
			E1_e=exp((-2.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(e_rad)));
			E2_i=exp((-1.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(theta_rad))*(1.0/tan(i_rad))*(1.0/tan(i_rad)));
			E2_e=exp((-1.0/PI)*(1.0/tan(theta_rad))*(1.0/tan(theta_rad))*(1.0/tan(e_rad))*(1.0/tan(e_rad)));
			mu_0e=x_theta*(cos(i_rad)+sin(i_rad)*tan(theta_rad)*((E2_i-sin(phi/2.0)*sin(phi/2.0)*E2_e)/(2.0-E1_i-(phi/PI)*E1_e)));
			mu_e=x_theta*(cos(e_rad)+sin(e_rad)*tan(theta_rad)*((cos(phi)*E2_i+sin(phi/2.0)*sin(phi/2.0)*E2_e)/(2.0-E1_i-(phi/PI)*E1_e))); 
			mu_0e0=x_theta*(cos(i_rad)+sin(i_rad)*tan(theta_rad)*(E2_i/(2.0-E1_i)));
			mu_e0=x_theta*(cos(e_rad)+sin(e_rad)*tan(theta_rad)*(E2_e/(2.0-E1_e)));
				S=(mu_e/mu_e0)*(cos(i_rad)/mu_0e0)*(x_theta/(1.0-f_phi+f_phi*x_theta*(cos(e_rad)/mu_e0)));
				
	    }
	    
	    r0=2.0/(1.0+sqrt(1.0-w))-1.0;
	    H1=1.0/(1.0-(1.0-sqrt(1.0-w))*mu_0e*(r0+(1.0-r0/2.0-r0*mu_0e)*log((1.0+mu_0e)/mu_0e)));
	    H2=1.0/(1.0-(1.0-sqrt(1.0-w))*mu_e*(r0+(1.0-r0/2.0-r0*mu_e)*log((1.0+mu_e)/mu_e)));
	    r=(w/(4.0*PI))*(mu_0e/(mu_0e+mu_e))*(((1.0+B)*p)+(H1*H2)-1.0)*S;	    
	    
	}
	    
   	return r;
}

