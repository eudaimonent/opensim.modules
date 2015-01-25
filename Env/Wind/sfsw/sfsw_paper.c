/*
 * Use FFTW v2.0
 * http://www.fftw.org
 *
 * gcc sfs.c -fPIC -W -Wall -I/usr/local/include -c
 * gcc sfs.o -L/usr/local/lib -lrfftw -lfftw -shared -O2 -o sfs.so
 * 
 */

#define FFTW_ENABLE_FLOAT

#include <math.h>
#include <rfftw.h>


static rfftwnd_plan plan_rc, plan_cr;

static fftw_complex* cmp_u = NULL;
static fftw_complex* cmp_v = NULL;



#define FFT(s,f,t) \
	if (s==1) rfftwnd_one_real_to_complex(plan_rc, (fftw_real*)f, (fftw_complex*)t);\
	else      rfftwnd_one_complex_to_real(plan_cr, (fftw_complex*)f, (fftw_real*)t);


#define floor(x) ((x)>=0.0 ? ((int)(x)) : (-((int)(1-(x)))))



void init_FFT(int n)
{
	plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
	plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

	cmp_u = (fftw_complex*)malloc(sizeof(fftw_complex)*(n+2)*n);
	cmp_v = (fftw_complex*)malloc(sizeof(fftw_complex)*(n+2)*n);
	
	u0 = (float*)malloc(sizeof(float)*(n+2)*n);
	v0 = (float*)malloc(sizeof(float)*(n+2)*n);
}



//rfftwnd_destroy_plan(p);

// n : 16
// dt: 1.0
// visc: 0.001 viscosity
void stable_solve(int n, float* u, float* v, float* fu, float* fv, float visc, float dt)
{
	float x, y, x0, y0, f, r, U[2], V[2], s, t;
	int i, j, i0, j0, i1, j1;

	for (i=0; i<n*n; i++) {
		u[i] += dt*fu[i];
		v[i] += dt*fv[i];
		u0[i] = u[i];
		v0[i] = v[i];
	}

	for (x=0.5/n,i=0; i<n; i++,x+=1.0/n) {
		for (y=0.5/n,j=0; j<n; j++,y+=1.0/n) {
			x0 = n*(x-dt*u0[i+n*j])-0.5; 
			y0 = n*(y-dt*v0[i+n*j])-0.5;
			i0 = floor(x0); s = x0-i0; i0 = (n+(i0%n))%n; i1 = (i0+1)%n;
			j0 = floor(y0); t = y0-j0; j0 = (n+(j0%n))%n; j1 = (j0+1)%n;
			u[i+n*j] = (1-s)*((1-t)*u0[i0+n*j0]+t*u0[i0+n*j1])+ s*((1-t)*u0[i1+n*j0]+t*u0[i1+n*j1]);
			v[i+n*j] = (1-s)*((1-t)*v0[i0+n*j0]+t*v0[i0+n*j1])+ s*((1-t)*v0[i1+n*j0]+t*v0[i1+n*j1]);
		}
	}

	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			u0[i+(n+2)*j] = u[i+n*j]; 
			v0[i+(n+2)*j] = v[i+n*j];
		}
	}

printf("1111111111111111111111111111111111111111111\n");
fflush(stdout);
	FFT(1, u0, cmp_u); 
	FFT(1, v0, cmp_v);

printf("2222222222222222222222222222222222222222222\n");
fflush(stdout);
/*
	//for (i=0; i<=n; i+=2) {
	for (i=0; i<n/2+1; i++) {
//		x = 0.5*i;
		x = i;
		for (j=0; j<n; j++) {
			y = (j<=n/2 ? j : j-n);
			r = x*x+y*y;
			if (r==0.0) continue;
			f = exp(-r*dt*visc);

			U[0] = cmp_u[i+(n+2)*j].re; 
			V[0] = cmp_v[i+(n+2)*j].re;
			U[1] = cmp_u[i+(n+2)*j].im;
			V[1] = cmp_v[i+(n+2)*j].im;
			cmp_u[i+(n+2)*j].re = f*((1-x*x/r)*U[0] - x*y/r    *V[0]);
			cmp_u[i+(n+2)*j].im = f*((1-x*x/r)*U[1] - x*y/r	   *V[1]);
			cmp_v[i+(n+2)*j].re = f*(   -y*x/r*U[0] + (1-y*y/r)*V[0]);
			cmp_v[i+(n+2)*j].im = f*(   -y*x/r*U[1] + (1-y*y/r)*V[1]);
		}
	}
*/
printf("3333333333333333333333333333333333333333333\n");
fflush(stdout);
	FFT(-1, cmp_u, u0); 
	FFT(-1, cmp_v, v0);

printf("4444444444444444444444444444444444444444444\n");
fflush(stdout);
	f = 1.0;///(n*n);
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			u[i+n*j] = f*u0[i+(n+2)*j];
			v[i+n*j] = f*v0[i+(n+2)*j];
		}
	}
printf("5555555555555555555555555555555555555555555\n");
fflush(stdout);
}

