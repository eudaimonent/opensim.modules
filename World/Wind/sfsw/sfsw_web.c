/*
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

static fftw_real* u0 = NULL;
static fftw_real* v0 = NULL;



#define FFT(s,u) \
	if (s==1) rfftwnd_one_real_to_complex(plan_rc, (fftw_real*)u, (fftw_complex*)u);\
	else      rfftwnd_one_complex_to_real(plan_cr, (fftw_complex*)u, (fftw_real*)u);


#define floor(x) ((x)>=0.0 ? ((int)(x)) : (-((int)(1-(x)))))



void init_FFT(int n)
{
	plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
	plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

	u0 = (fftw_real*)malloc(sizeof(fftw_real)*n*(n+2));
	v0 = (fftw_real*)malloc(sizeof(fftw_real)*n*(n+2));
}




void close_FFT(void)
{
	rfftwnd_destroy_plan(plan_rc);
	rfftwnd_destroy_plan(plan_cr);

	//
	if (u0!=NULL) {
		free(u0);
		u0 = NULL;
	}
	if (v0!=NULL) {
		free(v0);
		v0 = NULL;
	}
}


void stable_solve(int n, float* u, float* v, float* fu, float* fv, float dist, float visc, float dt)
{
	fftw_real x, y, f, r_sq, U[2], V[2], s, t; 
	int  i, j, i0, j0, i1, j1;

	for (i=0; i<n*n; i++) {
		u [i] += dt*fu[i]; 		// 速度の変化
		v [i] += dt*fv[i];
		u0[i]  = (fftw_real)u[i];
		v0[i]  = (fftw_real)v[i];
	}
	
	// advection step (-(u.G).u)
	for (j=0; j<n; j++) {
		for (i=0; i<n; i++) {
			x = i - dt*u0[i+n*j]/dist; 
			y = j - dt*v0[i+n*j]/dist;

			i0 = floor(x); 
			j0 = floor(y);
			s  = x - i0;		// 小数点以下
			t  = y - j0;

			i0 = (n + (i0%n))%n;
			i1 = (i0 + 1)%n;

			j0 = (n + (j0%n))%n; 
			j1 = (j0 + 1)%n;

			// 線型補間
			u[i+n*j] = (float)((1-s)*((1-t)*u0[i0+n*j0]+t*u0[i0+n*j1]) + s*((1-t)*u0[i1+n*j0]+t*u0[i1+n*j1]));
			v[i+n*j] = (float)((1-s)*((1-t)*v0[i0+n*j0]+t*v0[i0+n*j1]) + s*((1-t)*v0[i1+n*j0]+t*v0[i1+n*j1]));
		}
	}

	for (j=0; j<n; j++) {
		for (i=0; i<n; i++) {
			u0[i+(n+2)*j] = (fftw_real)u[i+n*j]; 
			v0[i+(n+2)*j] = (fftw_real)v[i+n*j];
		}
	}
	
	FFT(1, u0);
	FFT(1, v0);

	for (j=0; j<n; j++) {
		y = j<=n/2 ? j : j-n;
		//
		for (i=0; i<=n; i+=2) {
			x = 0.5*i;
			r_sq = x*x + y*y;
			if (r_sq==0.0) continue;
			f = exp(-r_sq*dt*visc);

			U[0] = u0[i  +(n+2)*j]; V[0] = v0[i  +(n+2)*j];
			U[1] = u0[i+1+(n+2)*j]; V[1] = v0[i+1+(n+2)*j];

			u0[i  +(n+2)*j] = f*((1-x*x/r_sq)*U[0]     -x*y/r_sq *V[0]);
			u0[i+1+(n+2)*j] = f*((1-x*x/r_sq)*U[1]     -x*y/r_sq *V[1]);
			v0[i+  (n+2)*j] = f*(  -y*x/r_sq *U[0] + (1-y*y/r_sq)*V[0]);
			v0[i+1+(n+2)*j] = f*(  -y*x/r_sq *U[1] + (1-y*y/r_sq)*V[1]);
		}
	}
	
	FFT(-1, u0);
	FFT(-1, v0);
	
	f = 1.0/(n*n);
	for (j=0; j<n; j++) {
		for (i=0; i<n; i++) {
			u[i+n*j] = (float)(f*u0[i+(n+2)*j]); 
			v[i+n*j] = (float)(f*v0[i+(n+2)*j]); 
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


	size_t len = sizeof(float)*n*(n+2)*2;
	float* fu = (float*)fftw_malloc(sizeof(float)*n*(n+2)*2);
	float* fv = (float*)fftw_malloc(sizeof(float)*n*(n+2)*2);

	size_t i;

	for (i=0; i<len; i++) {
		fu[i] = 1.0;
		fv[i] = 0.5;
	}
	memset(u, 0, sizeof(float)*n*n);
	memset(v, 0, sizeof(float)*n*n);

	float dist = 256./n;

	stable_solve(16, u, v, fu, fv, dist, 0.001, 1.0);
	printf("A = %f %f\n", u[0], v[0]);

	stable_solve(16, u, v, fu, fv, dist, 0.001, 1.0);
	printf("A = %f %f\n", u[0], v[0]);

	stable_solve(16, u, v, fu, fv, dist, 0.001, 1.0);
	printf("A = %f %f\n", u[0], v[0]);

	return 0;
}
*/
