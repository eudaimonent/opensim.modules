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


#define FFT(s,u) \
	if (s==1) rfftwnd_one_real_to_complex(plan_rc, (fftw_real*)u, (fftw_complex*)u);\
	else      rfftwnd_one_complex_to_real(plan_cr, (fftw_complex*)u, (fftw_real*)u);


#define floor(x) ((x)>=0.0 ? ((int)(x)) : (-((int)(1-(x)))))



void init_FFT(int n)
{
	plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
	plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);
}



void stable_solve(int n, float* u, float* v, float* u0, float* v0, float visc, float dt)
{
	float x, y, f, r_sq, U[2], V[2], s, t; 
	int   i, j, i0, j0, i1, j1;

	/* apply user defined forces (f) stored in u0 and v0 
	 * only add the confinement forces (vort_force_u and 
	 * vort_force_v) if they are activated by the user
	 */
	for (i=0; i<n*n; i++) {
		u [i] += dt*u0[i]; 		// 速度の変化
		u0[i]  = u[i];
		v [i] += dt*v0[i];
		v0[i]  = v[i];
	}
	
	/* advection step (-(u.G).u) */
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			/* compute exact position of particle one timestep ago */
			x = i - dt*u0[i+n*j]*n; 
			y = j - dt*v0[i+n*j]*n;

			/* discretize to grid and compute interpolation factors s and t */
			i0 = floor(x); 
			j0 = floor(y);
			s  = x - i0;		// 小数点以下
			t  = y - j0;

			/* make sure that advection wraps in u direction */
			i0 = (n + (i0%n))%n;
			i1 = (i0 + 1)%n;

			/* make sure that advection wraps in v direction */
			j0 = (n + (j0%n))%n; 
			j1 = (j0 + 1)%n;

			/* set new velocity to linear interpolation of previous
			   particle position using interpolation factors s and t
	 		   and the four grid positions i0, i1, j0 and j1 */
			// 線型補間
			u[i+n*j] = (1-s)*((1-t)*u0[i0+n*j0]+t*u0[i0+n*j1]) + s*((1-t)*u0[i1+n*j0]+t*u0[i1+n*j1]);
			v[i+n*j] = (1-s)*((1-t)*v0[i0+n*j0]+t*v0[i0+n*j1]) + s*((1-t)*v0[i1+n*j0]+t*v0[i1+n*j1]);
		}
	}

	/* swap grids (copy u and v into u0 and v0) */
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			u0[i+(n+2)*j] = u[i+n*j]; 
			v0[i+(n+2)*j] = v[i+n*j];
		}
	}
	
	/* fourier transform (in place: reason for arrays u0 and v0 to be a bit wider (see project report) */
	FFT(1, u0);
	FFT(1, v0);

	/* ----------------------------------------------------
	 * diffusion and projection steps in the fourier domain
	 * ----------------------------------------------------
	 *
	 * x(i) runs in the interval [0,n/2] and only touches the real parts
	 * of the complex fourier transform (i+=2) 
	 *
	 * y(j) runs in the interval [-n/2,n/2], but runs from 0 to n/2 and
	 * from -n/2 back up to zero (skewed by n/2)
	 *
	 * output of FFTW looks like this (all types are float's, but
	 * they differ in 'real' (r) and 'imaginary' (i) part of the transform
	 * 
	 *        x -> [0,n/2]
	 *				 
	 *          0   1   2 . . .	         n/2
	 *
	 * y  0		r i r i r i r i r i . . . r i
	 * |  1		r i r i r i r i r i . . . r i
	 * v        . . . . . . . . . . . . . . .
	 *    n/2   r i r i r i r i r i . . . r i
	 *	- n/2	r i r i r i r i r i . . . r i
	 *          . . . . . . . . . . . . . . .
	 *	- 1	    r i r i r i r i r i . . . r i
	 *    0	    r i r i r i r i r i . . . r i
	 *
	 */
	for (i=0; i<=n; i+=2) {
		/* x runs in the interval [0,n/2] */
		x = 0.5*i;
		for (j=0; j<n; j++) {
			/* x runs in the interval [-n/2,n/2] but in the sequence (from top to bottom) 0, n/2, -n/2, 0 */
			y = j<=n/2 ? j : j-n;

			/* r_sq is sq distance from the origin */
			r_sq = x*x + y*y;
			if (r_sq==0.0) continue;

			/* low-pass filter (viscosity/dampening) */
			f = exp(-r_sq*dt*visc);

			/* U[0] is real, U[1] is imaginary part of u transform
			 * V[0] is real, V[1] is imaginary part of v transform
			 */
			U[0] = u0[i  +(n+2)*j]; V[0] = v0[i  +(n+2)*j];
			U[1] = u0[i+1+(n+2)*j]; V[1] = v0[i+1+(n+2)*j];

			/* project both the real and imaginary parts of the complex transform
			 * onto lines perpendicular to the corresponding wavenumber and 
			 * multiply with the low-pass filter f. this line would 
			 * be a (hyper)plane in higher dimensions.
			 */
			u0[i  +(n+2)*j] = f*((1-x*x/r_sq)*U[0]     -x*y/r_sq *V[0]);
			u0[i+1+(n+2)*j] = f*((1-x*x/r_sq)*U[1]     -x*y/r_sq *V[1]);
			v0[i+  (n+2)*j] = f*(  -y*x/r_sq *U[0] + (1-y*y/r_sq)*V[0]);
			v0[i+1+(n+2)*j] = f*(  -y*x/r_sq *U[1] + (1-y*y/r_sq)*V[1]);
		}
	}
	
	/* inverse fourier transform */
	FFT(-1, u0);
	FFT(-1, v0);
	
	/* normalization step: 
	 * The RFFTWND transforms are unnormalized, so a forward followed 
	 * by a backward transform will result in the original data scaled 
	 * by the number of real data elements--that is, the product of the 
	 * (logical) dimensions of the real data. 
	 */
	f = 1.0/(n*n);
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			u[i+n*j] = f*u0[i+(n+2)*j]; 
			v[i+n*j] = f*v0[i+(n+2)*j]; 
		}
	}
}


