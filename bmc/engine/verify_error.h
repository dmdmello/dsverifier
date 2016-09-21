/**
# DSVerifier - Digital Systems Verifier (Error)
#
#                Universidade Federal do Amazonas - UFAM
#
# Authors:       Renato Abreu   <renatobabreu@yahoo.com.br>
#				 Iury Bessa     <iury.bessa@gmail.com>
#                Hussama Ismail <hussamaismail@gmail.com>
# ------------------------------------------------------
#
# For UNCERTAINTY: For use uncertainty, It's only permited
# to use DFI, DFII and TDFII forms.
#
# ------------------------------------------------------
*/

extern digital_system ds;
extern implementation impl;

int verify_error(void){

	overflow_mode = SATURATE;

	float a_cascade[100];
	int a_cascade_size;
	float b_cascade[100];
	int b_cascade_size;

	/* check the realization */
	#if	((REALIZATION == DFI) || (REALIZATION == DFII) || (REALIZATION == TDFII))
		fxp_t a_fxp[ds.a_size];
		fxp_t b_fxp[ds.b_size];
		/* quantize the denominator using fxp */
		fxp_float_to_fxp_array(ds.a, a_fxp, ds.a_size);
		/* quantize the numerator using fxp */
		fxp_float_to_fxp_array(ds.b, b_fxp, ds.b_size);
	#elif ((REALIZATION == DDFI)||(REALIZATION == DDFII)||(REALIZATION == TDDFII))
		float da[ds.a_size];
		float db[ds.b_size];
		get_delta_transfer_function(ds.b, db, ds.b_size,ds.a, da, ds.a_size, impl.delta);
		fxp_t a_fxp[ds.a_size];
		fxp_t b_fxp[ds.b_size];
		/* quantize delta denominators using fxp */
		fxp_float_to_fxp_array(da, a_fxp, ds.a_size);
		/* quantize delta numerator using fxp */
		fxp_float_to_fxp_array(db, b_fxp, ds.b_size);
	#elif ((REALIZATION == CDFI) || (REALIZATION == CDFII) || (REALIZATION == CTDFII))
		/* generate cascade realization for digital system */
		__DSVERIFIER_generate_cascade_controllers(&ds, a_cascade, a_cascade_size, b_cascade, b_cascade_size);
		fxp_t ac_fxp[100];
		fxp_t bc_fxp[100];
		/* quantize cascade denominators */
		fxp_float_to_fxp_array(a_cascade, ac_fxp, a_cascade_size);
		/* quantize cascade numerators */
		fxp_float_to_fxp_array(b_cascade, bc_fxp, b_cascade_size);
	#elif ((REALIZATION == CDDFI) || (REALIZATION == CDDFII) || (REALIZATION == CTDDFII))
		float da_cascade[100];
		float db_cascade[100];
		/* generate cascade realization with delta for the digital system */
		__DSVERIFIER_generate_cascade_delta_controllers(&ds, da_cascade, a_cascade_size, db_cascade, b_cascade_size, impl.delta);
		fxp_t ac_fxp[100];
		fxp_t bc_fxp[100];
		/* quantize cascade denominators */
		fxp_float_to_fxp_array(da_cascade, ac_fxp, a_cascade_size);
		/* quantize cascade numerators */
		fxp_float_to_fxp_array(db_cascade, bc_fxp, b_cascade_size);
	#endif

	fxp_t min_fxp = fxp_float_to_fxp(impl.min);
	fxp_t max_fxp = fxp_float_to_fxp(impl.max);

	fxp_t y[X_SIZE_VALUE];
	fxp_t x[X_SIZE_VALUE];
	float yf[X_SIZE_VALUE];
	float xf[X_SIZE_VALUE];
	float error[X_SIZE_VALUE];
	float snrvalue;

	int Nw = 0;
	#if ((REALIZATION == CDFI) || (REALIZATION == CDFII) || (REALIZATION == CTDFII) || (REALIZATION == CDDFII) || (REALIZATION == CDDFII) || (REALIZATION == CTDDFII))
		Nw = a_cascade_size > b_cascade_size ? a_cascade_size : b_cascade_size;
	#else
		Nw = ds.a_size > ds.b_size ? ds.a_size : ds.b_size;
	#endif

	fxp_t yaux[ds.a_size];
	fxp_t xaux[ds.b_size];
	fxp_t waux[Nw];

	float yfaux[ds.a_size];
	float xfaux[ds.b_size];
	float wfaux[Nw];

	fxp_t xk, temp;
	fxp_t *aptr, *bptr, *xptr, *yptr, *wptr;

	float xkf, tempf;
	float *afptr, *bfptr, *xfptr, *yfptr, *wfptr;

	fxp_t sum;
	float sumf;

	int i;
	for (i = 0; i < ds.a_size; ++i) {
		yaux[i] = 0;
		yfaux[i] = 0;
	}
	for (i = 0; i < ds.b_size; ++i) {
		xaux[i] = 0;
		xfaux[i] = 0;
	}
	for (i = 0; i < Nw; ++i) {
		waux[i] = 0;
		wfaux[i] = 0;
	}

	for (i = 0; i < X_SIZE_VALUE; ++i) {
		y[i] = 0;
		x[i] = nondet_int();
		__DSVERIFIER_assume(x[i] >= min_fxp && x[i] <= max_fxp);
		yf[i] = 0.0f;
		xf[i] = fxp_to_float(x[i]);
	}

	int j;
	for (i = 0; i < X_SIZE_VALUE; ++i) {

		#if (REALIZATION == DFI)
			/* fixed point implementation */
			shiftL(x[i], xaux, ds.b_size);
			y[i] = fxp_direct_form_1(yaux, xaux, a_fxp, b_fxp, ds.a_size, ds.b_size);
			shiftL(y[i], yaux, ds.a_size);
			/* float precision */
			shiftLDouble(xf[i], xfaux, ds.b_size);
			yf[i] = float_direct_form_1(yfaux, xfaux, ds.a, ds.b, ds.a_size, ds.b_size);
			shiftLDouble(yf[i], yfaux, ds.a_size);
		#endif

		#if (REALIZATION == DDFI)
			/* fixed point implementation */
			shiftL(x[i], xaux, ds.b_size);
			y[i] = fxp_direct_form_1(yaux, xaux, a_fxp, b_fxp, ds.a_size, ds.b_size);
			shiftL(y[i], yaux, ds.a_size);
			/* float precision implementation */
			shiftLDouble(xf[i], xfaux, ds.b_size);
			yf[i] = float_direct_form_1(yfaux, xfaux, da, db, ds.a_size, ds.b_size);
			shiftLDouble(yf[i], yfaux, ds.a_size);
		#endif

		#if (REALIZATION == DFII)
			shiftRboth(0.0f, wfaux, 0, waux, Nw);
			/* fixed point implementation */
			y[i] = fxp_direct_form_2(waux, x[i], a_fxp, b_fxp, ds.a_size, ds.b_size);
			/* float precision implementation */
			yf[i] = float_direct_form_2(wfaux, xf[i], ds.a, ds.b, ds.a_size, ds.b_size);
		#endif

		#if (REALIZATION == DDFII)
			shiftRboth(0.0f, wfaux, 0, waux, Nw);
			/* fixed point implementation */
			y[i] = fxp_direct_form_2(waux, x[i], a_fxp, b_fxp, ds.a_size, ds.b_size);
			/* float precision implementation */
			yf[i] = float_direct_form_2(wfaux, xf[i], da, db, ds.a_size, ds.b_size);
		#endif

		#if (REALIZATION == TDFII)
		  /* fixed point implementation */
			y[i] = fxp_transposed_direct_form_2(waux, x[i], a_fxp, b_fxp, ds.a_size, ds.b_size);
			/* float precision implementation */
			yf[i] = float_transposed_direct_form_2(wfaux, xf[i], ds.a, ds.b, ds.a_size, ds.b_size);
		#endif

		#if (REALIZATION == TDDFII)
		  /* fixed point implementation */
			y[i] = fxp_transposed_direct_form_2(waux, x[i], a_fxp, b_fxp, ds.a_size, ds.b_size);
			/* float precision implementation */
			yf[i] = float_transposed_direct_form_2(wfaux, xf[i], da, db, ds.a_size, ds.b_size);
		#endif

		/* error verification using a % setted by user */
		float __quant_error = ((fxp_to_float(yf[i]) - y[i])/y[i]) * 100;
		__DSVERIFIER_assert(__quant_error < impl.max_error && __quant_error > (-impl.max_error));

	}
	return 0;
}
