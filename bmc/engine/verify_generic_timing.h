/**
 * DSVerifier - Digital Systems Verifier (Timing)
 *
 * Federal University of Amazonas - UFAM
 *
 * Authors:       Hussama Ismail <hussamaismail@gmail.com>
 *                Iury Bessa     <iury.bessa@gmail.com>
 *
 * ------------------------------------------------------
 *
 * Verify time constraint considering generic instructions
 *
 * ------------------------------------------------------
*/

int nondet_int();
float nondet_float();

extern digital_system ds;
extern implementation impl;
extern hardware hw;

int generic_timer = 0;

int verify_generic_timing(void) {

	/* prepare initial values */
	float y[X_SIZE_VALUE];
	float x[X_SIZE_VALUE];
	int i;
	for (i = 0; i < X_SIZE_VALUE; ++i) {
		y[i] = 0;
		x[i] = nondet_float();
		__DSVERIFIER_assume(x[i] >= impl.min && x[i] <= impl.max);
	}

	int Nw = 0;
	#if ((REALIZATION == CDFI) || (REALIZATION == CDFII) || (REALIZATION == CTDFII))
		Nw = a_cascade_size > b_cascade_size ? a_cascade_size : b_cascade_size;
	#else
		Nw = ds.a_size > ds.b_size ? ds.a_size : ds.b_size;
	#endif

	float yaux[ds.a_size];
	float xaux[ds.b_size];
	float waux[Nw];

	for (i = 0; i < ds.a_size; ++i) {
		yaux[i] = 0;
	}
	for (i = 0; i < ds.b_size; ++i) {
		xaux[i] = 0;
	}
	for (i = 0; i < Nw; ++i) {
		waux[i] = 0;
	}

	float xk, temp;
	float *aptr, *bptr, *xptr, *yptr, *wptr;

	int j;

	generic_timer += ((2 * hw.assembly.std) + (1 * hw.assembly.rjmp));
	float initial_timer = generic_timer;
	for (i = 0; i < X_SIZE_VALUE; ++i) {

		generic_timer += ((2 * hw.assembly.ldd) + (1 * hw.assembly.adiw) + (2 * hw.assembly.std));
		generic_timer += ((2 * hw.assembly.ldd) + (1 * hw.assembly.cpi) + (1 * hw.assembly.cpc) + (1 * hw.assembly.brlt));

		/* direct form I realization */
		#if (REALIZATION == DFI || REALIZATION == DDFI)
			generic_timing_shift_l_float(x[i], xaux, ds.b_size);
			y[i] = generic_timing_float_direct_form_1(yaux, xaux, ds.a, ds.b, ds.a_size, ds.b_size);
			generic_timing_shift_l_float(y[i], yaux, ds.a_size);
		#endif

		/* direct form II realization */
		#if (REALIZATION == DFII || REALIZATION == DDFII)
			generic_timing_shift_r_float(0, waux, Nw);
			 y[i] = generic_timing_float_direct_form_2(waux, x[i], ds.a, ds.b, ds.a_size, ds.b_size);
		#endif

		/* transposed direct form II realization */
		#if (REALIZATION == TDFII || REALIZATION == TDDFII)
			y[i] = generic_timing_float_transposed_direct_form_2(waux, x[i], ds.a, ds.b, ds.a_size, ds.b_size);
		#endif

		float spent_time = (((float) generic_timer) * hw.cycle);
		assert(spent_time <= ds.sample_time);
		generic_timer = initial_timer;
	}
	return 0;
}
