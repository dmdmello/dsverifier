/**
 * DSVerifier - Digital Systems Verifier
 *
 * Federal University of Amazonas - UFAM
 *
 * Authors:       Hussama Ismail <hussamaismail@gmail.com>
 *                Iury Bessa     <iury.bessa@gmail.com>
 *                Renato Abreu   <renatobabreu@yahoo.com.br>
 *
 * ------------------------------------------------------
 *
 * util functions for DSVerifier
 *
 * ------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

/** initialize an array with zeroes */
void initialize_array(double v[], int n){
   int i;
   for(i=0; i<n; i++){
	   v[i] = 0;
   }
}

/** invert an array */
void revert_array(double v[], double out[], int n){
   initialize_array(out,n);
   int i;
   for(i=0; i<n; i++){
	  out[i] = v[n-i-1];
   }
}

/** an simplify equivalent for Math.pow() */
double internal_pow(double a, double b){
   int i;
   double acc = 1;
   for (i=0; i < b; i++){
	  acc = acc*a;
   }
   return acc;
}

/** calculate the fatorial of a number */
int fatorial(int n){
   return n == 0 ? 1 : n * fatorial(n-1);
}

/** check stability for a polynomial using jury criteria */
int check_stability(double a[], int n){
   int lines = 2 * n - 1;
   int columns = n;
   double m[lines][n];
   int i,j;

   /* to put current values in stability counter-example 
    * look for current_stability (use: --no-slice) */
   double current_stability[n];
   for (i=0; i < n; i++){
	   current_stability[i] = a[i];
   }

   for (i=0; i < lines; i++){
	  for (j=0; j < columns; j++){
		 m[i][j] = 0;
	  }
   }
   for (i=0; i < lines; i++){
	  for (j=0; j < columns; j++){
		 if (i == 0){
			m[i][j] = a[j];
			continue;
		 }
		 if (i % 2 != 0 ){
			 int x;
			 for(x=0; x<columns;x++){
				m[i][x] = m[i-1][columns-x-1];
			 }
			 columns = columns - 1;
			j = columns;
		 }else{
			m[i][j] = m[i-2][j] - (m[i-2][columns] / m[i-2][0]) * m[i-1][j];
		 }
	  }
   }
   int first_is_positive =  m[0][0] >= 0 ? 1 : 0;
   for (i=0; i < lines; i++){
	  if (i % 2 == 0){
		 int line_is_positive = m[i][0] >= 0 ? 1 : 0;
		 if (first_is_positive != line_is_positive){
			return 0;
		 }
		 continue;
	  }
   }
   return 1;
}

/**
 * The array ans will receive the sum a + b.
 * The arrays a and must be in the crescent degree order (e.g.: a0*1+a_1*x^1+a2*x^3...)
 * The result will be stored in ans[] and the size of ans[] will be stored in Nans.
 */
void poly_sum(double a[], int Na, double b[], int Nb, double ans[], int Nans){
	int i;
	Nans = Na>Nb? Na:Nb;

	for (i=0; i<Nans; i++){
		if (Na>Nb){
			ans[i]=a[i];
			if (i > Na-Nb-1){
				ans[i]=ans[i]+b[i-Na+Nb];
			}
		}else {
			ans[i]=b[i];
			if (i> Nb - Na -1){
				ans[i]=ans[i]+a[i-Nb+Na];
			}
		}
	}
}

/**
 * The array ans will receive the product a*b.
 * The arrays a and must be in the crescent degree order (e.g.: a0*1+a_1*x^1+a2*x^3...)
 * The result will be stored in ans[] and the size of ans[] will be stored in Nans.
 */
void poly_mult(double a[], int Na, double b[], int Nb, double ans[], int Nans){
	int i;
	int j;
	int k;
	Nans = Na+Nb-1;

	for (i=0; i<Na; i++){
		for (j=0; j<Nb; j++){
			k= Na + Nb - i - j - 2;
			ans[k]=0;
		}
	}

	for (i=0; i<Na; i++){
		for (j=0; j<Nb; j++){
			k= Na + Nb - i - j - 2;
			ans[k]=ans[k]+a[Na - i - 1]*b[Nb - j - 1];
		}
	}
}

/** function to check oscillations in an array (used in limit cycle property) */
void double_check_oscillations(double * y, int y_size){
	/* check if the first elements are the same, and if last repeats */
	__DSVERIFIER_assume(y[0] != y[y_size - 1]);
	int window_timer = 0;
	int window_count = 0;
	int i, j;
	for (i = 2; i < y_size; i++){
		int window_size = i;
		for(j=0; j<y_size; j++){
			if (window_timer > window_size){
				window_timer = 0;
				window_count = 0;
			}
			/* check bound of outputs */
			int window_index = j + window_size;
			if (window_index < y_size){
				/* check if window occurr */
				if (y[j] == y[window_index]){
					window_count++;
					/* window_count == window_size (the repeats occurs) */
					assert(!(window_count == window_size));
				}
			}else{
				break;
			}
			window_timer++;
		}
	}
}

/* verify limit_cycle oscilations in last outputs */
void double_check_limit_cycle(double * y, int y_size){
	/* last element is the reference */
	double reference = y[y_size - 1];
	int idx = 0;
	int window_size = 1;
	/* find window size */
	for(idx = (y_size-2); idx >= 0; idx--){
		if (y[idx] != reference){
			window_size++;
		}else{
			break;
		}
	}
	/* check if there is at least one repetition */
	__DSVERIFIER_assume(window_size != y_size && window_size != 1);
	printf("window_size %d\n", window_size);
	int desired_elements = 2 * window_size;
	int found_elements = 0;
	/* check if final oscillations occurs */
	for(idx = (y_size-1); idx >= 0; idx--){
		if (idx > (y_size-window_size-1)){
			printf("%.0f == %.0f\n", y[idx], y[idx-window_size]);
			int cmp_idx = idx - window_size;
			if ((cmp_idx > 0) && (y[idx] == y[idx-window_size])){
				found_elements = found_elements + 2;
			}else{
				break;
			}
		}
	}
	printf("desired_elements %d\n", desired_elements);
	printf("found_elements %d\n", found_elements);
	__DSVERIFIER_assert(desired_elements != found_elements);
}