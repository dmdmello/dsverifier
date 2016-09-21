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
 * verify stability in closed loop
 *
 * ------------------------------------------------------
*/

extern digital_system plant;
extern digital_system plant_cbmc;
extern digital_system controller;

int verify_stability_closedloop_using_dslib(void){

	/* generating closed loop for series or feedback */
	float * c_num = controller.b;
	int c_num_size = controller.b_size;
	float * c_den = controller.a;
	int c_den_size = controller.a_size;

	/* quantizing controller coefficients */
	fxp_t c_num_fxp[controller.b_size];
	fxp_float_to_fxp_array(c_num, c_num_fxp, controller.b_size);
	fxp_t c_den_fxp[controller.a_size];
	fxp_float_to_fxp_array(c_den, c_den_fxp, controller.a_size);

	/* getting quantized controller coefficients  */
	float c_num_qtz[controller.b_size];
	fxp_to_float_array(c_num_qtz, c_num_fxp, controller.b_size);
	float c_den_qtz[controller.a_size];
	fxp_to_float_array(c_den_qtz, c_den_fxp, controller.a_size);

	/* getting plant coefficients */
	#if (BMC == ESBMC)
		float * p_num = plant.b;
		int p_num_size = plant.b_size;
		float * p_den = plant.a;
		int p_den_size = plant.a_size;
	#elif (BMC == CBMC)
		float * p_num = plant_cbmc.b;
		int p_num_size = plant.b_size;
		float * p_den = plant_cbmc.a;
		int p_den_size = plant.a_size;
	#endif

	float ans_num[100];
	int ans_num_size = controller.b_size + plant.b_size - 1;
	float ans_den[100];
	int ans_den_size = controller.a_size + plant.a_size - 1;

	#if (CONNECTION_MODE == SERIES)
		ft_closedloop_series(c_num_qtz, c_num_size, c_den_qtz, c_den_size, p_num, p_num_size, p_den, p_den_size, ans_num, ans_num_size, ans_den, ans_den_size);
	#elif (CONNECTION_MODE == FEEDBACK)
    	printf("Verifying stability for controller\n");
		check_stability(c_den_qtz, c_den_size);
		ft_closedloop_feedback(c_num_qtz, c_num_size, c_den_qtz, c_den_size, p_num, p_num_size, p_den, p_den_size, ans_num, ans_num_size, ans_den, ans_den_size);
	#endif

	/* checking stability */
	printf("Verifying stability for closedloop function\n");
	__DSVERIFIER_assert(check_stability_closedloop(ans_den, ans_den_size, p_num, p_num_size, p_den, p_den_size));

	return 0;
}
