/*
 * A Simple Fluid Solver Wind
 *
 * Use FFTW v2.0
 * http://www.fftw.org
 *
 * gcc sfsw.c -fPIC -W -Wall -I/usr/local/include -c
 * gcc sfsw.o -L/usr/local/lib -lm -lrfftw -lfftw -shared -O2 -o libsfsw.so
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <rfftw.h>


static rfftwnd_plan plan_rc, plan_cr;

static fftw_complex* cmp_u = NULL;
static fftw_complex* cmp_v = NULL;

static fftw_real* u0 = NULL;
static fftw_real* v0 = NULL;



#define floor(x) ((x)>=0.0 ? ((int)(x)) : (-((int)(1-(x)))))



void init_SFSW(int n)
{
	plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_OUT_OF_PLACE);
	plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_OUT_OF_PLACE);

	cmp_u = (fftw_complex*)malloc(sizeof(fftw_complex)*n*(n/2+1));
	cmp_v = (fftw_complex*)malloc(sizeof(fftw_complex)*n*(n/2+1));

	u0 = (fftw_real*)malloc(sizeof(fftw_real)*n*n);
	v0 = (fftw_real*)malloc(sizeof(fftw_real)*n*n);
}


void free_SFSW(void)
{
//	rfftwnd_destroy_plan(plan_rc);	// OpenSim のマルチリージョンで終了時にエラー
//	rfftwnd_destroy_plan(plan_cr);

	//
	if (cmp_u!=NULL) {
		free(cmp_u);
		cmp_u = NULL;
	}
	if (cmp_v!=NULL) {
		free(cmp_v);
		cmp_v = NULL;
	}

	if (u0!=NULL) {
		free(u0);
		u0 = NULL;
	}
	if (v0!=NULL) {
		free(v0);
		v0 = NULL;
	}
}


void solve_SFSW(int n, float* u, float* v, float* fu, float* fv, int rsize, float visc, float dt)
{
	fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
	int i, j, i0, j0, i1, j1;

	for (i=0; i<n*n; i++) {
		u[i] += dt*fu[i];
		v[i] += dt*fv[i];
		u0[i] = (fftw_real)u[i]/rsize;
		v0[i] = (fftw_real)v[i]/rsize;
	}

	for (j=0; j<n; j++) {
		for (i=0; i<n; i++) {
			x0 = i - dt*u0[i+n*j]*n; 
			y0 = j - dt*v0[i+n*j]*n;

			i0 = floor(x0); 
			j0 = floor(y0);
			s  = x0 - i0; 
			t  = y0 - j0; 

			i0 = (n+(i0%n))%n; 
			i1 = (i0+1)%n;

			j0 = (n+(j0%n))%n; 
			j1 = (j0+1)%n;
			//
			u[i+n*j] = (float)((1-s)*((1-t)*u0[i0+n*j0]+t*u0[i0+n*j1]) + s*((1-t)*u0[i1+n*j0]+t*u0[i1+n*j1]));
			v[i+n*j] = (float)((1-s)*((1-t)*v0[i0+n*j0]+t*v0[i0+n*j1]) + s*((1-t)*v0[i1+n*j0]+t*v0[i1+n*j1]));
		}
	}

	for (i=0; i<n*n; i++) {
		u0[i] = (fftw_real)u[i];
		v0[i] = (fftw_real)v[i];
	}

	rfftwnd_one_real_to_complex(plan_rc, u0, cmp_u);
	rfftwnd_one_real_to_complex(plan_rc, v0, cmp_v);

	for (j=0; j<n; j++) {
		y = j<=n/2 ? j : j-n;
		//
		for (i=0; i<n/2+1; i++) {
			x = i;

			r = x*x+y*y;
			if (r==0.0) continue;
			f = exp(-r*dt*visc);

			U[0] = cmp_u[i+(n/2+1)*j].re; 
			V[0] = cmp_v[i+(n/2+1)*j].re;
			U[1] = cmp_u[i+(n/2+1)*j].im;
			V[1] = cmp_v[i+(n/2+1)*j].im;
			cmp_u[i+(n/2+1)*j].re = f*((1-x*x/r)*U[0] - x*y/r    *V[0]);
			cmp_u[i+(n/2+1)*j].im = f*((1-x*x/r)*U[1] - x*y/r    *V[1]);
			cmp_v[i+(n/2+1)*j].re = f*(   -y*x/r*U[0] + (1-y*y/r)*V[0]);
			cmp_v[i+(n/2+1)*j].im = f*(   -y*x/r*U[1] + (1-y*y/r)*V[1]);
		}
	}

	rfftwnd_one_complex_to_real(plan_cr, cmp_u, u0);
	rfftwnd_one_complex_to_real(plan_cr, cmp_v, v0);

	f = 1.0/(n*n);
	for (j=0; j<n; j++) {
		for (i=0; i<n; i++) {
			u[i+n*j] = (float)(f*u0[i+n*j])*rsize;
			v[i+n*j] = (float)(f*v0[i+n*j])*rsize;
		}
	}
}



/*

int main()
{
	int n = 16;

	init_FFT(n);
	
	float* u = (float*)malloc(sizeof(float)*n*n);
	float* v = (float*)malloc(sizeof(float)*n*n);

	memset(u, 0, sizeof(float)*n*n);
	memset(v, 0, sizeof(float)*n*n);


	float* fu = (float*)malloc(sizeof(float)*n*n);
	float* fv = (float*)malloc(sizeof(float)*n*n);

	int i;

	for (i=0; i<n*n; i++) {
		fu[i] = 1.0;
		fv[i] = 0.5;
	}
	memset(u, 0, sizeof(float)*n*n);
	memset(v, 0, sizeof(float)*n*n);

	int rsize = 256;

	stable_solve(16, u, v, fu, fv, rsize, 0.001, 1.0);
	printf("A = %f %f\n", u[0], v[0]);

	stable_solve(16, u, v, fu, fv, rsize, 0.001, 1.0);
	printf("A = %f %f\n", u[0], v[0]);

	stable_solve(16, u, v, fu, fv, rsize, 0.001, 1.0);
	printf("A = %f %f\n", u[0], v[0]);

	close_FFT();

	return 0;
}
*/
